#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include "funciones.hpp"
using namespace std;

// struct Venta {
//     int   nroVenta;
//     char  cliente[40];
//     int   codPelicula;
//     int   nroSala;
//     int   cantidad;
//     int   fila[10];          // asientos asignados
//     int   columna[10];
//     float importe;
// };

// struct ResumenPelicula {
//     int   codigo;
//     char  titulo[50];
//     int   entradasVendidas;
//     float recaudacion;
// };


//==================================================
// Módulo A - Cartelera
//==================================================

//Punto A.1: carga inicial. Abre peliculas.dat, lee los registros y los inserta
//en una lista simplemente enlazada ordenada por codigo
NodoPelicula* cargarPeliculas(){
	NodoPelicula* lista = NULL;
	FILE* arch = fopen("peliculas.dat","rb");

	if(arch == NULL){
		return lista; // todavia no existe el archivo, arranca vacia
	}

	Pelicula p;
	fread(&p,sizeof(Pelicula),1,arch);
	while(!feof(arch)){
		insertarPeliculaOrdenada(lista,p);
		fread(&p,sizeof(Pelicula),1,arch);
	}

	fclose(arch);
	return lista;
}

//Inserta un nodo en la lista, manteniendo el orden por codigo
void insertarPeliculaOrdenada(NodoPelicula*& lista, Pelicula p){
	NodoPelicula* nuevo = new NodoPelicula();
	nuevo->info = p;
	nuevo->sgte = NULL;

	NodoPelicula* aux = lista;
	NodoPelicula* ant = NULL;

	while(aux != NULL && aux->info.codigo < p.codigo){
		ant = aux;
		aux = aux->sgte;
	}

	if(ant != NULL){
		ant->sgte = nuevo;
	} else {
		lista = nuevo;
	}

	nuevo->sgte = aux;
}

//Punto A.2: recorre la lista y muestra codigo, titulo, duracion, clasificacion y precio
void listarCartelera(NodoPelicula* lista){
	if(lista == NULL){
		cout << "No hay peliculas cargadas" << endl;
		return;
	}

	NodoPelicula* aux = lista;

	cout << "--- Cartelera ---" << endl;
	while(aux != NULL){
		cout << aux->info.codigo << " - " << aux->info.titulo
		    << " - " << aux->info.duracion << " min - " << aux->info.clasificacion
		    << " - $" << aux->info.precio << endl;
		aux = aux->sgte;
	}
}

//Busca una pelicula por codigo y devuelve el nodo (NULL si no esta)
NodoPelicula* buscarPelicula(NodoPelicula* lista, int codigo){
	NodoPelicula* aux = lista;

	while(aux != NULL && aux->info.codigo != codigo){
		aux = aux->sgte;
	}

	return aux;
}

bool existeCodigoPelicula(NodoPelicula* lista, int codigo){
	return buscarPelicula(lista,codigo) != NULL;
}

bool clasificacionValida(char clasificacion[]){
	return strcmp(clasificacion,"ATP") == 0 || strcmp(clasificacion,"+13") == 0 || strcmp(clasificacion,"+16") == 0;
}

//Punto A.3: alta de pelicula, con las validaciones del enunciado
void altaPelicula(NodoPelicula*& lista){
	Pelicula p;

	cout << "Codigo: ";
	cin >> p.codigo;

	if(existeCodigoPelicula(lista,p.codigo)){
		cout << "Ya existe una pelicula con ese codigo" << endl;
		return;
	}

	cout << "Titulo: ";
	leerTexto(p.titulo,50);

	cout << "Duracion (minutos): ";
	cin >> p.duracion;

	cout << "Clasificacion (ATP, +13 o +16): ";
	leerTexto(p.clasificacion,5);

	cout << "Precio: ";
	cin >> p.precio;

	if(!clasificacionValida(p.clasificacion)){
		cout << "La clasificacion tiene que ser ATP, +13 o +16" << endl;
		return;
	}

	if(p.duracion <= 0 || p.precio <= 0){
		cout << "La duracion y el precio tienen que ser mayores que cero" << endl;
		return;
	}

	insertarPeliculaOrdenada(lista,p);
	cout << "Pelicula agregada correctamente" << endl;
}

//Verifica si una pelicula esta asignada a alguna sala (recorre el vector de salas)
bool peliculaAsignadaASala(Sala salas[], int cantSalas, int codigo){
	for(int i=0; i < cantSalas; i++){
		if(salas[i].codPelicula == codigo){
			return true;
		}
	}
	return false;
}

//Punto A.4: baja de pelicula. No se puede si esta asignada a alguna sala
void bajaPelicula(NodoPelicula*& lista, Sala salas[], int cantSalas){
	int codigo;

	cout << "Codigo de la pelicula a dar de baja: ";
	cin >> codigo;

	if(!existeCodigoPelicula(lista,codigo)){
		cout << "No existe una pelicula con ese codigo" << endl;
		return;
	}

	if(peliculaAsignadaASala(salas,cantSalas,codigo)){
		cout << "No se puede dar de baja: la pelicula esta asignada a una sala" << endl;
		return;
	}

	NodoPelicula* aux = lista;
	NodoPelicula* ant = NULL;

	while(aux != NULL && aux->info.codigo != codigo){
		ant = aux;
		aux = aux->sgte;
	}

	if(ant != NULL){
		ant->sgte = aux->sgte;
	} else {
		lista = aux->sgte;
	}

	delete aux;
	cout << "Pelicula eliminada correctamente" << endl;
}

//Punto A.5: busqueda por clasificacion
void buscarPorClasificacion(NodoPelicula* lista){
	char clasificacion[5];

	cout << "Clasificacion a buscar (ATP, +13 o +16): ";
	leerTexto(clasificacion,5);

	NodoPelicula* aux = lista;
	bool encontroAlguna = false;

	while(aux != NULL){
		if(strcmp(aux->info.clasificacion,clasificacion) == 0){
			cout << aux->info.codigo << " - " << aux->info.titulo << " - $" << aux->info.precio << endl;
			encontroAlguna = true;
		}
		aux = aux->sgte;
	}

	if(!encontroAlguna){
		cout << "No hay peliculas con esa clasificacion" << endl;
	}
}

//Punto A.6: guardado al salir. Recorre la lista, graba y libera toda la memoria
void guardarYLiberarPeliculas(NodoPelicula*& lista){
	FILE* arch = fopen("peliculas.dat","wb");

	NodoPelicula* aux = lista;
	while(aux != NULL){
		fwrite(&aux->info,sizeof(Pelicula),1,arch);
		aux = aux->sgte;
	}

	fclose(arch);

	// libero la memoria de la lista
	while(lista != NULL){
		aux = lista;
		lista = lista->sgte;
		delete aux;
	}
}



//==================================================
// Cola de boleteria + C.1 y C.2
//==================================================
//Operaciones basicas de la cola de clientes (boleteria)
void encolarCliente(NodoCliente*& colafte, NodoCliente*& colafin, Cliente c){
	NodoCliente* nuevo = new NodoCliente();
	nuevo->info = c;
	nuevo->sgte = NULL;

	if(colafte == NULL){
		colafte = nuevo;
	} else {
		colafin->sgte = nuevo;
	}
	colafin = nuevo;
}

Cliente desencolarCliente(NodoCliente*& colafte, NodoCliente*& colafin){
	Cliente ret = colafte->info;
	NodoCliente* aux = colafte;
	colafte = colafte->sgte;
	if(colafte == NULL){
		colafin = NULL;
	}
	delete aux;
	return ret;
}

bool colaClientesVacia(NodoCliente* colafte){
	return colafte == NULL;
}

//Punto C.1: llegada de un cliente. Valida la pelicula y lo encola
void llegadaCliente(NodoCliente*& colafte, NodoCliente*& colafin, NodoPelicula* listaPeliculas, Sala salas[], int cantSalas){
	Cliente c;

	cout << "Nombre del cliente: ";
	leerTexto(c.nombre,40);

	cout << "Codigo de la pelicula: ";
	cin >> c.codPelicula;

	if(!existeCodigoPelicula(listaPeliculas,c.codPelicula)){
		cout << "No existe una pelicula con ese codigo" << endl;
		return;
	}

	if(!peliculaAsignadaASala(salas,cantSalas,c.codPelicula)){
		cout << "Esa pelicula no esta asignada a ninguna sala en este momento" << endl;
		return;
	}

	cout << "Cantidad de entradas (1 a 10): ";
	cin >> c.cantidad;

	if(c.cantidad < 1 || c.cantidad > 10){
		cout << "La cantidad de entradas tiene que estar entre 1 y 10" << endl;
		return;
	}

	encolarCliente(colafte,colafin,c);
	cout << "Cliente encolado correctamente" << endl;
}

//Punto C.2: muestra cuantos esperan y los datos del proximo, sin desencolar
void verFila(NodoCliente*& colafte, NodoCliente*& colafin){
	if(colaClientesVacia(colafte)){
		cout << "No hay clientes esperando" << endl;
		return;
	}

	// para contar sin perder el contenido de la cola, uso una cola auxiliar
	NodoCliente* colafteAux = NULL;
	NodoCliente* colafinAux = NULL;
	int cantidad = 0;

	while(!colaClientesVacia(colafte)){
		encolarCliente(colafteAux,colafinAux,desencolarCliente(colafte,colafin));
		cantidad++;
	}

	while(!colaClientesVacia(colafteAux)){
		encolarCliente(colafte,colafin,desencolarCliente(colafteAux,colafinAux));
	}

	cout << "Personas esperando: " << cantidad << endl;
	cout << "Proximo a atender: " << colafte->info.nombre << " - pelicula " << colafte->info.codPelicula
	    << " - " << colafte->info.cantidad << " entradas" << endl;
}


void leerTexto(char destino[], int maxLen) {
    cin.getline(destino, maxLen);
    if (destino[0] == '\0') {
        cin.getline(destino, maxLen);
    }
}

