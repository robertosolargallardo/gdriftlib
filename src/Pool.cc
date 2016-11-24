#include "Pool.h"
Pool::Pool(const boost::property_tree::ptree &_findividual){
   //TODO ordenar cromosomas y genes por ID;
   this->_findividual=_findividual;
   this->_size=0U;
   
   for(auto fchromosome : _findividual.get_child("chromosomes"))
      this->_size+=fchromosome.second.get_child("genes").size();
   
   this->_pool=(vector<Reference*>**)malloc(sizeof(vector<Reference*>*)*this->size());
   for(uint32_t position=0U;position<this->size();position++) 
      this->_pool[position]=new vector<Reference*>();

   uint32_t position=0U,nucleotides,number_of_segregating_sites,number_of_alleles; 
   for(auto fchromosome : _findividual.get_child("chromosomes")){
      for(auto fgene : fchromosome.second.get_child("genes")){
         nucleotides=fgene.second.get<uint32_t>("nucleotides");
         number_of_alleles=fgene.second.get<uint32_t>("number-of-alleles");
         number_of_segregating_sites=fgene.second.get<uint32_t>("number-of-segregating-sites");
   
         this->populate(position++,nucleotides,number_of_alleles,number_of_segregating_sites);         
      }
   }
}
Individual* Pool::generate(const uint32_t &_id){
   Individual *individual=new Individual(_id,this->_findividual);
   int position=0;

   for(uint32_t cid=0U;cid<individual->n_chromosomes();cid++){
      for(uint32_t p=0U;p<uint32_t(individual->ploidy());p++){
         for(uint32_t gid=0U;gid<individual->chromosome(cid)[p]->n_genes();gid++){
            uniform_int_distribution<> uniform(0,this->_pool[position+gid]->size()-1);
				Reference *reference=(*this->_pool[position+gid])[uniform(rng)];
            individual->chromosome(cid)[p]->gene(gid)->reference(reference);
         }
      }
      position+=individual->chromosome(cid)[0]->n_genes();
   }

   return(individual);
}
Reference* Pool::push(const uint32_t &_position,Reference* _reference){
   for(vector<Reference*>::iterator i=this->_pool[_position]->begin();i!=this->_pool[_position]->end();i++)
      if((**i)==(*_reference)) return(*i);

   this->_pool[_position]->push_back(_reference);
   return(_reference);
}
uint32_t Pool::size(void){
   return(this->_size);
}
void Pool::decrease_all(void){
   for(uint32_t position=0U;position<this->size();position++)
      for_each(this->_pool[position]->begin(),this->_pool[position]->end(),[](Reference* &reference){if(reference->count()){reference->decrease();}});
}
void Pool::release(void){
   for(uint32_t position=0U;position<this->size();position++){
      for_each(this->_pool[position]->begin(),this->_pool[position]->end(),[](Reference* &reference){if(!reference->count()){delete reference;reference=nullptr;}});
      this->_pool[position]->erase(std::remove_if(this->_pool[position]->begin(),this->_pool[position]->end(),[](Reference* reference){return reference==nullptr;}),this->_pool[position]->end());
   }
}
void Pool::populate(const uint32_t &_position,const uint32_t &_nucleotides,const uint32_t &_number_of_alleles,const uint32_t &_number_of_segregating_sites){
   if(_number_of_alleles>pow(N_NUCLEOTIDES,_number_of_segregating_sites)){
      cerr << "Error:: Cannot generate " << _number_of_alleles << " alleles with " << _number_of_segregating_sites << " segregating sites" << endl;
      exit(EXIT_FAILURE);
   }
   if(_number_of_segregating_sites>_nucleotides){
      cerr << "Error:: Number of segregating sites is greater than the number of nucleotides" << "::" << _number_of_segregating_sites << " " << _nucleotides << endl;
      exit(EXIT_FAILURE);
   }
   
   this->_pool[_position]->reserve(_number_of_alleles);  
      
   for(uint32_t sequence=0U;sequence<_number_of_alleles;sequence++){
		Reference* reference=new Reference(_nucleotides,sequence);
      this->_pool[_position]->push_back(reference);
	}
}
Pool::~Pool(void){
   for(uint32_t position=0;position<this->size();position++){
      for_each(this->_pool[position]->begin(),this->_pool[position]->end(),[](Reference* &reference){delete reference;reference=nullptr;});
      this->_pool[position]->clear();
      delete this->_pool[position];
   }
   free(this->_pool);
}
