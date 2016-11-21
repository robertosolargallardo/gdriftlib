#ifndef _GENE_H_
#define _GENE_H_
#include "Reference.h"

class Gene{
   private: uint32_t    _id;
            Reference*  _ref;
            double      _mutation_rate;

   public:  Gene(const uint32_t&);
            Gene(const Gene&);

            void ref(Reference*);
            Reference* ref(void);
            uint32_t id(void) const;
            double mutation_rate(void) const;
            void mutation_rate(const double&);
            ~Gene(void);
};
#endif
