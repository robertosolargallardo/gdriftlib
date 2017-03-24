#include <iostream>
#include <Simulator.h>

#include "NanoTimer.h"

using namespace std;
random_device seed;
mt19937 rng(seed());
mt19937_64 rng64(seed());

int main(int argc,char** argv)
{
   boost::property_tree::ptree fsettings;
   read_json(argv[1],fsettings);

   /*Population *population=new Population();
   Individual *individual=new Individual(0U,fsettings.get_child("individual"));

   population->push(individual);
   population->push(new Individual(1U,fsettings.get_child("individual")));

   population->clear();

   delete population;*/
   

   cout<<"Test - Inicio (rng.max: "<<rng.max()<<", rng64.max: "<<rng64.max()<<")\n";
	
   NanoTimer timer;
   Simulator *sim=new Simulator(fsettings);
   sim->run();
   cout<<"Test - Simulator->run Terminado en "<<timer.getMilisec()<<" ms\n";

	Sample *all=new Sample("summary");
	map<string,Sample*> samples=sim->samples();
	for(map<string,Sample*>::iterator i=samples.begin();i!=samples.end();i++){
		boost::property_tree::ptree findices=i->second->indices();
      stringstream ss;
      write_json(ss,findices);
      cout << ss.str() << endl;
		all->merge(i->second);
	}
	boost::property_tree::ptree findices=all->indices();
   stringstream ss;
   write_json(ss,findices);
   cout << ss.str() << endl;
	delete all;

   delete sim;
   
	cout<<"Test - Fin (const_str: "<<VirtualSequence::count_str<<", const_int: "<<VirtualSequence::count_int<<", const_copy: "<<VirtualSequence::count_copy<<", const_mem: "<<VirtualSequence::count_mem<<", const_del: "<<VirtualSequence::count_del<<", const_del_mem: "<<VirtualSequence::count_del_mem<<", const_mut: "<<VirtualSequence::count_mut<<")\n";
	
   return(0);
}
