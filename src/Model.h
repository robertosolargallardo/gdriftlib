#ifndef _MODEL_H_
#define _MODEL_H_
#include "Population.h" 
#include "Individual.h" 
#include "Pool.h" 

enum Model{WRIGHTFISHER=0};

namespace model{
   template<Model M,Ploidy P>
   void run(Population* &_src,Population* &_dst,Pool* &_pool);
}
#endif
