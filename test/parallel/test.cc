#include <iostream>
#include <Simulator.h>

#include "NanoTimer.h"

using namespace std;

// Soporte para extern rng
// Esto debe ser reemplazado por una instancia de mt19937 en el thread
random_device seed;
mt19937 rng(seed());

// Mutex global para controlar acceso a cout u otros objetos compartidos
// Este mutex debiera pertenecer al Controller en el modelo real
mutex global_mutex;

void SimultionThread(unsigned int pid, const boost::property_tree::ptree &fsettings) {
	
	// Lock completo para una version secuencial de prueba
//	lock_guard<mutex> lock(global_mutex);
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Inicio\n";
	global_mutex.unlock();
	
	NanoTimer timer;
	Simulator sim(fsettings);
	sim.run();
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Simulator->run Terminado en "<<timer.getMilisec()<<" ms\n";
	global_mutex.unlock();

	Sample all("summary");
	map<string, Sample*> samples = sim.samples();
	for(map<string, Sample*>::iterator i = samples.begin(); i != samples.end(); ++i){
		boost::property_tree::ptree findices = i->second->indices();
		stringstream ss;
		write_json(ss, findices);
		
		global_mutex.lock();
//		cout << ss.str() << endl;
		global_mutex.unlock();
		
		all.merge(i->second);
	}
	boost::property_tree::ptree findices = all.indices();
	stringstream ss;
	write_json(ss, findices);
	
	global_mutex.lock();
//	cout << ss.str() << endl;
	global_mutex.unlock();
	
}

int main(int argc,char** argv)
{

	if(argc != 3){
		cout<<"\nUsage: ./test json_file n_threads\n";
		cout<<"\n";
		return 0;
	}

	boost::property_tree::ptree fsettings;
	read_json(argv[1],fsettings);
	unsigned int n_threads = atoi(argv[2]);
	
	cout<<"Test - Inicio (Preparando "<<n_threads<<" threads)\n";
	NanoTimer timer;
	
	vector<std::thread> threads_list;
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list.push_back( std::thread(SimultionThread, i, fsettings) );
	}
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list[i].join();
	}
	
	cout<<"Test - Fin ("<<timer.getMilisec()<<" ms, count_str: "<<VirtualSequence::count_str<<", count_copy: "<<VirtualSequence::count_copy<<", count_mem: "<<VirtualSequence::count_mem<<", count_del: "<<VirtualSequence::count_del<<", count_del_mem: "<<VirtualSequence::count_del_mem<<", count_mut: "<<VirtualSequence::count_mut<<")\n";
	
	return(0);
}













