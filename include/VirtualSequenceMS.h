#ifndef _VIRTUAL_SEQUENCE_MS_H_
#define _VIRTUAL_SEQUENCE_MS_H_

#include "VirtualSequence.h"

using namespace std;

class VirtualSequenceMS : public VirtualSequence{

protected:
	string tandem;
	unsigned int repeats;

public:
	
	VirtualSequenceMS();
	VirtualSequenceMS(string _tandem, unsigned int _repeats);
	VirtualSequenceMS(const VirtualSequenceMS &original);
	virtual ~VirtualSequenceMS();
	virtual VirtualSequence *clone();
		
	VirtualSequenceMS& operator=(const VirtualSequenceMS& original);
	
	// Aqui hay que hacer comparacion de tipo antes de comparar los datos
	virtual bool operator==(const VirtualSequenceMS&);
	
	virtual void mutate(mt19937 *arg_rng = NULL);
	virtual char at(seq_size_t pos) const;
	
	// Esto podria ser redefinido si una secuencia permite insert/delete
	virtual unsigned int length() const;
	
	// Convertir la secuencia completa en string
	virtual string to_string();
	
	// Este metodo es similar a to_string pero solo codifica las mutaciones de la secuencia
	// Notar que esto deberia poder usarse con otros tipos de secuencia, incluso MS
	virtual string codeMutations() const;
	
	// Metodo para debug
	virtual void printData();
	
	// Metodos de acceso directo a los datos
	// Esto es para calcular estadisticas mas rapido
	string getTandem(){
		return tandem;
	}
	
	unsigned int getRepeats(){
		return repeats;
	}
	
	
	
	
};

#endif
