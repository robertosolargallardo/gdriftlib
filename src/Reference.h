#ifndef _REFERENCE_H_
#define _REFERENCE_H_
#include "Bitset.h"
extern random_device seed;
extern mt19937 rng;

class Reference:public Bitset{
   private: uint32_t _count;
                     
   public:  Reference(const uint32_t&,const uint32_t&);
            Reference(const Reference&);
            void increase(void);
            void decrease(void);
            uint32_t count(void) const;
            void mutate(void);
            ~Reference(void);
};
#endif
