#ifndef _MODEL_H_
#define _MODEL_H_

#include "Population.h" 
#include "Individual.h" 
#include "Pool.h" 
#include "VirtualSequence.h" 
#include "NanoTimer.h" 

#include <random>

class Model{
private:
public:
	Model();
	virtual ~Model();
	
	virtual void run(Population *src, Population *dst, Pool *pool, Individual::Profile *profile);	
	
};


#endif
