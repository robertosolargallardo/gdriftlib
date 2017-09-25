#ifndef _MODEL_WF_H_
#define _MODEL_WF_H_

#include "Model.h" 

#include <random>


class ModelWF : public Model{

private:
	unsigned int processDNAGenes(Population *dst, Pool *pool, Individual::Profile *profile, unsigned int plo, unsigned int chrid, unsigned int genid);
//	unsigned int processMSGenes(Population *dst, Pool *pool, Individual::Profile *profile, unsigned int plo, unsigned int chrid, unsigned int genid);
	mt19937 *rng;
	
public:	
	ModelWF();
//	ModelWF(const ModelWF &original);
	~ModelWF();
	
//	ModelWF& operator=(const ModelWF &original);
	
	void run(Population *src, Population *dst, Pool *pool, Individual::Profile *profile);

};

















#endif
