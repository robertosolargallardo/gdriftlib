#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_
#include <map>

#include "EventList.h"
#include "Population.h"
#include "Sample.h"
#include "Pool.h"
#include "Individual.h"

#include "Model.h"
#include "ModelWF.h"

// Esto podria ser de un factory de Model
// Por ahora, ese trabajo esta en el Simulator
enum MODEL_CODE { WRIGHTFISHER = 0 };

using namespace std;

class Simulator{
   private: map<string,tuple<Population*,Population*>> populations;
			map<string,Sample*> _samples;

            boost::property_tree::ptree _fsettings;
            Pool *pool;
            EventList *evlist;
            Individual::Profile *profile;
            Model *model;
		

   public:  Simulator(const boost::property_tree::ptree&);
            ~Simulator(void);
         
			map<string,Sample*> &samples();
            void run(void);
};
#endif
