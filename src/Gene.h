#ifndef _GENE_H_
#define _GENE_H_
#include "Reference.h"

class Gene{
   private: uint32_t    _id;
            Reference*  _reference;
            double      _mutation_rate;

   public:  Gene(const uint32_t&);
            Gene(const Gene&);
            Gene(const uint32_t&,const string&);
            Gene(const uint32_t&,Reference*);

            void reference(Reference*);
            Reference* reference(void);
            uint32_t id(void) const;
            double mutation_rate(void) const;
            void mutation_rate(const double&);
            ~Gene(void);
};
#endif
