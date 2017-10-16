#include "Individual.h"

//mutex Individual::internal_mutex;

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






















