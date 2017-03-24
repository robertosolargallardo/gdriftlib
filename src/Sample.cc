#include "Sample.h"
Sample::Sample(void){
	;
}
Sample::Sample(const string &_name):Population(_name){
	;
}
Sample::Sample(const string &_name,Population* _population,const uint32_t &_size):Population(_name){
	this->_population.reserve(_size);
	_population->shuffle();
	uniform_int_distribution<> uniform(0U,_population->size()-1U);

	for(uint32_t i=0U;i<_size;i++)
		this->push(_population->at(uniform(rng)));
}
Sample::~Sample(void){
	this->_population.clear();
}
