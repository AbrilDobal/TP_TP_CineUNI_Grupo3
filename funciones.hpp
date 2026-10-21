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


// Registros obligatorios para el Módulo C y D
struct Venta {
    int   nroVenta;
    char  cliente[40];
    int   codPelicula;
    int   nroSala;
    int   cantidad;
    int   fila[10];// asientos asignados
    int   columna[10];
    float importe;
};

struct ResumenPelicula {
    int   codigo;
    char  titulo[50];
    int   entradasVendidas;
    float recaudacion;
};

// TDA Pila (Dinámica) para operaciones de venta
struct NodoVenta {
    Venta info;
    NodoVenta* sgte;
};

// Declaración de funciones de la Pila (Módulo C)
void apilar(NodoVenta*& pila, Venta v);
Venta desapilar(NodoVenta*& pila);
bool pilaVacia(NodoVenta* pila);

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

// Módulo B - Salas
// Punto B.1: Se hace la carga inicial de las salsa
bool cargarSalas(Sala salas[], int& cantSalas);
// Busca una sala por numero y devuelve la posicion en el vector
// Punto B.2: Muestra el plano grafico de una sala
int buscarSala(Sala salas[], int cantSalas, int nroSala);
void mostrarPlanoSala(Sala salas[], int cantSalas, NodoPelicula* listaPeliculas);
// Punto B.3: Muestra las estadisticas de ocupacion de una sala
void mostrarEstadisticasSala(Sala salas[], int cantSalas);
// Punto B.4: Busqueda de asientos contiguos
bool buscarAsientosContiguos(Sala salas[], int cantSalas, int posSala, int cantidad, int filasAsientos[], int columnasAsientos[]);
void consultarAsientosContiguos(Sala salas[], int cantSalas);
// Punto B.5
bool actualizarSalaArchivo(Sala salas[], int cantSalas, int posSala);
// Pünto B.6
void cambiarPeliculaSala(Sala salas[], int cantSalas, NodoPelicula* listaPeliculas);
// Punto C.3.3 (Se relaciona con B.4)
bool buscarAsientosSeparados(Sala salas[], int cantSalas, int posSala, int cantidad, int filasAsientos[], int columnasAsientos[]);
// Punto C.3.4 (Se relaciona con B.5)
bool marcarAsientosOcupados(Sala salas[], int cantSalas, int posSala, int cantidad, int filasAsientos[], int columnasAsientos[]);

//Cola de boleteria
void encolarCliente(NodoCliente*& colafte, NodoCliente*& colafin, Cliente c);
// Punto C.3.1
Cliente desencolarCliente(NodoCliente*& colafte, NodoCliente*& colafin);
bool colaClientesVacia(NodoCliente* colafte);
void llegadaCliente(NodoCliente*& colafte, NodoCliente*& colafin, NodoPelicula* listaPeliculas, Sala salas[], int cantSalas);
void verFila(NodoCliente*& colafte, NodoCliente*& colafin);

// Módulo D y C.8 - Preparación de la jornada
int obtenerProximoNroVenta();
void inicializarEstadisticasPeliculas(NodoPelicula* lista, ResumenPelicula stats[], int& cantStats);
// Atención al cliente y ventas
void atenderProximoCliente(NodoCliente*& colafte, NodoCliente*& colafin, Sala salas[], int cantSalas, NodoPelicula* listaPeliculas, NodoVenta*& pilaVentas, int& proximoNroVenta, int entradasPorSala[], ResumenPelicula stats[], int cantStats);

// Operaciones de Pila - Deshacer e Historial (C.4 y C.5)
void deshacerUltimaVenta(NodoVenta*& pilaVentas, Sala salas[], int cantSalas, int& proximoNroVenta, int entradasPorSala[], ResumenPelicula stats[], int cantStats);
void mostrarHistorialJornada(NodoVenta*& pilaVentas);

// Persistencia de Ventas (C.6 y C.7)
void guardarVentasYVaciarPila(NodoVenta*& pilaVentas);
void mostrarHistorialCompleto();

// Módulo D - Estadísticas y Reportes
void mostrarEstadisticasDelDia(ResumenPelicula stats[], int cantStats, int entradasPorSala[], int cantSalas, NodoVenta* pilaVentas);
void generarReporteRanking(ResumenPelicula stats[], int cantStats);
void consultarReporteGuardado();

#endif