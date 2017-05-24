#include "../include/Marker.h"
Marker::Marker(void) {
    ;
}
Marker::Marker(const MarkerType &_type,const std::string &_data) {
    this->_type=_type;
    this->_marker._sequence=new Sequence(_data);
}
Marker::Marker(const MarkerType &_type,const std::string &_tandem,const uint32_t &_repeat) {
    this->_type=_type;
    this->_marker._microsatellite=new Microsatellite(_tandem,_repeat);
}
Marker::Marker(const Marker &_m) {
    this->_type=_m._type;

    switch(this->_type) {
    case SEQUENCE: {
        this->_marker._sequence=new Sequence(*_m._marker._sequence);
        break;
    };
    case MICROSATELLITE: {
        this->_marker._microsatellite=new Microsatellite(*_m._marker._microsatellite);
        break;
    };
    default: {
        std::cerr << "Error::Unknown marker type: " << this->_type << std::endl;
        exit(EXIT_FAILURE);
    }
    }
}
Marker& Marker::operator=(const Marker &_m) {
    this->_type=_m._type;
    switch(this->_type) {
    case SEQUENCE: {
        this->_marker._sequence=new Sequence(*_m._marker._sequence);
        break;
    };
    case MICROSATELLITE: {
        this->_marker._microsatellite=new Microsatellite(*_m._marker._microsatellite);
        break;
    };
    default: {
        std::cerr << "Error::Unknown marker type: " << this->_type << std::endl;
        exit(EXIT_FAILURE);
    }
    }
    return(*this);
}

Marker::~Marker(void) {
    switch(this->_type) {
    case SEQUENCE: {
        delete this->_marker._sequence;
        break;
    };
    case MICROSATELLITE: {
        delete this->_marker._microsatellite;
        break;
    };
    default: {
        std::cerr << "Error::Unknown marker type: " << this->_type << std::endl;
        exit(EXIT_FAILURE);
    }
    }
}
MarkerType Marker::type(void) const{
	return(this->_type);
}
Marker::Sequence* Marker::sequence(void) const{
	return(this->_marker._sequence);
}
Marker::Microsatellite* Marker::microsatellite(void) const{
	return(this->_marker._microsatellite);
}
