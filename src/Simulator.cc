#include "Simulator.h"
Simulator::Simulator(const boost::property_tree::ptree &_fsettings){
   this->_fsettings=_fsettings;
   this->_pool=new Pool(_fsettings.get_child("individual"));
   this->_evlist=new EventList(_fsettings.get_child("scenario"));
}
void Simulator::run(void){
   if(this->_evlist->empty()) 
      return;

   uint32_t start=this->_evlist->top()->timestamp();

   for(uint32_t t=start;;t++){
      while(!this->_evlist->empty() && this->_evlist->top()->timestamp()==t){
         Event* e=this->_evlist->top();
         this->_evlist->pop();

         boost::property_tree::ptree params=e->params();
            
         switch(e->type()){
            case CREATE:{
               Population* src=new Population(params.get<string>("population.name"),params.get<uint32_t>("population.size"));
               Population* dst=new Population(params.get<string>("population.name"),params.get<uint32_t>("population.size"));

               for(uint32_t id=0U;id<params.get<uint32_t>("population.size");id++){
                  src->push(this->_pool->generate(id));
                  dst->push(new Individual(id,this->_fsettings.get_child("individual")));
               }
					this->_populations[params.get<string>("population.name")]=tuple<Population*,Population*>(src,dst);
      			
      			this->_pool->release();
               break;
            }
            case SPLIT:{
               vector<Population*> srcs=get<0>(this->_populations[params.get<string>("source.population.name")])->split(params.get<size_t>("partitions"));
               vector<Population*> dsts=get<1>(this->_populations[params.get<string>("source.population.name")])->split(params.get<size_t>("partitions"));

               int i=0;
               for(auto& destination : params.get_child("destination")){
                  srcs[i]->name(destination.second.get<string>("population.name"));
                  dsts[i]->name(destination.second.get<string>("population.name"));
                  this->_populations[destination.second.get<string>("population.name")]=tuple<Population*,Population*>(srcs[i],dsts[i]);
                  i++;
               }

               delete get<0>(this->_populations[params.get<string>("source.population.name")]);
               delete get<1>(this->_populations[params.get<string>("source.population.name")]);
               this->_populations.erase(this->_populations.find(params.get<string>("source.population.name")));
   
               srcs.clear();
               dsts.clear();

               break;
            }
            case MIGRATION:{
               uint32_t size=uint32_t(ceil(double(get<0>(this->_populations[params.get<string>("source.population.name")])->size())*params.get<double>("source.population.percentage")));

               Population* src=new Population(params.get<string>("destination.population.name"),size);
               Population* dst=new Population(params.get<string>("destination.population.name"),size);

               get<0>(this->_populations[params.get<string>("source.population.name")])->migration(src,size);
               get<1>(this->_populations[params.get<string>("source.population.name")])->migration(dst,size);

               this->_populations[params.get<string>("destination.population.name")]=tuple<Population*,Population*>(src,dst);

               break;
            }
            case MERGE:{
               uint32_t size=0U;

               for(auto& source : params.get_child("source"))  size+=(get<0>(this->_populations[source.second.get<string>("population.name")]))->size();

               Population* src=new Population(params.get<string>("destination.population.name"),size);
               Population* dst=new Population(params.get<string>("destination.population.name"),size);

               for(auto& source : params.get_child("source")){
                  src->merge(get<0>(this->_populations[source.second.get<string>("population.name")]));
                  dst->merge(get<1>(this->_populations[source.second.get<string>("population.name")]));

                  delete get<0>(this->_populations[source.second.get<string>("population.name")]);
                  delete get<1>(this->_populations[source.second.get<string>("population.name")]);
      
                  this->_populations.erase(this->_populations.find(source.second.get<string>("population.name")));
               }
               this->_populations[params.get<string>("destination.population.name")]=tuple<Population*,Population*>(src,dst);
               break;
            }
            case INCREMENT:{
               uint32_t size=uint32_t(ceil(double(get<0>(this->_populations[params.get<string>("source.population.name")])->size())*params.get<double>("source.population.percentage")));
               get<0>(this->_populations[params.get<string>("source.population.name")])->increase(size);
               get<1>(this->_populations[params.get<string>("source.population.name")])->increase(size);
               break;
            }
            case DECREMENT:{
               if(params.get<double>("source.population.percentage")==1.0){
                  delete get<0>(this->_populations[params.get<string>("source.population.name")]);
                  delete get<1>(this->_populations[params.get<string>("source.population.name")]);

                  this->_populations.erase(this->_populations.find(params.get<string>("source.population.name")));
               }
               else{
                  uint32_t size=uint32_t(ceil(double(get<0>(this->_populations[params.get<string>("source.population.name")])->size())*params.get<double>("source.population.percentage")));
                  get<0>(this->_populations[params.get<string>("source.population.name")])->decrease(size);
                  get<1>(this->_populations[params.get<string>("source.population.name")])->decrease(size);
               }
               break;
            }
            case ENDSIM:{
               delete e;
               return;
            }
            default:{
               cerr << "Error::Event " << e->type() << " not Supported" << endl;
               exit(EXIT_FAILURE);
            }

         }
         delete e;
      }

      //this->_pool->decrease_all();
      Model m=Model(this->_fsettings.get_child("scenario").get<int>("model"));
      switch(m){
         case WRIGHTFISHER:{
            Ploidy p=Ploidy(this->_fsettings.get_child("individual").get<int>("ploidy"));
            switch(p){
               case HAPLOID:{
                  for(map<string,tuple<Population*,Population*>>::iterator i=this->_populations.begin();i!=this->_populations.end();i++){
                     model::run<WRIGHTFISHER,HAPLOID>(get<0>(i->second),get<1>(i->second),this->_pool);   
							swap(get<0>(i->second),get<1>(i->second));
                  }
                  break;
               }  
               case DIPLOID:{
                  for(map<string,tuple<Population*,Population*>>::iterator i=this->_populations.begin();i!=this->_populations.end();i++){
            
                  }
                  break;
               }
               default:{
                  cerr << "Error::Ploidy " << m << " not Supported" << endl;
                  exit(EXIT_FAILURE);
               }
            }
            break;
         }
         default:{
            cerr << "Error::Model " << this->_fsettings.get_child("scenario").get<uint32_t>("model") << " not Supported" << endl;
            exit(EXIT_FAILURE);
         }
      }
      //this->_pool->release();
   }
}
vector<Population*> Simulator::populations(void){
   std::vector<Population*> p;
   p.reserve(this->_populations.size());
   std::for_each(this->_populations.begin(),this->_populations.end(),[&p](const map<string,tuple<Population*,Population*>>::value_type& _p){p.push_back(get<0>(_p.second));});
   return(p);
}
Simulator::~Simulator(void){
   for(map<string,tuple<Population*,Population*>>::iterator i=this->_populations.begin();i!=this->_populations.end();i++){
      delete get<0>(i->second);
      delete get<1>(i->second);
   }
   this->_populations.clear();    

   delete this->_pool;
   delete this->_evlist;
}
