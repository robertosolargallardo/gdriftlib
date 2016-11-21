#ifndef _EVENTLIST_H
#define _EVENTLIST_H
#include <memory>
#include <string>
#include <queue>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "Event.h"
#include "Util.h"
using namespace std;

class EventComparator : public std::binary_function<Event*,Event*,bool>{
    public:	bool operator()(const Event* lhs,const Event* rhs) const{
        			return(lhs->timestamp()>rhs->timestamp());
    			}
};

class EventList{
	private:	uint32_t _lvt;
				std::priority_queue< Event*,std::vector< Event*>,EventComparator> _list;

	public:	EventList(void);
            EventList(const boost::property_tree::ptree&);

				Event* top(void);
				void 	pop(void);
				bool 	empty(void);
		
				uint32_t lvt(void);
				
				~EventList(void);
};
#endif
