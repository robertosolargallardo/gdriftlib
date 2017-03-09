#include <iostream>
#include <Simulator.h>

using namespace std;
random_device seed;
mt19937 rng(seed());

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

   Simulator *sim=new Simulator(fsettings);
   sim->run();

	/*for(auto p : sim->populations()){
		boost::property_tree::ptree findices=p->indices(1.0);
      stringstream ss;
      write_json(ss,findices);
      cout << ss.str() << endl;
	}*/

   delete sim;
   
   return(0);
}
