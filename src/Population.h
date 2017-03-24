#ifndef _POPULATION_H_
#define _POPULATION_H_

#include <stdint.h>
#include <vector>
#include <map>
#include "Individual.h"
#include "VirtualSequence.h"

extern random_device seed;
extern mt19937 rng;

using namespace std;

class Population{
   protected:	vector<Individual*> _population;
            	string _name;
   
   public:  Population(void);
            Population(const uint32_t&);
            Population(const string&);
            Population(const string&,const uint32_t&);
            Population(const Ploidy&,const boost::property_tree::ptree&,const boost::property_tree::ptree&);
 
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
};
#endif
