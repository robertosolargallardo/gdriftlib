#ifndef _REFERENCE_H_
#define _REFERENCE_H_
#include "Bitset.h"
extern random_device seed;
extern mt19937 rng;

class Reference:public Bitset{
   private: uint32_t _count;
            bool     _read_only;
                     
   public:  Reference(const uint32_t&,const uint32_t&,const bool &_read_only=true);
            Reference(const string&,const bool &_read_only=true);
            Reference(const Reference&);
            void increase(void);
            void decrease(void);
            uint32_t count(void) const;
            void mutate(void);
            bool read_only(void) const;
            ~Reference(void);
};
#endif
