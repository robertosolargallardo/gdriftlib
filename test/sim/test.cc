#include <iostream>
#include <Simulator.h>

using namespace std;
random_device seed;
mt19937 rng(seed());

int main(int argc,char** argv)
{
   boost::property_tree::ptree fsettings;
   read_json(argv[1],fsettings);

   for(auto event : fsettings.get_child("scenario.events")){
      cout << event.second.get<uint32_t>("id") << endl;
   }

   /*Simulator *sim=new Simulator(fsettings);
   sim->run();

	for(auto p : sim->populations()){
		boost::property_tree::ptree findices=p->indices(1.0);
      stringstream ss;
      write_json(ss,findices);
      cout << ss.str() << endl;
	}

   delete sim;*/
   
   return(0);
}
