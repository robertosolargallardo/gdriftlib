#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <thread>
#include <mutex>

#include "VirtualSequence.h"

enum Ploidy{HAPLOID=1,DIPLOID=2,TRIPLOID=3,TETRAPLOID=4,PENTAPLOID=5,HEXAPLOID=6,HEPTAPLOID=7,OCTAPLOID=8};

class Individual{
	protected:
		
		static mutex internal_mutex;
		
		// Reference to the genes (as VirtualSequences)
		//  - To get the gen of a (ploidy, chr, gen_pos): gens + gens_ploidy*ploidy + gens_chr[chr] + gen_pos
		VirtualSequence **gens;
		
		/*
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
		*/
		
		unsigned int id;
		// Non static version (with smaller variables)
		// Total of genes (effective total, considering chromosomes AND ploidy)
		unsigned int n_gens;
		// Number of chromosomes sets
		unsigned char ploidy;
		// Number of chromosomes per set
		unsigned char n_chr;
		// Number of genes per chromosome (array, accumulated)
		unsigned short *gens_chr;
		// Number of genes per ploidy (the sum of the above)
		unsigned int gens_ploidy;
		
		// Mutation rate per gene
		// This should not be in individual, but in Model or Population (as all data related to the mutation model)
//		double *mut_rate;
		// Number of nucleotides per gene
//		static unsigned int *gen_len;
		
	public:
		// Forward declaration of Individual::Profile
		class Profile;
		
		Individual();
		Individual(unsigned int _id, const Profile &_profile);
		Individual(const Individual&);
		~Individual();
		
		Individual& operator=(const Individual& original){
			if (this != &original){
				// borrar, pedir, copiar
				//cout<<"Individual::operator= - Inicio\n";
				clear();
				if( gens_chr != NULL ){
					delete [] gens_chr;
					gens_chr = NULL;
				}
				if( gens != NULL ){
					delete [] gens;
					gens = NULL;
				}
				
				id = original.id;
				n_gens = original.n_gens;
				ploidy = original.ploidy;
				n_chr = original.n_chr;
				gens_ploidy = original.gens_ploidy;
				gens_chr = new unsigned short[n_chr];
				memcpy(gens_chr, original.gens_chr, n_chr*sizeof(short));
				if(n_gens == 0){
					gens = NULL;
				}
				else{
					//cout<<"Individual::operator= - Reservando espacio para "<<n_gens<<" genes\n";
					gens = new VirtualSequence*[n_gens];
					for(unsigned int i = 0; i < n_gens; ++i){
						gens[i] = NULL;
					}
				}
				// Notar que lo que sigue NO es recomendable por seguridad
				// En este caso es necesario, setParent DEBE recibir un non-const para increase de sus genes
				setParent(const_cast<Individual&>(original));
				
			}	
			return *this;
		}
		
		uint32_t getId() const;
		uint32_t n_chromosomes() const;
		void clear();
		
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
//			cout<<"Individual::setGene - Inicio (pos: "<<pos<<" de "<<n_gens<<", null? "<<(new_gene==NULL)<<")\n";
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
		
		inline unsigned int getGensPloidy(){
			return gens_ploidy;
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
		
		// Notar que lo ideal seria que parent fuese const
		// Lamentablemente, el increase / decrease de los genes lo impide
		inline void setParent(Individual &parent){
//			cout<<"Individual::setParent - Inicio\n";
			for(unsigned int i = 0; i < n_gens; ++i){
				if(gens[i] != NULL){
					gens[i]->decrease();
				}
				gens[i] = parent.getGene(i);
				if(gens[i] != NULL){
					gens[i]->increase();
				}
			}
//			cout<<"Individual::setParent - Fin\n";
		}
		
		// Notar que lo ideal seria que parent fuese const
		// Lamentablemente, el increase / decrease de los genes lo impide
		inline void setParents(Individual &parent1, Individual &parent2){
			
			// Esta primera version solo funciona para diploides
			if(ploidy != 2){
				cerr<<"Individual::setParents - Error, metodo implementado exclusivamente para Diploides (ploidy: "<<ploidy<<")\n";
				return;
			}
			
//			uniform_int_distribution<> coin(0, 1);
         static thread_local std::mt19937 rng;
			unsigned int rand_bits = rng();
			unsigned int mask = 0x80000000;
			
			// Conjunto de cromosomas de padre 1
			for(unsigned int i = 0; i < gens_ploidy; ++i){
				if(gens[i] != NULL){
					gens[i]->decrease();
				}
				if( rand_bits & mask ){
					gens[i] = parent1.getGene(i);
				}
				else{
					gens[i] = parent1.getGene(i + gens_ploidy);
				}
				if( (mask >>= 1) == 0 ){
					rand_bits = rng();
					mask = 0x80000000;
				}
				if(gens[i] != NULL){
					gens[i]->increase();
				}
			}
			
			// Conjunto de cromosomas de padre 2
			for(unsigned int i = 0; i < gens_ploidy; ++i){
				if(gens[i + gens_ploidy] != NULL){
					gens[i + gens_ploidy]->decrease();
				}
				if( rand_bits & mask ){
					gens[i + gens_ploidy] = parent2.getGene(i);
				}
				else{
					gens[i + gens_ploidy] = parent2.getGene(i + gens_ploidy);
				}
				if( (mask >>= 1) == 0 ){
					rand_bits = rng();
					mask = 0x80000000;
				}
				if(gens[i + gens_ploidy] != NULL){
					gens[i + gens_ploidy]->increase();
				}
			}
			
		}
		
//		static void setParameters(const boost::property_tree::ptree &findividual);
	
	// Simple class to store global information related to the individuals of the population
	// This includes number of chromosomes, genes, mutation, etc
	class Profile{
	public:
		// Total of genes (effective total, considering chromosomes AND ploidy)
		unsigned int n_gens;
		// Number of chromosomes sets
		unsigned char ploidy;
		// Number of chromosomes per set
		unsigned char n_chr;
		// Number of genes per ploidy (the sum of the above)
		unsigned int gens_ploidy;
		// Number of genes per chromosome (should they be the same across chromosome sets?, it is for now)
		unsigned short *gens_chr;
		// Mutation rate per gene
		double **mut_rate;
		unsigned int **gen_len;
		
		// Aqui debe almacenarse toda la informacion de mutaciones
		// Eso incluye potenciales tablas de prob de conversion por nucleotido para modelos mas complejos
		// Notar que eso, ademas, es por gen (quizas sea conveniente poner eso en una estructura en lugar de arreglos)
		// Tambien con tanta complejidad, quizas sea razonable separar esto en una clase independiente
		
		Profile(){
			n_gens = 0;
			ploidy = 0;
			n_chr = 0;
			gens_ploidy = 0;
			gens_chr = NULL;
			mut_rate = NULL;
			gen_len = NULL;
		}
		Profile(unsigned char _ploidy, unsigned char _n_chr, 
					const vector<unsigned int> &_gens_chr, 
					const vector<vector<double>> &_mut_rate = {}, 
					const vector<vector<unsigned int>> &_gen_len = {}
					){
			ploidy = _ploidy;
			n_chr = _n_chr;
			gens_chr = new unsigned short[n_chr];
			
			unsigned int total_gens = 0;
			for(unsigned int cid = 0; cid < n_chr; ++cid){
				gens_chr[cid] = _gens_chr[cid];
				total_gens += gens_chr[cid];
			}
			gens_ploidy = total_gens;
			n_gens = gens_ploidy * (unsigned int)ploidy;
			
			mut_rate = NULL;
			gen_len = NULL;
			
			if(!_mut_rate.empty()){
				mut_rate = new double*[n_chr];
				for(unsigned int cid = 0; cid < n_chr; ++cid){
					mut_rate[cid] = new double[gens_chr[cid]];
					for(unsigned int gid = 0; gid < gens_chr[cid]; ++gid){
						mut_rate[cid][gid] = _mut_rate[cid][gid];
					}
				}
			}
			
			if(!_gen_len.empty()){
				gen_len = new unsigned int*[n_chr];
				for(unsigned int cid = 0; cid < n_chr; ++cid){
					gen_len[cid] = new unsigned int[gens_chr[cid]];
					for(unsigned int gid = 0; gid < gens_chr[cid]; ++gid){
						gen_len[cid][gid] = _gen_len[cid][gid];
					}
				}
			}
			
		}
		Profile(const boost::property_tree::ptree &findividual){
//			cout<<"Profile - Inicio\n";
			ploidy = (unsigned char)(findividual.get<uint32_t>("ploidy"));
			n_chr = (unsigned char)(findividual.get_child("chromosomes").size());
//			cout<<"Profile - Guardando "<<(unsigned int)n_chr<<" chromosomas para Ploidy "<<(unsigned int)ploidy<<"\n";
			gens_chr = new unsigned short[n_chr];
	
			unsigned int total_gens = 0;
			unsigned int cid = 0;
			unsigned int gid = 0;
			for(auto fchromosome : findividual.get_child("chromosomes")){
				gens_chr[cid] = (unsigned short)(fchromosome.second.get_child("genes").size());
				total_gens += gens_chr[cid];
				++cid;
			}
			gens_ploidy = total_gens;
			n_gens = gens_ploidy * (unsigned int)ploidy;
//			cout<<"Profile - gens_ploidy: "<<gens_ploidy<<", n_gens: "<<n_gens<<"\n";
//			mut_rate = new double[total_gens];
			mut_rate = new double*[n_chr];
			gen_len = new unsigned int*[n_chr];
			for(unsigned int i = 0; i < n_chr; ++i){
				mut_rate[i] = new double[gens_chr[i]];
				gen_len[i] = new unsigned int[gens_chr[i]];
			}
	
			total_gens = 0;
			cid = 0;
			for(auto fchromosome : findividual.get_child("chromosomes")){
				gid = 0;
				for(auto fgene : fchromosome.second.get_child("genes")){
					boost::optional<boost::property_tree::ptree&> mutation_rate= fgene.second.get_child_optional("mutation.rate");
					if(!mutation_rate)
					   mut_rate[cid][gid]=0.0;
					else
					   mut_rate[cid][gid] = fgene.second.get<double>("mutation.rate");

					gen_len[cid][gid] = fgene.second.get<double>("nucleotides");
//					cout<<"Profile - mut_rate["<<cid<<"]["<<gid<<"]: "<<mut_rate[cid][gid]<<"\n";
					++gid;
				}
				++cid;
			}
		
//			cout<<"Profile - Fin\n";
		}
		~Profile(){
			if(gens_chr != NULL){
				delete [] gens_chr;
				gens_chr = NULL;
			}
			if(mut_rate != NULL){
				for(unsigned int i = 0; i < n_chr; ++i){
					if(mut_rate[i] != NULL){
						delete [] mut_rate[i];
					}
				}
				delete [] mut_rate;
				mut_rate = NULL;
			}
			if(gen_len != NULL){
				for(unsigned int i = 0; i < n_chr; ++i){
					if(gen_len[i] != NULL){
						delete [] gen_len[i];
					}
				}
				delete [] gen_len;
				gen_len = NULL;
			}
			n_gens = 0;
			ploidy = 0;
			n_chr = 0;
			gens_ploidy = 0;
		}
		
		// Returns the length of a gene, of a chromosome
		inline unsigned int geneLength(unsigned int gen, unsigned int chr){
			if(chr >= n_chr || gen >= gens_chr[chr]){
				return 0.0;
			}
			return gen_len[chr][gen];
		}
		
		// Returns the mutarion rate of a gene, of a chromosome
		inline double mutationRate(unsigned int gen, unsigned int chr){
			if(chr >= n_chr || gen >= gens_chr[chr]){
				return 0.0;
			}
			return mut_rate[chr][gen];
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
//				cerr<<"Individual::Profile::getGene - Error, chr "<<chr<<" invalido\n";
				return 0;
			}
			return gens_chr[chr];
		}
		

	};
		
};















#endif
