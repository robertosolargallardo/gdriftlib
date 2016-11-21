#include "Model.h" 

namespace model{
   template<>
   void run<WRIGHTFISHER,HAPLOID>(Population* &_src,Population* &_dst,Pool* &_pool){
      uniform_int_distribution<> uniform(0U,_src->size()-1U);
      uniform_real_distribution<double> mutation_rate(0.0,1.0);

      uint32_t position=0U;
      Reference* reference=nullptr;
      Reference* validator=nullptr;

      Individual* individual=nullptr;
      for(uint32_t id=0U;id<_dst->size();id++){
         individual=_src->at(uniform(rng));

         position=0U;
         for(uint32_t cid=0;cid<individual->n_chromosomes();cid++){
            for(uint32_t gid=0;gid<individual->chromosome(cid)[0]->n_genes();gid++,position++){
               if(mutation_rate(rng)<=_src->at(id)->chromosome(cid)[0]->gene(gid)->mutation_rate()){
                  reference=new Reference(*(_src->at(id)->chromosome(cid)[0]->gene(gid)->ref()));
                  reference->mutate();

                  validator=_pool->push(position,reference);
                  if((*validator)==(*reference)){
                     delete reference;
                     reference=validator;
                  }
                  _dst->at(id)->chromosome(cid)[0]->gene(gid)->ref(reference);
               }
               else
                  _dst->at(id)->chromosome(cid)[0]->gene(gid)->ref(_src->at(id)->chromosome(cid)[0]->gene(gid)->ref());
            }
         }
      }
   }
}
