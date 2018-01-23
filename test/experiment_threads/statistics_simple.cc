
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#include <algorithm>
#include <math.h>

#include <map>
#include <vector>

using namespace std;

double distance(vector<double> &stats, vector<double> &target){
	double d = 0.0;
	for(unsigned int i = 0; i < target.size(); ++i){
		d += pow( stats[i] - target[i], 2.0 );
	}
	d = pow(d, 0.5);
	return d;
}

int main(int argc,char** argv){

	if(argc != 6){
		cout<<"\nUsage: ./test results_file n_stats n_params target_file final_results\n";
		cout<<"\n";
		return 0;
	}
	
	string results_file = argv[1];
	unsigned int n_stats = atoi(argv[2]);
	unsigned int n_params = atoi(argv[3]);
	string target_file = argv[4];
	string final_results = argv[5];
	
	cout << "Statistics - Inicio (n_stats: " << n_stats << ", n_params: " << n_params << ")\n";
	
	vector< vector<double> > stats;
	vector< vector<double> > stats_norm;
	vector< vector<double> > params;
	unsigned int buff_size = 1020*1024;
	char buff[buff_size];
	fstream lector;
	double value = 0;
	cout << "Statistics - Leyendo resultados\n";
	lector.open(results_file, ifstream::in);
	if( ! lector.is_open() ){
		cout << "Statistics - Problemas al abrir archivo \"" << results_file << "\"\n";
		return 1;
	}
	else{
		
		while(true){
			lector.getline(buff, buff_size);
			if( !lector.good() || strlen(buff) < 1 ){
				break;
			}
			
			string line(buff);
			stringstream toks(line);
			
			vector<double> res_stats;
			vector<double> res_stats_norm;
			for(unsigned int i = 0; i < n_stats; ++i){
				value = 0.0;
				toks >> value;
				res_stats.push_back(value);
				res_stats_norm.push_back(value);
			}
			stats.push_back(res_stats);
			stats_norm.push_back(res_stats_norm);
			
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
		target[i] = ( target[i] - min_stats[i] )/(max_stats[i] - min_stats[i]);
	}
	for( vector<double> &res_stats : stats_norm ){
		for(unsigned int i = 0; i < res_stats.size(); ++i){
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
		cout << "Res_norm[" << k << "]: ";
		for(unsigned int i = 0; i < n_stats; ++i){
			cout << stats_norm[k][i] << " | ";
		}
		cout << "\n";
	}
	
	// Calcular distancias y preparar percentiles
	// El primer arreglo se mantiene en su orden original para la verificacion final
	vector<double> distancias;
	// Copia de distancias para ordenar y extraer percentiles
	vector<double> distancias_sort;
	
	for( vector<double> &res_stats : stats_norm ){
		value = distance( res_stats, target );
		distancias.push_back(value);
		distancias_sort.push_back(value);
	}
	
	std::sort(distancias_sort.begin(), distancias_sort.end());
	double p10 = distancias_sort[ (unsigned int)(0.1*distancias_sort.size()) ];
	double p90 = distancias_sort[ (unsigned int)(0.9*distancias_sort.size()) ];
	cout << "P10: " << p10 << "\n";
	cout << "P90: " << p90 << "\n";
	
	
	// Verificacion
	for(unsigned int k = 0; k < 10 && k < distancias.size(); ++k){
		cout << "distancias_sort[" << k << "]: (" << distancias_sort[k] <<")\n";
	}
	cout<<"-----\n";
	
	// Clasificacion con escritura de resultados
	
	fstream writer(final_results, fstream::out | fstream::trunc);
	unsigned int n10 = 0;
	unsigned int n90 = 0;
	for(unsigned int i = 0; i < distancias.size(); ++i){
		double d = distancias[i];
		writer << d << "\t";
		if( d <= p10 ){
			writer << "1\t";
			++n10;
		}
		else{
			writer << "0\t";
		}
		if( d >= p90 ){
			writer << "1\t";
			++n90;
		}
		else{
			writer << "0\t";
		}
		
		// Conteos para grafico de p10/t y p90/t
		if( (i+1) % 500 == 0 ){
			writer << n10 << "\t";
			n10 = 0;
			writer << n90 << "\t";
			n90 = 0;
		}
		
		writer << "\n";
		
	}
	writer.close();
	
	
	cout << "Statistics - Fin\n";
	
	
	
	
	
	return 0;
}













