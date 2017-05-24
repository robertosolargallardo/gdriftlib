#ifndef _MARKER_H_
#define _MARKER_H_
#include <string>
#include <iostream>

enum MarkerType {SEQUENCE,MICROSATELLITE};
class Marker {
private:
        class Sequence {
        private:
            std::string _data;
        public:
            Sequence(void) {
                ;
            }
            Sequence(const std::string &_data) {
                this->_data=_data;
            }
            Sequence(const Sequence &_s) {
                this->_data=_s._data;
            }
            Sequence& operator=(const Sequence &_s) {
                this->_data=_s._data;
                return(*this);
            }
            ~Sequence(void) {
                this->_data.clear();
            }
            std::string data(void) {
                return(this->_data);
            }
        };

        class Microsatellite {
        private:
            std::string _tandem;
            uint32_t 	_repeats;

        public:
            Microsatellite(void) {
                ;
            }
            Microsatellite(const std::string &_tandem,const uint32_t &_repeats) {
                this->_tandem=_tandem;
                this->_repeats=_repeats;
            }
            Microsatellite(const Microsatellite &_m) {
                this->_tandem=_m._tandem;
                this->_repeats=_m._repeats;
            }
            Microsatellite& operator=(const Microsatellite &_m) {
                this->_tandem=_m._tandem;
                this->_repeats=_m._repeats;
                return(*this);
            }
            ~Microsatellite(void) {
                this->_tandem.clear();
            }
            std::string tandem(void) {
                return(this->_tandem);
            }
            uint32_t repeats(void) {
                return(this->_repeats);
            }
        };
    MarkerType _type;
    union Selector{
       Sequence *_sequence;
       Microsatellite *_microsatellite;
    }_marker;

public:
    Marker(void);
    Marker(const MarkerType &_type,const std::string &_data);
    Marker(const MarkerType &_type,const std::string &_tandem,const uint32_t &_repeat);
    Marker(const Marker &_m);
    Marker& operator=(const Marker &_m);
    ~Marker(void);
    MarkerType type(void) const;
    Sequence* sequence(void) const;
    Microsatellite* microsatellite(void) const;
};
#endif
