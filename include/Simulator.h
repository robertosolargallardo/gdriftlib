#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_
#include <map>

#include "EventList.h"
#include "Population.h"
#include "Sample.h"
#include "Pool.h"
#include "Model.h"
#include "Individual.h"

using namespace std;

class Simulator{
   private: map<string,tuple<Population*,Population*>> _populations;
			map<string,Sample*> _samples;

            boost::property_tree::ptree _fsettings;
            Pool* _pool;
            EventList* _evlist;
            Individual::Profile profile;
		

   public:  Simulator(const boost::property_tree::ptree&);
            ~Simulator(void);
         
			map<string,Sample*> samples(void);
            void run(void);
};
#endif
