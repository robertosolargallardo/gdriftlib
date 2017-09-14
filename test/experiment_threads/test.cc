#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include <Simulator.h>

#include "NanoTimer.h"

using namespace std;
using boost::property_tree::ptree;

// Soporte para extern rng
// Esto debe ser reemplazado por una instancia de mt19937 en el thread
random_device seed;
mt19937 rng(seed());

// Mutex global para controlar acceso a cout u otros objetos compartidos
mutex global_mutex;
// Cola de trabajo consumida por los threads procesadores
vector<ptree> work_queue;

double generate(const ptree &fdistribution){
	cout<<"generate - Inicio\n";
	string type = fdistribution.get<string>("type");
	double value = 0.0;
	if( type.compare("uniform") == 0 ){
		cout<<"generate - UNIFORM\n";
		double a = fdistribution.get<double>("params.a");
		double b = fdistribution.get<double>("params.b");
		uniform_real_distribution<> uniform(a, b);
		value = uniform(rng);
	}
	else if( type.compare("normal") == 0 ){
		cout<<"generate - NORMAL\n";
		double mean = fdistribution.get<double>("params.mean");
		double stddev = fdistribution.get<double>("params.stddev");
		normal_distribution<> normal(mean, stddev);
		value = normal(rng);
	}
	else if( type.compare("gamma") == 0 ){
		cout<<"generate - GAMMA\n";
		double alpha = fdistribution.get<double>("params.alpha");
		double beta = fdistribution.get<double>("params.beta");
		gamma_distribution<double> gamma(alpha, beta);
		value = gamma(rng);
	}
	else{
		cerr<<"generate - Error, unknown distribution "<<type<<"\n";
	}
	return value;
}

template<class T>
T parse_value(const ptree &variable, bool force_limits, double forced_min, double forced_max){
	cout<<"parse_value - Inicio\n";
	string type = variable.get<string>("type");
	double value = 0.0;
	if( type.compare("random") == 0 ){
		value = generate(variable.get_child("distribution"));
	}
	else{
		value = variable.get<double>("value");
	}
	if(force_limits){
		if(value < forced_min){
			value = forced_min;
		}
		else if(value > forced_max){
			value = forced_max;
		}
	}
	cout<<"parse_value - Inicio\n";
	return static_cast<T>(value);
}

// Parsea e instania un individuo particular
// Notar que uso la copia misma de findividual para guardar y retornar el resultado
ptree parse_individual(ptree findividual){
	cout<<"parse_individual - Inicio\n";
	for(auto &fchromosome : findividual.get_child("chromosomes")){
		for(auto &fgene: fchromosome.second.get_child("genes")){
			ptree frate = fgene.second.get_child("mutation.rate");
			fgene.second.get_child("mutation").erase("rate");
			fgene.second.get_child("mutation").put<double>("rate", parse_value<double>(frate, true, 0.0, 1.0));
		}
	}
	cout<<"parse_individual - Inicio\n";
	return findividual;
}

ptree parse_scenario(ptree fscenario, unsigned int min_pop){
	cout<<"parse_scenario - Inicio\n";
	uint32_t last_timestamp = 0;
	// TODO: Este limite de seguridad al tamaño de la poblacion es arbitrario
	uint32_t max_value = 100000000;
	for(auto &fevent : fscenario.get_child("events")){
		ptree ftimestamp = fevent.second.get_child("timestamp");
		fevent.second.erase("timestamp");
		uint32_t timestamp = parse_value<uint32_t>(ftimestamp, true, 0, max_value);
		if(timestamp <= last_timestamp){
			timestamp = last_timestamp + 1;
		}
		last_timestamp = timestamp;
		fevent.second.put<uint32_t>("timestamp", timestamp);
		
		string type = fevent.second.get<string>("type");
		if( type.compare("create") == 0 ){
			ptree fsize = fevent.second.get_child("params.population.size");
			fevent.second.get_child("params.population").erase("size");
			fevent.second.get_child("params.population").put<uint32_t>("size", parse_value<uint32_t>(fsize, true, 0, max_value));
		}
		else if( type.compare("increment") == 0 ){
			ptree fpercentage = fevent.second.get_child("params.source.population.percentage");
			fevent.second.get_child("params.source.population").erase("percentage");
			fevent.second.get_child("params.source.population").put<double>("percentage", parse_value<double>(fpercentage, true, 0, max_value));
		}
		else if( type.compare("decrement") == 0 ){
			ptree fpercentage = fevent.second.get_child("params.source.population.percentage");
			fevent.second.get_child("params.source.population").erase("percentage");
			fevent.second.get_child("params.source.population").put<double>("percentage", parse_value<double>(fpercentage, true, 0, 1.0));
		}
		else if( type.compare("migration") == 0 ){
			ptree fpercentage = fevent.second.get_child("params.source.population.percentage");
			fevent.second.get_child("params.source.population").erase("percentage");
			fevent.second.get_child("params.source.population").put<double>("percentage", parse_value<double>(fpercentage, true, 0, 1.0));
		}
	}
	cout<<"parse_scenario - Fin\n";
	return fscenario;
}

// Equivalente al scheduler
// LLena la cola preparando "total" instancias de simulacion basadas en el json de config "fsettings"
// Notar que fsettings en este caso es un archivo de configuracion global, con distribuciones para cada valor
void fill_queue(ptree fsettings, unsigned int total){

	cout<<"fill_queue - Inicio\n";
	NanoTimer timer;
	
	string sim_id = fsettings.get<string>("id");
	vector<ptree> scenarios;
	for(auto &fscenario : fsettings.get_child("scenarios")){
		scenarios.push_back(fscenario.second);
	}
	
	cout<<"fill_queue - Generando "<<total<<" jobs de simulacion "<<sim_id<<"\n";
	for(uint32_t i = 0; i < total; i++){
		ptree fjob;
		fjob.put("id", sim_id);
		fjob.put("run", i);
		fjob.put("batch", 0);
		fjob.put("feedback", 0);
		fjob.put("batch-size", total);
		fjob.put("max-number-of-simulations", total);
		fjob.add_child("individual", parse_individual(fsettings.get_child("individual")));
		fjob.add_child("scenario", parse_scenario(scenarios[i%scenarios.size()], 100));
		work_queue.push_back(fjob);
	}
	
	cout<<"fill_queue - Fin ("<<timer.getMilisec()<<" ms)\n";
	
}

//void SimultionThread(unsigned int pid, const ptree &fsettings) {
//	
//	global_mutex.lock();
//	cout<<"SimultionThread["<<pid<<"] - Inicio\n";
//	global_mutex.unlock();
//	
//	NanoTimer timer;
//	Simulator sim(fsettings);
//	sim.run();
//	
//	global_mutex.lock();
//	cout<<"SimultionThread["<<pid<<"] - Simulator->run Terminado en "<<timer.getMilisec()<<" ms\n";
//	global_mutex.unlock();

//	Sample all("summary");
//	map<string, Sample*> samples = sim.samples();
//	for(map<string, Sample*>::iterator i = samples.begin(); i != samples.end(); ++i){
//		ptree findices = i->second->indices();
//		stringstream ss;
//		write_json(ss, findices);
//		
//		global_mutex.lock();
//		cout << ss.str() << endl;
//		global_mutex.unlock();
//		
//		all.merge(i->second);
//	}
//	ptree findices = all.indices();
//	stringstream ss;
//	write_json(ss, findices);
//	
//	global_mutex.lock();
//	cout << ss.str() << endl;
//	global_mutex.unlock();
//	
//}

int main(int argc,char** argv){

	if(argc != 4){
		cout<<"\nUsage: ./test json_file total_jobs n_threads\n";
		cout<<"\n";
		return 0;
	}

	ptree fsettings;
	read_json(argv[1], fsettings);
	unsigned int total = atoi(argv[2]);
	unsigned int n_threads = atoi(argv[3]);
	
	cout<<"Test - Inicio\n";
	NanoTimer timer;
	
	cout<<"Test - Preparando cola de trabajo\n";
	fill_queue(fsettings, total);
	
	// Solo para probar
	cout<<"Test - Resultados:\n";
	for(unsigned int i = 0; i < work_queue.size(); ++i){
		cout<<"----- Trabajo "<<i<<" ------\n";
		std::stringstream ss;
		write_json(ss, work_queue[i]);
		cout<<ss.str()<<"\n";
		cout<<"-----      ------\n";
	}
	
	return(0);
}













