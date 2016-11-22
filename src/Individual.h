#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_
#include "Chromosome.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

enum Ploidy{HAPLOID=1,DIPLOID=2,TRIPLOID=3,TETRAPLOID=4,PENTAPLOID=5,HEXAPLOID=6,HEPTAPLOID=7,OCTAPLOID=8};

class Individual{
   private: uint32_t       _id;
            Ploidy         _ploidy;
            uint32_t       _n_chromosomes;
            Chromosome***  _chromosomes;
  
   public:  Individual(const uint32_t&,const boost::property_tree::ptree&);
            Individual(const uint32_t&,const Ploidy&,const uint32_t&);
            Individual(const Individual&);
            uint32_t id(void) const;
            uint32_t n_chromosomes(void) const;
            Chromosome**& chromosome(const uint32_t&);
            Ploidy ploidy(void) const;
            ~Individual(void);                     

            void show(void){
               for(uint32_t cid=0U;cid<this->n_chromosomes();cid++){
                  cout << "Chromosome::"<<cid<<endl;
                  Chromosome** chromosome=this->chromosome(cid);
                  for(uint32_t j=0U;j<uint32_t(this->ploidy());j++){
                     cout << "Ploidy::"<<j<<endl;
                     for(uint32_t k=0U;k<chromosome[j]->n_genes();k++){
                        cout << "Gene::"<<k<<endl;
                        cout << chromosome[j]->gene(k)->reference()->to_string() << " " << chromosome[j]->gene(k)->mutation_rate() << endl;
                        
                     }
                  }
               }
            }
};
#endif
