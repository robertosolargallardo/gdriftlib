#include <iostream>
#include <Simulator.h>

using namespace std;
random_device seed;
mt19937 rng(seed());

int main(int argc,char** argv)
{
   boost::property_tree::ptree fpopulations;
   read_json(argv[1],fpopulations);


   Population* all=new Population("summary");
   for(auto& population : fpopulations.get_child("populations")){
      Population* p=new Population(Ploidy(fpopulations.get<uint32_t>("ploidy")),population.second);
		boost::property_tree::ptree findices=p->indices(1.0);
      stringstream ss;
      write_json(ss,findices);
      cout << ss.str() << endl;
      all->merge(p);
      delete p;
   }
	boost::property_tree::ptree findices=all->indices(1.0);
   stringstream ss;
   write_json(ss,findices);
   cout << ss.str() << endl;
   delete all;
  /* Simulator *sim=new Simulator(fpopulations);

	for(auto p : sim->populations()){
		boost::property_tree::ptree findices=p->indices(1.0);
      stringstream ss;
      write_json(ss,findices);
      cout << ss.str() << endl;
	}

   delete sim; */
   
   return(0);
}
