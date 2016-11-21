#ifndef _POOL_H_
#define _POOL_H_
#include <map>
#include <stdint.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Reference.h"
#include "Individual.h"

extern random_device seed;
extern mt19937 rng;

class Pool{
   private: uint32_t _size;
            vector<Reference*>** _pool;
            boost::property_tree::ptree _findividual;

   public:  Pool(const boost::property_tree::ptree&);

            void populate(const uint32_t&,const uint32_t&,const uint32_t&,const uint32_t&);
            Individual* generate(const uint32_t&);
            void release(void);
            void decrease_all(void);
            Reference* push(const uint32_t&,Reference*);
            uint32_t size(void);
      
            ~Pool(void);
};
#endif
