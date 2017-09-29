#include "Sample.h"
Sample::Sample(void){
	;
}
Sample::Sample(const string &_name) : Population(_name){
	;
}
Sample::Sample(const string &_name, Population *_population, const uint32_t &_size) : Population(_name){
   static thread_local std::mt19937 rng;
	this->_population.reserve(_size);
	// Ralizar shuffle de la poblacion padre NO ES CORRECTO
	// Esto requiere una copia local del vector, luego shuffle, y luego SACAR el resto (dejando al padre intacto)
	_population->shuffle();
	uniform_int_distribution<> uniform(0, _population->size()-1);

	for(uint32_t i = 0; i < _size; ++i)
		this->push(_population->at(uniform(rng)));
}
Sample::Sample(const std::string &_name,const std::map<uint32_t,map<uint32_t,std::vector<Marker>>> &_population, const boost::property_tree::ptree &_fsettings):Population(_name,_population,_fsettings){
	;
}
Sample::Sample(const Ploidy &_ploidy, const boost::property_tree::ptree &_fpopulation, const boost::property_tree::ptree &_fsettings)
: Population(_ploidy, _fpopulation, _fsettings)
{
}
Sample::~Sample(void){
	this->_population.clear();
}
