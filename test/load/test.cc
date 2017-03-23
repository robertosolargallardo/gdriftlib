#include <iostream>
#include <Simulator.h>

using namespace std;
random_device seed;
mt19937 rng(seed());

int main(int argc,char** argv)
{
//	cout<<"Main - Inicio\n";
	
   boost::property_tree::ptree fpopulations;
   read_json(argv[1], fpopulations);

//	cout<<"Main - Creando Population all\n";
   Population* all=new Population("summary");
   for(auto& population : fpopulations.get_child("populations")){
//		cout<<"Main - Population p\n";
//      Population* p=new Population(Ploidy(fpopulations.get<uint32_t>("ploidy")),population.second);
      Population* p=new Population(Ploidy(fpopulations.get<uint32_t>("ploidy")),population.second, fpopulations);
		boost::property_tree::ptree findices=p->indices(1.0);
      stringstream ss;
      write_json(ss,findices);
      cout << ss.str() << endl;
//		cout<<"Main - Merge\n";
      all->merge(p);
//		cout<<"Main - Borrando p\n";
      delete p;
   }
	boost::property_tree::ptree findices=all->indices(1.0);
   stringstream ss;
   write_json(ss,findices);
   cout << ss.str() << endl;
//	cout<<"Main - Borrando all\n";
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
