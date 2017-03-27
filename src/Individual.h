#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <thread>
#include <mutex>

#include "VirtualSequence.h"

enum Ploidy{HAPLOID=1,DIPLOID=2,TRIPLOID=3,TETRAPLOID=4,PENTAPLOID=5,HEXAPLOID=6,HEPTAPLOID=7,OCTAPLOID=8};

extern mt19937 rng;

class Individual{
	private:
		uint32_t _id;
		
		static mutex internal_mutex;
		
		// Reference to the genes (as VirtualSequences)
		//  - To get the gen of a (ploidy, chr, gen_pos): gens + gens_ploidy*ploidy + gens_chr[chr] + gen_pos
		VirtualSequence **gens;
		
		// All this numbers can be static if we are using a single specie
		// Total of genes (effective total, considering chromosomes AND ploidy)
		static unsigned int n_gens;
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
		// Number of nucleotides per gene
//		static unsigned int *gen_len;
		
	public:
		Individual(const uint32_t&,const boost::property_tree::ptree&);
		Individual(const uint32_t&,const Ploidy&,const uint32_t&);
		Individual(const Individual&);
		uint32_t id(void) const;
		uint32_t n_chromosomes(void) const;
		~Individual(void);
		void clear(void);
		
		// Metodos de acceso a nuevos miembros de clase
		
		// Returns the gene of an absolute position
		inline VirtualSequence *getGene(unsigned int pos){
			if(pos >= n_gens){
				cerr<<"Individual::getGene - Error, posicion "<<pos<<" invalida\n";
				return NULL;
			}
			return gens[pos];
		}
		
		inline unsigned int genePosition(unsigned int pos, unsigned int chr, unsigned int plo){
			if(plo >= ploidy || chr >= n_chr){
				cerr<<"Individual::genePosition - Error, datos invalidos (pos: "<<pos<<", chr: "<<chr<<", plo: "<<plo<<")\n";
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
				cerr<<"Individual::setGene - Error, posicion "<<pos<<" invalida\n";
				return;
			}
//			cout<<"Individual::setGene - Inicio (pos: "<<pos<<" de "<<n_gens<<")\n";
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
				cerr<<"Individual::getGene - Error, chr "<<chr<<" invalido\n";
				return 0;
			}
			if(chr == 0){
//				cout<<"Individual::getGene - chr "<<chr<<", "<<gens_chr[0]<<" genes\n";
				return gens_chr[0];
			}
//			cout<<"Individual::getGene - chr "<<chr<<", "<<(gens_chr[chr] - gens_chr[chr-1])<<" genes\n";
			return (gens_chr[chr] - gens_chr[chr-1]);
		}
		
		// Returns the TOTAL number of genes (the real total, considering chromosomes and ploidy)
		inline unsigned int getGenes(){
			return n_gens;
		}
		
		// Returns the mutarion rate of a gene, of a chromosome
		inline double mutationRate(unsigned int gen, unsigned int chr){
			unsigned int pos = genePosition(gen, chr, 0);
			return mut_rate[pos];
		}
		
		// Returns the mutarion rate of a gene in absolute position
		// Note that the rates repeat themselves for different ploidy sets (so we take % gens_ploidy)
		inline double mutationRate(unsigned int gen){
			return mut_rate[ (gen % gens_ploidy) ];
		}
		
//		// Returns the number of nucleotides of a gene, of a chromosome
//		inline static unsigned int geneLength(unsigned int gen, unsigned int chr){
//			unsigned int pos = genePosition(gen, chr, 0);
//			return gen_len[pos];
//		}
		
		// Returns the chromosome id of a gen in absolute position
		// This can be implemented with a binary search (log), for new its lineal in num of chromosomes
		inline unsigned int getChromosome(unsigned int pos){
			if(pos >= n_gens){
				return 0;
			}
			pos %= gens_ploidy;
			unsigned int res = 0;
			while( gens_chr[res] < pos ){
				++res;
			}
			return res;
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
		}
		
		inline void setParents(Individual *parent1, Individual *parent2){
			
			// Esta primera version solo funciona para diploides
			if(ploidy != 2){
				cerr<<"Individual::setParents - Error, metodo implementado exclusivamente para Diploides (ploidy: "<<ploidy<<")\n";
				return;
			}
			
//			uniform_int_distribution<> coin(0, 1);
			unsigned int rand_bits = rng();
			unsigned int mask = 0x80000000;
			
			// Conjunto de cromosomas de padre 1
			for(unsigned int i = 0; i < gens_ploidy; ++i){
				if(gens[i] != NULL){
					gens[i]->decrease();
				}
				if( rand_bits & mask ){
					gens[i] = parent1->getGene(i);
				}
				else{
					gens[i] = parent1->getGene(i + gens_ploidy);
				}
				if( (mask >>= 1) == 0 ){
					rand_bits = rng();
					mask = 0x80000000;
				}
				gens[i]->increase();
			}
			
			// Conjunto de cromosomas de padre 2
			for(unsigned int i = 0; i < gens_ploidy; ++i){
				if(gens[i + gens_ploidy] != NULL){
					gens[i + gens_ploidy]->decrease();
				}
				if( rand_bits & mask ){
					gens[i + gens_ploidy] = parent2->getGene(i);
				}
				else{
					gens[i + gens_ploidy] = parent2->getGene(i + gens_ploidy);
				}
				if( (mask >>= 1) == 0 ){
					rand_bits = rng();
					mask = 0x80000000;
				}
				gens[i + gens_ploidy]->increase();
			}
			
		}
		
		static void setParameters(const boost::property_tree::ptree &findividual);
		
};















#endif
