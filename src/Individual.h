#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_
#include "Chromosome.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

enum Ploidy{HAPLOID=1,DIPLOID=2,TRIPLOID=3,TETRAPLOID=4,PENTAPLOID=5,HEXAPLOID=6,HEPTAPLOID=7,OCTAPLOID=8};

class Individual{
	private:
		uint32_t		 _id;
//		Ploidy			_ploidy;
//		uint32_t		 _n_chromosomes;
//		Chromosome***  _chromosomes;
		
		// Total of genes (esto tambien puede ser static)
		static unsigned int n_gens;
		// Reference to the genes (as VirtualSequences)
		//  - To get the gen of a (ploidy, chr, gen_pos): gens + gens_ploidy*ploidy + gens_chr[chr] + gen_pos
		VirtualSequence **gens;
		
		// All this numbers can be static if we are using a single specie
		// Number of chromosomes sets
		static unsigned int ploidy;
		// Number of chromosomes per set
		static unsigned int n_chr;
		// Number of genes per chromosome (array, accumulated)
		static unsigned int *gens_chr;
		// Number of genes per ploidy (the sum of the above)
		static unsigned int gens_ploidy;
		// Mutation rate per gene
		static double *mut_rate;
		
	public:
		Individual(const uint32_t&,const boost::property_tree::ptree&);
		Individual(const uint32_t&,const Ploidy&,const uint32_t&);
		Individual(const Individual&);
		uint32_t id(void) const;
		uint32_t n_chromosomes(void) const;
//		Chromosome**& chromosome(const uint32_t&);
//		Ploidy ploidy(void) const;
		~Individual(void);
		void clear(void);
		
		// Metodos de acceso a nuevos miembros de clase
		
		// Returns the gene of an absolute position
		inline VirtualSequence *getGene(unsigned int pos){
			if(pos >= n_gens){
				cerr<<"VirtualSequence::getGene - Error, posicion "<<pos<<" invalida\n";
				return NULL;
			}
			return gens[pos];
		}
		
		inline unsigned int genePosition(unsigned int pos, unsigned int chr, unsigned int plo){
			if(plo >= ploidy || chr >= n_chr){
				cerr<<"VirtualSequence::genePosition - Error, datos invalidos (pos: "<<pos<<", chr: "<<chr<<", plo: "<<plo<<")\n";
				return 0;
			}
			if(chr == 0){
				// Si es el primer cromosoma, no habia cromosomas anteriores
				return (pos + gens_ploidy * plo);
			}
			// Si NO es el primero, hay que sumar los genes de los cromosomas anteriores
			return (pos + gens_chr[chr - 1] + gens_ploidy * plo);
		}
		
		// Returns the gene of the position pos of a chromosome chr, of a ploidy plo
		inline VirtualSequence *getGene(unsigned int pos, unsigned int chr, unsigned int plo){
			return getGene( genePosition(pos, chr, plo) );
		}
		
		// Sets the gene of an absolute position
		inline void setGene(unsigned int pos, VirtualSequence *new_gene){
			if(pos >= n_gens){
				cerr<<"VirtualSequence::setGene - Error, posicion "<<pos<<" invalida\n";
				return;
			}
			if( gens[pos] != NULL ){
				gens[pos]->decrease();
			}
			gens[pos] = new_gene;
			gens[pos]->increase();
		}
		
		// Sets the gene of the position pos of a chromosome chr, of a ploidy plo
		inline void setGene(unsigned int pos, unsigned int chr, unsigned int plo, VirtualSequence *new_gene){
			pos = genePosition(pos, chr, plo);
			setGene(pos, new_gene);
		}
		
		inline unsigned int getPloidy(){
			return ploidy;
		}
		
		// Returns the number of chromosome
		inline unsigned int getChromosomes(){
			return n_chr;
		}
		
		// Returns the number of genes of a chromosome
		inline unsigned int getGenes(unsigned int chr){
			if(chr >= n_chr){
				cerr<<"VirtualSequence::setGene - Error, chr "<<chr<<" invalido\n";
				return 0;
			}
			if(chr == 0){
				return gens_chr[0];
			}
			return (gens_chr[chr] - gens_chr[chr-1]);
		}
		
		// Returns the mutarion rate of a gene, of a chromosome
		inline double mutationRate(unsigned int gen, unsigned int chr){
//			cout<<"Individual::mutationRate - genePosition("<<gen<<", "<<chr<<", 0)...\n";
			unsigned int pos = genePosition(gen, chr, 0);
//			cout<<"Individual::mutationRate - pos: "<<pos<<"\n";
//			cout<<"Individual::mutationRate - res "<<mut_rate[pos]<<"\n";
			return mut_rate[pos];
		}
		
		// Fin Metodos de acceso
		
		/*
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
		*/
		
		inline void setParent(Individual *parent){
			for(unsigned int i = 0; i < n_gens; ++i){
				if(gens[i] != NULL){
					gens[i]->decrease();
				}
				gens[i] = parent->getGene(i);
				gens[i]->increase();
			}
//			for(unsigned int i = 0; i < _n_chromosomes; ++i){
//				for(unsigned int j = 0; j < _chromosomes[i][0]->n_genes(); ++j){
//					_chromosomes[i][0]->gene(j)->reference(individual->chromosome(i)[0]->gene(j)->reference());
//				}
//			}
		}
		
		static void setParameters(const boost::property_tree::ptree &findividual);
		
};















#endif
