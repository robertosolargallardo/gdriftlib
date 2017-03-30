#include "VirtualSequence.h"

const unsigned int VirtualSequence::alphabet_size = 4;
const char VirtualSequence::alphabet[] = {'A', 'C', 'G', 'T'};
////mt19937 VirtualSequence::class_rng;
unsigned int VirtualSequence::count_str = 0;
unsigned int VirtualSequence::count_int = 0;
unsigned int VirtualSequence::count_copy = 0;
unsigned int VirtualSequence::count_mem = 0;
unsigned int VirtualSequence::count_del = 0;
unsigned int VirtualSequence::count_del_mem = 0;
unsigned int VirtualSequence::count_mut = 0;

mutex VirtualSequence::internal_mutex;

VirtualSequence::VirtualSequence(bool _read_only){

	size = 0;
	data = NULL;
	owns_data = false;
	
	cur_count = 0;
	cur_read = _read_only;
}

// Constructor real que COPIA el buffer de texto
VirtualSequence::VirtualSequence(const char *_ref, unsigned int _size, bool _read_only){
	
	#ifdef VS_DEBUG
	internal_mutex.lock();
	++count_str;
	++count_mem;
	internal_mutex.unlock();
	#endif
	
//	cout<<"VirtualSequence - Inicio (text: "<<_ref<<", size: "<<_size<<")\n";
	size = (seq_size_t)_size;
	unsigned int data_size = (size>>2);
	if( size & 0x3 ){
		++data_size;
	}
//	cout<<"VirtualSequence - Pidiendo "<<data_size<<" bytes de memoria\n";
	data = new unsigned char[ data_size ];
	memset(data, 0, data_size);
	
//	cout<<"VirtualSequence - data: "<<(unsigned long long)data<<"\n";
	
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
//		cout<<"VirtualSequence - data["<<i<<"]: "<<(unsigned int)data[i]<<"\n";
//	}
	
//	cout<<"VirtualSequence - Fin\n";
	
	cur_count = 0;
	cur_read = _read_only;
}

// Constructor real que COPIA el buffer de texto, pero que recibe solo 1 entero (32 bits) de secuencia
// Llena el resto de A's (con un memset... 0)
VirtualSequence::VirtualSequence(const unsigned int _size, const unsigned int _seq, bool _read_only){
	
	#ifdef VS_DEBUG
	internal_mutex.lock();
	++count_int;
	++count_mem;
	internal_mutex.unlock();
	#endif
	
	size = (seq_size_t)_size;
	unsigned int data_size = (size>>2);
	if( size & 0x3 ){
		++data_size;
	}
//	cout<<"VirtualSequence - Pidiendo "<<data_size<<" bytes de memoria\n";
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
	owns_data = true;
	
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
	
	#ifdef VS_DEBUG
	internal_mutex.lock();
	++count_copy;
	internal_mutex.unlock();
	#endif
	
//	cout<<"VirtualSequence - Copia\n";
	size = original.size;
	data = original.data;
	owns_data = false;
	//cout<<"VirtualSequence - size: "<<size<<". data[0]: "<<((data==NULL)?(0xffffffff):(unsigned int)data[0])<<"\n";
	
	cur_count = 0;
	cur_read = original.cur_read;
	
	// Muaciones
	//cout<<"VirtualSequence - Agregando "<<original.mutations.size()<<" mutaciones\n";
//	mutations.insert(original.mutations.begin(), original.mutations.end());
	mutations.insert(mutations.begin(), original.mutations.begin(), original.mutations.end());
}

VirtualSequence::~VirtualSequence(){
//	cout<<"VirtualSequence::~VirtualSequence \n";
	
	#ifdef VS_DEBUG
	internal_mutex.lock();
	if(owns_data){
		++count_del_mem;
	}
	else{
		++count_del;
	}
	internal_mutex.unlock();
	#endif
	
	inserts.clear();
	mutations.clear();
	if(owns_data){
//		cout<<"VirtualSequence::~VirtualSequence - Borrando data original\n";
		delete [] data;
		data = NULL;
		owns_data = false;
	}
	data = NULL;
	size = 0;
}

// Ajustar este metodo para cada tipo de mutacion
bool VirtualSequence::verifyDecompression(){
	
	// Condicion de descompresion
	// Las mutaciones se vuelcan en un nuevo arreglo de datos si son muchas
	// De momento desactivo esto mientras verificamos las condiciones
//	cout<<"VirtualSequence::verifyDecompression - Mutaciones: "<<mutations.size()<<"\n";
	if( false && mutations.size() >= size/4 ){
		// Si NO es dueño de datos, pedirlos antes de agrgar mutaciones
		// Si ya tiene datos, bsata con agregar las mutaciones
//		cout<<"VirtualSequence::verifyDecompression - Descomprimiendo secuencia por numero de mutaciones\n";
		if(!owns_data){
			++count_mem;
			unsigned char *original_data = data;
			unsigned int data_size = (size>>2);
			if( size & 0x3 ){
				++data_size;
			}
//			cout<<"VirtualSequence::verifyDecompression - Pidiendo "<<data_size<<" bytes de memoria\n";
			data = new unsigned char[ data_size ];
			//memset(data, 0, data_size);
			memcpy(data, original_data, data_size);
			owns_data = true;
		}
//		set<seq_size_t>::iterator it;
		vector<seq_size_t>::iterator it;
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

// Este metodo asume que el arreglo de mutaciones esta ordenado crecientemente
// Retorna true SOLO si la mutacion fue encontrada
// Almacena en mut_pos la posicion de la mutacion (si fue encontrada) o del primer elemento mayor (donde tendria que ser insertada)
bool VirtualSequence::findMutation(seq_size_t pos, seq_size_t &mut_pos) const{
	mut_pos = 0;
	if( mutations.empty() ){
		return false;
	}
	// seq_size_t h = 0;
	seq_size_t l = 0;
	seq_size_t h = (seq_size_t)mutations.size() - 1;
	seq_size_t m;
	while(l < h){
		m = l + ((h-l)>>1);
		if( mutations[m] < pos ){
			l = m+1;
		}
		else{
			h = m;
		}
	}
//	cout<<"VirtualSequence::findMutation - BB terminada (h: "<<h<<", l: "<<l<<")\n";
	if( (h < mutations.size()) && (mutations[h] < pos) ){
		++h;
	}
	mut_pos = h;
	if( (h < mutations.size()) && (mutations[h] == pos) ){
		return true;
	}
	return false;
}

// Este metodo puede entrar en conflicto con insert
void VirtualSequence::mutate(mt19937 *arg_rng){
	
	#ifdef VS_DEBUG
	internal_mutex.lock();
	++count_mut;
	internal_mutex.unlock();
	#endif
	
	if(arg_rng == NULL){
		arg_rng = &rng;
	}
	
//	cout<<"VirtualSequence::mutate - Inicio\n";
	
	// Version directa en bits
	// Notar que aqui pos representa la poscion del bit mutado, de ahi el size*2
	uniform_int_distribution<> pos_dist(0, (size<<1) - 1);
	seq_size_t pos = pos_dist(*arg_rng);
	mutateBit(pos);
	
}

void VirtualSequence::printData(){
	cout<<"VirtualSequence["<<(unsigned long long)this<<"]: |";
	for(unsigned int i = 0; i < ( (size>16)?16:size ); ++i){
		cout<<(unsigned int)data[i]<<" |";
	}
	cout<<"\n";
	cout<<"Mutations ("<<mutations.size()<<"): ";
//	set<seq_size_t>::iterator it;
	vector<seq_size_t>::iterator it;
	for(it = mutations.begin(); it != mutations.end(); it++){
		cout<<*it<<" ";
	}
//	for(unsigned int i = 0; i < mutations.size(); ++i){
//		cout<<(mutations[i])<<" ";
//	}
	cout<<"\n";
	cout<<"Insertions ("<<inserts.size()<<"): ";
	for(unsigned int i = 0; i < inserts.size(); ++i){
		cout<<"("<<(inserts[i].first)<<", "<<(inserts[i].second)<<") ";
	}
	cout<<"\n";
}
	
// Aplica una mutacion cambiando el BIT de la posicion absoluta pos 
// Este metodo puede entrar en conflicto con insert
void VirtualSequence::mutateBit(unsigned int pos){
	if(pos >= ((unsigned int)size<<1)){
//		cout<<"VirtualSequence::mutateBit - Omitiendo bit "<<pos<<"\n";
		return;
	}
	
//	cout<<"VirtualSequence::mutateBit - Modificando bit "<<pos<<" (VirtualSequence "<<(unsigned long long)this<<", "<<mutations.size()<<" mutations)\n";
	seq_size_t pos_mut = 0;
	if(! findMutation(pos, pos_mut)){
//		cout<<"VirtualSequence::mutateBit - Agregando "<<pos<<" (en "<<pos_mut<<")\n";
		mutations.insert(mutations.begin() + pos_mut, pos);
	}
	else{
//		cout<<"VirtualSequence::mutateBit - Eliminando "<<pos<<" (en "<<pos_mut<<")\n";
		mutations.erase(mutations.begin() + pos_mut);
		
	}
	
//	cout<<"VirtualSequence::mutateBit - Resultado: ";
//	for(unsigned int i = 0; i < mutations.size(); ++i){
//		cout<<mutations[i]<<" ";
//	}
//	cout<<"\n";
		
	verifyDecompression();
}

// Aplica una mutacion cambiando TODOS los bits de mask, partiendo desde el byte byte_ini de data
// Notar que esto puede modificar un maximo de 4 bytes (32 bits de mask)
void VirtualSequence::mutateBitMask(unsigned int mask, unsigned int byte_ini){

	#ifdef VS_DEBUG
	internal_mutex.lock();
	++count_mut;
	internal_mutex.unlock();
	#endif
	
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
	if(pos >= length() || data == NULL){
		// exception !
		return 0;
	}
	
	// Si hay inserciones y borrados, la posicion puede no corresponder a la posicion directa de los datos
	// En ese caso sera necesario usar una pos ajustada
	// Si pos calza con un insert, la respuesta es directa
	// Si pos es mayor a algun insert, hay que restar el total de inserts a la pos de los datos
	// Esto es similar al ajuste de metadatos en relz
	// Notar tambien que este modelo implica prioridades entre mutaciones diferentes
	// Probablemente el resultado seria primero verificar borrados, luego inserciones, y luego los datos con sus mutaciones puntuales
	// Actualmente, las mutaciones puntutales SOLO se pueden aplicar a los datos (sin contar inserciones)
	
//	cout<<"VirtualSequence::at - Inicio (pos: "<<pos<<")\n";
	
	/*
	char res = 0;
	unsigned int pos_insert;
	pos_insert = countInserts(pos, res);
	if(res != 0){
		return res;
	}
	else{
		// Esto se puede resolver con una estructura de rank y select
		// La suma de los inserts anteriores a pos es rank(pos)
		// Notar que el siguiente caso nunca deberia pasar, esto es solo precautorio
		if(pos < pos_insert){
			pos = 0;
		}
		else{
			pos -= pos_insert;
		}
//		cout<<"VirtualSequence::at - Ajustando pos: "<<pos<<"\n";
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
//	cout<<"VirtualSequence::at - val ajustado: "<<(unsigned int)val<<" ('"<<alphabet[val]<<"')\n";
	// Ahora aplico mutaciones a ambos bits
	seq_size_t pos_bit_1 = pos<<1;
	seq_size_t pos_bit_2 = (pos<<1) + 1;
//	cout<<"VirtualSequence::at - Buscando mutacion en bits: "<<pos_bit_1<<" y "<<pos_bit_2<<"\n";
//	if(mutations.find(pos_bit_1) != mutations.end()){
	seq_size_t pos_mut = 0;
	if( findMutation(pos_bit_1, pos_mut) ){
		val ^= 0x1;
//		cout<<"VirtualSequence::at - val mut bit 1: "<<(unsigned int)val<<" ('"<<alphabet[val]<<"', pos "<<pos<<", data: "<<(unsigned long long)data<<")\n";
	}
//	if(mutations.find(pos_bit_2) != mutations.end()){
	if( findMutation(pos_bit_2, pos_mut) ){
		val ^= 0x2;
//		cout<<"VirtualSequence::at - val mut bit 2: "<<(unsigned int)val<<" ('"<<alphabet[val]<<"', pos "<<pos<<", data: "<<(unsigned long long)data<<")\n";
	}
//	cout<<"VirtualSequence::at - val final: "<<(unsigned int)val<<" ('"<<alphabet[val]<<"')\n";
	return alphabet[val];
	
}

// Cuenta el numero de inserts hasta pos
// Si pos calza exactamente con un insert, retorna el caracter
// Para este metodo, inserts puede ser un vector< pair<seq_size_t, char> >
seq_size_t VirtualSequence::countInserts(seq_size_t pos, char &res) const{
//	cout<<"VirtualSequence::countInserts - Inicio (pos: "<<pos<<")\n";
	if( inserts.empty() ){
		res = 0;
		return 0;
	}
	// seq_size_t h = 0;
	seq_size_t l = 0;
	seq_size_t h = (seq_size_t)inserts.size() - 1;
	seq_size_t m;
	while(l < h){
		m = l + ((h-l)>>1);
		if( inserts[m].first < pos ){
			l = m+1;
		}
		else{
			h = m;
		}
	}
//	cout<<"VirtualSequence::countInserts - BB terminada (total: "<<h<<")\n";
	// Notar que el while que sigue no es realmente necesario, pero lo uso para debug
	while( (h < inserts.size()) && (inserts[h].first < pos) ){
		++h;
	}
	if(inserts[h].first == pos){
		res = inserts[h].second;
	}
//	cout<<"VirtualSequence::countInserts - Fin (total: "<<h<<", res: '"<<((res==0)?'0':res)<<"')\n";
	return h;
}

void VirtualSequence::mutateInsert(seq_size_t pos, char c){
//	cout<<"VirtualSequence::mutateInsert - Inicio ("<<pos<<", '"<<c<<"')\n";
	if(pos >= length()){
		return;
	}
	char res = 0;
	unsigned int pos_insert;
	pos_insert = countInserts(pos, res);
	if(res != 0){
		// Ya hay una insercion en la misma posicion, reemplazo el char
//		cout<<"VirtualSequence::mutateInsert - Insert replicado (char anterior: '"<<res<<"')\n";
		inserts[pos_insert].second = c;
	}
	else{
		// Agregar insert en pos_insert
//		cout<<"VirtualSequence::mutateInsert - Agregando insert en pos_insert: "<<pos_insert<<"\n";
		vector< pair<seq_size_t, char> >::iterator it = inserts.begin() + pos_insert;
		inserts.insert(it, pair<seq_size_t, char>(pos, c));
	}
}

string VirtualSequence::to_string(){
	string seq;
	if(length() == 0 || data == NULL){
		return seq;
	}
//	cout<<"VirtualSequence::to_string - Decodificando "<<length()<<" caracteres\n";
	for(unsigned int i = 0; i < length(); ++i){
		// La logica de la decodificacion y aplicacion de mutaciones esta en at()
		seq.push_back(at(i));
	}
	// reverse ???
	// Esto no es necesario ahora que el at decodifica en el orden correcto
	// reverse(seq.begin(), seq.end());
//	cout<<"VirtualSequence::to_string - res: \""<<seq<<"\"\n";
	return seq;
}

void VirtualSequence::increase(){
	++cur_count;
}

void VirtualSequence::decrease(){
	if(cur_count > 0){
		--cur_count;
	}
}

uint32_t VirtualSequence::count() const{
	return cur_count;
}

bool VirtualSequence::read_only() const{
	return cur_read;
}

bool VirtualSequence::operator==(const VirtualSequence &seq){
	
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
//		set<seq_size_t>::const_iterator it1, it2;
		vector<seq_size_t>::const_iterator it1, it2;
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
//	return (unsigned int)size;
	return (unsigned int)(size + inserts.size());
}














