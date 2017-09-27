#include <iostream>
#include <Simulator.h>

#include "Individual.h"
#include "VirtualSequence.h"
#include "VirtualSequenceDNA.h"

#include "NanoTimer.h"
#include "Sample.h"

#include <random>

using namespace std;
random_device seed;
mt19937 rng(seed());

int main(int argc,char** argv)
{
	
	if(argc != 4){
		cout<<"\nUsage: ./test sample_size seq_len mutations\n";
		cout<<"\n";
		return 0;
	}
	
	unsigned int n_sample = atoi(argv[1]);
	unsigned int len = atoi(argv[2]);
	unsigned int n_muts = atoi(argv[3]);
	
	rng.seed(0);
	
	cout<<"Test - Inicio\n";
	NanoTimer timer;
	
	Sample sample("Test Sample");
	// Notar que hay que mantener las secuencias en un pool, independiente de la poblacion
	// Ni la poblacion, ni los individuos borran secuencias, eso es trabajo del pool
	vector<VirtualSequence*> pool;
	
	vector<unsigned int> gens;
	gens.push_back(1);
	Individual::Profile profile(1, 1, gens);
	
	cout<<"Test - Creando Secuencia Original\n";
	string str(len, 'A');
	VirtualSequence *seq_original = new VirtualSequenceDNA(str);
	pool.push_back(seq_original);
	cout<<"Test - Agregando Individuo\n";
	sample.add(0, &profile);
	cout<<"Test - Asociando secuencia\n";
	sample.at(0).setGene(0, 0, 0, seq_original);
	
	cout<<"Test - Iterando\n";
	for(unsigned int i = 1; i < n_sample; ++i){
		cout<<"Test - Creando Secuencia "<<i<<"\n";
		VirtualSequence *seq = seq_original->clone();
		// Mutar seq
		for(unsigned int j = 0; j < n_muts; ++j){
			seq->mutate(&rng);
		}
		pool.push_back(seq);
		cout<<"Test - Agregando Individuo\n";
		sample.add(i, &profile);
		cout<<"Test - Asociando secuencia\n";
		sample.at(i).setGene(0, 0, 0, seq);
	}
	stringstream ss;
	
	boost::property_tree::ptree findices = sample.indices();
	write_json(ss, findices);
	cout << ss.str() << endl;
	
	findices = sample.indices_seq();
	write_json(ss, findices);
	cout << ss.str() << endl;
	
	// Borrado de la poblacion
	sample.deleteData();
	// Borrado del pool
	for(unsigned int i = 0; i < pool.size(); ++i){
		if(pool[i] != NULL){
			delete pool[i];
		}
	}
	pool.clear();
	
	cout<<"Test - Fin (count_str: "<<VirtualSequence::count_str<<", count_int: "<<VirtualSequence::count_int<<", count_copy: "<<VirtualSequence::count_copy<<", count_mem: "<<VirtualSequence::count_mem<<", count_del: "<<VirtualSequence::count_del<<", count_del_mem: "<<VirtualSequence::count_del_mem<<", count_mut: "<<VirtualSequence::count_mut<<")\n";
	
	return(0);
	
}




















