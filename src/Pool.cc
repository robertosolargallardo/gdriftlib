#include "Pool.h"
Pool::Pool(const boost::property_tree::ptree &_findividual){
	this->_findividual=_findividual;
	rng = new mt19937(std::chrono::system_clock::now().time_since_epoch().count());
	uint32_t cid,gid,nucleotides,number_of_segregating_sites,number_of_alleles; 
	for(auto fchromosome : _findividual.get_child("chromosomes")){
		cid=fchromosome.second.get<uint32_t>("id");
		for(auto fgene : fchromosome.second.get_child("genes")){
			nucleotides = fgene.second.get<uint32_t>("nucleotides");
			number_of_alleles = fgene.second.get<uint32_t>("number-of-alleles");
			number_of_segregating_sites = fgene.second.get<uint32_t>("number-of-segregating-sites");
			gid=fgene.second.get<uint32_t>("id");

		  	this->populate(cid, gid, nucleotides, number_of_alleles, number_of_segregating_sites);			
		}
	}
}
Individual* Pool::generate(const uint32_t &_id, const Individual::Profile &profile){
	// Crear idividuo vacio y escoger genes del pool
//	cout<<"Pool::generate - Inicio (_id: "<<_id<<")\n";
	Individual *individual = new Individual(_id, profile);
	//void setGene(unsigned int gen, unsigned int chr, unsigned int plo, VirtualSequence *new_gene);
	for(unsigned int plo = 0; plo < individual->getPloidy(); ++plo){
		for(unsigned int chr = 0; chr < individual->getChromosomes(); ++chr){
			for(unsigned int gen = 0; gen < individual->getGenes(chr); ++gen){
				uniform_int_distribution<> uniform(0, _pool[pair<uint32_t, uint32_t>(chr, gen)].size() - 1);
				VirtualSequence *seq = _pool[pair<uint32_t, uint32_t>(chr, gen)][uniform(*rng)];
				individual->setGene(gen, chr, plo, seq);
			}
		}
	}
//	cout<<"Pool::generate - Fin\n";
	return individual;
}
void Pool::regenerate(Individual *individual){
//	cout<<"Pool::regenerate - Inicio (_id: "<<_id<<")\n";
	individual->clear();
	for(unsigned int plo = 0; plo < individual->getPloidy(); ++plo){
		for(unsigned int chr = 0; chr < individual->getChromosomes(); ++chr){
			for(unsigned int gen = 0; gen < individual->getGenes(chr); ++gen){
				uniform_int_distribution<> uniform(0, _pool[pair<uint32_t, uint32_t>(chr, gen)].size() - 1);
				VirtualSequence *seq = _pool[pair<uint32_t, uint32_t>(chr, gen)][uniform(*rng)];
				individual->setGene(gen, chr, plo, seq);
			}
		}
	}
//	cout<<"Pool::regenerate - Fin\n";
}
VirtualSequence* Pool::push(const uint32_t &_cid,const uint32_t &_gid,VirtualSequence* _reference){
//	for(vector<VirtualSequence*>::iterator i=this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].begin();i!=this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].end();i++)
//		if((**i)==(*_reference)) return(*i);

	this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].push_back(_reference);
	return(nullptr);
}
uint32_t Pool::size(void){
	return(this->_pool.size());
}
void Pool::decrease_all(void){
	for(map<pair<uint32_t,uint32_t>,vector<VirtualSequence*>>::iterator i=this->_pool.begin();i!=this->_pool.end();i++)
		for_each(i->second.begin(),i->second.end(),[](VirtualSequence* &reference){reference->decrease();});
}
void Pool::release(void){
	unsigned int total_deletes = 0;
//	NanoTimer timer;
//	cout<<"Pool::release - Inicio ("<<_pool.size()<<", "<<_pool.begin()->second.size()<<", reuse.begin: "<<reuse.begin()->second.size()<<")\n";
	/*
	for(map<pair<uint32_t,uint32_t>,vector<VirtualSequence*>>::iterator i=this->_pool.begin();i!=this->_pool.end();i++){
		total_deletes = i->second.size();
		for_each(i->second.begin(),i->second.end(),[](VirtualSequence* &reference){if(reference->count()<=0){delete reference;reference=nullptr;}});
		i->second.erase(std::remove_if(i->second.begin(),i->second.end(),[](VirtualSequence* &reference){return reference==nullptr;}),i->second.end());
		total_deletes -= i->second.size();
	}
	// ms: 0.02 / 2.7 / 7.1 ... / 33
	*/
	
	
	vector<VirtualSequence*> swap_arr;
	for(map<pair<uint32_t,uint32_t>, vector<VirtualSequence*>>::iterator i = this->_pool.begin(); i != this->_pool.end(); ++i){
		
//		for(vector<VirtualSequence*>::iterator it = i->second.begin(); it != i->second.end(); it++){
//			if( (*it != nullptr) && ((*it)->count() <= 0) ){
//				delete *it;
//				*it = nullptr;
//				++total_deletes;
//			}
//		}
//		i->second.erase(std::remove_if(i->second.begin(),i->second.end(),[](VirtualSequence* &reference){return reference==nullptr;}),i->second.end());
//		// ms: 0.02 / 2.7 / 7.3 ... / 33
		
//		vector<VirtualSequence*>::iterator it = i->second.begin();
//		while( it != i->second.end() ){
//			if( (*it)->count() == 0 ){
//				reuse[i->first].push_back(*it);
//				it = i->second.erase(it);
//				++total_deletes;
//			}
//			else{
//				it++;
//			}
//		}
//		// ms: 0.02 / 653 / 2290 ... / 3307
		
		for(vector<VirtualSequence*>::iterator it = i->second.begin(); it != i->second.end(); it++){
			if( (*it)->count() == 0 ){
			
				// Reuse
//				reuse[i->first].push_back(*it);
				
				// NO Resuse
				delete *it;
				
				++total_deletes;
			}
			else{
				swap_arr.push_back(*it);
			}
		}
		i->second.swap(swap_arr);
		swap_arr.clear();
		// ms: 
		
	}
	
//	cout<<"Pool::release - Fin (total_deletes: "<<total_deletes<<", total reuse.begin: "<<reuse.begin()->second.size()<<", "<<timer.getMilisec()<<" ms)\n";
}
void Pool::populate(const uint32_t &_cid,const uint32_t &_gid,const uint32_t &_nucleotides,const uint32_t &_number_of_alleles,const uint32_t &_number_of_segregating_sites){
	if(_number_of_alleles>pow(N_NUCLEOTIDES,_number_of_segregating_sites)){
		cerr << "Error:: Cannot generate " << _number_of_alleles << " alleles with " << _number_of_segregating_sites << " segregating sites" << endl;
		exit(EXIT_FAILURE);
	}
	if(_number_of_segregating_sites>_nucleotides){
		cerr << "Error:: Number of segregating sites is greater than the number of nucleotides" << "::" << _number_of_segregating_sites << " " << _nucleotides << endl;
		exit(EXIT_FAILURE);
	}
	
	this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].reserve(_number_of_alleles);  
	
	// Creacion del original
//	cout<<"Pool::populate - Creando original (cid: "<<_cid<<", gid: "<<_gid<<", nucleotides: "<<_nucleotides<<", number_of_alleles: "<<_number_of_alleles<<")\n";
	//string texto_original(_nucleotides, 'A');
	//VirtualSequence *inicial_ref = new VirtualSequenceDNA(texto_original);
	VirtualSequence *inicial_ref = new VirtualSequenceDNA(_nucleotides, rng);
	inicial_ref->increase();
	this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].push_back(inicial_ref);
//	cout<<"Pool::populate - Creando "<<_number_of_alleles-1<<" mutaciones adicionales\n";
	// Llenado con mutaciones
	// Uso el metodo mutateBitMask que recibe una mascara (sequence) para modificar todos esos bits
	for(uint32_t sequence = 1; sequence < _number_of_alleles; ++sequence){
//		VirtualSequence *reference = new VirtualSequenceDNA(*inicial_ref);
		VirtualSequence *reference = inicial_ref->clone();
		// Notar el static_cast solo es valido porque acabo de definir a inicial_ref como VirtualSequenceDNA
		static_cast<VirtualSequenceDNA*>(reference)->mutateBitMask(sequence);
		this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].push_back(reference);
	}
//	cout<<"Pool::populate - Fin\n";
	
	
}
Pool::~Pool(void){
	for(map<pair<uint32_t,uint32_t>,vector<VirtualSequence*>>::iterator i = this->_pool.begin(); i != this->_pool.end(); ++i){
		for(vector<VirtualSequence*>::iterator it = i->second.begin(); it != i->second.end(); it++){
			delete (*it);
		}
		i->second.clear();
	}
	this->_pool.clear();
	
	for(map<pair<uint32_t,uint32_t>, vector<VirtualSequence*>>::iterator i = reuse.begin(); i != reuse.end(); ++i){
		for(vector<VirtualSequence*>::iterator it = i->second.begin(); it != i->second.end(); it++){
			delete (*it);
		}
		i->second.clear();
	}
	reuse.clear();
	delete rng;
}









