#ifndef _SAMPLE_H_
#define _SAMPLE_H_
#include "Population.h"

#define SAMPLING_PERCENT 0.05

class Sample : public Population{
	public:	Sample(void);
				Sample(const string&);
				Sample(const string&,Population*,const uint32_t&);
				// Constructor de Population, para test/load/test.cc
				Sample(const Ploidy &_ploidy,const boost::property_tree::ptree &_fpopulation, const boost::property_tree::ptree &_fsettings);
				~Sample(void);

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
            pair<double,double> rarest_nucleotides_statistics_seq(const vector<VirtualSequence*> &_sequences){
               map<char,int> count;
               vector<int> stats;
               double mean=0.0;
               
               VirtualSequence *ref = _sequences[0];
               for(size_t i = 0; i < ref->length(); i++){

                  count['A']=count['C']=count['G']=count['T']=0;
                  for(size_t j=0; j<_sequences.size();j++){
                     count[_sequences[j]->at(i)]++;
                  }
                  int c=0;
                  for(auto j : count)
                     if(j.second<int(double(_sequences.size())*0.01))
                        c++;
                  stats.push_back(c);
                  mean+=double(c);
               }
               mean/=double(ref->length());

               double diff,variance=0.0;
               for(auto s : stats){
                  diff=s-mean;
                  variance+=diff*diff;
               }
               variance/=double(ref->length());

               return(pair<double,double>(mean,variance));
            }
            double number_of_haplotypes(const vector<string> &_sequences){
            	if(_sequences.size() <= 1){
            		return 0.0;
            	}
            	
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
            double number_of_haplotypes_seq(const vector<VirtualSequence*> &_sequences){
            	if(_sequences.size() == 1){
            		return 0.0;
            	}
               map<string,double> haplotypes;

               // for(auto& seq : _sequences) haplotypes[seq]=(haplotypes.count(seq))?haplotypes[seq]+1.0:1.0;
				string str;
				for(unsigned int i = 0; i < _sequences.size(); ++i){
//					vector< pair<seq_size_t, char> > mutations = _sequences[i]->get_mutations();
//					haplotypes[mutations] = (haplotypes.count(mutations))?haplotypes[mutations]+1.0:1.0;
					str = _sequences[i]->to_string();
					haplotypes.emplace(str, 0.0);
					haplotypes[str] += 1;
				}

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
               double segregating_sites = 0.0;
   
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
            double number_of_segregating_sites_seq(const vector<VirtualSequence*> &_sequences){
               double segregating_sites = 0.0;
   
               VirtualSequence *ref=_sequences[0];
               for(size_t i=0;i<ref->length();i++){
                  for(size_t j=1; j<_sequences.size(); j++){
                     if(ref->at(i) != _sequences[j]->at(i)){
                        segregating_sites += 1.0;
                        break;
                     }
                  }
               }
               return(segregating_sites);
            }
            pair<double,double> pairwise_statistics(const vector<string> &_sequences){
            	if(_sequences.size() <= 1){
            		return pair<double, double>(0.0, 0.0);
            	}
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
	           mean /= double(pairwise_differences.size());
   
               double variance=0.0;
               for(auto& diff : pairwise_differences)
                  variance+=(diff-mean)*(diff-mean);

               variance/=double(pairwise_differences.size());
               pairwise_differences.clear();

               return(make_pair(mean,variance));
            }
            pair<double,double> pairwise_statistics_seq(const vector<VirtualSequence*> &_sequences){
            	if(_sequences.size() <= 1){
            		return pair<double, double>(0.0, 0.0);
            	}
               vector<double> pairwise_differences;
               double mean = 0.0;
   
               for(size_t i = 0; i < _sequences.size(); i++){
                  for(size_t j = i+1; j < _sequences.size(); j++){
                     double diff = 0.0;
                     for(size_t k = 0; k < _sequences[i]->length(); k++){
                        if( _sequences[i]->at(k) != _sequences[j]->at(k) )
                           diff += 1.0;
                     }
                     pairwise_differences.push_back(diff);
                     mean += diff;
                  }
               }
               mean /= double(pairwise_differences.size());
   
               double variance=0.0;
               for(auto& diff : pairwise_differences)
                  variance+=(diff-mean)*(diff-mean);

               variance/=double(pairwise_differences.size());
               pairwise_differences.clear();

               return(make_pair(mean,variance));
            }
            double tajima_d_statistics(const double &_n,const double &_ss,const double &_mpd){
            	if( _n <= 1 || _ss == 0 || _mpd <= 0){
            		return 0.0;
            	}
            	
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
   
            boost::property_tree::ptree indices(void){
               //random_shuffle(this->_population.begin(),this->_population.end());
               auto sample=this->_population;//vector<Individual*>(this->_population.begin(),this->_population.begin()+int(floor(double(this->size())*_percentage)));

               map<uint32_t,map<uint32_t,vector<string>>> sequences;
//               map<uint32_t,map<uint32_t,vector<VirtualSequence*>>> sequences;

//               cout<<"sample[0]: "<<sample[0]->id()<<" (de " <<sample.size()<<")\n";
               for(auto& individual : sample){
                  for(unsigned int pid = 0; pid < individual->getPloidy(); pid++){
                     for(uint32_t cid = 0; cid < individual->getChromosomes(); cid++){
                        for(uint32_t gid = 0; gid < individual->getGenes(cid); gid++){
                           sequences[cid][gid].push_back(
                           	individual->getGene(gid, cid, pid)->to_string()
                           	);
//                           sequences[cid][gid].push_back(individual->chromosome(cid)[pid]->gene(gid)->reference()->to_string());
//                           sequences[cid][gid].push_back(individual->chromosome(cid)[pid]->gene(gid)->reference());
                        }
                     }
                  }
               }
//               cout<<"sequences[0][0][0]: "<<sequences[0][0][0]<<"\n";

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
                     
//                     findices.put("number-of-haplotypes",this->number_of_haplotypes_seq(sequences[cid][gid]));
//                     findices.put("number-of-segregating-sites",this->number_of_segregating_sites_seq(sequences[cid][gid]));
//                     tie(mean_of_the_number_of_pairwise_differences,variance_of_the_number_of_pairwise_differences)=this->pairwise_statistics_seq(sequences[cid][gid]);
                     
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
