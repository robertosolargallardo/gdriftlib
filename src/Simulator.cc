#include "Simulator.h"

Simulator::Simulator(const boost::property_tree::ptree &_fsettings){
//	cout<<"Simulator - Inicio\n";
	this->_fsettings=_fsettings;
	// Definition of the specie
	profile = new Individual::Profile(_fsettings.get_child("individual"));
	// Preparing the pool and the EventList
	pool = new Pool(_fsettings.get_child("individual"));
	evlist = new EventList(_fsettings.get_child("scenario"));
	
	model = NULL;
	MODEL_CODE code = MODEL_CODE(this->_fsettings.get_child("scenario").get<int>("model"));
	switch(code){
		case WRIGHTFISHER :{
			model = new ModelWF();
			break;
		}
		default : {
			cerr << "Simulator - Error, Model " <<code<< " not Supported.\n";
			exit(EXIT_FAILURE);
		}
	}
	
	detected_errors = 0;
//	cout<<"Simulator - Fin\n";
	
}
void Simulator::run(void){
   if(evlist->empty()) 
      return;
	
//	cout<<"Simulator::run - Inicio\n";

   uint32_t start = evlist->top()->timestamp();
   for(uint32_t t = start; ; t++){
//      cout<<"Simulator::run - Generation "<<t<<"\n";
//      if(t%100==0) cout<<"Simulator::run - Generation "<<t<<"\n";
      while(!evlist->empty() && evlist->top()->timestamp()==t){
         Event *e = evlist->top();
         evlist->pop();

         boost::property_tree::ptree fparams=e->params();
            
         switch(e->type()){
            case CREATE:{
               uint32_t size = fparams.get<uint32_t>("population.size");
//               cout<<"Simulator::run - CREATE ("<<size<<")\n";
               tuple<Population*,Population*> target(
                  new Population(fparams.get<string>("population.name"), size), 
                  new Population(fparams.get<string>("population.name"), size)
               );

               for(uint32_t id = 0U; id < fparams.get<uint32_t>("population.size"); id++){
                  get<0>(target)->add(id, profile, pool);
                  get<1>(target)->add(id, profile);
               }
               populations[fparams.get<string>("population.name")] = target;
               pool->release();
               
//               cout<<"-----      -----\n";
//               cout<<"Simulator::run - Revisando poblaciones\n";
//               for(auto pop : populations){
//                  cout<<"Simulator::run - Population \""<<pop.first<<"\"\n";
//                  Sample sample(pop.first, get<0>(pop.second), (get<0>(pop.second)->size()<10000)?get<0>(pop.second)->size():10000);
//                  sample.indices();
//               }
//               cout<<"-----      -----\n";
               
//               cout<<"Simulator::run - Fin CREATE\n";
               break;
            }
            case SPLIT:{
               if( populations.find(fparams.get<string>("source.population.name")) == populations.end() ){
                  break;
               }
//               cout<<"Simulator::run - SPLIT\n";
               vector<Population*> srcs = get<0>(populations[fparams.get<string>("source.population.name")])->split(fparams.get<size_t>("partitions"));
               vector<Population*> dsts = get<1>(populations[fparams.get<string>("source.population.name")])->split(fparams.get<size_t>("partitions"));

               for(auto& destination : fparams.get_child("destination")){
                  srcs.back()->name(destination.second.get<string>("population.name"));
                  dsts.back()->name(destination.second.get<string>("population.name"));
                  populations[destination.second.get<string>("population.name")] = tuple<Population*, Population*>(srcs.back(), dsts.back());
                  srcs.pop_back();
                  dsts.pop_back();
               }
               
               delete get<0>(populations[fparams.get<string>("source.population.name")]);
               delete get<1>(populations[fparams.get<string>("source.population.name")]);
               populations.erase(populations.find(fparams.get<string>("source.population.name")));
               
//               cout<<"-----      -----\n";
//               cout<<"Simulator::run - Revisando poblaciones\n";
//               for(auto pop : populations){
//                  cout<<"Simulator::run - Population \""<<pop.first<<"\"\n";
//                  Sample sample(pop.first, get<0>(pop.second), (get<0>(pop.second)->size()<10000)?get<0>(pop.second)->size():10000);
//                  sample.indices();
//               }
//               cout<<"-----      -----\n";
               
//               cout<<"Simulator::run - Fin SPLIT\n";
               break;
            }
            case MIGRATION:{
               if( populations.find(fparams.get<string>("source.population.name")) == populations.end() ){
                  break;
               }
//               cout<<"Simulator::run - MIGRATION\n";
               uint32_t size=uint32_t(ceil(double(get<0>(populations[fparams.get<string>("source.population.name")])->size())*fparams.get<double>("source.population.percentage")));
               tuple<Population*,Population*> target;

               if(populations.count(fparams.get<string>("destination.population.name"))==0)
                  target=tuple<Population*,Population*>(new Population(fparams.get<string>("destination.population.name"),size), new Population(fparams.get<string>("destination.population.name"),size));
               else
                  target=populations[fparams.get<string>("destination.population.name")];

               get<0>(populations[fparams.get<string>("source.population.name")])->migration(get<0>(target),size);
               get<1>(populations[fparams.get<string>("source.population.name")])->migration(get<1>(target),size);

               populations[fparams.get<string>("destination.population.name")]=target;
               
//               cout<<"-----      -----\n";
//               cout<<"Simulator::run - Revisando poblaciones\n";
//               for(auto pop : populations){
//                  cout<<"Simulator::run - Population \""<<pop.first<<"\"\n";
//                  Sample sample(pop.first, get<0>(pop.second), (get<0>(pop.second)->size()<10000)?get<0>(pop.second)->size():10000);
//                  sample.indices();
//               }
//               cout<<"-----      -----\n";

//               cout<<"Simulator::run - Fin MIGRATION\n";
               break;
            }
            case MERGE:{
//               cout<<"Simulator::run - MERGE\n";
               uint32_t size=0U;
               tuple<Population*,Population*> target;

               for(auto& source : fparams.get_child("source"))  
                  size+=(get<0>(populations[source.second.get<string>("population.name")]))->size();

               if(populations.count(fparams.get<string>("destination.population.name"))==0)
                  target=tuple<Population*,Population*>(new Population(fparams.get<string>("destination.population.name"),size), new Population(fparams.get<string>("destination.population.name"),size));
               else
                  target=populations[fparams.get<string>("destination.population.name")];

               for(auto& source : fparams.get_child("source")){
                  get<0>(target)->merge(get<0>(populations[source.second.get<string>("population.name")]));
                  get<1>(target)->merge(get<1>(populations[source.second.get<string>("population.name")]));

                  delete get<0>(populations[source.second.get<string>("population.name")]);
                  delete get<1>(populations[source.second.get<string>("population.name")]);
      
                  populations.erase(populations.find(source.second.get<string>("population.name")));
               }
               populations[fparams.get<string>("destination.population.name")]=target;
               
//               cout<<"-----      -----\n";
//               cout<<"Simulator::run - Revisando poblaciones\n";
//               for(auto pop : populations){
//                  cout<<"Simulator::run - Population \""<<pop.first<<"\"\n";
//                  Sample sample(pop.first, get<0>(pop.second), (get<0>(pop.second)->size()<10000)?get<0>(pop.second)->size():10000);
//                  sample.indices();
//               }
//               cout<<"-----      -----\n";
               
//               cout<<"Simulator::run - Fin MERGE\n";
               break;
            }
            case INCREMENT:{
//               cout<<"Simulator::run - INCREMENT ("<<(fparams.get<string>("source.population.name"))<<", exists? "<<((populations.find(fparams.get<string>("source.population.name")) == populations.end())?"No":"Yes")<<")\n";
               if( populations.find(fparams.get<string>("source.population.name")) == populations.end() ){
                  break;
               }
               uint32_t size = uint32_t(ceil(double(get<0>(populations[fparams.get<string>("source.population.name")])->size()) * fparams.get<double>("source.population.percentage")));
//               cout<<"Simulator::run - size: "<<size<<"\n";
               get<0>(populations[fparams.get<string>("source.population.name")])->increase(size);
               for(uint32_t id = 0; id < size; ++id){
                  // get<1>(populations[fparams.get<string>("source.population.name")])->push(new Individual(id, *profile));
                  get<1>(populations[fparams.get<string>("source.population.name")])->add(id, profile);
               }
               
//               cout<<"-----      -----\n";
//               cout<<"Simulator::run - Revisando poblaciones\n";
//               for(auto pop : populations){
//                  cout<<"Simulator::run - Population \""<<pop.first<<"\"\n";
//                  Sample sample(pop.first, get<0>(pop.second), (get<0>(pop.second)->size()<10000)?get<0>(pop.second)->size():10000);
//                  sample.indices();
//               }
//               cout<<"-----      -----\n";
               
//               cout<<"Simulator::run - Fin INCREMENT\n";
               break;
            }
            case DECREMENT:{
               if( populations.find(fparams.get<string>("source.population.name")) == populations.end() ){
                  break;
               }
//               cout<<"Simulator::run - DECREMENT (percentage: "<<fparams.get<double>("source.population.percentage")<<")\n";
               if(fparams.get<double>("source.population.percentage") == 1.0){
//                  cout<<"Simulator::run - caso 1\n";
                  delete get<0>(populations[fparams.get<string>("source.population.name")]);
                  delete get<1>(populations[fparams.get<string>("source.population.name")]);
//                  cout<<"Simulator::run - erase...\n";
                  populations.erase(populations.find(fparams.get<string>("source.population.name")));
               }
               else{
                  uint32_t size = uint32_t(ceil(double(get<0>(populations[fparams.get<string>("source.population.name")])->size())*fparams.get<double>("source.population.percentage")));
//                  cout<<"Simulator::run - caso 2, size: "<<size<<"\n";
                  get<0>(populations[fparams.get<string>("source.population.name")])->decrease(size);
//                  cout<<"Simulator::run - decrease...\n";
                  get<1>(populations[fparams.get<string>("source.population.name")])->decrease(size);
               }
               
//               cout<<"-----      -----\n";
//               cout<<"Simulator::run - Revisando poblaciones\n";
//               for(auto pop : populations){
//                  cout<<"Simulator::run - Population \""<<pop.first<<"\"\n";
//                  Sample sample(pop.first, get<0>(pop.second), (get<0>(pop.second)->size()<10000)?get<0>(pop.second)->size():10000);
//                  sample.indices();
//               }
//               cout<<"-----      -----\n";
               
//               cout<<"Simulator::run - Fin DECREMENT\n";
               break;
            }
            case EXTINCTION:{
               if( populations.find(fparams.get<string>("source.population.name")) == populations.end() ){
                  break;
               }
//               cout<<"Simulator::run - EXTINCTION\n";
               if( populations.find(fparams.get<string>("source.population.name")) == populations.end() ){
                  break;
               }
//               cout<<"Simulator::run - delete de src y dst...\n";
               delete get<0>(populations[fparams.get<string>("source.population.name")]);
               delete get<1>(populations[fparams.get<string>("source.population.name")]);
//               cout<<"Simulator::run - erase...\n";
               populations.erase(populations.find(fparams.get<string>("source.population.name")));
               
//               cout<<"-----      -----\n";
//               cout<<"Simulator::run - Revisando poblaciones\n";
//               for(auto pop : populations){
//                  cout<<"Simulator::run - Population \""<<pop.first<<"\"\n";
//                  Sample sample(pop.first, get<0>(pop.second), (get<0>(pop.second)->size()<10000)?get<0>(pop.second)->size():10000);
//                  sample.indices();
//               }
//               cout<<"-----      -----\n";
               
//               cout<<"Simulator::run - Fin EXTINCTION\n";
               break;
            }
            case ENDSIM:{
//				cout<<"Simulator::run - ENDSIM\n";
				if(fparams.get_child_optional("sampling")){
					uint32_t size = 0U;
					for(auto fsampling : fparams.get_child("sampling")){
						if( populations.find(fsampling.second.get<string>("source.population.name")) == populations.end() ){
//							cout<<"Simulator::run - Error de poblaciones detectado.\n";
							detected_errors = 1;
							_samples[fsampling.second.get<string>("name")] = new Sample();
							continue;
						}
						uint32_t pop_size = get<0>(populations[fsampling.second.get<string>("source.population.name")])->size();
//						cout<<"Simulator::run - Population "<<fsampling.second.get<string>("source.population.name")<<", size: "<<pop_size<<"\n";
						size = (uint32_t)( (double)pop_size*SAMPLING_PERCENT );
						if( size < 1000 ){
							size = (pop_size<1000)?pop_size:1000;
						}
						_samples[fsampling.second.get<string>("name")] = new Sample(fsampling.second.get<string>("name"), get<0>(populations[fsampling.second.get<string>("source.population.name")]), size);
					}
				}
				delete e;
//				cout<<"Simulator::run - Fin ENDSIM\n";
				return;
            }
            default:{
               cerr << "Error::Event " << e->type() << " not Supported" << endl;
               exit(EXIT_FAILURE);
            }

         }
         delete e;
      }

//		cout<<"Simulator::run - Using Model\n";
		NanoTimer timer;
		for(map<string,tuple<Population*, Population*>>::iterator i = populations.begin(); i != populations.end(); i++){
			model->run(
				get<0>(i->second), get<1>(i->second), pool, profile
			);
			swap(get<0>(i->second),get<1>(i->second));
			get<1>(i->second)->clear();
		}
        model_time += timer.getMilisec();
        
//       cout<<"-----      -----\n";
//       cout<<"Simulator::run - Revisando poblaciones\n";
//       for(auto pop : populations){
//          cout<<"Simulator::run - Population \""<<pop.first<<"\"\n";
//          Sample sample(pop.first, get<0>(pop.second), (get<0>(pop.second)->size()<1000)?get<0>(pop.second)->size():1000);
//          sample.indices();
//       }
//       cout<<"-----      -----\n";
        
      pool->release();
//      cout<<"Simulator::run - Generation Finished\n";
   }
	cout<<"Simulator::run - Fin\n";
}
map<string,Sample*> &Simulator::samples(){
	return _samples;
}

/*vector<Population*> &Simulator::populations(){
   std::vector<Population*> p;
   p.reserve(populations.size());
   std::for_each(populations.begin(),populations.end(),[&p](const map<string,tuple<Population*,Population*>>::value_type& _p){p.push_back(get<0>(_p.second));});
   return(p);
}*/

Simulator::~Simulator(void){
	for(map<string,Sample*>::iterator i = _samples.begin(); i != _samples.end(); ++i){
		delete i->second;
	}
	for(map<string,tuple<Population*,Population*>>::iterator i = populations.begin(); i != populations.end(); i++){
		delete get<0>(i->second);
		delete get<1>(i->second);
	}
	_samples.clear();    
	populations.clear();    
	
	if(pool != NULL){
		delete pool;
		pool = NULL;
	}
	if(evlist != NULL){
		delete evlist;
		evlist = NULL;
	}
	if(profile != NULL){
		delete profile;
		profile = NULL;
	}
	if(model != NULL){
		delete model;
		model = NULL;
	}
}






