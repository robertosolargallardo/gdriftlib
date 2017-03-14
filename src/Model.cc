#include "Model.h" 

namespace model{
	template<>
	void run<WRIGHTFISHER,HAPLOID>(Population* &_src, Population* &_dst, Pool* &_pool){
		uniform_int_distribution<> src_dist(0, _src->size() - 1);
		uniform_int_distribution<> dst_dist(0, _dst->size() - 1);
//		uniform_real_distribution<double> mutation_rate(0.0, 1.0);

//		Reference* reference=nullptr;
//		Reference* validator=nullptr;
//		VirtualSequence* validator=nullptr;

		
		cout<<"Model::run - ITerando por "<<_dst->size()<<" individuos ("<<_src->at(0)->n_chromosomes()<<" chromosomes, "<<_src->at(0)->chromosome(0)[0]->n_genes()<<" genes in ch0)\n";
		NanoTimer timer;
		
		// En el nuevo modelo, en lugar de iterar por individuo y gen para mutar, decidimos el numero de mutaciones primero y buscamos donde mutar la poblacion
		// Este proceso lo realizo por gen (global a la poblacion completa) para considerar las diferentes tasas de mutacion
		// Para cada gen, decido el numero de mutaciones totales
		// Para cada mutacion, decido al azar una posicion en la posicion completa (es decir, en largo_gen x n_individuos)
		// Luego aplico la mutacion normalmente en la referencia del gen del individuo escogido
		
		
		// Individuo arbitrario usado solo para contar chromosomas y genes
		Individual *counter = _src->at(0);
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
//			for(unsigned int chrid = 0; chrid < parent->n_chromosomes(); ++chrid){
//				Chromosome *chr = parent->chromosome(chrid)[0];
//				for(unsigned int genid = 0; genid < chr->n_genes(); ++genid){
//					Gene *gen = chr->gene(genid);
//					_dst->at(id)->chromosome(chrid)[0]->gene(genid)->reference(gen->reference());
//				}
//			}
		}
		cout<<"Model::run - Padres asignados en "<<timer.getMilisec()<<" ms\n";
		
		for(unsigned int chrid = 0; chrid < counter->n_chromosomes(); ++chrid){
			Chromosome *chr = counter->chromosome(chrid)[0];
			for(unsigned int genid = 0; genid < chr->n_genes(); ++genid){
				Gene *gen = chr->gene(genid);
				double rate = gen->mutation_rate();
				unsigned int length = gen->reference()->length();
				unsigned int total_muts = (unsigned int)(rate * length * _dst->size());
				cout<<"Model::run - Mutando chr "<<chrid<<", gen "<<genid<<", rate: "<<rate<<", total_muts: "<<total_muts<<"\n";
//				dst_dist
			}
		}
		
		/*
		VirtualSequence* reference=nullptr;
		Individual* individual=nullptr;
		
		for(uint32_t id=0U; id<_dst->size(); id++){
			individual=_src->at(src_dist(rng));
			
			for(uint32_t cid=0;cid<individual->n_chromosomes();cid++){
				for(uint32_t gid=0;gid<individual->chromosome(cid)[0]->n_genes();gid++){
					if(mutation_rate(rng)<=individual->chromosome(cid)[0]->gene(gid)->mutation_rate()){
//						reference=new Reference(*(individual->chromosome(cid)[0]->gene(gid)->reference()));
						
						reference = new VirtualSequence(*(individual->chromosome(cid)[0]->gene(gid)->reference()));
						reference->mutate();
						
						_pool->push(cid,gid,reference);
//						validator=_pool->push(cid,gid,reference);
//						if(validator!=nullptr){
//							delete reference;
//							reference=validator;
//						}
						
						_dst->at(id)->chromosome(cid)[0]->gene(gid)->reference(reference);
					}
					else{
//						reference = new VirtualSequence(*(individual->chromosome(cid)[0]->gene(gid)->reference()));
//						_dst->at(id)->chromosome(cid)[0]->gene(gid)->reference(reference);
						_dst->at(id)->chromosome(cid)[0]->gene(gid)->reference(individual->chromosome(cid)[0]->gene(gid)->reference());
					}
				}
			}
		}
		*/
		cout<<"Model::run - Terminado ("<<timer.getMilisec()<<" ms)\n";
		
	}
	template<>
	void run<WRIGHTFISHER,DIPLOID>(Population* &_src,Population* &_dst,Pool* &_pool){
		constexpr uint32_t PARENTS=2U;
		uniform_int_distribution<> coin(0U,1U);
		uniform_int_distribution<> uniform(0U,_src->size()-1U);
		uniform_real_distribution<double> mutation_rate(0.0,1.0);

//		Reference* reference=nullptr;
//		Reference* validator=nullptr;
		VirtualSequence* reference=nullptr;
		VirtualSequence* validator=nullptr;

		array<Individual*,PARENTS> individual={nullptr,nullptr};
		array<Gene*,PARENTS> gene={nullptr,nullptr};

		for(uint32_t id=0U;id<_dst->size();id++){
			individual[0]=_src->at(uniform(rng));
			individual[1]=_src->at(uniform(rng));

			for(uint32_t cid=0;cid<individual[0]->n_chromosomes();cid++){
				for(uint32_t gid=0;gid<individual[0]->chromosome(cid)[0]->n_genes();gid++){
					gene[0]=individual[0]->chromosome(cid)[coin(rng)]->gene(gid);
					gene[1]=individual[1]->chromosome(cid)[coin(rng)]->gene(gid);
					
					for(uint32_t i=0U;i<uint32_t(DIPLOID);i++){
						if(mutation_rate(rng)<=gene[i]->mutation_rate()){
//							reference=new Reference(*(gene[i]->reference()));
							reference=new VirtualSequence(*(gene[i]->reference()));
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
