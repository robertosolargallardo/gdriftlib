#include "Individual.h"

		// Total of genes (esto tambien puede ser static)
		unsigned int Individual::n_gens;
		
		// All this numbers can be static if we are using a single specie
		// Number of chromosomes sets
		unsigned int Individual::ploidy;
		// Number of chromosomes per set
		unsigned int Individual::n_chr;
		// Number of genes per chromosome (array, accumulated)
		unsigned int *Individual::gens_chr;
		// Number of genes per ploidy (the sum of the above)
		unsigned int Individual::gens_ploidy;
		// Mutation rate per gene
		double *Individual::mut_rate;
		

Individual::Individual(const uint32_t &_id, const boost::property_tree::ptree &_findividual){
	this->_id=_id;
	/*
	this->_ploidy=Ploidy(_findividual.get<uint32_t>("ploidy"));
	this->_n_chromosomes=_findividual.get_child("chromosomes").size();
	this->_chromosomes=(Chromosome***)malloc(sizeof(Chromosome**)*this->_n_chromosomes);

	uint32_t cid;
	for(auto fchromosome : _findividual.get_child("chromosomes")){
		cid=fchromosome.second.get<uint32_t>("id");
		this->_chromosomes[cid]=(Chromosome**)malloc(sizeof(Chromosome*)*int(this->_ploidy));
		for(int pid=0;pid<int(this->_ploidy);pid++){
			this->_chromosomes[cid][pid]=new Chromosome(cid,fchromosome.second.get_child("genes").size());

			for(auto fgene : fchromosome.second.get_child("genes")){
				this->_chromosomes[cid][pid]->gene(fgene.second.get<uint32_t>("id"))=new Gene(fgene.second.get<uint32_t>("id"));
//				this->_chromosomes[cid][pid]->gene(fgene.second.get<uint32_t>("id"))->mutation_rate(fgene.second.get<double>("mutation-rate"));
				this->_chromosomes[cid][pid]->gene(fgene.second.get<uint32_t>("id"))->mutation_rate(fgene.second.get<double>("mutation.rate"));
			}
		}
	}
	*/
	
	if(n_gens == 0){
		gens = NULL;
	}
	else{
		gens = new VirtualSequence*[n_gens];
		for(unsigned int i = 0; i < n_gens; ++i){
			gens[i] = NULL;
		}
	}
	
}
Individual::Individual(const uint32_t &_id, const Ploidy &_ploidy, const uint32_t &_n_chromosomes){
	this->_id=_id;
	/*
	this->_ploidy=_ploidy;
	this->_n_chromosomes=_n_chromosomes;
	this->_chromosomes=(Chromosome***)malloc(sizeof(Chromosome**)*this->_n_chromosomes);

	for(uint32_t id=0U;id<this->_n_chromosomes;id++){
		this->_chromosomes[id]=(Chromosome**)malloc(sizeof(Chromosome*)*int(this->_ploidy));

		for(int pid=0;pid<int(this->_ploidy);pid++)
			this->_chromosomes[id][pid]=nullptr;
	}
	*/
	
	if(n_gens == 0){
		gens = NULL;
	}
	else{
		gens = new VirtualSequence*[n_gens];
		for(unsigned int i = 0; i < n_gens; ++i){
			gens[i] = NULL;
		}
	}
}
Individual::Individual(const Individual &_individual){
	this->_id=_individual._id;
	/*
	this->_ploidy=_individual._ploidy;
	this->_n_chromosomes=_individual._n_chromosomes;
	this->_chromosomes=(Chromosome***)malloc(sizeof(Chromosome**)*this->_n_chromosomes);

	for(uint32_t cid=0U;cid<this->_n_chromosomes;cid++){
		this->_chromosomes[cid]=(Chromosome**)malloc(sizeof(Chromosome*)*int(this->_ploidy));

		for(int pid=0;pid<int(this->_ploidy);pid++)
			this->_chromosomes[cid][pid]=new Chromosome(*(_individual._chromosomes[cid][pid]));
	}
	*/
	
	if(n_gens == 0){
		gens = NULL;
	}
	else{
		gens = new VirtualSequence*[n_gens];
		for(unsigned int i = 0; i < n_gens; ++i){
			gens[i] = NULL;
		}
	}
	setParent((Individual*)(&_individual));
}

void Individual::setParameters(const boost::property_tree::ptree &findividual){
//	cout<<"Individual::setParameters - Inicio\n";
	ploidy = Ploidy(findividual.get<uint32_t>("ploidy"));
	n_chr = findividual.get_child("chromosomes").size();
	if(gens_chr != NULL){
		delete [] gens_chr;
	}
//	cout<<"Individual::setParameters - Guardando "<<n_chr<<" chromosomas para Ploidy "<<ploidy<<"\n";
	gens_chr = new unsigned int[n_chr];
	
	unsigned int total_gens = 0;
//	for(unsigned int i = 0; i < n_chr; ++i){
//		total_gens += findividual.get_child("chromosomes").second.get_child("genes").size();
//		gens_chr[i] = total_gens;
//		cout<<"Individual::setParameters - Total genes hasta chr["<<i<<"]: "<<total_gens<<"\n";
//	}
	uint32_t cid = 0;
	for(auto fchromosome : findividual.get_child("chromosomes")){
		total_gens += fchromosome.second.get_child("genes").size();
		gens_chr[cid] = total_gens;
//		cout<<"Individual::setParameters - Total genes hasta chr["<<cid<<"]: "<<total_gens<<"\n";
	}
	gens_ploidy = total_gens;
	n_gens = ploidy*gens_ploidy;
//	cout<<"Individual::setParameters - gens_ploidy: "<<gens_ploidy<<", n_gens: "<<n_gens<<"\n";
	mut_rate = new double[total_gens];
	
	total_gens = 0;
	for(auto fchromosome : findividual.get_child("chromosomes")){
		cid = fchromosome.second.get<uint32_t>("id");
//		for(unsigned int pid = 0; pid < ploidy; pid++){
		for(auto fgene : fchromosome.second.get_child("genes")){
//			this->_chromosomes[cid][pid]->gene(fgene.second.get<uint32_t>("id"))->mutation_rate(fgene.second.get<double>("mutation.rate"));
			mut_rate[total_gens] = fgene.second.get<double>("mutation.rate");
//			cout<<"Individual::setParameters - mut_rate["<<total_gens<<"]: "<<mut_rate[total_gens]<<" (chr "<<cid<<")\n";
			++total_gens;
		}
//		}
	}
	
//	cout<<"Individual::setParameters - Fin\n";
	
	
}

//Chromosome**& Individual::chromosome(const uint32_t &_id){
//	return(this->_chromosomes[_id]);
//}
uint32_t Individual::id(void) const{
	return(this->_id);
}
//uint32_t Individual::n_chromosomes(void) const{
//	return(this->_n_chromosomes);
//}
//Ploidy Individual::ploidy(void) const{
//	return(this->_ploidy);
//}
Individual::~Individual(void){
	/*
	for(uint32_t i=0U;i<this->_n_chromosomes;i++){
		for(int j=0;j<int(this->_ploidy);j++)
			delete this->_chromosomes[i][j];
		free(this->_chromosomes[i]);
	}
	free(this->_chromosomes);
	*/
	if(gens != NULL){
		for(unsigned int i = 0; i < n_gens; ++i){
			if(gens[i] != NULL){
				gens[i]->decrease();
			}
		}
		delete [] gens;
	}
	gens = NULL;
	
}

// Por que es necesario esto?
// Creo que en nuevo modelo este metodo no es necesario
void Individual::clear(void){
//	for(uint32_t i=0U;i<this->_n_chromosomes;i++)
//		for(int j=0;j<int(this->_ploidy);j++)
//			for(uint32_t k=0;k<this->_chromosomes[i][j]->n_genes();k++)
//				this->_chromosomes[i][j]->gene(k)->clear();
	if(gens != NULL){
		for(unsigned int i = 0; i < n_gens; ++i){
			if(gens[i] != NULL){
				gens[i]->decrease();
				gens[i] = NULL;
			}
		}
	}
	
}






















