
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <vector>

#include <Simulator.h>
#include <Sample.h>

#include "NanoTimer.h"

using namespace std;
using boost::property_tree::ptree;

// Mutex global para controlar acceso a cout u otros objetos compartidos
mutex global_mutex;
// Cola de trabajo consumida por los threads procesadores
vector<ptree> work_queue;
unsigned int global_pos = 0;

double generate(const ptree &fdistribution, mt19937 &generator){
//	cout<<"generate - Inicio\n";
	string type = fdistribution.get<string>("type");
	double value = 0.0;
	if( type.compare("uniform") == 0 ){
//		cout<<"generate - UNIFORM\n";
		double a = fdistribution.get<double>("params.a");
		double b = fdistribution.get<double>("params.b");
		uniform_real_distribution<> uniform(a, b);
		value = uniform(generator);
	}
	else if( type.compare("normal") == 0 ){
//		cout<<"generate - NORMAL\n";
		double mean = fdistribution.get<double>("params.mean");
		double stddev = fdistribution.get<double>("params.stddev");
		normal_distribution<> normal(mean, stddev);
		value = normal(generator);
	}
	else if( type.compare("gamma") == 0 ){
//		cout<<"generate - GAMMA\n";
		double alpha = fdistribution.get<double>("params.alpha");
		double beta = fdistribution.get<double>("params.beta");
		gamma_distribution<double> gamma(alpha, beta);
		value = gamma(generator);
	}
	else{
		cerr<<"generate - Error, unknown distribution "<<type<<"\n";
	}
	return value;
}

template<class T>
T parse_value(const ptree &variable, mt19937 &generator, bool force_limits, double forced_min, double forced_max){
//	cout<<"parse_value - Inicio\n";
	string type = variable.get<string>("type");
	double value = 0.0;
	if( type.compare("random") == 0 ){
		value = generate(variable.get_child("distribution"), generator);
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
//	cout<<"parse_value - Inicio\n";
	return static_cast<T>(value);
}

// Parsea e instania un individuo particular
// Notar que uso la copia misma de findividual para guardar y retornar el resultado
ptree parse_individual(ptree findividual, mt19937 &generator){
//	cout<<"parse_individual - Inicio\n";
	for(auto &fchromosome : findividual.get_child("chromosomes")){
		for(auto &fgene: fchromosome.second.get_child("genes")){
			ptree frate = fgene.second.get_child("mutation.rate");
			fgene.second.get_child("mutation").erase("rate");
			fgene.second.get_child("mutation").put<double>("rate", parse_value<double>(frate, generator, true, 0.0, 1.0));
		}
	}
//	cout<<"parse_individual - Inicio\n";
	return findividual;
}

ptree parse_scenario(ptree fscenario, mt19937 &generator, unsigned int min_pop){
//	cout<<"parse_scenario - Inicio\n";
	uint32_t last_timestamp = 0;
	// TODO: Este limite de seguridad al tamaño de la poblacion es arbitrario
	uint32_t max_value = 100000000;
	for(auto &fevent : fscenario.get_child("events")){
		ptree ftimestamp = fevent.second.get_child("timestamp");
		fevent.second.erase("timestamp");
		uint32_t timestamp = parse_value<uint32_t>(ftimestamp, generator, true, 0, max_value);
		if(timestamp <= last_timestamp){
			timestamp = last_timestamp + 1;
		}
		last_timestamp = timestamp;
		fevent.second.put<uint32_t>("timestamp", timestamp);
		
		string type = fevent.second.get<string>("type");
		if( type.compare("create") == 0 ){
			ptree fsize = fevent.second.get_child("params.population.size");
			fevent.second.get_child("params.population").erase("size");
			fevent.second.get_child("params.population").put<uint32_t>("size", parse_value<uint32_t>(fsize, generator, true, 0, max_value));
		}
		else if( type.compare("increment") == 0 ){
			ptree fpercentage = fevent.second.get_child("params.source.population.percentage");
			fevent.second.get_child("params.source.population").erase("percentage");
			fevent.second.get_child("params.source.population").put<double>("percentage", parse_value<double>(fpercentage, generator, true, 0, max_value));
		}
		else if( type.compare("decrement") == 0 ){
			ptree fpercentage = fevent.second.get_child("params.source.population.percentage");
			fevent.second.get_child("params.source.population").erase("percentage");
			fevent.second.get_child("params.source.population").put<double>("percentage", parse_value<double>(fpercentage, generator, true, 0, 1.0));
		}
		else if( type.compare("migration") == 0 ){
			ptree fpercentage = fevent.second.get_child("params.source.population.percentage");
			fevent.second.get_child("params.source.population").erase("percentage");
			fevent.second.get_child("params.source.population").put<double>("percentage", parse_value<double>(fpercentage, generator, true, 0, 1.0));
		}
	}
//	cout<<"parse_scenario - Fin\n";
	return fscenario;
}

// Equivalente al scheduler
// LLena la cola preparando "total" instancias de simulacion basadas en el json de config "fsettings"
// Notar que fsettings en este caso es un archivo de configuracion global, con distribuciones para cada valor
void fill_queue(ptree fsettings, unsigned int total, mt19937 &generator){

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
		fjob.add_child("individual", parse_individual(fsettings.get_child("individual"), generator));
		fjob.add_child("scenario", parse_scenario(scenarios[i%scenarios.size()], generator, 100));
		work_queue.push_back(fjob);
	}
	
	cout<<"fill_queue - Fin ("<<timer.getMilisec()<<" ms)\n";
	
}

vector<double> get_statistics(map<string, Sample*> &samples){
//	cout << "get_statistics - Inicio\n";
		
	ptree json_stats;
	Sample all("summary");
	for(map<string, Sample*>::iterator i = samples.begin(); i != samples.end(); ++i){
		json_stats.push_back(std::make_pair("", i->second->indices()));
		all.merge(i->second);
	}
	json_stats.push_back(std::make_pair("", all.indices()));

	map<string, map<string, double>> grouped_stats;
	for( auto &population_stats : json_stats ){
		string pop_name = population_stats.second.get<string>("name");
		map<string, double> pop_stats;
		for(auto &c : population_stats.second.get_child("chromosomes")){
			for(auto &g : c.second.get_child("genes")){
				for(auto i : g.second.get_child("indices")){
					pop_stats[i.first] = std::stod(i.second.data());
				}
			}
		}
		grouped_stats[pop_name] = pop_stats;
	}
	
	vector<double> statistics;
	for( auto par_1 : grouped_stats ){
//		cout << "Population name: " << par_1.first << "\n";
		for( auto par_2 : par_1.second ){
//			cout << par_2.first << ": " << par_2.second << "\n";
			statistics.push_back(par_2.second);
		}
//		cout << "-----     -----\n";
	}
	
//	cout << "get_statistics - Fin\n";
	return statistics;
}

vector<double> get_params(ptree &fjob){
//	cout << "get_params - Inicio\n";
	
	map<string, double> params_res;
	
	// LLeno params_res con los parametros de este resultado
	ptree scenario = fjob.get_child("scenario");
	for(auto &event : scenario.get_child("events")){
		
		unsigned int eid = event.second.get<unsigned int>("id");
		string param_name_base = "events.";
		param_name_base += std::to_string(eid);
		param_name_base += ".";
		
		// Primer parametro (de todos los eventos) timestamp
		// Segundo parametro, si es create, tiene size
		// Tercer parametro, si NO es endsim, split ni extinction (ni create), tiene percentage
		// Notar que esos 3 casos no tienen parametros adicionales
		
		unsigned int timestamp = event.second.get<unsigned int>("timestamp");
		params_res[param_name_base + "timestamp"] = timestamp;
		
		string type = event.second.get<string>("type");
		if( type.compare("create") == 0 ){
			unsigned int size = event.second.get<unsigned int>("params.population.size");
			params_res[param_name_base + "params.population.size"] = size;
		}
		else if( type.compare("endsim") != 0 
			&& type.compare("split") != 0 
			&& type.compare("extinction") != 0 ){
			double percentage = event.second.get<double>("params.source.population.percentage");
			params_res[param_name_base + "params.source.population.percentage"] = percentage;
		}
		
	}
	
	// Tambien incluyo las mutaciones de cada gen
	ptree individual = fjob.get_child("individual");
	for(auto &c : individual.get_child("chromosomes")){
		uint32_t cid = c.second.get<uint32_t>("id");
		for(auto &g : c.second.get_child("genes")){
			uint32_t gid = g.second.get<uint32_t>("id");
			double rate = g.second.get<double>("mutation.rate");
			string param_name = "chromosomes.";
			param_name += std::to_string(cid);
			param_name += ".genes.";
			param_name += std::to_string(gid);
			param_name += ".mutation.rate";
			params_res[param_name] = rate;
		}
	}
	
	vector<double> params;
	for(auto par : params_res){
//		cout << "param[" << par.first << "]: " << par.second << "\n";
		params.push_back(par.second);
	}
//	cout << "get_params - End\n";
	
	return params;
}

void SimultionThread(unsigned int pid, unsigned int n_threads){
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Inicio\n";
	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	unsigned int cur_pos = 0;
	
	double model_time = 0;
	
	string file_name = "results_";
	file_name += std::to_string(pid);
	file_name += ".txt";
	fstream writer(file_name, fstream::trunc | fstream::out);
	
	while(true){
		global_mutex.lock();
		cur_pos = global_pos++;
		global_mutex.unlock();
		
		if( cur_pos >= work_queue.size() ){
			global_mutex.lock();
			cout<<"SimultionThread["<<pid<<"] - Cola de trabajo vacia, saliendo\n";
			global_mutex.unlock();
			break;
		}
		
		ptree fjob = work_queue[cur_pos];
		++procesados;
		
		Simulator sim(fjob);
		sim.model_time = 0;
		sim.run();
		model_time += sim.model_time;
		
		// Parte local del analyzer
		// Esto requiere el target 
		// Falta definir e implementar la normalizacion
		
		if( sim.detectedErrors() == 0 ){
			vector<double> statistics = get_statistics(sim.samples());
			vector<double> params = get_params(fjob);

			// Con los estadisticos y los parametros en json, tengo que sacar esos datos a double
			// En la salida van los 15 estadisticos, y los parametros
			for( double value : statistics ){
				writer << value << "\t";
			}
			for( double value : params ){
				writer << value << "\t";
			}
			writer << "\n";
			
			
		}
		
	}
	
	writer.close();
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<", Model ms: "<<model_time<<")\n";
	global_mutex.unlock();
	
}

void DummyThread(unsigned int pid, unsigned int local_jobs){
	
	global_mutex.lock();
	cout<<"DummyThread["<<pid<<"] - Inicio\n";
	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	
	unsigned int n_gens = 100;
	unsigned int pop_size = 100000;
	unsigned int *src = new unsigned int[pop_size];
	unsigned int *dst = new unsigned int[pop_size];
	
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(1, pop_size);
	
	for(unsigned int i = 0; i < local_jobs; ++i){
		++procesados;
		for(unsigned int g = 0; g < n_gens; ++g){
			for(unsigned int k = 0; k < pop_size; ++k){
				dst[k] = dist(gen);
				src[k] = dist(gen);
			}
		}
	}
	
	delete [] src;
	delete [] dst;
	
	global_mutex.lock();
	cout<<"DummyThread["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<")\n";
	global_mutex.unlock();
	
}

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
	mt19937 generator((std::random_device())());
	
	cout<<"Test - Preparando Cola de Trabajo\n";
	fill_queue(fsettings, total, generator);
	
	double ms_preparation = timer.getMilisec();
	cout<<"Test - Preparacion terminada en "<<ms_preparation<<" ms\n";
	
	cout<<"Test - Iniciando "<<n_threads<<" threads\n";
	vector<thread> threads_list;
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list.push_back( thread(SimultionThread, i, n_threads) );
//		threads_list.push_back( thread(DummyThread, i, total/n_threads) );
		
//		// Tomar pthread de este thread
//		pthread_t current_thread = threads_list.back().native_handle();
//		// Preparar datos para setear afinidad
//		cpu_set_t cpuset;
//		CPU_ZERO(&cpuset);
//		CPU_SET(i, &cpuset);
//		// Setear afinidad
//		pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
		
	}
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list[i].join();
	}
	double ms_processing = timer.getMilisec();
	cout<<"Test - Procesamiento terminado en "<<(ms_processing - ms_preparation)<<" ms\n";
	
	// Analyzer
	
	double ms_analysis = timer.getMilisec();
	cout<<"Test - Analisis terminado en "<<(ms_analysis - ms_processing)<<" ms\n";
	
	cout<<"Test - Tiempo total: "<<ms_analysis<<" ms\n";
	
	
	return(0);
}













