#ifndef _POOL_H_
#define _POOL_H_

#include <map>
#include <stdint.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Individual.h"
#include "VirtualSequence.h"
#include "VirtualSequenceDNA.h"
#include "NanoTimer.h"

extern random_device seed;
extern mt19937 rng;
class Pool{
   private: map<pair<uint32_t,uint32_t>,vector<VirtualSequence*>> _pool;
            boost::property_tree::ptree _findividual;

   public:  Pool(const boost::property_tree::ptree&);

            void populate(const uint32_t&,const uint32_t&,const uint32_t&,const uint32_t&,const uint32_t&);
            Individual* generate(const uint32_t &id, const Individual::Profile &profile);
            // Borra el contenido (con clear) y regenera un individuo
            void regenerate(Individual *individual);
            void release(void);
            void decrease_all(void);
            VirtualSequence* push(const uint32_t&,const uint32_t&,VirtualSequence*);
            uint32_t size(void);
      		
            ~Pool(void);
			map<pair<uint32_t,uint32_t>, vector<VirtualSequence*>> reuse;
};
#endif
