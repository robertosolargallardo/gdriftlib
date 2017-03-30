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

using namespace std;


// #define seq_size_t unsigned long long
 #define seq_size_t unsigned int
//#define seq_size_t unsigned short

// Tercera Version, texto en 2 bits por base
// En esta version, el constructor de copia ahorra espacio (guarda solo un puntero al buff del original)

extern random_device seed;
extern mt19937 rng;

class VirtualSequence{

protected:

	static const unsigned int alphabet_size;
	static const char alphabet[];
	
//	static mt19937 class_rng;
	
	// Texto de referencia de la INSTANCIA (=> ahorra espacio solo el constructor de copia)
	// Notar que size es el largo del TEXTO (n_bytes = size/4)
	seq_size_t size;
	unsigned char *data;
	bool owns_data;
	
	// Variables de la instancia (mutaciones)
//	set<seq_size_t> mutations;
	vector<seq_size_t> mutations;
	
	// Estructura para inserciones
	vector< pair<seq_size_t, char> > inserts;
	
	uint32_t cur_count;
	bool cur_read;
	
	// Verifica si el espacio de mutaciones es demasiado, para aplicar descompresion
	// En ese caso, pide memoria (si no tiene ya reservada), vuelca las mutaciones en data y elimina las mutaciones
	// Deberia llamarse a este metodo luego de aplicar cualquier mutacion
	// Este metodo debe considerar las diferentes formas de mutacion para el espacio
	bool verifyDecompression();
	
	// Cuenta el numero de inserts hasta pos
	// Si pos calza exactamente con un insert, retorna el caracter
	seq_size_t countInserts(seq_size_t pos, char &res) const;
	
//	const static unsigned int NOT_FOUND = 0xffffffff;
//	unsigned int findMutation(seq_size_t pos) const;
	bool findMutation(seq_size_t pos, seq_size_t &pos_mut) const;

public:
	
	VirtualSequence(bool _read_only = true);
	VirtualSequence(const char *_ref, unsigned int _size, bool _read_only = true);
	VirtualSequence(const string &_ref, bool _read_only = true);
	VirtualSequence(const unsigned int _size, const unsigned int _seq, bool _read_only = true);
	VirtualSequence(const VirtualSequence &original);
	
	virtual ~VirtualSequence();
	
	void mutate(mt19937 *arg_rng = NULL);
	char at(seq_size_t pos) const;
	
	// Aplica una mutacion cambiando el BIT de la posicion absoluta pos 
	// Este metodo puede entrar en conflicto con insert
	void mutateBit(unsigned int pos);
	
	// Un metodo para aplicar varias mutaciones de una vez puede ser mejor
	// Esto es porque, al ser un vector, hay que hacer ajustes con cada insercion
	// Se podria implementar una version que prepare las inserciones y solo haga un sort al final
	// void mutateMany(unsigned int num_mutations);
	
	// Aplica una mutacion cambiando TODOS los bits de mask, partiendo desde el byte byte_ini de data
	// Notar que esto puede modificar un maximo de 4 bytes (32 bits de mask)
	// Este metodo puede entrar en conflicto con insert
	void mutateBitMask(unsigned int mask, unsigned int byte_ini = 0);
	
	// Este metodo puede entrar en conflicto con insert
	void mutateInsert(seq_size_t pos, char c);
	
	string to_string();
	
	void increase();
	void decrease();
	uint32_t count() const;
	bool read_only() const;
	bool operator==(const VirtualSequence&);
	unsigned int length() const;
	
	// Metodo para debug
	void printData();
	
	// Metodos de acceso directo a los datos
	// Esto es para calcular estadisticas mas rapido
	unsigned char *getData(){
		return data;
	}
	vector<seq_size_t> &getMutations(){;
		return mutations;
	}
	
	// Contador de constructores
	static unsigned int count_str;
	static unsigned int count_int;
	static unsigned int count_copy;
	static unsigned int count_mem;
	static unsigned int count_del;
	static unsigned int count_del_mem;
	static unsigned int count_mut;
	
};

#endif
