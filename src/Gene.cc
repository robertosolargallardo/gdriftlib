#include "Gene.h"
Gene::Gene(const uint32_t &_id){
   this->_id=_id;
   this->_mutation_rate=0.0;
   this->_reference=nullptr;
}
//Gene::Gene(const uint32_t &_id,Reference* _reference){
Gene::Gene(const uint32_t &_id,VirtualSequence* _reference){
   this->_id=_id;
   this->_mutation_rate=0.0;
   this->_reference=_reference;
   this->_reference->increase();
}
Gene::Gene(const Gene &_gene){
   this->_id=_gene._id;
   this->_mutation_rate=_gene._mutation_rate;
   this->_reference=_gene._reference;
	this->_reference->increase();
}
//void Gene::reference(Reference* _reference){
void Gene::reference(VirtualSequence* _reference){
	// Creo que aqui falta reducir el contador de la referencia anterior
//	if(this->_reference != nullptr){
//		this->_reference->decrease();
//	}
   this->_reference = _reference;
   this->_reference->increase();
}
uint32_t Gene::id(void) const{
   return(this->_id);
}
//Reference* Gene::reference(void){
VirtualSequence* Gene::reference(void){
   return(this->_reference);
}
double Gene::mutation_rate(void) const{
   return(this->_mutation_rate);
}
void Gene::mutation_rate(const double &_mutation_rate){
   this->_mutation_rate=_mutation_rate;
}
Gene::~Gene(void){
   if(this->_reference!=nullptr){
      this->_reference->decrease();
      if(!this->_reference->read_only())
         delete this->_reference;
	   this->_reference=nullptr;
   }
}
void Gene::clear(void){
	if(_reference != nullptr){
		_reference->decrease();
	}
   this->_reference=nullptr;
}




