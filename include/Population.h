#ifndef _POPULATION_H_
#define _POPULATION_H_

#include <stdint.h>
#include <vector>
#include <map>

#include "Individual.h"
#include "VirtualSequence.h"

// De momento conservo rng externo
// La idea es sacarlo cuando ya nadie lo use
// El constructor referencia el externo si no recibe un objeto adecuado 
// extern random_device seed;
extern mt19937 rng;

using namespace std;

class Population{
   protected:
		vector<Individual*> _population;
		string _name;
		mt19937 *rng_gen;
		Individual::Profile *profile;
   
   public: 
		Population(void);
        Population(const uint32_t &_size);
        Population(const string &_name);
        Population(const string &_name,const uint32_t &_size);
		Population(const Ploidy &_ploidy,const boost::property_tree::ptree &_fpopulation, const boost::property_tree::ptree &_fsettings);
		
		void setRng(mt19937 *_rng);
		
        uint32_t size(void);
        string name(void);
        void name(const string&);
        bool empty(void);
        Individual* top(void);
        Individual* at(const uint32_t&);
		vector<Individual*> population(void);

        void push(Individual*);
        void pop(void);
        void clear(void);
        void shuffle(void);

        vector<Population*> split(const size_t&);
        void migration(Population*,const uint32_t&);
        void decrease(const uint32_t&);
        void increase(const uint32_t&);
        void merge(Population*);

        virtual ~Population(void);
        
//        Individual::Profile *getProfile(){
//        	return profile;
//        }
};
#endif
