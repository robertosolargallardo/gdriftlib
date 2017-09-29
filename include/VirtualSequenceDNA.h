#ifndef _VIRTUAL_SEQUENCE_DNA_H_
#define _VIRTUAL_SEQUENCE_DNA_H_

#include "VirtualSequence.h"

using namespace std;


class VirtualSequenceDNA : public VirtualSequence {

protected:
	
	// Texto de referencia de la INSTANCIA (=> ahorra espacio solo el constructor de copia)
	// Notar que size es el largo del TEXTO (n_bytes = size/4)
	seq_size_t size;
	unsigned char *data;
	bool owns_data;
	
	// Variables de la instancia (mutaciones)
	vector<seq_size_t> mutations;
	
	// Estructura para inserciones
	vector< pair<seq_size_t, char> > inserts;
	
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
	
	VirtualSequenceDNA();
	VirtualSequenceDNA(unsigned int _size);
	VirtualSequenceDNA(const char *_ref, unsigned int _size);
	VirtualSequenceDNA(const string &_ref);
	VirtualSequenceDNA(const unsigned int _size, const unsigned int _seq);
	VirtualSequenceDNA(const VirtualSequenceDNA &original);
	virtual ~VirtualSequenceDNA();
	virtual VirtualSequence *clone();
		
	VirtualSequenceDNA& operator=(const VirtualSequenceDNA& original);
	virtual bool operator==(const VirtualSequenceDNA&);
	
	virtual void mutate(void);
	virtual char at(seq_size_t pos) const;
	
	// Aplica una mutacion cambiando el BIT de la posicion absoluta pos 
	// Este metodo puede entrar en conflicto con insert
	void mutateBit(unsigned int pos);
	
	// Aplica una mutacion cambiando TODOS los bits de mask, partiendo desde el byte byte_ini de data
	// Notar que esto puede modificar un maximo de 4 bytes (32 bits de mask)
	// Este metodo puede entrar en conflicto con insert
	void mutateBitMask(unsigned int mask, unsigned int byte_ini = 0);
	
	// Este metodo puede entrar en conflicto con insert
	void mutateInsert(seq_size_t pos, char c);
	
	virtual unsigned int length() const;
	
	// Convertir la secuencia completa en string
	virtual string to_string();
	
	// Este metodo es similar a to_string pero solo codifica las mutaciones de la secuencia
	// Notar que esto deberia poder usarse con otros tipos de secuencia, incluso MS
	virtual string codeMutations() const;
	
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
	
//	// Contador de constructores
//	static unsigned int count_str;
//	static unsigned int count_int;
//	static unsigned int count_copy;
//	static unsigned int count_mem;
//	static unsigned int count_del;
//	static unsigned int count_del_mem;
//	static unsigned int count_mut;
	
};

#endif
