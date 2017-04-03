#ifndef _MODEL_WF_H_
#define _MODEL_WF_H_

#include "Population.h" 
#include "Pool.h" 
#include "Individual.h" 
#include "VirtualSequence.h" 
#include "NanoTimer.h" 

//Population *dst, Pool *pool, Individual::Profile *profile

#include <random>

//namespace model{
//   template<Model M,Ploidy P>
//   void run(Population* &_src,Population* &_dst,Pool* &_pool, Individual::Profile *profile);
//}



class ModelWF{

private:
	
public:	
	ModelWF(){}
//	ModelWF(const ModelWF &original){}
	~ModelWF(){}
	
//	ModelWF& operator=(const ModelWF &original){
//		if (this != &original){
//		
//		}
//		return *this;
//	}
	
	void run(Population *src, Population *dst, Pool *pool, Individual::Profile *profile);

};

















#endif
