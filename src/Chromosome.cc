#include "Chromosome.h"
Chromosome::Chromosome(const uint32_t &_id,const uint32_t &_n_genes){
   this->_id=_id;
   this->_n_genes=_n_genes;
   this->_genes=(Gene**)malloc(sizeof(Gene*)*this->_n_genes);

   for(uint32_t id=0;id<this->_n_genes;id++)
      this->_genes[id]=nullptr;
}
Chromosome::Chromosome(const Chromosome &_chromosome){
   this->_id=_chromosome._id;
   this->_n_genes=_chromosome._n_genes;
   this->_genes=(Gene**)malloc(sizeof(Gene*)*this->_n_genes);

   for(uint32_t id=0U;id<this->_n_genes;id++){
      this->_genes[id]=new Gene(*(_chromosome._genes[id]));
	}
}
uint32_t Chromosome::id(void) const{
   return(this->_id);
}
uint32_t Chromosome::n_genes(void) const{
   return(this->_n_genes);
}
Gene*& Chromosome::gene(const uint32_t &_id){
   return(this->_genes[_id]);
}
Chromosome::~Chromosome(void){
   for(uint32_t id=0;id<this->_n_genes;id++)
      delete this->_genes[id];
   free(this->_genes);
}
