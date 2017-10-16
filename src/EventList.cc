#include "EventList.h"

EventList::EventList(void){
	this->_lvt=0U;
}
EventList::EventList(const boost::property_tree::ptree &_scenario_file){
	this->_lvt=0U;

   for(auto& e : _scenario_file.get_child("events")){
      uint32_t timestamp = e.second.get<uint32_t>("timestamp");
      string type = e.second.get<string>("type");
//      cout<<"EventList - Event timestamp "<<timestamp<<" - "<<type<<"\n";
      this->_list.push(new Event(timestamp, EventType(util::hash(type)),e.second.get_child("params")));
   }
}
Event* EventList::top(void){
	return(this->_list.empty()?nullptr:this->_list.top());
}
void EventList::pop(void){
	if(!this->empty()){
		this->_lvt=this->top()->timestamp();
		this->_list.pop();
	}
}
uint32_t EventList::lvt(void){
	return(this->_lvt);
}
bool EventList::empty(void){
	return(this->_list.empty());
}
EventList::~EventList(void){
   while(!this->_list.empty()){
      delete this->_list.top();
      this->_list.pop();
   }
}
