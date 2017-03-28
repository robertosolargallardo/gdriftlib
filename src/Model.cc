#include "Model.h" 

namespace model{
	
	template<>
	void run<WRIGHTFISHER,HAPLOID>(Population* &_src, Population* &_dst, Pool* &_pool, Individual::Profile *profile){

//		cout<<"Model::run (HAPLOID) - Iterando por "<<_dst->size()<<" individuos ("<<_src->at(0)->getChromosomes()<<" chromosomes, "<<_src->at(0)->getGenes(0)<<" genes in ch0)\n";
		NanoTimer timer;
		unsigned int mutations = 0;
	
		uniform_int_distribution<> src_dist(0, _src->size() - 1);
		uniform_int_distribution<> dst_dist(0, _dst->size() - 1);
		
		// En el nuevo modelo, en lugar de iterar por individuo y gen para mutar, decidimos el numero de mutaciones primero y buscamos donde mutar la poblacion
		// Este proceso lo realizo por gen (global a la poblacion completa) para considerar las diferentes tasas de mutacion
		// Para cada gen, decido el numero de mutaciones totales
		// Para cada mutacion, decido al azar una posicion en la posicion completa (es decir, en largo_gen x n_individuos)
		// Luego aplico la mutacion normalmente en la referencia del gen del individuo escogido
		
//		Individual *ind = _src->at(0);
		// Padre elegido para el individuo de _dst
		Individual *parent = NULL;
		// Secuencia del gen que se modifica
		VirtualSequence *seq = NULL;
		
		// En esta version intermedia, primero creo la poblacion dst sin mutaciones para luego aplicar mutaciones al poblacion completa
		// Para generar dst, simplemente intero por cada individuo escogiendo un padre para cada uno y copiando los datos
		// Quizas eso se pueda hace de modo mas eficiente
		
		for(unsigned int id = 0; id < _dst->size(); ++id){
			parent = _src->at(src_dist(rng));
			_dst->at(id)->setParent(parent);
		}
//		cout<<"Model::run - Padres asignados en "<<timer.getMilisec()<<" ms\n";
		
		for(unsigned int chrid = 0; chrid < profile->getChromosomes(); ++chrid){
			for(unsigned int genid = 0; genid < profile->getGenes(chrid); ++genid){
//				double rate = gen->mutation_rate();
//				cout<<"Model::run - Iniciando mutacion de chr "<<chrid<<", gen "<<genid<<"\n";
//				double rate = ind->mutationRate(genid, chrid);
				double rate = profile->mutationRate(genid, chrid);
//				cout<<"Model::run - rate: "<<rate<<"\n";
//				unsigned int length = ind->getGene(genid, chrid, 0)->length();
				unsigned int length = profile->geneLength(genid, chrid);
//				cout<<"Model::run - length: "<<length<<"\n";
				// Calcular el total de mutaciones que se realizaran para este gen en TODA la poblacion
				// Notar que se puede hacer algo mas sofisticado aqui
				// Por ahora, solo multiplico la probabilidad de mutacion, por el largo del gen, por toda la poblacion
//				unsigned int total_muts = (unsigned int)(rate * length * _dst->size());
				// Version binomial (Notar que considero rate como la prob de mutacion POR NUCLEOTIDO de cada individuo)
				binomial_distribution<unsigned int> binomial_dist(length * _dst->size(), rate);
				unsigned int total_muts = binomial_dist(rng);
//				cout<<"Model::run - total_muts: "<<total_muts<<" ("<<(length * _dst->size())<<", "<<rate<<")\n";
				
				for(unsigned int mut = 0; mut < total_muts; ++mut){
					// Escoger individuo para mutar
					unsigned int mut_pos = dst_dist(rng);
//					cout<<"Model::run - mut_pos: "<<mut_pos<<" de "<<_dst->size()<<"\n";
					
					// Creo una nueva secuencia con el gen actual del individuo para mutar y reemplazar
					VirtualSequence *original = _dst->at(mut_pos)->getGene(genid, chrid, 0);
					seq = new VirtualSequence(*original);
//					cout<<"Model::run - mutate...\n";
					seq->mutate();
					++mutations;
//					cout<<"Model::run - push...\n";
					_pool->push(chrid, genid, seq);
//					cout<<"Model::run - setGene...\n";
					_dst->at(mut_pos)->setGene(genid, chrid, 0, seq);
//					cout<<"Model::run - Fin\n";
				}
			}
		}
		
//		cout<<"Model::run - Terminado ("<<timer.getMilisec()<<" ms, mutations: "<<mutations<<")\n";
		
	}// Fin run
	template<>
	void run<WRIGHTFISHER,DIPLOID>(Population* &_src, Population* &_dst, Pool* &_pool, Individual::Profile *profile){
	
//		cout<<"Model::run (DIPLOID) - Iterando por "<<_dst->size()<<" individuos ("<<_src->at(0)->getChromosomes()<<" chromosomes, "<<_src->at(0)->getGenes(0)<<" genes in ch0)\n";
		NanoTimer timer;
		unsigned int mutations = 0;
		
		uniform_int_distribution<> src_dist(0, _src->size() - 1);
		uniform_int_distribution<> dst_dist(0, _dst->size() - 1);
		
//		Individual *ind = _src->at(0);
		// Padres elegidos para el individuo de _dst
		Individual *parent1 = NULL;
		Individual *parent2 = NULL;
		// Secuencia del gen que se modifica
		VirtualSequence *seq = NULL;
		
		for(unsigned int id = 0; id < _dst->size(); ++id){
			parent1 = _src->at(src_dist(rng));
			parent2 = _src->at(src_dist(rng));
			_dst->at(id)->setParents(parent1, parent2);
		}
//		cout<<"Model::run - Padres asignados en "<<timer.getMilisec()<<" ms\n";
		
		// Mutaciones
		for(unsigned int plo = 0; plo < profile->getPloidy(); ++plo){
			for(unsigned int chrid = 0; chrid < profile->getChromosomes(); ++chrid){
				for(unsigned int genid = 0; genid < profile->getGenes(chrid); ++genid){
//					double rate = gen->mutation_rate();
//					cout<<"Model::run - Iniciando mutacion de chr "<<chrid<<", gen "<<genid<<", plo: "<<plo<<"\n";
					double rate = profile->mutationRate(genid, chrid);
//					cout<<"Model::run - rate: "<<rate<<"\n";
//					unsigned int length = ind->getGene(genid, chrid, plo)->length();
					unsigned int length = profile->geneLength(genid, chrid);
//					cout<<"Model::run - length: "<<length<<"\n";
					// Calcular el total de mutaciones que se realizaran para este gen en TODA la poblacion
					// Notar que se puede hacer algo mas sofisticado aqui
					// Por ahora, solo multiplico la probabilidad de mutacion, por el largo del gen, por toda la poblacion
//					unsigned int total_muts = (unsigned int)(rate * length * _dst->size());
					// Version binomial (Notar que considero rate como la prob de mutacion POR NUCLEOTIDO de cada individuo)
					binomial_distribution<unsigned int> binomial_dist(length * _dst->size(), rate);
					unsigned int total_muts = binomial_dist(rng);
//					cout<<"Model::run - total_muts: "<<total_muts<<"\n";
					for(unsigned int mut = 0; mut < total_muts; ++mut){
						// Escoger individuo para mutar
						unsigned int mut_pos = dst_dist(rng);
					
						// Creo una nueva secuencia con el gen actual del individuo para mutar y reemplazar
						VirtualSequence *original = _dst->at(mut_pos)->getGene(genid, chrid, plo);
						seq = new VirtualSequence(*original);
						seq->mutate();
						++mutations;
						_pool->push(chrid, genid, seq);
						_dst->at(mut_pos)->setGene(genid, chrid, plo, seq);
					}
				}
			}
		}
		
//		cout<<"Model::run - Terminado ("<<timer.getMilisec()<<" ms, mutations: "<<mutations<<")\n";
		
	}// Fin run
}















