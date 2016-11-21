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
   
   return(0);
}
