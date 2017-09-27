#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include <Simulator.h>

#include "NanoTimer.h"

using namespace std;
using boost::property_tree::ptree;

random_device seed;
mt19937 rng(seed());

// Mutex global para controlar acceso a cout u otros objetos compartidos
mutex global_mutex;
// Cola de trabajo consumida por los threads procesadores
vector<ptree> work_queue;
unsigned int global_pos = 0;

double generate(const ptree &fdistribution){
//	cout<<"generate - Inicio\n";
	string type = fdistribution.get<string>("type");
	double value = 0.0;
	if( type.compare("uniform") == 0 ){
//		cout<<"generate - UNIFORM\n";
		double a = fdistribution.get<double>("params.a");
		double b = fdistribution.get<double>("params.b");
		uniform_real_distribution<> uniform(a, b);
		value = uniform(rng);
	}
	else if( type.compare("normal") == 0 ){
//		cout<<"generate - NORMAL\n";
		double mean = fdistribution.get<double>("params.mean");
		double stddev = fdistribution.get<double>("params.stddev");
		normal_distribution<> normal(mean, stddev);
		value = normal(rng);
	}
	else if( type.compare("gamma") == 0 ){
//		cout<<"generate - GAMMA\n";
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
//	cout<<"parse_value - Inicio\n";
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
//	cout<<"parse_value - Inicio\n";
	return static_cast<T>(value);
}

// Parsea e instania un individuo particular
// Notar que uso la copia misma de findividual para guardar y retornar el resultado
ptree parse_individual(ptree findividual){
//	cout<<"parse_individual - Inicio\n";
	for(auto &fchromosome : findividual.get_child("chromosomes")){
		for(auto &fgene: fchromosome.second.get_child("genes")){
			ptree frate = fgene.second.get_child("mutation.rate");
			fgene.second.get_child("mutation").erase("rate");
			fgene.second.get_child("mutation").put<double>("rate", parse_value<double>(frate, true, 0.0, 1.0));
		}
	}
//	cout<<"parse_individual - Inicio\n";
	return findividual;
}

ptree parse_scenario(ptree fscenario, unsigned int min_pop){
//	cout<<"parse_scenario - Inicio\n";
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
//	cout<<"parse_scenario - Fin\n";
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

void SimultionThread(unsigned int pid, unsigned int n_threads){
	
//	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Inicio\n";
//	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	unsigned int cur_pos = 0;
	
	double model_time = 0;
	
	while(true){
		global_mutex.lock();
		cur_pos = global_pos++;
		global_mutex.unlock();
		
		if( cur_pos >= work_queue.size() ){
			cout<<"SimultionThread["<<pid<<"] - Cola de trabajo vacia, saliendo\n";
			break;
		}
		
		ptree fjob = work_queue[cur_pos];
		++procesados;
		
//		cout<<"SimultionThread["<<pid<<"] - Creando Simulator para tarea "<<cur_pos<<"\n";
		Simulator sim(fjob);
//		cout<<"SimultionThread["<<pid<<"] - Simulator::run\n";
		sim.run();
		model_time += sim.model_time;
		
		// Parte local del analyzer
		// Esto requiere el target 
		// Falta definir e implementar la normalizacion
		
	}
	
//	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<", Model ms: "<<model_time<<")\n";
//	global_mutex.unlock();
	
}

void SimultionThreadLocalParsing(unsigned int pid, unsigned int local_jobs, ptree fsettings){
	
//	global_mutex.lock();
	cout<<"SimultionThreadLocalParsing["<<pid<<"] - Inicio\n";
//	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	
	string sim_id = fsettings.get<string>("id");
	vector<ptree> scenarios;
	for(auto &fscenario : fsettings.get_child("scenarios")){
		scenarios.push_back(fscenario.second);
	}
	
	double model_time = 0;
	
	for(unsigned int i = 0; i < local_jobs; ++i){
		
		ptree fjob;
		fjob.put("id", sim_id);
		fjob.put("run", i);
		fjob.put("batch", 0);
		fjob.put("feedback", 0);
		fjob.put("batch-size", local_jobs);
		fjob.put("max-number-of-simulations", local_jobs);
		fjob.add_child("individual", parse_individual(fsettings.get_child("individual")));
		fjob.add_child("scenario", parse_scenario(scenarios[i%scenarios.size()], 100));
		
		++procesados;
		
//		cout<<"SimultionThreadLocalParsing["<<pid<<"] - Creando Simulator para tarea "<<i<<"\n";
		Simulator sim(fjob);
//		cout<<"SimultionThreadLocalParsing["<<pid<<"] - Simulator::run\n";
		sim.run();
		model_time += sim.model_time;
		
		// Parte local del analyzer
		// Esto requiere el target 
		// Falta definir e implementar la normalizacion
		
	}
	
//	global_mutex.lock();
	cout<<"SimultionThreadLocalParsing["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<", Model ms: "<<model_time<<")\n";
//	global_mutex.unlock();
	
}

void DummyThreadLocalParsing(unsigned int pid, unsigned int local_jobs, ptree fsettings){
	
//	global_mutex.lock();
	cout<<"DummyThreadLocalParsing["<<pid<<"] - Inicio\n";
//	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	
	string sim_id = fsettings.get<string>("id");
	vector<ptree> scenarios;
	for(auto &fscenario : fsettings.get_child("scenarios")){
		scenarios.push_back(fscenario.second);
	}
	
	double model_time = 0;
	
	for(unsigned int i = 0; i < local_jobs; ++i){
		
		ptree fjob;
		fjob.put("id", sim_id);
		fjob.put("run", i);
		fjob.put("batch", 0);
		fjob.put("feedback", 0);
		fjob.put("batch-size", local_jobs);
		fjob.put("max-number-of-simulations", local_jobs);
		fjob.add_child("individual", parse_individual(fsettings.get_child("individual")));
		fjob.add_child("scenario", parse_scenario(scenarios[i%scenarios.size()], 100));
		
		++procesados;
		
//		cout<<"DummyThreadLocalParsing["<<pid<<"] - Creando Simulator para tarea "<<i<<"\n";
		
		NanoTimer sim_timer;
		unsigned int n_gens = 10000;
		unsigned int pop_size = 100000;
		unsigned int *src = new unsigned int[pop_size];
		unsigned int *dst = new unsigned int[pop_size];
		for(unsigned int k = 0; k < pop_size; ++k){
			src[k] = k;
		}
		for(unsigned int g = 0; g < n_gens; ++g){
			for(unsigned int k = 0; k < pop_size; ++k){
				if( (k & 0x1) == 0 ){
					dst[i] = src[ ((k * g)>>2) % pop_size ];
				}
				else{
					dst[i] = src[ ((k * g)<<2) % pop_size ];
				}
			}
			unsigned int *tmp = src;
			src = dst;
			dst = tmp;
		}
		model_time += sim_timer.getMilisec();
		
		
		
		// Parte local del analyzer
		// Esto requiere el target 
		// Falta definir e implementar la normalizacion
		
	}
	
//	global_mutex.lock();
	cout<<"DummyThreadLocalParsing["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<", Model ms: "<<model_time<<")\n";
//	global_mutex.unlock();
	
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
	
	cout<<"Test - Preparando Cola de Trabajo\n";
	fill_queue(fsettings, total);
	
	double ms_preparation = timer.getMilisec();
	cout<<"Test - Preparacion terminada en "<<ms_preparation<<" ms\n";
	
	cout<<"Test - Iniciando "<<n_threads<<" threads\n";
	vector<thread> threads_list;
	for(unsigned int i = 0; i < n_threads; ++i){
//		threads_list.push_back( thread(SimultionThread, i, n_threads) );
//		threads_list.push_back( thread(SimultionThreadLocalParsing, i, total/n_threads, fsettings) );
		threads_list.push_back( thread(DummyThreadLocalParsing, i, total/n_threads, fsettings) );
	
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













