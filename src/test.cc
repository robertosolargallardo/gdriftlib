#include <iostream>
#include "Simulator.h"

using namespace std;
random_device seed;
mt19937 rng(seed());

int main(int argc,char** argv)
{

	/*Bitset* b=new Bitset(18,100001);
	cout << b->to_string() << endl;
	Bitset* c=new Bitset(*b);
	cout << c->to_string() << endl;

	Reference* r=new Reference(18,100001);
	cout << r->to_string() << endl;
	Reference* s=new Reference(*r);
	cout << s->to_string() << endl;

	delete b;
	delete c;
	delete r;
	delete s;*/
   boost::property_tree::ptree fsettings;
   read_json(argv[1],fsettings);

   Simulator *sim=new Simulator(fsettings);
   sim->run();

	for(auto p : sim->populations()){
		boost::property_tree::ptree findices=p->indices(1.0);
      stringstream ss;
      write_json(ss,findices);
      cout << ss.str() << endl;
	}

   delete sim;
   
   return(0);
}
