#include "Population.h"
Population::Population(void){
   ;
}
Population::Population(const string &_name,const uint32_t &_size){
   this->_name=_name;
   this->_population.reserve(_size);
}
Population::Population(const uint32_t &_size){
   this->_population.reserve(_size);
}
void Population::push(Individual* _individual){
   this->_population.push_back(_individual);
}
Individual* Population::at(const uint32_t &_id){
   return(this->_population[_id]);
}
uint32_t Population::size(void){
   return(this->_population.size());
}
bool Population::empty(void){
   return(this->_population.empty());
}
Individual* Population::top(){
   return(this->_population.back());
}
void Population::pop(void){
   this->_population.pop_back();
}
Population::~Population(void){
   while(!this->_population.empty()){
      delete this->_population.back();
      this->_population.pop_back();
   }
}
string Population::name(void){
   return(this->_name);
}
void Population::name(const string &_name){
   this->_name=_name;
}
vector<Population*> Population::split(const size_t &_n_populations){
   uint32_t round_robin=0U;
   vector<Population*> populations;
   uint32_t size=uint32_t(ceil(double(this->size())/double(_n_populations)));

   for(uint32_t i=0U;i<_n_populations;i++) populations.push_back(new Population(size));
   
   random_shuffle(this->_population.begin(),this->_population.end());

   while(!this->_population.empty()){
      populations[round_robin]->push(this->_population.back());
      this->_population.pop_back();
      ++round_robin%=_n_populations;
   }
   return(populations);
}
void Population::migration(Population* _population,const uint32_t &_size){
   random_shuffle(this->_population.begin(),this->_population.end());

   for(uint32_t i=0U;i<_size;i++){
      _population->push(this->_population.back());
      this->_population.pop_back();
   }
}
void Population::decrease(const uint32_t &_size){
   random_shuffle(this->_population.begin(),this->_population.end());

   for(uint32_t i=0U;i<_size;i++){
      delete this->_population.back();
      this->_population.pop_back();
   }
}
void Population::increase(const uint32_t &_size){
   uniform_int_distribution<> uniform(0U,this->size()-1U);

   for(uint32_t i=0U;i<_size;i++)
      this->_population.push_back(new Individual(*this->_population[uniform(rng)]));
}
void Population::merge(Population* _population){
   while(!_population->empty()){
      this->push(_population->top());
      _population->pop();
   }
}
