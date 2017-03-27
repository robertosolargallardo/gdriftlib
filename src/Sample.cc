#include "Sample.h"
Sample::Sample(void){
	;
}
Sample::Sample(const string &_name) : Population(_name){
	;
}
Sample::Sample(const string &_name, Population *_population, const uint32_t &_size) : Population(_name){
	this->_population.reserve(_size);
	_population->shuffle();
	uniform_int_distribution<> uniform(0, _population->size()-1);

	for(uint32_t i = 0; i < _size; ++i)
		this->push(_population->at(uniform(rng)));
}
Sample::Sample(const Ploidy &_ploidy, const boost::property_tree::ptree &_fpopulation, const boost::property_tree::ptree &_fsettings)
: Population(_ploidy, _fpopulation, _fsettings)
{
}
Sample::~Sample(void){
	this->_population.clear();
}
