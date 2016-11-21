#include "Gene.h"
Gene::Gene(const uint32_t &_id){
   this->_id=_id;
   this->_ref=nullptr;
}
Gene::Gene(const Gene &_gene){
   this->_id=_gene._id;
   this->_mutation_rate=_gene._mutation_rate;
   this->_ref=_gene._ref;
}
void Gene::ref(Reference* _ref){
   this->_ref=_ref;
   this->_ref->increase();
}
uint32_t Gene::id(void) const{
   return(this->_id);
}
Reference* Gene::ref(void){
   return(this->_ref);
}
double Gene::mutation_rate(void) const{
   return(this->_mutation_rate);
}
void Gene::mutation_rate(const double &_mutation_rate){
   this->_mutation_rate=_mutation_rate;
}
Gene::~Gene(void){
   this->_ref->decrease();
   this->_ref=nullptr;
}
