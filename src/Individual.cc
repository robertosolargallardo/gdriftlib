#include "Individual.h"
Individual::Individual(const uint32_t &_id,const boost::property_tree::ptree &_findividual){
   this->_id=_id;
   this->_ploidy=Ploidy(_findividual.get<uint32_t>("ploidy"));
   this->_n_chromosomes=_findividual.get_child("chromosomes").size();
   this->_chromosomes=(Chromosome***)malloc(sizeof(Chromosome**)*this->_n_chromosomes);

   uint32_t id;
   for(auto fchromosome : _findividual.get_child("chromosomes")){
      id=fchromosome.second.get<uint32_t>("id");
      this->_chromosomes[id]=(Chromosome**)malloc(sizeof(Chromosome*)*int(this->_ploidy));
      for(int c=0;c<int(this->_ploidy);c++){
         this->_chromosomes[id][c]=new Chromosome(id,fchromosome.second.get_child("genes").size());

         for(auto fgene : fchromosome.second.get_child("genes"))
            this->_chromosomes[id][c]->gene(fgene.second.get<uint32_t>("id"))->mutation_rate(fgene.second.get<double>("mutation-rate"));
      }
   }

}
Individual::Individual(const Individual &_individual){
   this->_id=_individual._id;
   this->_ploidy=_individual._ploidy;
   this->_n_chromosomes=_individual._n_chromosomes;
   this->_chromosomes=(Chromosome***)malloc(sizeof(Chromosome**)*this->_n_chromosomes);

   for(uint32_t id=0U;id<this->_n_chromosomes;id++){
      this->_chromosomes[id]=(Chromosome**)malloc(sizeof(Chromosome*)*int(this->_ploidy));

      for(int c=0;c<int(this->_ploidy);c++)
         this->_chromosomes[id][c]=new Chromosome(*(_individual._chromosomes[id][c]));
   }
}
Chromosome** Individual::chromosome(const uint32_t &_id){
   return(this->_chromosomes[_id]);
}
uint32_t Individual::id(void) const{
   return(this->_id);
}
uint32_t Individual::n_chromosomes(void) const{
   return(this->_n_chromosomes);
}
Ploidy Individual::ploidy(void) const{
   return(this->_ploidy);
}
Individual::~Individual(void){
   for(uint32_t i=0U;i<this->_n_chromosomes;i++){
      for(int j=0;j<int(this->_ploidy);j++)
         delete this->_chromosomes[i][j];
      free(this->_chromosomes[i]);
   }
   free(this->_chromosomes);
}
