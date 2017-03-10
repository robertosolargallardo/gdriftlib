#include <iostream>
#include <Simulator.h>

#include "VirtualSequence.h"
#include "Reference.h"
#include "Bitset.h"
#include "NanoTimer.h"

using namespace std;
random_device seed;
mt19937 rng(seed());

int main(int argc,char** argv)
{
	/*
	// Prueba de correctitud de VirtualSequence
	rng.seed(0);
	Reference *ref = NULL;
	VirtualSequence *seq = NULL;
	VirtualSequence *seq_copy = NULL;
	string s1, s2;
//	string ref_text = "AACACGTTTAACACGTTTAACACGTTT";
	string ref_text = "ACTGTT";
	uniform_int_distribution<> dist(0, 100);
	
	// Constructor con texto de referencia
	cout<<"Constructor de Texto\n";
	ref = new Reference(ref_text);
	seq = new VirtualSequence(ref_text);
	s1 = ref->to_string();
	s2 = seq->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	if( s1.compare(s2) != 0 ){
		cout<<"Error texto\n";
	}
	cout<<"-----\n";
	delete ref;
	delete seq;
	
	// Constructor con int
	cout<<"Constructor de Int\n";
	ref = new Reference(20, 123456);
	seq = new VirtualSequence(20, 123456);
	s1 = ref->to_string();
	s2 = seq->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	if( s1.compare(s2) != 0 ){
		cout<<"Error int\n";
	}
	cout<<"-----\n";
	delete ref;
	delete seq;
	
	// Constructor de copia
	cout<<"Constructor de Copia\n";
	seq = new VirtualSequence(ref_text);
	seq_copy = new VirtualSequence(*seq);
	s1 = seq->to_string();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	if( s1.compare(s2) != 0 ){
		cout<<"Error Copia\n";
	}
	cout<<"-----\n";
	delete seq_copy;
	cout<<"Mutando seq\n";
	seq->mutate();
	seq_copy = new VirtualSequence(*seq);
	s1 = seq->to_string();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	if( s1.compare(s2) != 0 ){
		cout<<"Error Copia\n";
	}
	cout<<"-----\n";
	delete seq_copy;
	cout<<"Mutando seq\n";
	seq->mutate();
	seq_copy = new VirtualSequence(*seq);
	s1 = seq->to_string();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	if( s1.compare(s2) != 0 ){
		cout<<"Error Copia\n";
	}
	cout<<"-----\n";
	delete seq_copy;
	cout<<"Mutando seq\n";
	seq->mutate();
	seq_copy = new VirtualSequence(*seq);
	s1 = seq->to_string();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	if( s1.compare(s2) != 0 ){
		cout<<"Error Copia\n";
	}
	cout<<"-----\n";
	delete seq_copy;
	
	
	// Constructor de copia
	cout<<"Constructor de Copia y mutaciones multiples\n";
	seq_copy = new VirtualSequence(*seq);
	cout<<"Mutando seq_copy\n";
	seq_copy->mutate();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	cout<<"-----\n";
	cout<<"Mutando seq_copy\n";
	seq_copy->mutate();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	cout<<"-----\n";
	cout<<"Mutando seq_copy\n";
	seq_copy->mutate();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	cout<<"-----\n";
	cout<<"Mutando seq_copy\n";
	seq_copy->mutate();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	cout<<"-----\n";
	cout<<"Mutando seq_copy\n";
	seq_copy->mutate();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	cout<<"-----\n";
	cout<<"Mutando seq_copy\n";
	seq_copy->mutate();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	cout<<"-----\n";
	cout<<"Mutando seq_copy\n";
	seq_copy->mutate();
	s2 = seq_copy->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"s2: "<<s2<<" ("<<s2.length()<<")\n";
	cout<<"-----\n";
	delete seq_copy;
	
	// Mutacion por mascara
	cout<<"Mutacion por mascara\n";
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	seq->mutateBitMask(6);
	s1 = seq->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"-----\n";
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	seq->mutateBitMask(24582);
	s1 = seq->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"-----\n";
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	seq->mutateBitMask(24582, 1);
	s1 = seq->to_string();
	cout<<"s1: "<<s1<<" ("<<s1.length()<<")\n";
	cout<<"-----\n";
	
	cout<<"Seq Original: \n";
	seq->printData();
	s1 = seq->to_string();
	cout<<"Texto: \""<<s1<<"\" ("<<s1.length()<<")\n";
	cout<<"-----\n";
	
	cout<<"Mutando\n";
//	seq->mutate();
//	seq->mutate();
	seq->mutateInsert(1, 'K');
	seq->mutateInsert(10, 'L');
	seq->mutateInsert(3, 'M');
	cout<<"Seq Final: \n";
	seq->printData();
	s1 = seq->to_string();
	cout<<"Texto: \""<<s1<<"\" ("<<s1.length()<<")\n";
	cout<<"-----\n";
	
	cout<<"Replicando insert\n";
	seq->mutateInsert(0, 'N');
	seq->mutateInsert(3, 'O');
	cout<<"Seq Final: \n";
	seq->printData();
	s1 = seq->to_string();
	cout<<"Texto: \""<<s1<<"\" ("<<s1.length()<<")\n";
	cout<<"-----\n";
	*/
	
	// Prueba de espacio en ram
	// Estaticamente mido tamaño en varios casos, luego pruebo la carga y un sleep para verificar el uso efectivo
	if(argc != 5){
		cout<<"\nModo de Uso: test input n_objects str_size n_muts\n";
		cout<<"\n";
		return 0;
	}

	cout<<"Calculo estatico de memoria\n";
	cout<<"sizeof(VirtualSequence) "<<sizeof(VirtualSequence)<<"\n";
	unsigned int n_objects = atoi(argv[2]);
	unsigned int str_size = atoi(argv[3]);
	unsigned int n_mutations = atoi(argv[4]);
	VirtualSequence **pool = new VirtualSequence*[n_objects];
	VirtualSequence *seq = NULL;
	string str(str_size, 'A');
	cout<<"Creando pool de "<<n_objects<<" secuencias de largo "<<str_size<<" con "<<n_mutations<<" mutations\n";
	unsigned int n_batch = 5;
	unsigned int batch_size = n_objects / n_batch;
	for(unsigned int i = 0; i < n_batch; ++i){
		cout<<"Agregando "<<batch_size<<" secuencias\n";
		for(unsigned int j = 0; j < batch_size; ++j){
			seq = new VirtualSequence(str);
			for(unsigned int k = 0; k < n_mutations; ++k){
				seq->mutate();
			}
			pool[ i*batch_size + j ] = seq;
		}
		cout<<"Esperando...\n";
		string str_cin;
		cin>>str_cin;
	}
	cout<<"Borrando\n";
	for(unsigned int i = 0; i < n_objects; ++i){
		delete pool[i];
	}
	delete [] pool;
	cout<<"Terminando\n";
	
//	string str(10, 'A');
//	VirtualSequence *seq = new VirtualSequence(str);
//	cout<<"sizeof(str) "<<sizeof(str)<<"\n";
//	cout<<"sizeof(seq) "<<sizeof(*seq)<<"\n";
//	delete seq;
//	str = string(100, 'A');
//	seq = new VirtualSequence(str);
//	cout<<"sizeof(str) "<<sizeof(str)<<"\n";
//	cout<<"sizeof(seq) "<<sizeof(*seq)<<"\n";
//	delete seq;
//	str = string(1000, 'A');
//	seq = new VirtualSequence(str);
//	cout<<"sizeof(str) "<<sizeof(str)<<"\n";
//	cout<<"sizeof(seq) "<<sizeof(*seq)<<"\n";
//	delete seq;
//	cout<<"-----\n";
//	str = string(1000, 'A');
//	seq = new VirtualSequence(str);
//	for(unsigned int i = 0; i < 100; ++i){
//		seq->mutate();
//	}
//	cout<<"sizeof(str) "<<sizeof(str)<<"\n";
//	cout<<"sizeof(seq) con 100 mutaciones "<<sizeof(*seq)<<"\n";
	
	
	/*
	rng.seed(0);
	
	cout<<"Test - Inicio\n";
	NanoTimer timer;
	
	boost::property_tree::ptree fsettings;
	cout<<"Test - read_json...\n";
	read_json(argv[1],fsettings);

	cout<<"Test - new Simulator...\n";
	Simulator *sim = new Simulator(fsettings);
	cout<<"Test - sim->run...\n";
	sim->run();
	cout<<"Test - sim->run terminado en "<<timer.getMilisec()<<" ms\n";
	timer.reset();
	
	cout<<"Test - sim->populations...\n";
	for(auto p : sim->populations()){
		boost::property_tree::ptree findices=p->indices(1.0);
		stringstream ss;
		write_json(ss,findices);
		cout << ss.str() << endl;
	}
	cout<<"Test - populations terminado en "<<timer.getMilisec()<<" ms\n";
	
	delete sim;
	
	cout<<"Test - Fin (const_str: "<<VirtualSequence::count_str<<", const_int: "<<VirtualSequence::count_int<<", const_copy: "<<VirtualSequence::count_copy<<", const_mem: "<<VirtualSequence::count_mem<<", const_del: "<<VirtualSequence::count_del<<", const_del_mem: "<<VirtualSequence::count_del_mem<<", const_mut: "<<VirtualSequence::count_mut<<")\n";
//	cout<<"Test - Fin (const_str: "<<Bitset::count_str<<", const_int: "<<Bitset::count_int<<", const_copy: "<<Bitset::count_copy<<", const_del: "<<Bitset::count_del<<", const_mut: "<<Bitset::count_mut<<")\n";
	return(0);
	*/
	
	
}
