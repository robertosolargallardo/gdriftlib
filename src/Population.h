#ifndef _POPULATION_H_
#define _POPULATION_H_

#include <stdint.h>
#include <vector>
#include <map>
#include "Individual.h"
#include "VirtualSequence.h"

extern random_device seed;
extern mt19937 rng;

using namespace std;

class Population{
   private: vector<Individual*> _population;
            string _name;
   
   public:  Population(void);
            Population(const uint32_t&);
            Population(const string&);
            Population(const string&,const uint32_t&);
            Population(const Ploidy&,const boost::property_tree::ptree&);
 
            uint32_t size(void);
            string name(void);
            void name(const string&);
            bool empty(void);
            void push(Individual*);
            void pop(void);
            Individual* top(void);
            Individual* at(const uint32_t&);
            ~Population(void);
				vector<Individual*> individuals(void);
   
            vector<Population*> split(const size_t&);
            void migration(Population*,const uint32_t&);
            void decrease(const uint32_t&);
            void increase(const uint32_t&);
            void merge(Population*);


            /*indices*/
            pair<double,double> rarest_nucleotides_statistics(const vector<string> &_sequences){
               map<char,int> count;
               vector<int> stats;
               double mean=0.0;
               
               string ref=_sequences[0];
               for(size_t i=0;i<ref.length();i++){

                  count['A']=count['C']=count['G']=count['T']=0;
                  for(size_t j=0;j<_sequences.size();j++){
                     count[_sequences[j].at(i)]++;
                  }
                  int c=0;
                  for(auto j : count)
                     if(j.second<int(double(_sequences.size())*0.01))
                        c++;
                  stats.push_back(c);
                  mean+=double(c);
               }
               mean/=double(ref.length());

               double diff,variance=0.0;
               for(auto s : stats){
                  diff=s-mean;
                  variance+=diff*diff;
               }
               variance/=double(ref.length());

               return(pair<double,double>(mean,variance));
            }
            double number_of_haplotypes(const vector<string> &_sequences){
               map<string,double> haplotypes;

               for(auto& seq : _sequences) haplotypes[seq]=(haplotypes.count(seq))?haplotypes[seq]+1.0:1.0;

               double sum=0.0;
               double N=double(_sequences.size());
               for(auto& h : haplotypes){
                  double x=double(h.second)/N;
                  sum+=(x*x);
               }

               haplotypes.clear();
               return((N/(N-1.0))*(1.0-sum));
            }
            double number_of_segregating_sites(const vector<string> &_sequences){
               double segregating_sites=0.0;
   
               string ref=_sequences[0];
               for(size_t i=0;i<ref.length();i++){
                  for(size_t j=1;j<_sequences.size();j++){
                     if(ref.at(i)!=_sequences[j].at(i)){
                        segregating_sites+=1.0;
                        break;
                     }
                  }
               }
               return(segregating_sites);
            }
            pair<double,double> pairwise_statistics(const vector<string> &_sequences){
               vector<double> pairwise_differences;
               double mean=0.0;
   
               for(size_t i=0;i<_sequences.size();i++){
                  for(size_t j=i+1;j<_sequences.size();j++){
                     double diff=0.0;
                     for(size_t k=0;k<_sequences[i].length();k++){
                        if(_sequences[i][k]!=_sequences[j][k])
                           diff+=1.0;
                     }
                     pairwise_differences.push_back(diff);
                     mean+=diff;
                  }
               }
               mean/=double(pairwise_differences.size());
   
               double variance=0.0;
               for(auto& diff : pairwise_differences)
                  variance+=(diff-mean)*(diff-mean);

               variance/=double(pairwise_differences.size());
               pairwise_differences.clear();

               return(make_pair(mean,variance));
            }
            double tajima_d_statistics(const double &_n,const double &_ss,const double &_mpd){
               double a1=0.0,a2=0.0;

               for(size_t k=1;k<size_t(_n);k++){
                  a1+=1.0/double(k);
                  a2+=1.0/double(k*k);
               }

               double b1=(_n+1.0)/(3.0*(_n-1.0));
               double b2=(2.0*(_n*_n+_n+3.0))/((9.0*_n)*(_n-1.0));
               double c1=b1-(1.0/a1);
               double c2=b2+((_n+2.0)/(a1*_n))+a2/(a1*a1);
        
               double e1=c1/a1;
               double e2=c2/(a1*a1+a2);

               return((_mpd-(_ss/a1))/sqrt(e1*_ss+e2*_ss*(_ss-1.0)));
            }          
   
            boost::property_tree::ptree indices(const double &_percentage){
               random_shuffle(this->_population.begin(),this->_population.end());
               auto sample=vector<Individual*>(this->_population.begin(),this->_population.begin()+int(floor(double(this->size())*_percentage)));

               map<uint32_t,map<uint32_t,vector<string>>> sequences;

               for(auto& individual : sample){
                   for(uint32_t cid=0U;cid<individual->n_chromosomes();cid++){
                     for(int pid=0;pid<int(individual->ploidy());pid++){
                        for(uint32_t gid=0;gid<individual->chromosome(cid)[pid]->n_genes();gid++){
                           sequences[cid][gid].push_back(individual->chromosome(cid)[pid]->gene(gid)->reference()->to_string());
                        }
                     }
                  }
               }

               //this->rarest_nucleotides_statistics(sequences[0][0]);
   
               double mean_of_the_number_of_pairwise_differences,variance_of_the_number_of_pairwise_differences;
               boost::property_tree::ptree fpopulation;

               boost::property_tree::ptree fchromosomes;
               fpopulation.put("name",this->name());

               for(uint32_t cid=0U;cid<sequences.size();cid++){
                  boost::property_tree::ptree fchromosome;
                  fchromosome.put("id",cid);

                  for(size_t gid=0U;gid<sequences[cid].size();gid++){
                     boost::property_tree::ptree fgenes;
                     boost::property_tree::ptree fgene;
                     fgene.put("id",gid);

                     boost::property_tree::ptree findices;
                     findices.put("number-of-haplotypes",this->number_of_haplotypes(sequences[cid][gid]));
                     findices.put("number-of-segregating-sites",this->number_of_segregating_sites(sequences[cid][gid]));

                     tie(mean_of_the_number_of_pairwise_differences,variance_of_the_number_of_pairwise_differences)=this->pairwise_statistics(sequences[cid][gid]);
                     findices.put("mean-of-the-number-of-pairwise-differences",mean_of_the_number_of_pairwise_differences);
                     findices.put("variance-of-the-number-of-pairwise-differences",variance_of_the_number_of_pairwise_differences);

                     findices.put("tajima-d-statistics",this->tajima_d_statistics(double(sequences[cid][gid].size()),
                                                                                  findices.get<double>("number-of-segregating-sites"),
                                                                                  mean_of_the_number_of_pairwise_differences));
                     fgene.push_back(std::make_pair("indices",findices));
                     fgenes.push_back(std::make_pair("",fgene));
                     fchromosome.push_back(make_pair("genes",fgenes));
                  }
                  fchromosomes.push_back(make_pair("",fchromosome));
               }
               fpopulation.push_back(std::make_pair("chromosomes",fchromosomes));               

               return(fpopulation);
            }
            /*indices*/
};
#endif
