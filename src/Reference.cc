#include "Reference.h"
Reference::Reference(const Reference &_reference):Bitset(_reference){
   this->_count=0U;
   this->_read_only=_reference._read_only;
}
Reference::Reference(const string &_sequence,const bool &_read_only):Bitset(_sequence){
   this->_count=0U;
   this->_read_only=_read_only;
}
Reference::Reference(const uint32_t &_nucleotides,const uint32_t &_sequence,const bool &_read_only):Bitset(_nucleotides,_sequence){
   this->_count=0U;
   this->_read_only=_read_only;
}
void Reference::increase(void){
   this->_count++;
}
void Reference::decrease(void){
   this->_count--;
}
uint32_t Reference::count(void) const{
   return(this->_count);
}
bool Reference::read_only(void) const{
   return(this->_read_only);
}
void Reference::mutate(void){
   uint32_t size=uint32_t(ceil(double(this->_nucleotides)/double(N_NUCLEOTIDES)));
   uniform_int_distribution<> position(0U,size-1U);
   uniform_int_distribution<> nucleotide(0,CHAR_BIT-1);

   char mask=1;
   unsigned int p=0,n=0;
   
   do{
      p=position(rng);
      n=nucleotide(rng);
   }while((p*CHAR_BIT+n)>=this->_nucleotides);

   char value=(this->_data[p])&(mask<<n);

   if(!value) value=mask<<n;
	
   this->_data[p]^=value;
}
Reference::~Reference(void){
   ;
}
