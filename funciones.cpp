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


//==================================================
// Módulo B - Salas
//==================================================

// Punto B.1: Carga inicial de las salas
bool cargarSalas(Sala salas[], int& cantSalas) {
    FILE* arch = fopen("salas.dat","rb");

    cantSalas = 0;

    if(arch == NULL){
        cout << "No se pudo abrir salas.dat" << endl;
        return false;
    }

    while(cantSalas < MAX_SALAS &&
          fread(&salas[cantSalas],sizeof(Sala),1,arch) == 1){
        cantSalas++;
    }

    fclose(arch);

    return true;
}

// Busca una sala por numero y devuelve la posicion en el vector
int buscarSala(Sala salas[], int cantSalas, int nroSala) {
    int i = 0;

    while(i < cantSalas && salas[i].nroSala != nroSala){
        i++;
    }

    if(i < cantSalas){
        return i;
    } else {
        return -1;
    }
}

// Punto B.2: Muestra el plano grafico de una sala
void mostrarPlanoSala(Sala salas[], int cantSalas, NodoPelicula* listaPeliculas) {
    int nroSala;

    cout << "Ingrese el numero de sala: ";
    cin >> nroSala;

    int pos = buscarSala(salas, cantSalas, nroSala);

    if(pos == -1){
        cout << "No existe una sala con ese numero" << endl;
        return;
    }

    cout << endl;
    cout << "========== PLANO DE LA SALA " << nroSala << " ==========" << endl;

    NodoPelicula* pelicula = buscarPelicula(listaPeliculas, salas[pos].codPelicula);

    if(pelicula != NULL){
        cout << "Pelicula: " << pelicula->info.titulo << endl;
    } else {
        cout << "No se encontro la pelicula asignada a esta sala" << endl;
    }

    cout << endl;

    cout << "   ";
    for(int j = 0; j < salas[pos].columnas; j++){
        cout << "\t" << j + 1;
    }
    cout << endl;

    for(int i = 0; i < salas[pos].filas; i++){
        cout << char('A' + i) << "  ";

        for(int j = 0; j < salas[pos].columnas; j++){
            if(salas[pos].asientos[i][j] == 0){
                cout << "\t[ ]";
            } else {
                cout << "\t[X]";
            }
        }

        cout << endl;
    }
}

// Punto B.3: Muestra las estadisticas de ocupacion de una sala
void mostrarEstadisticasSala(Sala salas[], int cantSalas) {
    int nroSala;

    cout << "Ingrese el numero de sala: ";
    cin >> nroSala;

    int pos = buscarSala(salas,cantSalas,nroSala);

    if(pos == -1){
        cout << "No existe una sala con ese numero" << endl;
        return;
    }

    int filas = salas[pos].filas;
    int columnas = salas[pos].columnas;

    if(filas <= 0 || filas > MAX_FIL ||
       columnas <= 0 || columnas > MAX_COL){
        cout << "Las dimensiones de la sala no son validas" << endl;
        return;
    }

    int libres = 0;
    int ocupados = 0;

    // Cuenta los asientos libres y ocupados
    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            if(salas[pos].asientos[i][j] == 0){
                libres++;
            } else if(salas[pos].asientos[i][j] == 1){
                ocupados++;
            }
        }
    }

    // Calcula el porcentaje de ocupacion
    int total = filas * columnas;
    float porcentaje = (float)ocupados / total * 100;

    // Busca la fila con mas asientos libres
    int filaMax = 0;
    int maxLibresFila = -1;

    for(int i = 0; i < filas; i++){
        int libresFila = 0;

        for(int j = 0; j < columnas; j++){
            if(salas[pos].asientos[i][j] == 0){
                libresFila++;
            }
        }

        if(libresFila > maxLibresFila){
            maxLibresFila = libresFila;
            filaMax = i;
        }
    }

    // Busca la columna con mas asientos ocupados
    int columnaMax = 0;
    int maxOcupadosCol = -1;

    for(int j = 0; j < columnas; j++){
        int ocupadosCol = 0;

        for(int i = 0; i < filas; i++){
            if(salas[pos].asientos[i][j] == 1){
                ocupadosCol++;
            }
        }

        if(ocupadosCol > maxOcupadosCol){
            maxOcupadosCol = ocupadosCol;
            columnaMax = j;
        }
    }

    // Muestra los resultados
    cout << endl;
    cout << "===== Estadisticas de la sala " << nroSala << " =====" << endl;
    cout << "Asientos libres: " << libres << endl;
    cout << "Asientos ocupados: " << ocupados << endl;
    cout << "Porcentaje de ocupacion: " << porcentaje << "%" << endl;

    cout << "Fila con mas asientos libres: "
         << char('A' + filaMax) << " (" << maxLibresFila << " libres)" << endl;

    cout << "Columna con mas asientos ocupados: "
         << columnaMax + 1 << " (" << maxOcupadosCol << " ocupados)" << endl;
}

bool buscarAsientosContiguos(Sala salas[], int cantSalas, int posSala, int cantidad, int filasAsientos[], int columnasAsientos[]) {

    if(posSala < 0 || posSala >= cantSalas){
        return false;
    }

    int filas = salas[posSala].filas;
    int columnas = salas[posSala].columnas;

    if(filas <= 0 || filas > MAX_FIL ||
       columnas <= 0 || columnas > MAX_COL ||
       cantidad <= 0 || cantidad > columnas){
        return false;
    }

    for(int i = 0; i < filas; i++){
        int consecutivos = 0;

        for(int j = 0; j < columnas; j++){

            if(salas[posSala].asientos[i][j] == 0){
                consecutivos++;

                if(consecutivos == cantidad){
                    int primeraColumna = j - cantidad + 1;

                    for(int k = 0; k < cantidad; k++){
                        filasAsientos[k] = i;
                        columnasAsientos[k] = primeraColumna + k;
                    }

                    return true;
                }

            } else {
                consecutivos = 0;
            }
        }
    }

    return false;
}

void consultarAsientosContiguos(Sala salas[], int cantSalas) {
    int nroSala;
    int cantidad;

    cout << "Ingrese el numero de sala: ";
    cin >> nroSala;

    int posSala = buscarSala(salas,cantSalas,nroSala);

    if(posSala == -1){
        cout << "No existe una sala con ese numero" << endl;
        return;
    }

    cout << "Ingrese la cantidad de asientos que necesita: ";
    cin >> cantidad;

    if(cantidad < 1 || cantidad > 10){
        cout << "La cantidad debe estar entre 1 y 10" << endl;
        return;
    }

    int filasAsientos[10];
    int columnasAsientos[10];

    if(buscarAsientosContiguos(salas,cantSalas,posSala,cantidad,filasAsientos,columnasAsientos)){
        cout << "Se encontraron asientos contiguos." << endl;
        cout << "Fila " << char('A' + filasAsientos[0]);
        cout << ", asientos " << columnasAsientos[0] + 1;
        cout << " a " << columnasAsientos[cantidad - 1] + 1 << endl;
    } else {
        cout << "No hay suficientes asientos contiguos en esa sala." << endl;
    }
}

// Punto C.3.3 (Se relaciona con B.4): busca asientos separados en la sala, sin importar si son contiguos
bool buscarAsientosSeparados(Sala salas[], int cantSalas, int posSala, int cantidad, int filasAsientos[], int columnasAsientos[]) {

    if(posSala < 0 || posSala >= cantSalas){
        return false;
    }

    int filas = salas[posSala].filas;
    int columnas = salas[posSala].columnas;

    if(filas <= 0 || filas > MAX_FIL ||
       columnas <= 0 || columnas > MAX_COL ||
       cantidad < 1 || cantidad > 10){
        return false;
    }

    int encontrados = 0;

    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){

            if(salas[posSala].asientos[i][j] == 0){
                filasAsientos[encontrados] = i;
                columnasAsientos[encontrados] = j;

                encontrados++;

                if(encontrados == cantidad){
                    return true;
                }
            }
        }
    }

    return false;
}

// Punto B.5
bool actualizarSalaArchivo(Sala salas[], int cantSalas, int posSala) {

    if(posSala < 0 || posSala >= cantSalas){
        return false;
    }

    int nroSala = salas[posSala].nroSala;

    if(nroSala < 1 || nroSala > MAX_SALAS){
        cout << "El numero de sala no es valido" << endl;
        return false;
    }

    FILE* arch = fopen("salas.dat","r+b");

    if(arch == NULL){
        cout << "No se pudo abrir salas.dat para actualizar" << endl;
        return false;
    }

    long desplazamiento = (long)(nroSala - 1) * sizeof(Sala);

    if(fseek(arch,desplazamiento,SEEK_SET) != 0){
        cout << "No se pudo posicionar en el registro de la sala" << endl;
        fclose(arch);
        return false;
    }

    bool actualizado = (fwrite(&salas[posSala],sizeof(Sala),1,arch) == 1);

    if(!actualizado){
        cout << "No se pudo actualizar la sala en el archivo" << endl;
    }

    fclose(arch);

    return actualizado;
}

// Punto C.3.4 (Se relaciona con B.5): marca los asientos como ocupados en la sala y actualiza el archivo
bool marcarAsientosOcupados(Sala salas[], int cantSalas, int posSala, int cantidad, int filasAsientos[], int columnasAsientos[]) {

    if(posSala < 0 || posSala >= cantSalas){
        return false;
    }

    int filas = salas[posSala].filas;
    int columnas = salas[posSala].columnas;

    if(cantidad < 1 || cantidad > 10){
        return false;
    }

    // Verifica todas las ubicaciones antes de modificar la matriz
    for(int k = 0; k < cantidad; k++){

        if(filasAsientos[k] < 0 || filasAsientos[k] >= filas ||
           columnasAsientos[k] < 0 || columnasAsientos[k] >= columnas){
            return false;
        }

        if(salas[posSala].asientos[filasAsientos[k]][columnasAsientos[k]] != 0){
            cout << "Uno de los asientos seleccionados ya no esta libre" << endl;
            return false;
        }

        // Evita recibir dos veces la misma ubicacion
        for(int m = 0; m < k; m++){
            if(filasAsientos[k] == filasAsientos[m] &&
               columnasAsientos[k] == columnasAsientos[m]){
                return false;
            }
        }
    }

    // Marca los asientos como ocupados
    for(int k = 0; k < cantidad; k++){
        salas[posSala].asientos[filasAsientos[k]][columnasAsientos[k]] = 1;
    }

    // Actualiza el registro correspondiente en salas.dat
    if(!actualizarSalaArchivo(salas,cantSalas,posSala)){

        // Si falla la actualizacion, revierte los cambios en memoria
        for(int k = 0; k < cantidad; k++){
            salas[posSala].asientos[filasAsientos[k]][columnasAsientos[k]] = 0;
        }

        return false;
    }

    return true;
}

void cambiarPeliculaSala(Sala salas[], int cantSalas, NodoPelicula* listaPeliculas) {

    int nroSala;

    cout << "Ingrese el numero de sala: ";
    cin >> nroSala;

    int pos = buscarSala(salas,cantSalas,nroSala);

    if(pos == -1){
        cout << "No existe una sala con ese numero" << endl;
        return;
    }

    // Verifica que las dimensiones de la sala sean validas
    int filas = salas[pos].filas;
    int columnas = salas[pos].columnas;

    if(filas <= 0 || filas > MAX_FIL ||
       columnas <= 0 || columnas > MAX_COL){
        cout << "Las dimensiones de la sala no son validas" << endl;
        return;
    }

    // Verifica que todos los asientos esten libres
    bool salaVacia = true;

    for(int i = 0; i < filas && salaVacia; i++){
        for(int j = 0; j < columnas && salaVacia; j++){
            if(salas[pos].asientos[i][j] != 0){
                salaVacia = false;
            }
        }
    }

    if(!salaVacia){
        cout << "No se puede cambiar la pelicula: la sala tiene asientos ocupados" << endl;
        return;
    }

    // Solicita el codigo de la nueva pelicula
    int codigoNuevo;

    cout << "Ingrese el codigo de la nueva pelicula: ";
    cin >> codigoNuevo;

    NodoPelicula* pelicula = buscarPelicula(listaPeliculas,codigoNuevo);

    if(pelicula == NULL){
        cout << "No existe una pelicula con ese codigo" << endl;
        return;
    }

    // Guarda el codigo anterior por si falla la actualizacion
    int codigoAnterior = salas[pos].codPelicula;

    // Modifica la pelicula asignada en memoria
    salas[pos].codPelicula = codigoNuevo;

    // Actualiza el registro de la sala en el archivo
    if(!actualizarSalaArchivo(salas,cantSalas,pos)){
        salas[pos].codPelicula = codigoAnterior;
        cout << "No se pudo guardar el cambio en salas.dat" << endl;
        return;
    }

    cout << "Pelicula de la sala " << nroSala << " cambiada correctamente" << endl;
    cout << "Nueva pelicula: " << pelicula->info.titulo << endl;
}