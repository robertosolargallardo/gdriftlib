#ifndef _BITSET_H_
#define _BITSET_H_
#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <limits.h>
using namespace std;

#define N_NUCLEOTIDES 4
enum Nucleotide{A=0,C=1,G=2,T=3};

class Bitset{
   protected:  char*  _data;
               uint32_t _nucleotides;
            
   public:  Bitset(const string&);
            Bitset(const Bitset&);
            Bitset(const uint32_t&,const uint32_t&);
            string to_string(void);
            virtual ~Bitset(void);

            bool operator==(const Bitset&);
};
#endif
