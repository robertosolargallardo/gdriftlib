#include <iostream>
#include "Simulator.h"

using namespace std;
random_device seed;
mt19937 rng(seed());

int main(int argc,char** argv)
{
   
   boost::property_tree::ptree fsettings;
   read_json(argv[1],fsettings);

   Simulator *sim=new Simulator(fsettings);
   sim->run();

   delete sim;

   /*boost::property_tree::ptree fpopulations;
   read_json(argv[1],fpopulations);
   
   for(auto population : fpopulations.get_child("populations")){
      Population *p=new Population(Ploidy(fpopulations.get<int>("ploidy")),population.second);
      boost::property_tree::ptree findices=p->indices(1.0);
      stringstream ss;
      write_json(ss,findices);
      cout << ss.str() << endl;
      delete p;
   }*/
   
   return(0);
}
