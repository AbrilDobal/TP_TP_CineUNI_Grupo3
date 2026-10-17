#ifndef FUNCIONES_HPP
#define FUNCIONES_HPP

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
using namespace std;

const int MAX_FIL = 10;
const int MAX_COL = 12;
const int MAX_SALAS = 10;

//Registros obligatorios (enunciado)
struct Pelicula {
	int   codigo;
	char  titulo[50];
	int   duracion;          // en minutos
	char  clasificacion[5];  // "ATP", "+13", "+16"
	float precio;
};

//Sala: la carga y manejo del vector de salas es del Modulo B (otro integrante).
//Aca solo se la consulta (baja de pelicula y llegada de cliente).
struct Sala {
	int nroSala;
	int filas;
	int columnas;
	int codPelicula;
	int asientos[MAX_FIL][MAX_COL];   // 0 = libre, 1 = ocupado
};

//Lista simplemente enlazada de peliculas, ordenada por codigo
struct NodoPelicula {
	Pelicula info;
	NodoPelicula* sgte;
};

//Cliente que espera en la cola de boleteria
struct Cliente {
	char nombre[40];
	int  codPelicula;
	int  cantidad;
};

struct NodoCliente {
	Cliente info;
	NodoCliente* sgte;
};

void leerTexto(char destino[], int maxLen);

//Modulo A - Cartelera
NodoPelicula* cargarPeliculas();
void insertarPeliculaOrdenada(NodoPelicula*& lista, Pelicula p);
void listarCartelera(NodoPelicula* lista);
NodoPelicula* buscarPelicula(NodoPelicula* lista, int codigo);
bool existeCodigoPelicula(NodoPelicula* lista, int codigo);
bool clasificacionValida(char clasificacion[]);
void altaPelicula(NodoPelicula*& lista);
bool peliculaAsignadaASala(Sala salas[], int cantSalas, int codigo);
void bajaPelicula(NodoPelicula*& lista, Sala salas[], int cantSalas);
void buscarPorClasificacion(NodoPelicula* lista);
void guardarYLiberarPeliculas(NodoPelicula*& lista);

//Cola de boleteria
void encolarCliente(NodoCliente*& colafte, NodoCliente*& colafin, Cliente c);
Cliente desencolarCliente(NodoCliente*& colafte, NodoCliente*& colafin);
bool colaClientesVacia(NodoCliente* colafte);
void llegadaCliente(NodoCliente*& colafte, NodoCliente*& colafin, NodoPelicula* listaPeliculas, Sala salas[], int cantSalas);
void verFila(NodoCliente*& colafte, NodoCliente*& colafin);

#endif