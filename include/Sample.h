#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include "Population.h"
#include "NanoTimer.h"

#include "VirtualSequence.h"
#include "VirtualSequenceDNA.h"

#define SAMPLING_PERCENT 0.05

class Sample : public Population{
	public:
		Sample(void);
		Sample(const string&);
		Sample(const string&, Population*, const uint32_t&);
		// Constructor de Population, para test/load/test.cc
		Sample(const Ploidy &_ploidy, const boost::property_tree::ptree &_fpopulation, const boost::property_tree::ptree &_fsettings);
		~Sample(void);
		
		// Real delete of the data (instead of just clearing the internal vector)
		// This is for the case of just using a Sample without a parent Population
		void deleteData(){
//			for(unsigned int i = 0; i < _population.size(); ++i){
//				if(_population[i] != NULL){
//					delete _population[i];
//				}
//			}
			_population.clear();
		}

		/*indices*/
		pair<double,double> rarest_nucleotides_statistics(const vector<string> &_sequences){
//			cout<<"Sample::rarest_nucleotides_statistics - Inicio\n";
//			NanoTimer timer;
			map<char,int> count;
			vector<int> stats;
			double mean = 0.0;

			string ref = _sequences[0];
			for(size_t i = 0; i < ref.length(); ++i){
				count['A'] = count['C'] = count['G'] = count['T'] = 0;
				for(size_t j = 0; j < _sequences.size(); ++j){
					count[_sequences[j].at(i)]++;
				}
				int c = 0;
				for(auto j : count){
					if(j.second<int(double(_sequences.size())*0.01)){
						++c;
					}
				}
				stats.push_back(c);
				mean+=double(c);
			}
			mean /= double(ref.length());

			double diff,variance=0.0;
			for(auto s : stats){
			diff = s-mean;
			variance += diff*diff;
			}
			variance /= double(ref.length());

//			cout<<"Sample::rarest_nucleotides_statistics - Fin ("<<timer.getMilisec()<<" ms)\n";
			return(pair<double,double>(mean,variance));
		}
				
		pair<double,double> rarest_nucleotides_statistics_seq(const vector<VirtualSequence*> &_sequences){
//			cout<<"Sample::rarest_nucleotides_statistics_seq - Inicio\n";
//			NanoTimer timer;
			map<char,int> count;
			vector<int> stats;
			double mean = 0.0;
			
			VirtualSequence *ref = _sequences[0];
			for(size_t i = 0; i < ref->length(); i++){
				count['A'] = count['C'] = count['G'] = count['T'] = 0;
				for(size_t j = 0; j < _sequences.size(); j++){
					count[_sequences[j]->at(i)]++;
				}
				int c = 0;
				for(auto j : count){
					if(j.second < int(double(_sequences.size())*0.01)){
						c++;
					}
				}
				stats.push_back(c);
				mean += double(c);
			}
			mean /= double(ref->length());

			double diff,variance=0.0;
			for(auto s : stats){
				diff=s-mean;
				variance+=diff*diff;
			}
			variance /= double(ref->length());

//			cout<<"Sample::rarest_nucleotides_statistics_seq - Fin ("<<timer.getMilisec()<<" ms)\n";
			return pair<double,double>(mean,variance);
		}
		
		double number_of_haplotypes(const vector<string> &_sequences){
//			cout<<"Sample::number_of_haplotypes - Inicio\n";
//			NanoTimer timer;
			if(_sequences.size() <= 1){
				return 0.0;
			}
			
			map<string,double> haplotypes;
			
			for(auto& seq : _sequences) haplotypes[seq]=(haplotypes.count(seq))?haplotypes[seq]+1.0:1.0;

			double sum = 0.0;
			double N = double(_sequences.size());
			for(auto& h : haplotypes){
				double x=double(h.second)/N;
				sum+=(x*x);
			}
			haplotypes.clear();
//			cout<<"Sample::number_of_haplotypes - Fin ("<<timer.getMilisec()<<" ms)\n";
			return ((N/(N-1.0))*(1.0-sum));
		}
		
		// Este metodo resulto ser mas lento que la version string
		double number_of_haplotypes_seq(const vector<VirtualSequence*> &sequences){
//			cout<<"Sample::number_of_haplotypes_seq - Inicio\n";
//			NanoTimer timer;
			if(sequences.size() == 1){
				return 0.0;
			}
			map<string, double> haplotypes;
			
			// En lugar de almacenar todas las secuencias completas, puedo almacenar un string diferente
			// Tomo una como referencia, si data es igual, almaceno una codificacion de las mutaciones
			// Si es diferente, almaceno la secuencia completa
			// Lo importante es que la codificacion de mutaciones no choquen, pero para eso basta con codificar los numeros directamente
			
			// Notar que la forma segura de realizar el cast sisuiente es con dynamic_cast, y comprobar si el resultado es valido
			// Por ahora dejo el cast NO SEGURO estatico hacia abajo por razones de tiempo
			
			VirtualSequenceDNA *ref = static_cast<VirtualSequenceDNA*>(sequences[0]);
			VirtualSequenceDNA *seq = NULL;
			string str;
			for(unsigned int i = 0; i < sequences.size(); ++i){
				seq = static_cast<VirtualSequenceDNA*>(sequences[i]);
				if( ref->getData() == seq->getData() ){
					str = seq->codeMutations();
				}
				else{
					str = seq->to_string();
				}
				haplotypes.emplace(str, 0.0);
				haplotypes[str] += 1;
			}
			
			double sum = 0.0;
			double N = double(sequences.size());
			for(auto& h : haplotypes){
				double x = double(h.second) / N;
				sum+=(x*x);
			}

			haplotypes.clear();
//			cout<<"Sample::number_of_haplotypes_seq - Fin ("<<timer.getMilisec()<<" ms)\n";
			return((N/(N-1.0))*(1.0-sum));
		}
		double number_of_segregating_sites(const vector<string> &_sequences){
//			cout<<"Sample::number_of_segregating_sites - Inicio\n";
//			NanoTimer timer;
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
//			cout<<"Sample::number_of_segregating_sites - Fin ("<<timer.getMilisec()<<" ms)\n";
			return(segregating_sites);
		}
		double number_of_segregating_sites_seq(const vector<VirtualSequence*> &_sequences){
//			cout<<"Sample::number_of_segregating_sites_seq - Inicio\n";
//			NanoTimer timer;
			double segregating_sites = 0.0;

			VirtualSequence *ref = _sequences[0];
			for(size_t i=0;i<ref->length();i++){
				for(size_t j=1; j<_sequences.size(); j++){
					if(ref->at(i) != _sequences[j]->at(i)){
						segregating_sites += 1.0;
						break;
					}
				}
			}
//			cout<<"Sample::number_of_segregating_sites_seq - Fin ("<<timer.getMilisec()<<" ms)\n";
			return(segregating_sites);
		}
		pair<double,double> pairwise_statistics(const vector<string> &_sequences){
//			cout<<"Sample::pairwise_statistics - Inicio\n";
//			NanoTimer timer;
			if(_sequences.size() <= 1){
				return pair<double, double>(0.0, 0.0);
			}
			vector<double> pairwise_differences;
			double mean=0.0;

			for(size_t i = 0; i < _sequences.size(); ++i){
				for(size_t j = i+1; j < _sequences.size(); ++j){
					double diff = 0.0;
					for(size_t k = 0; k < _sequences[i].length(); ++k){
						if(_sequences[i][k] != _sequences[j][k]){
							diff += 1.0;
						}
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

//			cout<<"Sample::pairwise_statistics - Fin ("<<timer.getMilisec()<<" ms)\n";
			return(make_pair(mean,variance));
		}
		
		// Cost (n_seqs^2 * n_muts) in the best case, (n_seqs^2 * seq_len) in worst case
		pair<double,double> pairwise_statistics_seq(const vector<VirtualSequence*> &sequences){
//			cout<<"Sample::pairwise_statistics_seq - Inicio\n";
//			NanoTimer timer;
			if(sequences.size() <= 1){
				return pair<double, double>(0.0, 0.0);
			}
			vector<double> pairwise_differences;
			double mean = 0.0;
			
			// Notar que aqui tambien estoy dejando un cast estatico NO SEGURO por eficiencia
			// La forma correcta es dynamic_cast y verificacion de resultado valido
			VirtualSequenceDNA *seq_i = NULL;
			VirtualSequenceDNA *seq_j = NULL;
			
			for(size_t i = 0; i < sequences.size(); i++){
				seq_i = static_cast<VirtualSequenceDNA*>(sequences[i]);
				vector< pair<seq_size_t, char> > muts_i = seq_i->getMutations();
				
				for(size_t j = i+1; j < sequences.size(); j++){
					double diff = 0.0;
					seq_j = static_cast<VirtualSequenceDNA*>(sequences[j]);
					if( seq_i->getData() == seq_j->getData() ){
						// Caso rapido, solo usar mutaciones
						vector< pair<seq_size_t, char> > muts_j = seq_j->getMutations();
						
						vector< pair<seq_size_t, char> > res;
						
						// Lo siguiente asume que los arreglos de mutaciones estan ORDENADOS
						// En caso contrario, habria que copiar y ordenar
						std::set_symmetric_difference(muts_i.begin(), muts_i.end(), muts_j.begin(), muts_j.end(), back_inserter(res));
						diff += res.size();
						
						// Lo que sigue YA NO ES NECESARIO pues las mutaciones ahora se aplican una por caracter
						// De las mutaciones reultantes, descuento las que se aplican a un mismo caracter
						// Las mutaciones son posicionales en bit, y cada letra tiene 2 bits, uno par y uno impar
						// Si dos mutaciones son consecutivas, y la menor es par, entonces se aplican a la misma letra
//						for(unsigned int k = 1; k < res.size(); ++k){
//							// La condicion es:
//							// Si la mutacion k es consecutiva a la anterior Y si la anterior a k era par
//							if( (res[k] == (res[k-1] + 1)) 
//								&& ((res[k-1] & 0x1) == 0) ){
//								diff -= 1;
//							}
//						}
						
					}
					else{
						// Caso lento, comparar con at
						for(size_t k = 0; k < sequences[i]->length(); k++){
							if( sequences[i]->at(k) != sequences[j]->at(k) )
								diff += 1.0;
						}
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

//			cout<<"Sample::pairwise_statistics_seq - Fin ("<<timer.getMilisec()<<" ms)\n";
			return(make_pair(mean,variance));
		}
		double tajima_d_statistics(const double &_n,const double &_ss,const double &_mpd){
//			cout<<"Sample::tajima_d_statistics - Inicio\n";
//			NanoTimer timer;
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

//			cout<<"Sample::tajima_d_statistics - Fin ("<<timer.getMilisec()<<" ms)\n";
			return((_mpd-(_ss/a1))/sqrt(e1*_ss+e2*_ss*(_ss-1.0)));
		}
		
		boost::property_tree::ptree indices_seq(void){

			map<uint32_t,map<uint32_t,vector<VirtualSequence*>>> sequences;

//			cout<<"Sample::indices - Decompressing sequences from "<<_population.size()<<" individuals\n";
			for(auto& individual : _population){
				for(unsigned int pid = 0; pid < individual.getPloidy(); pid++){
					for(uint32_t cid = 0; cid < individual.getChromosomes(); cid++){
						for(uint32_t gid = 0; gid < individual.getGenes(cid); gid++){
							sequences[cid][gid].push_back(individual.getGene(gid, cid, pid));
						}
					}
				}
			}
//			cout<<"Sample::indices - Evaluating statistics\n";

			//this->rarest_nucleotides_statistics(sequences[0][0]);

			double mean_of_the_number_of_pairwise_differences,variance_of_the_number_of_pairwise_differences;
			boost::property_tree::ptree fpopulation;

			boost::property_tree::ptree fchromosomes;
			fpopulation.put("name",this->name());

			for(uint32_t cid = 0U; cid < sequences.size(); ++cid){
				boost::property_tree::ptree fchromosome;
				fchromosome.put("id",cid);

				for(size_t gid = 0U; gid < sequences[cid].size(); ++gid){
					boost::property_tree::ptree fgenes;
					boost::property_tree::ptree fgene;
					fgene.put("id",gid);

					boost::property_tree::ptree findices;
					
					findices.put("number-of-haplotypes",this->number_of_haplotypes_seq(sequences[cid][gid]));
					findices.put("number-of-segregating-sites",this->number_of_segregating_sites_seq(sequences[cid][gid]));
					tie(mean_of_the_number_of_pairwise_differences,variance_of_the_number_of_pairwise_differences)=this->pairwise_statistics_seq(sequences[cid][gid]);
					
					findices.put("mean-of-the-number-of-pairwise-differences", mean_of_the_number_of_pairwise_differences);
					findices.put("variance-of-the-number-of-pairwise-differences", variance_of_the_number_of_pairwise_differences);

					findices.put("tajima-d-statistics", this->tajima_d_statistics(double(sequences[cid][gid].size()),
																									 findices.get<double>("number-of-segregating-sites"),
																									 mean_of_the_number_of_pairwise_differences));
					fgene.push_back(std::make_pair("indices",findices));
					fgenes.push_back(std::make_pair("",fgene));
					fchromosome.push_back(make_pair("genes",fgenes));
				}
				fchromosomes.push_back(make_pair("",fchromosome));
			}
			fpopulation.push_back(std::make_pair("chromosomes",fchromosomes));					

//			cout<<"Sample::indices - End\n";
			return(fpopulation);
		}
		
		boost::property_tree::ptree indices(void){

			map<uint32_t, map<uint32_t, vector<string>>> sequences_str;
			map<uint32_t, map<uint32_t, vector<VirtualSequence*>>> sequences;

			cout<<"Sample::indices - Decompressing sequences from "<<_population.size()<<" individuals\n";
			for(auto& individual : _population){
				for(unsigned int pid = 0; pid < individual.getPloidy(); pid++){
					for(uint32_t cid = 0; cid < individual.getChromosomes(); cid++){
						for(uint32_t gid = 0; gid < individual.getGenes(cid); gid++){
							sequences_str[cid][gid].push_back(individual.getGene(gid, cid, pid)->to_string());
							sequences[cid][gid].push_back(individual.getGene(gid, cid, pid));
						}
					}
				}
			}
			cout<<"Sample::indices - Evaluating statistics\n";

			//this->rarest_nucleotides_statistics(sequences[0][0]);

			double mean_of_the_number_of_pairwise_differences,variance_of_the_number_of_pairwise_differences;
			boost::property_tree::ptree fpopulation;

			boost::property_tree::ptree fchromosomes;
			fpopulation.put("name",this->name());

			for(uint32_t cid = 0U; cid < sequences.size(); ++cid){
				boost::property_tree::ptree fchromosome;
				fchromosome.put("id",cid);

				for(size_t gid = 0U; gid < sequences[cid].size(); ++gid){
					boost::property_tree::ptree fgenes;
					boost::property_tree::ptree fgene;
					fgene.put("id",gid);

					boost::property_tree::ptree findices;
					
//					findices.put("number-of-haplotypes",this->number_of_haplotypes(sequences_str[cid][gid]));
//					findices.put("number-of-segregating-sites",this->number_of_segregating_sites(sequences_str[cid][gid]));
//					tie(mean_of_the_number_of_pairwise_differences,variance_of_the_number_of_pairwise_differences)=this->pairwise_statistics(sequences_str[cid][gid]);
					
//					findices.put("number-of-haplotypes",this->number_of_haplotypes_seq(sequences[cid][gid]));
//					findices.put("number-of-segregating-sites",this->number_of_segregating_sites_seq(sequences[cid][gid]));
//					tie(mean_of_the_number_of_pairwise_differences,variance_of_the_number_of_pairwise_differences)=this->pairwise_statistics_seq(sequences[cid][gid]);
//					
					findices.put("number-of-haplotypes",this->number_of_haplotypes(sequences_str[cid][gid]));
					findices.put("number-of-segregating-sites",this->number_of_segregating_sites(sequences_str[cid][gid]));
					tie(mean_of_the_number_of_pairwise_differences, variance_of_the_number_of_pairwise_differences) = this->pairwise_statistics_seq(sequences[cid][gid]);
					
					findices.put("mean-of-the-number-of-pairwise-differences", mean_of_the_number_of_pairwise_differences);
					findices.put("variance-of-the-number-of-pairwise-differences", variance_of_the_number_of_pairwise_differences);

					findices.put("tajima-d-statistics", this->tajima_d_statistics(double(sequences[cid][gid].size()),
																									 findices.get<double>("number-of-segregating-sites"),
																									 mean_of_the_number_of_pairwise_differences));
					fgene.push_back(std::make_pair("indices",findices));
					fgenes.push_back(std::make_pair("",fgene));
					fchromosome.push_back(make_pair("genes",fgenes));
				}
				fchromosomes.push_back(make_pair("",fchromosome));
			}
			fpopulation.push_back(std::make_pair("chromosomes",fchromosomes));					

			cout<<"Sample::indices - End\n";
			return(fpopulation);
		}
		
		/*indices*/

};
#endif
