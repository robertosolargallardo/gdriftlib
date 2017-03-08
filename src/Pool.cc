#include "Pool.h"
Pool::Pool(const boost::property_tree::ptree &_findividual){
	this->_findividual=_findividual;

	uint32_t cid,gid,nucleotides,number_of_segregating_sites,number_of_alleles; 
	for(auto fchromosome : _findividual.get_child("chromosomes")){
		cid=fchromosome.second.get<uint32_t>("id");
		for(auto fgene : fchromosome.second.get_child("genes")){
			nucleotides=fgene.second.get<uint32_t>("nucleotides");
			number_of_alleles=fgene.second.get<uint32_t>("number-of-alleles");
			number_of_segregating_sites=fgene.second.get<uint32_t>("number-of-segregating-sites");
			gid=fgene.second.get<uint32_t>("id");

		  	this->populate(cid,gid,nucleotides,number_of_alleles,number_of_segregating_sites);			
		}
	}
}
Individual* Pool::generate(const uint32_t &_id){
	Individual *individual=new Individual(_id,this->_findividual);

	for(uint32_t cid=0U;cid<individual->n_chromosomes();cid++){
		for(uint32_t p=0U;p<uint32_t(individual->ploidy());p++){
			for(uint32_t gid=0U;gid<individual->chromosome(cid)[p]->n_genes();gid++){
				uniform_int_distribution<> uniform(0,this->_pool[pair<uint32_t,uint32_t>(cid,gid)].size()-1);
//				Reference *reference=this->_pool[pair<uint32_t,uint32_t>(cid,gid)][uniform(rng)];
				VirtualSequence *reference=this->_pool[pair<uint32_t,uint32_t>(cid,gid)][uniform(rng)];
				individual->chromosome(cid)[p]->gene(gid)->reference(reference);
			}
		}
	}

	return(individual);
}
//Reference* Pool::push(const uint32_t &_cid,const uint32_t &_gid,Reference* _reference){
//	for(vector<Reference*>::iterator i=this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].begin();i!=this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].end();i++)
VirtualSequence* Pool::push(const uint32_t &_cid,const uint32_t &_gid,VirtualSequence* _reference){
	for(vector<VirtualSequence*>::iterator i=this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].begin();i!=this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].end();i++)
		if((**i)==(*_reference)) return(*i);

	this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].push_back(_reference);
	return(nullptr);
}
uint32_t Pool::size(void){
	return(this->_pool.size());
}
void Pool::decrease_all(void){
//	for(map<pair<uint32_t,uint32_t>,vector<Reference*>>::iterator i=this->_pool.begin();i!=this->_pool.end();i++)
//		for_each(i->second.begin(),i->second.end(),[](Reference* &reference){reference->decrease();});
	for(map<pair<uint32_t,uint32_t>,vector<VirtualSequence*>>::iterator i=this->_pool.begin();i!=this->_pool.end();i++)
		for_each(i->second.begin(),i->second.end(),[](VirtualSequence* &reference){reference->decrease();});
}
void Pool::release(void){
//	for(map<pair<uint32_t,uint32_t>,vector<Reference*>>::iterator i=this->_pool.begin();i!=this->_pool.end();i++){
//		for_each(i->second.begin(),i->second.end(),[](Reference* &reference){if(reference->count()<=0){delete reference;reference=nullptr;}});
//		i->second.erase(std::remove_if(i->second.begin(),i->second.end(),[](Reference* &reference){return reference==nullptr;}),i->second.end());	
	for(map<pair<uint32_t,uint32_t>,vector<VirtualSequence*>>::iterator i=this->_pool.begin();i!=this->_pool.end();i++){
		for_each(i->second.begin(),i->second.end(),[](VirtualSequence* &reference){if(reference->count()<=0){delete reference;reference=nullptr;}});
		i->second.erase(std::remove_if(i->second.begin(),i->second.end(),[](VirtualSequence* &reference){return reference==nullptr;}),i->second.end());	
	}
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
	
	/*
	// CAMBIAR ESTE CODIGO al llenado con mutaciones
	for(uint32_t sequence=0U;sequence<_number_of_alleles;sequence++){
//		Reference* reference=new Reference(_nucleotides,sequence);
		VirtualSequence* reference=new VirtualSequence(_nucleotides,sequence);
		this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].push_back(reference);
	}
	*/
	
	// Creacion del original
	cout<<"Pool::populate - Creando original\n";
	string texto_original(_nucleotides, 'A');
	VirtualSequence *inicial_ref = new VirtualSequence(texto_original);
	this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].push_back(inicial_ref);
	cout<<"Pool::populate - Creando "<<_number_of_alleles-1<<" mutaciones adicionales\n";
	// Llenado con mutaciones
	// Uso el metodo mutateBitMask que recibe una mascara (sequence) para modificar todos esos bits
	for(uint32_t sequence = 1; sequence < _number_of_alleles; ++sequence){
		VirtualSequence *reference = new VirtualSequence(*inicial_ref);
//		reference->mutate();
		reference->mutateBitMask(sequence);
		this->_pool[pair<uint32_t,uint32_t>(_cid,_gid)].push_back(reference);
	}
	cout<<"Pool::populate - Fin\n";
	
	
}
Pool::~Pool(void){
//	for(map<pair<uint32_t,uint32_t>,vector<Reference*>>::iterator i=this->_pool.begin();i!=this->_pool.end();i++){
//		for_each(i->second.begin(),i->second.end(),[](Reference* &reference){delete reference;reference=nullptr;});
	for(map<pair<uint32_t,uint32_t>,vector<VirtualSequence*>>::iterator i=this->_pool.begin();i!=this->_pool.end();i++){
		for_each(i->second.begin(),i->second.end(),[](VirtualSequence* &reference){delete reference;reference=nullptr;});
		i->second.clear();
	}
	this->_pool.clear();
}
