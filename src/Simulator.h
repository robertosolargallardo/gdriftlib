#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_
#include <map>
#include "EventList.h"
#include "Population.h"
#include "Pool.h"
#include "Model.h"
using namespace std;

class Simulator{
   private: map<string,tuple<Population*,Population*>> _populations;
            boost::property_tree::ptree _fsettings;
            Pool* _pool;
            EventList* _evlist;

   public:  Simulator(const boost::property_tree::ptree&);
            ~Simulator(void);
         
            vector<Population*> populations(void);
            void run(void);
};
#endif
