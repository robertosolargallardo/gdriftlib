#include "Individual.h"

/*
// Total of genes (effective total, considering chromosomes AND ploidy)
unsigned int Individual::n_gens = 0;
// All this numbers can be static if we are using a single specie
// Number of chromosomes sets
unsigned int Individual::ploidy = 0;
// Number of chromosomes per set
unsigned int Individual::n_chr = 0;
// Number of genes per chromosome (array, accumulated)
unsigned int *Individual::gens_chr = NULL;
// Number of genes per ploidy (the sum of the above)
unsigned int Individual::gens_ploidy = 0;
// Mutation rate per gene
double *Individual::mut_rate = NULL;
// Number of nucleotides per gene
//unsigned int *Individual::gen_len;
*/

mutex Individual::internal_mutex;
/*
Individual::Individual(const uint32_t &_id, const boost::property_tree::ptree &_findividual){
	id = _id;
	if(n_gens == 0){
		gens = NULL;
	}
	else{
//		cout<<"Individual - Reservando espacio para "<<n_gens<<" genes\n";
		gens = new VirtualSequence*[n_gens];
		for(unsigned int i = 0; i < n_gens; ++i){
			gens[i] = NULL;
		}
	}
}
Individual::Individual(const uint32_t &_id, const Ploidy &_ploidy, const uint32_t &_n_chromosomes){
	id = _id;
	if(n_gens == 0){
		gens = NULL;
	}
	else{
//		cout<<"Individual - Reservando espacio para "<<n_gens<<" genes\n";
		gens = new VirtualSequence*[n_gens];
		for(unsigned int i = 0; i < n_gens; ++i){
			gens[i] = NULL;
		}
	}
}
*/

Individual::Individual(){
	id = 0;
	n_gens = 0;
	ploidy = 0;
	n_chr = 0;
	gens_ploidy = 0;
	gens_chr = NULL;
	gens = NULL;
}

Individual::Individual(const Individual &original){
//	cout<<"Individual - Inicio (copiando)\n";
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
//		cout<<"Individual - Reservando espacio para "<<n_gens<<" genes\n";
		gens = new VirtualSequence*[n_gens];
		for(unsigned int i = 0; i < n_gens; ++i){
			gens[i] = NULL;
		}
	}
	// Notar que lo que sigue NO es recomendable por seguridad
	// En este caso es necesario, setParent DEBE recibir un non-const para increase de sus genes
	setParent(const_cast<Individual&>(original));
}

Individual::Individual(unsigned int _id, const Profile &profile){
//	cout<<"Individual - Inicio (creando individuo "<<_id<<")\n";
	id = _id;
	n_gens = profile.n_gens;
	ploidy = profile.ploidy;
	n_chr = profile.n_chr;
	gens_ploidy = profile.gens_ploidy;
//	cout<<"Individual - n_gens: "<<n_gens<<", ploidy: "<<(unsigned int)ploidy<<", n_chr: "<<(unsigned int)n_chr<<", gens_ploidy: "<<gens_ploidy<<"\n";
	gens_chr = new unsigned short[n_chr];
//	cout<<"Individual - memcpy...\n";
	memcpy(gens_chr, profile.gens_chr, n_chr*sizeof(short));
	// Note that Profile stores the real number of genes per chromosome
	// Individual needs the accumulated to accelerate its searches
//	cout<<"Individual - acumulando...\n";
	for(unsigned int i = 1; i < n_chr; ++i){
		gens_chr[i] += gens_chr[i-1];
	}
	
	if(n_gens == 0){
		gens = NULL;
	}
	else{
//		cout<<"Individual - Reservando espacio para "<<n_gens<<" genes\n";
		gens = new VirtualSequence*[n_gens];
		for(unsigned int i = 0; i < n_gens; ++i){
			gens[i] = NULL;
		}
	}
//	cout<<"Individual - Fin\n";
}
Individual::~Individual(){
//	cout<<"Individual::~Individual \n";
	if(gens != NULL){
		for(unsigned int i = 0; i < n_gens; ++i){
			if(gens[i] != NULL){
				gens[i]->decrease();
			}
		}
		delete [] gens;
	}
	gens = NULL;
	if(gens_chr != NULL){
		delete [] gens_chr;
		gens_chr = NULL;
	}
}

/*
void Individual::setParameters(const boost::property_tree::ptree &findividual){
	
	// Protego este metodo para thread-safe
	lock_guard<mutex> lock(internal_mutex);
	
	cout<<"Individual::setParameters - Inicio\n";
	ploidy = Ploidy(findividual.get<uint32_t>("ploidy"));
	n_chr = findividual.get_child("chromosomes").size();
	if(gens_chr != NULL){
		delete [] gens_chr;
	}
	cout<<"Individual::setParameters - Guardando "<<n_chr<<" chromosomas para Ploidy "<<ploidy<<"\n";
	gens_chr = new unsigned int[n_chr];
	
	unsigned int total_gens = 0;
	uint32_t cid = 0;
	for(auto fchromosome : findividual.get_child("chromosomes")){
		total_gens += fchromosome.second.get_child("genes").size();
		gens_chr[cid] = total_gens;
		// cout<<"Individual::setParameters - Total genes hasta chr["<<cid<<"]: "<<total_gens<<"\n";
		++cid;
	}
	gens_ploidy = total_gens;
	n_gens = ploidy*gens_ploidy;
	cout<<"Individual::setParameters - gens_ploidy: "<<gens_ploidy<<", n_gens: "<<n_gens<<"\n";
	mut_rate = new double[total_gens];
//	gen_len = new unsigned int[total_gens];
	
	total_gens = 0;
	for(auto fchromosome : findividual.get_child("chromosomes")){
		cid = fchromosome.second.get<uint32_t>("id");
		for(auto fgene : fchromosome.second.get_child("genes")){
			mut_rate[total_gens] = fgene.second.get<double>("mutation.rate");
//			gen_len[total_gens] = fgene.second.get<double>("nucleotides");
			cout<<"Individual::setParameters - mut_rate["<<total_gens<<"]: "<<mut_rate[total_gens]<<" (chr "<<cid<<")\n";
			++total_gens;
		}
	}
	
	cout<<"Individual::setParameters - Fin\n";
	
}
*/

uint32_t Individual::getId() const{
	return id;
}

// Por que es necesario esto?
// Creo que en nuevo modelo este metodo no es necesario
void Individual::clear(){
	if(gens != NULL){
		for(unsigned int i = 0; i < n_gens; ++i){
			if(gens[i] != NULL){
				gens[i]->decrease();
				gens[i] = NULL;
			}
		}
	}
}






















