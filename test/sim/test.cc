#include <iostream>
#include <Simulator.h>

#include "NanoTimer.h"

using namespace std;
random_device seed;
mt19937 rng(seed());
mt19937_64 rng64(seed());

int main(int argc,char** argv)
{
	cout<<"Test - Inicio\n";
	
	rng.seed(0);
	
	boost::property_tree::ptree fsettings;
	read_json(argv[1], fsettings);

	/*Population *population=new Population();
	Individual *individual=new Individual(0U,fsettings.get_child("individual"));

	population->push(individual);
	population->push(new Individual(1U,fsettings.get_child("individual")));

	population->clear();

	delete population;*/
	
	NanoTimer timer;
	Simulator sim(fsettings);
	sim.run();
	cout<<"Test - Simulator->run Terminado en "<<timer.getMilisec()<<" ms\n";
	timer.reset();

	Sample all("summary");
	map<string, Sample*> samples = sim.samples();
	for(map<string,Sample*>::iterator i = samples.begin();i != samples.end(); ++i){
		boost::property_tree::ptree findices = i->second->indices();
		stringstream ss;
		write_json(ss, findices);
		cout << ss.str() << endl;
		all.merge(i->second);
	}
	boost::property_tree::ptree findices = all.indices();
	stringstream ss;
	write_json(ss,findices);
	cout << ss.str() << endl;
	
	cout<<"Test - Estadisticos calculados en "<<timer.getMilisec()<<" ms\n";
	
	cout<<"Test - Fin (count_str: "<<VirtualSequence::count_str<<", count_int: "<<VirtualSequence::count_int<<", count_copy: "<<VirtualSequence::count_copy<<", count_mem: "<<VirtualSequence::count_mem<<", count_del: "<<VirtualSequence::count_del<<", count_del_mem: "<<VirtualSequence::count_del_mem<<", count_mut: "<<VirtualSequence::count_mut<<")\n";
	
	return(0);
}
