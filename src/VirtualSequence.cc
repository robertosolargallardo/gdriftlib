#include "VirtualSequence.h"

const unsigned int VirtualSequence::alphabet_size = 4;
const char VirtualSequence::alphabet[] = {'A', 'C', 'G', 'T'};

//unsigned int VirtualSequence::count_str = 0;
//unsigned int VirtualSequence::count_int = 0;
//unsigned int VirtualSequence::count_copy = 0;
//unsigned int VirtualSequence::count_mem = 0;
//unsigned int VirtualSequence::count_del = 0;
//unsigned int VirtualSequence::count_del_mem = 0;
//unsigned int VirtualSequence::count_mut = 0;

//mutex VirtualSequence::internal_mutex;

VirtualSequence::VirtualSequence(){
//	cout<<"VirtualSequence::VirtualSequence\n";
	cur_count = 0;
}

// Constructor de copia que solo almacena un puntero al buffer de texto
// Tambien incluye las mutaciones del original en el mapa de la nueva instancia
VirtualSequence::VirtualSequence(const VirtualSequence &original){
//	cout<<"VirtualSequence::VirtualSequence\n";
	cur_count = 0;
}

VirtualSequence::~VirtualSequence(){
//	cout<<"VirtualSequence::~VirtualSequence\n";
}

VirtualSequence& VirtualSequence::operator=(const VirtualSequence& original){
	if (this != &original){
		// borrar, pedir, copiar
	}
	return *this;
}

VirtualSequence *VirtualSequence::clone(){
//	cout<<"VirtualSequence::clone\n";
	return new VirtualSequence(*this);
}

void VirtualSequence::mutate(mt19937 *arg_rng){
	cerr<<"VirtualSequence::mutate - Not Implemented.\n";
}

void VirtualSequence::printData(){
}

char VirtualSequence::at(seq_size_t pos) const{
	return 0;
}

string VirtualSequence::to_string(){
	return string();
}

uint32_t VirtualSequence::count() const{
	return cur_count;
}

void VirtualSequence::increase(){
	++cur_count;
}

void VirtualSequence::decrease(){
	if(cur_count > 0){
		--cur_count;
	}
}

bool VirtualSequence::operator==(const VirtualSequence &seq){
	// No se si tiene sentido tener implementacion aqui arrojar error de No implementacion
	// Por ahora dejo implementado con lo que hay, el largo
	if( length() != seq.length() ){
		return false;
	}
	return true;
}

unsigned int VirtualSequence::length() const{
	return 0;
}

string VirtualSequence::codeMutations() const{
	return string();
}













