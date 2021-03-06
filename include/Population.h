#ifndef _POPULATION_H_
#define _POPULATION_H_

#include <stdint.h>
#include <vector>
#include <map>

#include "Individual.h"
#include "Pool.h"
#include "Marker.h"
#include "VirtualSequence.h"
#include "VirtualSequenceDNA.h"

using namespace std;

class Population{
   protected:
		vector<Individual> _population;
		string _name;
		mt19937 *rng_gen;
		Individual::Profile *profile;
   
   public: 
		Population();
        Population(const uint32_t &_size);
        Population(const string &_name);
        Population(const string &_name,const uint32_t &_size);
		Population(const Ploidy &_ploidy,const boost::property_tree::ptree &_fpopulation, const boost::property_tree::ptree &_fsettings);
		Population(const std::string& _name,const std::map<uint32_t,map<uint32_t,std::vector<Marker>>> &_population, const boost::property_tree::ptree &_fsettings);
		
		void setRng(mt19937 *_rng);
		
        uint32_t size();
        string name();
        void name(const string&);
        bool empty();
        Individual &top();
        Individual &at(const uint32_t);
		vector<Individual> &population(void);
		// Agrega un nuevo individuo a la poblacio, con id y basado en un cierto profile
		// Lo crea vacio y le asigna datos del pool (con Pool::regenerate), si se recibe uno
		void add(unsigned int id, Individual::Profile *_profile, Pool *pool = NULL);

        void push(Individual&);
        void pop();
        void clear();
        void shuffle();

        vector<Population*> split(const size_t&);
        void migration(Population*,const uint32_t&);
        void decrease(const uint32_t&);
        void increase(const uint32_t&);
        void merge(Population*);

        virtual ~Population();
        
};
#endif
