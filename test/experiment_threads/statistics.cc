
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#include <map>
#include <vector>

using namespace std;

int main(int argc,char** argv){

	if(argc != 8){
		cout<<"\nUsage: ./test results_base n_threads n_stats n_params target_file percentage distributions_file\n";
		cout<<"\n";
		return 0;
	}
	
	string results_base = argv[1];
	unsigned int n_threads = atoi(argv[2]);
	unsigned int n_stats = atoi(argv[3]);
	unsigned int n_params = atoi(argv[4]);
	string target_file = argv[5];
	float percentage = atof(argv[6]);
	string distributions_file = argv[7];
	
	cout << "Statistics - Inicio (n_stats: " << n_stats << ", n_params: " << n_params << ", percentage: " << percentage << ")\n";
	
	vector< vector<double> > stats;
	vector< vector<double> > params;
	unsigned int buff_size = 1020*1024;
	char buff[buff_size];
	fstream lector;
	double value = 0;
	cout << "Statistics - Leyendo resultados\n";
	for(unsigned int pid = 0; pid < n_threads; ++pid){
		string file_name = results_base;
		file_name += std::to_string(pid);
		file_name += ".txt";
		lector.open(file_name, fstream::in);
		
		while(true){
			lector.getline(buff, buff_size);
			if( !lector.good() || strlen(buff) < 1 ){
				break;
			}
			
			string line(buff);
			stringstream toks(line);
			
			vector<double> res_stats;
			for(unsigned int i = 0; i < n_stats; ++i){
				value = 0.0;
				toks >> value;
				res_stats.push_back(value);
			}
			stats.push_back(res_stats);
			
			vector<double> res_params;
			for(unsigned int i = 0; i < n_stats; ++i){
				value = 0.0;
				toks >> value;
				res_params.push_back(value);
			}
			params.push_back(res_params);
			
		}
		
		lector.close();
	}
	cout << "Statistics - Resultados: " << stats.size() << "\n";
	
	
	vector<double> target;
	cout << "Statistics - Cargando target\n";
	lector.open(target_file, ifstream::in);
	if( ! lector.is_open() ){
		cout << "Statistics - Problemas al abrir archivo \"" << target_file << "\"\n";
		return 1;
	}
	else{
		lector.getline(buff, buff_size);
		
		//Linea valida de largo > 0
		string line(buff);
		stringstream toks(line);
		
		// stats
		for(unsigned int i = 0; i < n_stats; ++i){
			value = 0.0;
			toks >> value;
			target.push_back( value );
		}
		
		cout << "Target: ";
		for(unsigned int i = 0; i < target.size(); ++i){
			cout << target[i] << " | ";
		}
		cout << "\n";
	
	}
	lector.close();
	
	cout << "Statistics - Calculando minimos y maximos por stat\n";
	vector<double> min_stats;
	vector<double> max_stats;

	// Inicializo con los valores del target y busco en los resultados
	for( double d : target ){
		min_stats.push_back(d);
		max_stats.push_back(d);
	}
	
	unsigned int procesados = 0;
	for( vector<double> &res_stats : stats ){
		++procesados;
		for(unsigned int i = 0; i < res_stats.size(); ++i){
			value = res_stats[i];
			if( value < min_stats[i] ){
				min_stats[i] = value;
			}
			if( value > max_stats[i] ){
				max_stats[i] = value;
			}
		}
	}
	cout << "Statistics - Procesados para min/max: " << procesados << "\n";
	
	// Verificacion
	cout << "Min: ";
	for(unsigned int i = 0; i < n_stats; ++i){
		cout << min_stats[i] << " | ";
	}
	cout << "\n";
	cout << "Max: ";
	for(unsigned int i = 0; i < n_stats; ++i){
		cout << max_stats[i] << " | ";
	}
	cout << "\n";
	
	cout << "Statistics - Normalizando target y stats de resultados\n";
	for(unsigned int i = 0; i < target.size(); ++i){
//		target[i] = normalizar(target[i], min_stats[i], max_stats[i]);
		target[i] = ( target[i] - min_stats[i] )/(max_stats[i] - min_stats[i]);
	}
	for( vector<double> &res_stats : stats ){
		for(unsigned int i = 0; i < res_stats.size(); ++i){
//			res_stats[i] = normalizar(res_stats[i], min_stats[i], max_stats[i]);
			res_stats[i] = ( res_stats[i] - min_stats[i] )/(max_stats[i] - min_stats[i]);
		}
	}
	
	// Verificacion
	cout << "Target: ";
	for(unsigned int i = 0; i < n_stats; ++i){
		cout << target[i] << " | ";
	}
	cout << "\n";
	for(unsigned int k = 0; k < 10 && k < stats.size(); ++k){
		cout << "Res[" << k << "]: ";
		for(unsigned int i = 0; i < n_stats; ++i){
			cout << stats[k][i] << " | ";
		}
		cout << "\n";
	}
	
//	vector<double> distancia;
	
	
	
	cout << "Statistics - Fin\n";
	
	
	
	
	
	return 0;
}













