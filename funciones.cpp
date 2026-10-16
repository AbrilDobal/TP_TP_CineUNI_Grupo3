#include <iostream>
#include <string>
#include <stdlib.h>
#include "funciones.hpp"
using namespace std;

//Modulo A - Cartelera
//Registros Obligatorios
const int MAX_FIL = 10;
const int MAX_COL = 12;

struct Pelicula {
    int   codigo;
    char  titulo[50];
    int   duracion;          // en minutos
    char  clasificacion[5];  // "ATP", "+13", "+16"
    float precio;
};

struct Sala {
    int nroSala;
    int filas;
    int columnas;
    int codPelicula;
    int asientos[MAX_FIL][MAX_COL];   // 0 = libre, 1 = ocupado
};

struct Venta {
    int   nroVenta;
    char  cliente[40];
    int   codPelicula;
    int   nroSala;
    int   cantidad;
    int   fila[10];          // asientos asignados
    int   columna[10];
    float importe;
};

struct ResumenPelicula {
    int   codigo;
    char  titulo[50];
    int   entradasVendidas;
    float recaudacion;
};

