#include "Bitset.h"
Bitset::Bitset(const Bitset &_bitset){
   this->_nucleotides=_bitset._nucleotides;
   uint32_t size=ceil(double(this->_nucleotides)/double(N_NUCLEOTIDES));
   this->_data=(char*)malloc(sizeof(char)*size);
   memcpy(this->_data,_bitset._data,sizeof(char)*size);
}
Bitset::Bitset(const uint32_t &_nucleotides,const uint32_t &_sequence){
   this->_nucleotides=_nucleotides;
   uint32_t size=ceil(double(this->_nucleotides)/double(N_NUCLEOTIDES));
   this->_data=(char*)malloc(sizeof(char)*size);
   memset(this->_data,0,sizeof(char)*size);
  
   uint32_t mask=UCHAR_MAX;
	int max_disp=sizeof(uint32_t)*CHAR_BIT;

   for(int i=size-1,disp=0;i>=0 && disp<max_disp;i--,disp+=CHAR_BIT)
      this->_data[i]=char((_sequence & (mask<<disp))>>disp);
}
Bitset::Bitset(const string &_sequence){
   this->_nucleotides=_sequence.length();
   uint32_t size=ceil(double(this->_nucleotides)/double(N_NUCLEOTIDES));
   this->_data=(char*)malloc(sizeof(char)*size);
   memset(this->_data,0,sizeof(char)*size);

   Nucleotide n=T;
   uint32_t dist,disp,index;

   for(int i=_sequence.length()-1;i>=0;i--){
      dist=this->_nucleotides-i-1;
      disp=(CHAR_BIT/N_NUCLEOTIDES)*(dist%N_NUCLEOTIDES);
      index=dist/N_NUCLEOTIDES;
            
      switch(*(_sequence.data()+i)){
                     case 'A':   n=A; break;
                     case 'C':   n=C; break;
                     case 'G':   n=G; break;
                     case 'T':   n=T; break;
      }
      this->_data[size-index-1]|=(n<<disp);
   }
}
string Bitset::to_string(void){
   std::stringbuf buffer;
   std::ostream os(&buffer);

   Nucleotide mask=T,n=T;
   int size=ceil(double(this->_nucleotides)/double(N_NUCLEOTIDES));
   int disp=0;

   for(int i=0;i<size;i++){
      for(uint32_t j=0;j<N_NUCLEOTIDES && (i*N_NUCLEOTIDES+j)<this->_nucleotides;j++){
         disp=j*(CHAR_BIT/N_NUCLEOTIDES);
         n=Nucleotide((*(this->_data+(size-i-1)))&(mask<<disp));

         switch(n>>disp){
            case A:   os << "A"; break;
            case C:   os << "C"; break;
            case G:   os << "G"; break;
            case T:   os << "T"; break;
         }
      }
   }
   string sequence(buffer.str());
   reverse(sequence.begin(),sequence.end()); 

   return(sequence);
}
bool Bitset::operator==(const Bitset &_bitset){
   if(this->_nucleotides!=_bitset._nucleotides)
      return(false);

   uint32_t size=ceil(double(this->_nucleotides)/double(N_NUCLEOTIDES));
	auto r=memcmp(this->_data,_bitset._data,sizeof(char)*size);
   return(r==0);
}
Bitset::~Bitset(void){
   if(this->_data!=nullptr){
      free(this->_data);
      this->_data=nullptr;
   }
}
