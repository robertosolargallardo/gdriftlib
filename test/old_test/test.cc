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
	
	// Pruebas preliminares de datos 
	/*
	cout<<"Test - Pruebas preliminares de datos\n";
	
	VirtualSequence v_seq("AACACGTTT", 9);
	for(unsigned int i = 0; i < 12; ++i){
		cout<<"v_seq["<<i<<"]: "<<v_seq.at(i)<<"\n";
	}
	
	Bitset seq(14, 2572888);
	string seq_str = seq.to_string();
	cout<<"Test - Bitset(14, 2572888): "<<seq_str<<"\n";
	
	VirtualSequence v_seq2(14, 2572888);
	string seq2_str = v_seq2.to_string();
	cout<<"Test - VirtualSequence(14, 2572888): "<<seq2_str<<"\n";
	
	cout<<"Test - Pruebas preliminares terminadas\n-----  -----\n";
	*/
	
	/*
	// Prueba de correctitud de VirtualSequence
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
	*/
	
	
	
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
	
	cout<<"Test - Fin (const_str: "<<VirtualSequence::count_str<<", const_int: "<<VirtualSequence::count_int<<", const_copy: "<<VirtualSequence::count_copy<<", const_del: "<<VirtualSequence::count_del<<", const_mut: "<<VirtualSequence::count_mut<<", const_mem: "<<VirtualSequence::count_mem<<")\n";
//	cout<<"Test - Fin (const_str: "<<Bitset::count_str<<", const_int: "<<Bitset::count_int<<", const_copy: "<<Bitset::count_copy<<", const_del: "<<Bitset::count_del<<", const_mut: "<<Bitset::count_mut<<")\n";
	return(0);
	
	
	
}
