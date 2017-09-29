#include <iostream>
#include <Simulator.h>

#include "NanoTimer.h"

using namespace std;

void run(boost::property_tree::ptree fsettings){
	std::chrono::steady_clock::time_point start,end;
	start=std::chrono::steady_clock::now();
	Simulator sim(fsettings);
	sim.run();
	end=std::chrono::steady_clock::now();
	std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start)).count() << std::endl;
}

int main(int argc,char** argv)
{
   int num_threads=std::stoi(argv[2]);
	
	boost::property_tree::ptree fsettings[num_threads];

   for(int i=0;i<num_threads;++i)
	   read_json(argv[1],fsettings[i]);
  
   std::thread t[num_threads];

   for(int i=0;i<num_threads;++i) 
      t[i]=std::thread(run,fsettings[i]);
   for(int i=0;i<num_threads;++i)
      t[i].join();

	return(0);
}
