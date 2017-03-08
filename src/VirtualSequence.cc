#include "VirtualSequence.h"

const unsigned int VirtualSequence::alphabet_size = 4;
const char VirtualSequence::alphabet[] = {'A', 'C', 'G', 'T'};
////mt19937 VirtualSequence::class_rng;
unsigned int VirtualSequence::count_str = 0;
unsigned int VirtualSequence::count_int = 0;
unsigned int VirtualSequence::count_copy = 0;
unsigned int VirtualSequence::count_mem = 0;
unsigned int VirtualSequence::count_del = 0;
unsigned int VirtualSequence::count_mut = 0;

VirtualSequence::VirtualSequence(bool _read_only){

	size = 0;
	data = NULL;
	owns_data = false;
	
	cur_count = 0;
	cur_read = _read_only;
}

// Constructor real que COPIA el buffer de texto
VirtualSequence::VirtualSequence(const char *_ref, unsigned int _size, bool _read_only){
	++count_str;
	++count_mem;
	
//	cout<<"VirtualSequence - Inicio (text: "<<_ref<<", size: "<<_size<<")\n";
	size = (seq_size_t)_size;
	unsigned int data_size = (size>>2);
	if( size & 0x3 ){
		++data_size;
	}
	data = new unsigned char[ data_size ];
	memset(data, 0, data_size);
	unsigned char *p = data;
	unsigned int disp = 0;
	unsigned char value = 0;
	for(unsigned int i = 0; i < size; ++i){
		switch ( _ref[i] ){
			// Tambien notar que seria mas seguro asociar esto a alphabet
			case 'A' : value = 0; break;
			case 'C' : value = 1; break;
			case 'G' : value = 2; break;
			case 'T' : value = 3; break;
			default : break;
		}
//		cout<<"Escribiendo "<<_ref[i]<<" (value: "<<(unsigned int)value<<", disp: "<<disp<<")\n";
		value <<= disp;
//		cout<<"New value: "<<(unsigned int)value<<"\n";
		*p |= value;
//		cout<<"*p: "<<(unsigned int)(*p)<<"\n";
		disp += 2;
		//if(disp == 8)
		if(disp > 6){
			++p;
			disp = 0;
		}
	}
	owns_data = true;
	
//	for(unsigned int i = 0; i < data_size; ++i){
//		cout<<"data["<<i<<"]: "<<(unsigned int)data[i]<<"\n";
//	}
	
	cur_count = 0;
	cur_read = _read_only;
}

// Constructor real que COPIA el buffer de texto, pero que recibe solo 1 entero (32 bits) de secuencia
// Llena el resto de A's (con un memset... 0)
VirtualSequence::VirtualSequence(const unsigned int _size, const unsigned int _seq, bool _read_only){
	++count_int;
	++count_mem;
	
	size = (seq_size_t)_size;
	unsigned int data_size = (size>>2);
	if( size & 0x3 ){
		++data_size;
	}
	data = new unsigned char[ data_size ];
	memset(data, 0, data_size);
	
	// Leo cada numero e invierto su posicion
	// Para la lectura, itero por cada numero de seq y voy moviendo mask
	// Para tomar el valor aplico mask y la muevo en pos<<1 (x2 pues son 2 bits por valor)
	// Para la invertir la posicion, la resto de size-1
	// Uso la posicion inversa para escoger el byte (/4, es decir >>2)
	// ... y para mover los bits (el resto, es decir &0x3, <<1 pues son 2 bits por valor)
	unsigned int mask = 0x3;
	unsigned int new_pos = 0;
	unsigned int value = 0;
	for(unsigned int pos = 0; pos < size; ++pos, mask<<=2){
		value = (_seq & mask) >> (pos<<1);
		new_pos = size - 1 - pos;
		data[ (new_pos>>2) ] |= (value << ((new_pos & 0x3)<<1) );
	}
	
//	for(unsigned int i = 0; i < data_size; ++i){
//		cout<<"data["<<i<<"]: "<<(unsigned int)data[i]<<"\n";
//	}
	
	cur_count = 0;
	cur_read = _read_only;
	
}

// Constructor real que COPIA el buffer de texto
VirtualSequence::VirtualSequence(const string &_ref, bool _read_only)
 : VirtualSequence(_ref.c_str(), _ref.length(), _read_only)
{

}

// Constructor de copia que solo almacena un puntero al buffer de texto
// Tambien incluye las mutaciones del original en el mapa de la nueva instancia
VirtualSequence::VirtualSequence(const VirtualSequence &original){
	++count_copy;
	
	//cout<<"VirtualSequence - Copia\n";
	size = original.size;
	data = original.data;
	owns_data = false;
	//cout<<"VirtualSequence - size: "<<size<<". data[0]: "<<((data==NULL)?(0xffffffff):(unsigned int)data[0])<<"\n";
	
	cur_count = 0;
	cur_read = original.cur_read;
	
	// Muaciones
	//cout<<"VirtualSequence - Agregando "<<original.mutations.size()<<" mutaciones\n";
	mutations.insert(original.mutations.begin(), original.mutations.end());
}

VirtualSequence::~VirtualSequence(){
	++count_del;
	
	mutations.clear();
	if(owns_data){
		delete [] data;
	}
	data = NULL;
	size = 0;
}


bool VirtualSequence::verifyDecompression(){
	
	// Condicion de descompresion
	// Las mutaciones se vuelcan en un nuevo arreglo de datos si son muchas
	// Falta revisar esta condicion para que sea mas clara y precisa
	//cout<<"VirtualSequence::verifyDecompression - Mutaciones: "<<mutations.size()<<"\n";
	if( mutations.size() >= size/4 ){
		// Si NO es dueño de datos, pedirlos antes de agrgar mutaciones
		// Si ya tiene datos, bsata con agregar las mutaciones
		//cout<<"VirtualSequence::verifyDecompression - Descomprimiendo secuencia por numero de mutaciones\n";
		if(!owns_data){
			++count_mem;
			//cout<<"VirtualSequence::verifyDecompression - Pidiendo memoria\n";
			unsigned char *original_data = data;
			unsigned int data_size = (size>>2);
			if( size & 0x3 ){
				++data_size;
			}
			data = new unsigned char[ data_size ];
			//memset(data, 0, data_size);
			memcpy(data, original_data, size);
			owns_data = true;
		}
		set<seq_size_t>::iterator it;
		seq_size_t pos;
		unsigned int pos_byte;
		unsigned int pos_bit;
		for(it = mutations.begin(); it != mutations.end(); it++){
			pos = *it;
			pos_byte = (pos >> 3);
			pos_bit = (pos & 0x7);
			data[pos_byte] ^= (0x1 << pos_bit);
		}
		mutations.clear();
		
		return true;
	}
	
	return false;
}

void VirtualSequence::mutate(mt19937 *arg_rng){
	++count_mut;
	
	// escoger al azar posicion
	// mutar (agregar a mapa)
	// para que esto sea totalmente efectivo, 
	// ...hay que mutar hasta encontrar una posicion no escogida previamente (o el maximo)
	if(arg_rng == NULL){
		arg_rng = &rng;
	}
	/*
	// Version de texto completo, podría usarse una expresión reducida de la mutacion
	// En esta version NO estoy verificando que la mutacion sea efectiva
	uniform_int_distribution<> pos_dist(0, size-1);
	uniform_int_distribution<> val_dist(0, alphabet_size-1);
	seq_size_t pos = pos_dist(*arg_rng);
	char value = alphabet[val_dist(*arg_rng)];
	mutations[pos] = value;
	*/
	
	/*
	// Version replica de Reference
	uniform_int_distribution<> mutation(0U, size-1);
	uint32_t m = mutation(rng);
	int p = 0, n = 0;
	char mask = 1;
	p = int(floor(double(m)/double(4)));
	n = int(m%uint32_t(4));
	
	// Tomo el dato y le aplico las mutaciones apropiadas
	unsigned char dato = data[p];
	// mutaciones para el dato
//	for(unsigned int i = 0; i < 4; ++i){
//		map<seq_size_t, char>::iterator res = mutations.find(p+i);
//		if( res != mutations.end() ){
//			switch ( res->second ){
//				case 'A' : 
//					
//					break;
//			}
//			
//		}
//	}
	
	char value = (dato)&(mask<<n);
	
	if(!value) value = mask<<n;
	
//	this->_data[p] ^= value;
	unsigned char c = dato;
	c ^= value;
	
	// Revision de resultados (notar que estoy reconstruyendo la posicion del char modificado)
	// Eso es porque el original usa un rand en nucleotidos, no en bits (que seria mas directo)
	
	cout<<"VirtualSequence::mutate - data["<<p<<"]: "<<(unsigned int)dato<<" -> "<<(unsigned int)c<<"\n";
	mask = 0x1;
	for( n = 0; n < 8; ++n ){
		if( (c & mask) != (dato & mask) ){
			break;
		}
		mask <<= 1;
	}
	n /= 2;
	cout<<"VirtualSequence::mutate - Pos real: "<<(p*4+3-n)<<", char: "<<alphabet[ (c>>(n*2)) ]<<"\n";
	
	cout<<"VirtualSequence::mutate - Agregando mutations["<<p*4+3-n<<"/"<<size<<"]: "<<alphabet[ (c>>(n*2)) ]<<"\n";
	mutations[p*4+3-n] = alphabet[ (c>>(n*2)) ];
	*/
	
	
	// Version directa en bits
	// Notar que aqui pos representa la poscion del bit mutado, de ahi el size*2
	uniform_int_distribution<> pos_dist(0, (size<<1) - 1);
	seq_size_t pos = pos_dist(*arg_rng);
	set<seq_size_t>::iterator it = mutations.find(pos);
	if(it == mutations.end()){
		mutations.insert(pos);
	}
	else{
		mutations.erase(it);
	}
	
	verifyDecompression();
	
}

// Aplica una mutacion cambiando el BIT de la posicion absoluta pos 
void VirtualSequence::mutateBit(unsigned int pos){
	if(pos >= ((unsigned int)size<<1)){
//		cout<<"VirtualSequence::mutateBit - Omitiendo bit "<<pos<<"\n";
		return;
	}
//	cout<<"VirtualSequence::mutateBit - Modificando bit "<<pos<<"\n";
	set<seq_size_t>::iterator it = mutations.find(pos);
	if(it == mutations.end()){
		mutations.insert(pos);
	}
	else{
		mutations.erase(it);
	}
	verifyDecompression();
}

// Aplica una mutacion cambiando TODOS los bits de mask, partiendo desde el byte byte_ini de data
// Notar que esto puede modificar un maximo de 4 bytes (32 bits de mask)
void VirtualSequence::mutateBitMask(unsigned int mask, unsigned int byte_ini){
	++count_mut;
	unsigned int bit_ini = (byte_ini<<3);
	unsigned int test_mask = 0x1;
	for(unsigned int pos_mask = 0; pos_mask < 32; ++pos_mask){
		if( mask & test_mask ){
			mutateBit(bit_ini + pos_mask);
		}
		test_mask <<= 1;
	}
}

char VirtualSequence::at(seq_size_t pos) const{
	if(pos >= size || data == NULL){
		// exception !
		return 0;
	}
	/*
	// Aplico la mutacion si existe (es decir, tiene prioridad)
	map<seq_size_t, char>::const_iterator res = mutations.find(pos);
	if( res != mutations.end() ){
		return res->second;
	}
	else{
		// Primero tomo el byte (data en posicion pos/4)
		unsigned char byte = data[ pos>>2 ];
//		cout<<"VirtualSequence::at - pos: "<<pos<<", byte: "<<(unsigned int)byte<<"\n";
		// Ahora tomo el valor correcto del byte, los 2 bits que busco
		// Para eso aplico la mascara 0x3 (2 bits) corrida en el resto de pos/4, x2
		// El resto de pos/4 lo calculo como pos & 0x3 (por los 2 bits de la division)
		// El x2 (porque son 2 bits por posicion) lo aplico con un <<1 adicional
		unsigned char val = byte & (0x3 << ((pos & 0x3)<<1) );
//		cout<<"VirtualSequence::at - val: "<<(unsigned int)val<<"\n";
		// Por ultimo, muevo el valor (que estaba en medio del byte) a su posicion inicial
		val >>= ((pos & 0x3)<<1);
//		cout<<"VirtualSequence::at - final val: "<<(unsigned int)val<<" ("<<alphabet[val]<<")\n";
		return alphabet[val];
	}
	*/
	
	// Version con mutaciones a nivel de bits
	// Primero tomo el byte (data en posicion pos/4)
	unsigned char byte = data[ pos>>2 ];
//	cout<<"VirtualSequence::at - pos: "<<pos<<", byte: "<<(unsigned int)byte<<"\n";
	// Ahora tomo el valor correcto del byte, los 2 bits que busco
	// Para eso aplico la mascara 0x3 (2 bits) corrida en el resto de pos/4, x2
	// El resto de pos/4 lo calculo como pos & 0x3 (por los 2 bits de la division)
	// El x2 (porque son 2 bits por posicion) lo aplico con un <<1 adicional
	unsigned char val = byte & (0x3 << ((pos & 0x3)<<1) );
//	cout<<"VirtualSequence::at - val: "<<(unsigned int)val<<"\n";
	// Por ultimo, muevo el valor (que estaba en medio del byte) a su posicion inicial
	val >>= ((pos & 0x3)<<1);
//	cout<<"VirtualSequence::at - val ajustado: "<<(unsigned int)val<<" ("<<alphabet[val]<<")\n";
	// Ahora aplico mutaciones a ambos bits
	seq_size_t pos_bit_1 = pos<<1;
	seq_size_t pos_bit_2 = (pos<<1) + 1;
//	cout<<"VirtualSequence::at - Buscando mutacion en bits: "<<pos_bit_1<<" y "<<pos_bit_2<<"\n";
	if(mutations.find(pos_bit_1) != mutations.end()){
		val ^= 0x1;
//		cout<<"VirtualSequence::at - val mut bit 1: "<<(unsigned int)val<<" ("<<alphabet[val]<<")\n";
	}
	if(mutations.find(pos_bit_2) != mutations.end()){
		val ^= 0x2;
//		cout<<"VirtualSequence::at - val mut bit 2: "<<(unsigned int)val<<" ("<<alphabet[val]<<")\n";
	}
//	cout<<"VirtualSequence::at - val final: "<<(unsigned int)val<<" ("<<alphabet[val]<<")\n";
	return alphabet[val];
	
}

string VirtualSequence::to_string(){
	string seq;
	if(size == 0 || data == NULL){
		return seq;
	}
	for(unsigned int i = 0; i < size; ++i){
		// La logica de la decodificacion y aplicacion de mutaciones esta en at()
		seq.push_back(at(i));
	}
	// reverse ???
	// Esto no es necesario ahora que el at decodifica en el orden correcto
	// reverse(seq.begin(), seq.end());
	return seq;
}

/*
vector< pair<seq_size_t, char> > VirtualSequence::get_mutations(){
	vector< pair<seq_size_t, char> > res;
	map<seq_size_t, char>::iterator it;
	for(it = mutations.begin(); it != mutations.end(); it++){
		res.push_back(pair<seq_size_t, char>(it->first, it->second));
	}
	return res;
}
*/

vector<seq_size_t> VirtualSequence::get_mutations(){
	vector<seq_size_t> res;
	res.insert(res.begin(), mutations.begin(), mutations.end());
	return res;
}

void VirtualSequence::increase(){
	++cur_count;
}

void VirtualSequence::decrease(){
	--cur_count;
}

uint32_t VirtualSequence::count() const{
	return cur_count;
}

bool VirtualSequence::read_only() const{
	return cur_read;
}

bool VirtualSequence::operator==(const VirtualSequence &seq){
	/*
	if( mutations.size() != seq.mutations.size() ){
		return false;
	}
	map<seq_size_t, char>::const_iterator it1, it2;
	for( it1 = mutations.begin(), it2 = seq.mutations.begin(); it1 != mutations.end(); it1++, it2++ ){
		if( (it1->first != it2->first) || (it1->second != it2->second) ){
			return false;
		}
	}
	return true;
	*/
	
	/*
	// Version mas general
	// Notar que este esquema es MUCHO mas lento
	// Se puede implementar una version mejor de la revision de secuencia y de mutaciones
	// Esa version tendria que revisar el largo, los punteros a la secuencia,
	// ... la secuencia (si los punteros son diferentes) y luego cada estructura de mutaciones
	if( length() != seq.length() ){
		return false;
	}
	for(unsigned int i = 0; i < length(); ++i ){
		if(at(i) != seq.at(i)){
			return false;
		}
	}
	return true;
	*/
	
//	cout<<"VirtualSequence::operator== - Inicio ("<<(unsigned long long)this<<")\n";
	
	// Version detallada de revision por componente
	if( length() != seq.length() ){
		return false;
	}
	// Creo que esta comparacion hay que hacerla al reves, verificar si son iguales para acelerar, revisar por caracter si no se puede
	// Eso es porque los datos PUEDEN ser diferentes y aun asi ser iguales las secuencias considerando las mutaciones
	if( data == seq.data ){
		if( mutations.size() != seq.mutations.size() ){
			return false;
		}
//		cout<<"VirtualSequence::operator== - Comparando mutaciones\n";
		// Solo en este caso se puede facilmente comparar las mutaciones sin los datos
		set<seq_size_t>::const_iterator it1, it2;
		for( it1 = mutations.begin(), it2 = seq.mutations.begin(); it1 != mutations.end(); it1++, it2++ ){
			if( *it1 != *it2 ){
				return false;
			}
		}
		return true;
	}
//	cout<<"VirtualSequence::operator== - Peor caso\n";
	for(unsigned int i = 0; i < length(); ++i ){
		if(at(i) != seq.at(i)){
			return false;
		}
	}
//	cout<<"VirtualSequence::operator== - Iguales\n";
	return true;
	
}

unsigned int VirtualSequence::length() const{
	return (unsigned int)size;
}














