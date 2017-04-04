#include "VirtualSequenceMS.h"

VirtualSequenceMS::VirtualSequenceMS() : VirtualSequence() {
//	cout<<"VirtualSequenceMS::VirtualSequenceMS\n";
	tandem = string();
	repeats = 0;
}

VirtualSequenceMS::VirtualSequenceMS(string _tandem, unsigned int _repeats) : VirtualSequence() {
//	cout<<"VirtualSequenceMS::VirtualSequenceMS\n";
	tandem = _tandem;
	repeats = _repeats;
}

// Constructor de copia que solo almacena un puntero al buffer de texto
// Tambien incluye las mutaciones del original en el mapa de la nueva instancia
VirtualSequenceMS::VirtualSequenceMS(const VirtualSequenceMS &original) : VirtualSequence() {
//	cout<<"VirtualSequenceMS::VirtualSequenceMS\n";
	// Copiar data
	tandem = original.tandem;
	repeats = original.repeats;
}

VirtualSequenceMS::~VirtualSequenceMS(){
//	cout<<"VirtualSequenceMS::~VirtualSequenceMS\n";
}

//VirtualSequenceMS& VirtualSequenceMS::operator=(const VirtualSequenceMS& original){
//	if (this != &original){
//		// borrar, pedir, copiar
//	}	
//	return *this;
//}

VirtualSequence *VirtualSequenceMS::clone(){
//	cout<<"VirtualSequenceMS::clone\n";
	return new VirtualSequenceMS(*this);
}

void VirtualSequenceMS::mutate(mt19937 *arg_rng){
	// Hay que revisar que seria una mutacion en microsatelite
}

void VirtualSequenceMS::printData(){
	cout<<"VirtualSequenceMS["<<(unsigned long long)this<<"]: ";
	cout<<"\""<<tandem<<"\", x"<<repeats<<" ";
	cout<<"\n";
}

char VirtualSequenceMS::at(seq_size_t pos) const{
	seq_size_t tandem_pos = (pos % tandem.length());
	return tandem[tandem_pos];
}

string VirtualSequenceMS::to_string(){
	string seq;
	if(repeats == 0 || tandem.empty()){
		return seq;
	}
	for(unsigned int i = 0; i < repeats; ++i){
		seq.append(tandem);
	}
	return seq;
}

bool VirtualSequenceMS::operator==(const VirtualSequenceMS &seq){
	// En el caso de comparar microsatelites, si el tandem y el largo son iguales es true
	// Tambien se puede comprar si un tandem es repeticiones de otro y los n repeticiones son correctos
	// Si tambien debe soportarse la comparacion contra VirtualSequence directamente, habria que revisar char a char
	if( length() != seq.length() ){
		return false;
	}
	if( tandem.compare( seq.tandem ) == 0 && repeats == seq.repeats ){
		return true;
	}
	for(unsigned int i = 0; i < length(); ++i){
		if( at(i) != seq.at(i) ){
			return false;
		}
	}
	return true;
}

unsigned int VirtualSequenceMS::length() const{
	return (tandem.length() * repeats);
}

string VirtualSequenceMS::codeMutations() const{
	return string();
}













