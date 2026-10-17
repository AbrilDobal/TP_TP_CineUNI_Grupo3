#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include "funciones.hpp"
using namespace std;

int main(int argc, char** argv) {

	//Carga inicial de la cartelera (Modulo A)
	NodoPelicula* listaPeliculas = cargarPeliculas();

	//TEMPORAL: lectura minima de salas.dat solo para poder probar este modulo.
	//Se reemplaza por cargarSalas() del Modulo B cuando se integre el codigo.
	Sala salas[MAX_SALAS];
	int cantSalas = 0;
	FILE* archSalas = fopen("salas.dat","rb");
	if(archSalas != NULL){
		while(cantSalas < MAX_SALAS && fread(&salas[cantSalas],sizeof(Sala),1,archSalas) == 1){
			cantSalas++;
		}
		fclose(archSalas);
	}

	NodoCliente* colafte = NULL;
	NodoCliente* colafin = NULL;

	int opcion;

	do{
		cout << endl << "===== CineUNI - Cartelera y Boleteria =====" << endl;
		cout << "1. Listar cartelera" << endl;
		cout << "2. Alta de pelicula" << endl;
		cout << "3. Baja de pelicula" << endl;
		cout << "4. Buscar por clasificacion" << endl;
		cout << "5. Llegada de un cliente" << endl;
		cout << "6. Ver la fila" << endl;
		cout << "0. Salir" << endl;
		cout << "Opcion: ";
		cin >> opcion;

		switch(opcion){
			case 1: listarCartelera(listaPeliculas); break;
			case 2: altaPelicula(listaPeliculas); break;
			case 3: bajaPelicula(listaPeliculas,salas,cantSalas); break;
			case 4: buscarPorClasificacion(listaPeliculas); break;
			case 5: llegadaCliente(colafte,colafin,listaPeliculas,salas,cantSalas); break;
			case 6: verFila(colafte,colafin); break;
			case 0: cout << "Cerrando el sistema..." << endl; break;
			default: cout << "Opcion invalida" << endl;
		}
	} while(opcion != 0);

	//Guardado al salir: reescribe peliculas.dat y libera la lista
	guardarYLiberarPeliculas(listaPeliculas);

	return 0;
}