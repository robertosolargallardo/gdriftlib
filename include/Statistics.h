#ifndef _STATISTICS_H_
#define _STATISTICS_H_
#include "Sample.h"

class Statistics{
	/*
	map<uint32_t,map<uint32_t,vector<string>>> sequences;
	map<uint32_t,map<uint32_t,vector<VirtualSequence*>>> sequences; 
	*/
	public:	Statistics(void);
				Statistics(const map<string,Sample*>&);
				~Statistics(void);
};

#endif
