#ifndef _CHROMOSOME_H_
#define _CHROMOSOME_H_
#include "Gene.h"
#include <stdint.h>
class Gene;

class Chromosome{
   private: uint32_t _id;
            uint32_t _n_genes;
            Gene**   _genes;
            
   public:  Chromosome(const uint32_t&,const uint32_t&);
            Chromosome(const Chromosome&);
            uint32_t id(void) const;
            uint32_t n_genes(void) const;
            Gene*& gene(const uint32_t&);
            ~Chromosome(void);
};
#endif
