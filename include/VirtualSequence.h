#ifndef _VIRTUAL_SEQUENCE_H_
#define _VIRTUAL_SEQUENCE_H_

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <limits.h>

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <random>

#include <thread>
#include <mutex>
#include <climits>

using namespace std;

// Number of nucleotides, equal to alphabet_size
#define N_NUCLEOTIDES 4

// #define seq_size_t unsigned long long
#define seq_size_t unsigned int
//#define seq_size_t unsigned short

//#define VS_DEBUG

extern random_device seed;
extern mt19937 rng;

class VirtualSequence{

protected:
		
	static mutex internal_mutex;

	static const unsigned int alphabet_size;
	static const char alphabet[];
	
	// Contador de uso
	uint32_t cur_count;

public:
	
	VirtualSequence();
	VirtualSequence(const VirtualSequence &original);
	virtual ~VirtualSequence();
	virtual VirtualSequence *clone();
	
	VirtualSequence& operator=(const VirtualSequence& original);
	
	// Aqui hay que hacer comparacion de tipo antes de comparar los datos
	virtual bool operator==(const VirtualSequence&);
	
	virtual void mutate(mt19937 *arg_rng = NULL);
	virtual char at(seq_size_t pos) const;
	
	// Metodos relacionados con el contador de uso NO son virtuales
	// Esto es fijo para todas las clases
	unsigned int count() const;
	void increase();
	void decrease();
	
	// Esto podria ser redefinido si una secuencia permite insert/delete
	virtual unsigned int length() const;
	
	// Convertir la secuencia completa en string
	virtual string to_string();
	
	// Este metodo es similar a to_string pero solo codifica las mutaciones de la secuencia
	// Notar que esto deberia poder usarse con otros tipos de secuencia, incluso MS
	virtual string codeMutations() const;
	
	// Metodo para debug
	virtual void printData();
	
	// Contador de la clase
	static unsigned int count_str;
	static unsigned int count_int;
	static unsigned int count_copy;
	static unsigned int count_mem;
	static unsigned int count_del;
	static unsigned int count_del_mem;
	static unsigned int count_mut;
	
};

#endif
