#include "Population.h"
Population::Population(void){
	profile = NULL;
	this->_name.clear();
	this->_population.clear();
	//Soporte para extern rng
	rng_gen = &rng;
}
Population::Population(const std::string &_name,const std::map<uint32_t,map<uint32_t,std::vector<Marker>>> &_population, const boost::property_tree::ptree &_fsettings){
	profile = new Individual::Profile(_fsettings.get_child("individual"));
   Ploidy ploidy=Ploidy(_fsettings.get_child("individual").get<uint32_t>("ploidy"));
	std::map<uint32_t,Individual> individuals;

	for(auto& chromosome : _population){
		for(auto& gene : chromosome.second){
			uint32_t id=0U;
			for(auto& marker : gene.second){
				/*creating individuals*/
				if(individuals.count(id)==0) individuals[id]=Individual(id,*profile);

				/*duplicating the marker depending on the ploidy*/
				for(uint32_t p=0U;p<uint32_t(ploidy);p++){
					switch(marker.type()){
						case SEQUENCE:{
							VirtualSequence* reference = new VirtualSequenceDNA(marker.sequence()->data());
							individuals[id].setGene(gene.first,chromosome.first,p,reference);
							break;
						};
						case MICROSATELLITE:{
							//TODO not yet implemented
							break;
						};
					}
				}
				++id;
			}
		}
	}
	this->_name=_name;
	for(auto& individual : individuals)
		this->_population.push_back(individual.second);
	individuals.clear();
}
/*
 *@deprecated
 */
Population::Population(const Ploidy &_ploidy, const boost::property_tree::ptree &_fpopulation, const boost::property_tree::ptree &_fsettings){
//	cout<<"Population - Inicio\n";
	
	profile = new Individual::Profile(_fsettings.get_child("individual"));
	
	this->_name=_fpopulation.get<string>("name");
	this->_population.reserve(_fpopulation.get_child("individuals").size());
	
	for(auto& findividual : _fpopulation.get_child("individuals")){
		Individual individual(findividual.second.get<uint32_t>("id"), *profile);
		for(auto& fchromosome : findividual.second.get_child("chromosomes")){
			for(auto& fgene : fchromosome.second.get_child("genes")){
				unsigned int pid = 0;
				// una secuencia por ploidy
				for(auto& fsequence : fgene.second.get_child("sequences")){
					VirtualSequence* reference = new VirtualSequenceDNA(fsequence.second.data());
					individual.setGene(fgene.second.get<uint32_t>("id"), fchromosome.second.get<uint32_t>("id"), pid, reference);
					++pid;
				}
			}
		}
		this->_population.push_back(individual);
	}
	//Soporte para extern rng
	rng_gen = &rng;
//	cout<<"Population - Fin\n";
}
Population::Population(const string &_name){
	this->_name=_name;
	//Soporte para extern rng
	rng_gen = &rng;
	profile = NULL;
}
Population::Population(const string &_name,const uint32_t &_size){
	this->_name=_name;
	this->_population.reserve(_size);
	//Soporte para extern rng
	rng_gen = &rng;
	profile = NULL;
}
Population::Population(const uint32_t &_size){
	this->_population.reserve(_size);
	//Soporte para extern rng
	rng_gen = &rng;
	profile = NULL;
}

vector<Individual> &Population::population(void){
	return(this->_population);
}

// Agrega un nuevo individuo a la poblacio, con id y basado en un cierto profile
// Lo crea vacio y le asigna datos del pool, si se entrega
void Population::add(unsigned int id, Individual::Profile *_profile, Pool *pool){
//	cout<<"Population::add - Inicio (id: "<<id<<")\n";
	_population.push_back( Individual(id, *_profile) );
	if(pool != NULL){
		pool->regenerate(&(_population.back()));
	}
//	cout<<"Population::add - Fin\n";
	
}

void Population::push(Individual &individual){
	this->_population.push_back(individual);
}
Individual &Population::at(const uint32_t pos){
	if(pos >= this->size()){
		cerr << "Population::at - Error (Cannot access position "<<pos<<")\n";
		exit(EXIT_FAILURE);
	}
	return this->_population[pos];
}
uint32_t Population::size(void){
	return(this->_population.size());
}
bool Population::empty(void){
	return(this->_population.empty());
}
Individual &Population::top(){
	return this->_population.back();
}
void Population::pop(void){
	this->_population.pop_back();
}
Population::~Population(void){
	this->_population.clear();
	if(profile != NULL){
		delete profile;
		profile = NULL;
	}
}
string Population::name(void){
	return this->_name;
}
void Population::name(const string &_name){
	this->_name = _name;
}
vector<Population*> Population::split(const size_t &_n_populations){
	uint32_t round_robin=0U;
	vector<Population*> populations(_n_populations);
	size_t size = size_t(ceil(this->size()/_n_populations));

	for(uint32_t i=0U;i<_n_populations;i++)
		populations[i] = new Population(size);
	
	this->shuffle();

	while(!this->_population.empty()){
		populations[round_robin]->push(this->top());
		this->pop();
		++round_robin %= _n_populations;
	}

	return (populations);
}
void Population::migration(Population *_population,const uint32_t &_size){
	this->shuffle();
	for(uint32_t i=0U;i<_size;i++){
		_population->push(this->top());
		this->pop();
	}
}
void Population::decrease(const uint32_t &_size){
	this->shuffle();
	for(uint32_t i = 0; i < _size; ++i){
//		delete this->top();
		this->pop();
	}
}
void Population::increase(const uint32_t &size){
	uniform_int_distribution<> uniform(0, this->size() - 1);
	for(uint32_t i=0; i < size; ++i){
		Individual ind(_population[uniform(*rng_gen)]);
		_population.push_back(ind);
	}
}
void Population::shuffle(void){
	std::shuffle(this->_population.begin(),this->_population.end(), *rng_gen);
}
void Population::merge(Population* _population){
	while(!_population->empty()){
		this->push(_population->top());
		_population->pop();
	}
}
void Population::clear(void){
	for(unsigned int i = 0; i < _population.size(); ++i){
		_population[i].clear();
	}
}







