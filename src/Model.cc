#include "Model.h" 

namespace model{
   template<>
   void run<WRIGHTFISHER,HAPLOID>(Population* &_src,Population* &_dst,Pool* &_pool){
      uniform_int_distribution<> uniform(0U,_src->size()-1U);
      uniform_real_distribution<double> mutation_rate(0.0,1.0);

      Reference* reference=nullptr;
      Reference* validator=nullptr;

      Individual* individual=nullptr;

      for(uint32_t id=0U;id<_dst->size();id++){
         individual=_src->at(uniform(rng));

         for(uint32_t cid=0;cid<individual->n_chromosomes();cid++){
            for(uint32_t gid=0;gid<individual->chromosome(cid)[0]->n_genes();gid++){
               if(mutation_rate(rng)<=individual->chromosome(cid)[0]->gene(gid)->mutation_rate()){
                  reference=new Reference(*(individual->chromosome(cid)[0]->gene(gid)->reference()));
                  reference->mutate();
						
						validator=_pool->push(cid,gid,reference);
                  if(validator!=nullptr){
                     delete reference;
                     reference=validator;
                  }
                  _dst->at(id)->chromosome(cid)[0]->gene(gid)->reference(reference);
               }
               else
                  _dst->at(id)->chromosome(cid)[0]->gene(gid)->reference(individual->chromosome(cid)[0]->gene(gid)->reference());
            }
         }
      }
   }
   template<>
   void run<WRIGHTFISHER,DIPLOID>(Population* &_src,Population* &_dst,Pool* &_pool){
      uniform_int_distribution<> coin(0U,1U);
      uniform_int_distribution<> uniform(0U,_src->size()-1U);
      uniform_real_distribution<double> mutation_rate(0.0,1.0);

      Reference* reference=nullptr;
      Reference* validator=nullptr;

      array<Individual*,2> individual={nullptr,nullptr};
      array<Gene*,2> gene={nullptr,nullptr};

      for(uint32_t id=0U;id<_dst->size();id++){
         individual[0]=_src->at(uniform(rng));
         individual[1]=_src->at(uniform(rng));

         for(uint32_t cid=0;cid<individual[0]->n_chromosomes();cid++){
            for(uint32_t gid=0;gid<individual[0]->chromosome(cid)[0]->n_genes();gid++){
               gene[0]=individual[0]->chromosome(cid)[coin(rng)]->gene(gid);
               gene[1]=individual[1]->chromosome(cid)[coin(rng)]->gene(gid);
               
               for(uint32_t i=0U;i<uint32_t(DIPLOID);i++){
                  if(mutation_rate(rng)<=gene[i]->mutation_rate()){
                     reference=new Reference(*(gene[i]->reference()));
                     reference->mutate();
						
   						validator=_pool->push(cid,gid,reference);
                     if(validator!=nullptr){
                        delete reference;
                        reference=validator;
                     }
                     _dst->at(id)->chromosome(cid)[i]->gene(gid)->reference(reference);

                  }
                  else
                     _dst->at(id)->chromosome(cid)[i]->gene(gid)->reference(gene[i]->reference());
               }
            }
         }
      }
   }
}
