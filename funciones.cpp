#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include "funciones.hpp"
using namespace std;


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
bool cargarSalas(Sala salas[], int& cantSalas) {
    FILE* arch = fopen("salas.dat","rb");

    cantSalas = 0;

    if(arch == NULL){
        cout << "No se pudo abrir salas.dat" << endl;
        return false;
    }

    while(cantSalas < MAX_SALAS && fread(&salas[cantSalas],sizeof(Sala),1,arch) == 1){
        cantSalas++;
    }

    fclose(arch);

    return true;
}

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

    if(filas <= 0 || filas > MAX_FIL || columnas <= 0 || columnas > MAX_COL){
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

    if(filas <= 0 || filas > MAX_FIL || columnas <= 0 || columnas > MAX_COL || cantidad <= 0 || cantidad > columnas){
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


bool buscarAsientosSeparados(Sala salas[], int cantSalas, int posSala, int cantidad, int filasAsientos[], int columnasAsientos[]) {

    if(posSala < 0 || posSala >= cantSalas){
        return false;
    }

    int filas = salas[posSala].filas;
    int columnas = salas[posSala].columnas;

    if(filas <= 0 || filas > MAX_FIL || columnas <= 0 || columnas > MAX_COL || cantidad < 1 || cantidad > 10){
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

        if(filasAsientos[k] < 0 || filasAsientos[k] >= filas || columnasAsientos[k] < 0 || columnasAsientos[k] >= columnas){
            return false;
        }

        if(salas[posSala].asientos[filasAsientos[k]][columnasAsientos[k]] != 0){
            cout << "Uno de los asientos seleccionados ya no esta libre" << endl;
            return false;
        }

        // Evita recibir dos veces la misma ubicacion
        for(int m = 0; m < k; m++){
            if(filasAsientos[k] == filasAsientos[m] && columnasAsientos[k] == columnasAsientos[m]){
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



//==================================================
// TDA Pila - Operaciones de Ventas 
//==================================================

bool pilaVacia(NodoVenta* pila) {
    return pila == NULL; // Retorna true si la pila está vacía
}

void apilar(NodoVenta*& pila, Venta v) {
    NodoVenta* nuevo = new NodoVenta();
    nuevo->info = v;
    nuevo->sgte = pila; // El nuevo nodo apunta al que antes era el tope
    pila = nuevo;       // El tope de la pila ahora es el nuevo nodo
}

Venta desapilar(NodoVenta*& pila) {
    Venta ret = pila->info;     // Guardamos la información a devolver
    NodoVenta* aux = pila;      // Apuntamos al nodo a eliminar
    pila = pila->sgte;          // El tope pasa a ser el siguiente nodo
    delete aux;                 // Liberamos la memoria
    return ret;                 // Retornamos la venta
}

//==================================================
// Módulo D y C.8 - Preparación de la jornada
//==================================================

// Punto C.8: Numeración correlativa con fseek inverso
int obtenerProximoNroVenta() {
    FILE* arch = fopen("ventas.dat", "rb");
    
    if (arch == NULL) {
        return 1; // Si el archivo no existe, es la primera venta del sistema
    }
    
    Venta ultimaVenta;
    // Nos posicionamos al final menos el tamaño de un registro Venta
    fseek(arch, -static_cast<long>(sizeof(Venta)), SEEK_END);
    
    int leidos = fread(&ultimaVenta, sizeof(Venta), 1, arch);
    fclose(arch);
    
    if (leidos == 1) {
        return ultimaVenta.nroVenta + 1; // Continuamos desde el último
    } else {
        return 1; // Por si el archivo existe pero está vacío
    }
}

// Módulo D: Prepara el vector de películas leyendo la cartelera actual
void inicializarEstadisticasPeliculas(NodoPelicula* lista, ResumenPelicula stats[], int& cantStats) {
    cantStats = 0;
    NodoPelicula* aux = lista;
    
    while (aux != NULL) {
        stats[cantStats].codigo = aux->info.codigo;
        strcpy(stats[cantStats].titulo, aux->info.titulo);
        stats[cantStats].entradasVendidas = 0;
        stats[cantStats].recaudacion = 0.0;
        
        cantStats++;
        aux = aux->sgte;
    }
}

//==================================================
// Atención de Boletería (C.3)
//==================================================
//Analizar para modularizar
void atenderProximoCliente(NodoCliente*& colafte, NodoCliente*& colafin, Sala salas[], int cantSalas, NodoPelicula* listaPeliculas, NodoVenta*& pilaVentas, int& proximoNroVenta, int entradasPorSala[], ResumenPelicula stats[], int cantStats) {
    
    if (colaClientesVacia(colafte)) {
        cout << "No hay clientes esperando en la fila." << endl;
        return;
    }

    // 1. Desencolamos al cliente
    Cliente c = desencolarCliente(colafte, colafin);
    cout << "\nAtendiendo a: " << c.nombre << " (Pelicula " << c.codPelicula << ", " << c.cantidad << " entradas)" << endl;

    // 2. Buscamos la sala donde se proyecta su película
    int posSala = -1;
    for (int i = 0; i < cantSalas; i++) {
        if (salas[i].codPelicula == c.codPelicula) {
            posSala = i;
            break;
        }
    }

    if (posSala == -1) {
        cout << "Error: La pelicula ya no esta asignada a ninguna sala." << endl;
        return;
    }

    // 3. Buscamos asientos contiguos (B.4 / C.3.3)
    int filasAsientos[10];
    int columnasAsientos[10];
    bool asientosEncontrados = buscarAsientosContiguos(salas, cantSalas, posSala, c.cantidad, filasAsientos, columnasAsientos);

    // Si no hay contiguos, ofrecemos separados
    if (!asientosEncontrados) {
        cout << "No hay suficientes asientos contiguos." << endl;
        asientosEncontrados = buscarAsientosSeparados(salas, cantSalas, posSala, c.cantidad, filasAsientos, columnasAsientos);
        
        if (asientosEncontrados) {
            char respuesta;
            cout << "Se encontraron " << c.cantidad << " asientos separados. Acepta? (S/N): ";
            cin >> respuesta;
            if (respuesta != 'S' && respuesta != 's') {
                cout << "Venta cancelada por el cliente." << endl;
                return; // Venta cancelada
            }
        } else {
            cout << "No hay lugar suficiente en la sala. Venta cancelada." << endl;
            return; // No hay lugar ni siquiera separado
        }
    }

    // 4. Marcamos los asientos y guardamos en salas.dat (C.3.4 y B.5)
    if (!marcarAsientosOcupados(salas, cantSalas, posSala, c.cantidad, filasAsientos, columnasAsientos)) {
        cout << "Hubo un error al marcar los asientos. Venta cancelada." << endl;
        return;
    }

    // 5. Armamos el registro Venta y calculamos el importe
    NodoPelicula* peli = buscarPelicula(listaPeliculas, c.codPelicula);
    float importeTotal = c.cantidad * peli->info.precio;

    Venta nuevaVenta;
    nuevaVenta.nroVenta = proximoNroVenta;
    strcpy(nuevaVenta.cliente, c.nombre);
    nuevaVenta.codPelicula = c.codPelicula;
    nuevaVenta.nroSala = salas[posSala].nroSala;
    nuevaVenta.cantidad = c.cantidad;
    nuevaVenta.importe = importeTotal;
    
    for (int k = 0; k < c.cantidad; k++) {
        nuevaVenta.fila[k] = filasAsientos[k];
        nuevaVenta.columna[k] = columnasAsientos[k];
    }

    // 6. Apilamos la venta (Pila dinámica)
    apilar(pilaVentas, nuevaVenta);

    // 7. Actualizamos los vectores de la jornada (Módulo D)
    entradasPorSala[salas[posSala].nroSala - 1] += c.cantidad;
    
    for (int i = 0; i < cantStats; i++) {
        if (stats[i].codigo == c.codPelicula) {
            stats[i].entradasVendidas += c.cantidad;
            stats[i].recaudacion += importeTotal;
            break;
        }
    }

    // 8. Informamos éxito, incrementamos NroVenta y mostramos ubicaciones
    proximoNroVenta++;
    cout << "\nVenta Exitosa - Ticket Nro: " << nuevaVenta.nroVenta << endl;
    cout << "Importe Total: $" << importeTotal << endl;
    cout << "Asientos asignados en Sala " << nuevaVenta.nroSala << ":" << endl;
    for (int k = 0; k < c.cantidad; k++) {
        cout << "- Fila " << char('A' + nuevaVenta.fila[k]) << ", Asiento " << nuevaVenta.columna[k] + 1 << endl;
    }

}

//==================================================
// Deshacer Venta e Historial (C.4 y C.5)
//==================================================

void deshacerUltimaVenta(NodoVenta*& pilaVentas, Sala salas[], int cantSalas, int& proximoNroVenta, int entradasPorSala[], ResumenPelicula stats[], int cantStats) {
    if (pilaVacia(pilaVentas)) {
        cout << "Error: No hay ventas registradas en esta jornada para deshacer." << endl;
        return;
    }

    // 1. Desapilamos la última venta
    Venta v = desapilar(pilaVentas);

    // 2. Liberamos los asientos en la matriz
    int posSala = buscarSala(salas, cantSalas, v.nroSala);
    if (posSala != -1) {
        for (int k = 0; k < v.cantidad; k++) {
            salas[posSala].asientos[v.fila[k]][v.columna[k]] = 0; // 0 = libre
        }
        
        // 3. Actualizamos salas.dat por acceso directo
        if (actualizarSalaArchivo(salas, cantSalas, posSala)) {
            cout << "Asientos liberados en la sala y archivo actualizado." << endl;
        } else {
            cout << "Error al actualizar el archivo de la sala." << endl;
        }
    }

    // 4. Descontamos las entradas y la recaudación de los vectores del Módulo D
    entradasPorSala[v.nroSala - 1] -= v.cantidad;

    for (int i = 0; i < cantStats; i++) {
        if (stats[i].codigo == v.codPelicula) {
            stats[i].entradasVendidas -= v.cantidad;
            stats[i].recaudacion -= v.importe;
            break;
        }
    }

    // 5. Retrocedemos el contador correlativo para re-usar el número de ticket
    proximoNroVenta--;

    cout << "Venta Nro " << v.nroVenta << " a nombre de " << v.cliente << " fue DESHECHA exitosamente." << endl;
}

void mostrarHistorialJornada(NodoVenta*& pilaVentas) {
    if (pilaVacia(pilaVentas)) {
        cout << "La jornada actual no tiene ventas registradas todavia." << endl;
        return;
    }

    // Pila auxiliar para no perder los datos
    NodoVenta* pilaAux = NULL;

    cout << endl << "===== Historial de la Jornada (Mas reciente a mas antigua) =====" << endl;
    
    // 1. Desapilar de la original, mostrar y apilar en la auxiliar
    while (!pilaVacia(pilaVentas)) {
        Venta v = desapilar(pilaVentas);
        
        cout << "Ticket: " << v.nroVenta 
             << " | Cliente: " << v.cliente 
             << " | Pelicula: " << v.codPelicula 
             << " | Entradas: " << v.cantidad 
             << " | Importe: $" << v.importe << endl;
             
        apilar(pilaAux, v);
    }

    // 2. Devolver todo a la pila original para restaurar su estado
    while (!pilaVacia(pilaAux)) {
        apilar(pilaVentas, desapilar(pilaAux));
    }
    cout << "================================================================" << endl;
}


//==================================================
// Persistencia de Ventas (C.6 y C.7)
//==================================================

// C.6: Cierre de caja al salir (volcar pila a ventas.dat en orden cronológico)
void guardarVentasYVaciarPila(NodoVenta*& pilaVentas) {
    if (pilaVacia(pilaVentas)) {
        cout << "No hay ventas nuevas en esta sesion para guardar." << endl;
        return; 
    }

    // Pila auxiliar para invertir el orden (de LIFO a FIFO / cronológico)
    NodoVenta* pilaAux = NULL;

    // Pasamos todo a la auxiliar
    while (!pilaVacia(pilaVentas)) {
        apilar(pilaAux, desapilar(pilaVentas));
    }

    // Abrimos el archivo en modo append binario ("ab")
    FILE* arch = fopen("ventas.dat", "ab");
    if (arch == NULL) {
        cout << "Error al abrir ventas.dat para guardar." << endl;
        return;
    }

    // Desapilamos de la auxiliar (ahora salen desde la más antigua a la más nueva) y grabamos
    int guardadas = 0;
    while (!pilaVacia(pilaAux)) {
        Venta v = desapilar(pilaAux);
        fwrite(&v, sizeof(Venta), 1, arch);
        guardadas++;
    }

    fclose(arch);
    cout << "Cierre de caja: Se guardaron " << guardadas << " ventas en ventas.dat de forma cronologica." << endl;
}

// C.7: Historial completo acumulado
void mostrarHistorialCompleto() {
    FILE* arch = fopen("ventas.dat", "rb");
    if (arch == NULL) {
        cout << "No hay un historial de ventas anterior (el archivo aun no existe)." << endl;
        return;
    }

    Venta v;
    int totalVentas = 0;
    float montoAcumulado = 0.0;

    cout << endl << "===== Historial Completo de Ventas (Historico) =====" << endl;
    
    // Leemos secuencialmente (rápido y simple)
    fread(&v, sizeof(Venta), 1, arch);
    while (!feof(arch)) {
        cout << "Ticket: " << v.nroVenta 
             << " | Cliente: " << v.cliente 
             << " | Sala: " << v.nroSala
             << " | Pelicula: " << v.codPelicula 
             << " | Entradas: " << v.cantidad 
             << " | Importe: $" << v.importe << endl;
        
        totalVentas++;
        montoAcumulado += v.importe;
        
        fread(&v, sizeof(Venta), 1, arch);
    }

    fclose(arch);

    cout << "----------------------------------------------------" << endl;
    cout << "Cantidad total de ventas historicas: " << totalVentas << endl;
    cout << "Monto total acumulado: $" << montoAcumulado << endl;
    cout << "====================================================" << endl;
}

//==================================================
// Módulo D - Estadísticas y Reportes
//==================================================

void mostrarEstadisticasDelDia(ResumenPelicula stats[], int cantStats, int entradasPorSala[], int cantSalas, NodoVenta* pilaVentas) {
    if (cantStats == 0) {
        cout << "No hay peliculas en cartelera." << endl;
        return;
    }

    int maxEntradas = -1, minEntradas = 999999;
    int posMaxPeli = 0, posMinPeli = 0;
    float recaudacionTotal = 0.0;
    int totalEntradas = 0;

    // Calculamos máximos, mínimos y totales de las películas
    for (int i = 0; i < cantStats; i++) {
        recaudacionTotal += stats[i].recaudacion;
        totalEntradas += stats[i].entradasVendidas;

        if (stats[i].entradasVendidas > maxEntradas) {
            maxEntradas = stats[i].entradasVendidas;
            posMaxPeli = i;
        }
        if (stats[i].entradasVendidas < minEntradas) {
            minEntradas = stats[i].entradasVendidas;
            posMinPeli = i;
        }
    }

    // Calculamos la sala con más ventas
    int maxSala = -1;
    int posMaxSala = 0;
    for (int i = 0; i < cantSalas; i++) {
        if (entradasPorSala[i] > maxSala) {
            maxSala = entradasPorSala[i];
            posMaxSala = i;
        }
    }

    // Contamos la cantidad de ventas del día recorriendo la pila con una auxiliar
    int cantVentas = 0;
    NodoVenta* pilaAux = NULL;
    while (!pilaVacia(pilaVentas)) {
        apilar(pilaAux, desapilar(pilaVentas));
        cantVentas++;
    }
    while (!pilaVacia(pilaAux)) {
        apilar(pilaVentas, desapilar(pilaAux));
    }

    cout << endl << "===== Estadisticas del Dia =====" << endl;
    cout << "Pelicula mas vendida: " << stats[posMaxPeli].titulo << " (" << maxEntradas << " entradas)" << endl;
    cout << "Pelicula menos vendida: " << stats[posMinPeli].titulo << " (" << minEntradas << " entradas)" << endl;
    cout << "Sala con mas entradas vendidas: Sala " << posMaxSala + 1 << " (" << maxSala << " entradas)" << endl;
    cout << "Recaudacion total: $" << recaudacionTotal << endl;
    
    if (cantVentas > 0) {
        cout << "Promedio de entradas por venta: " << (float)totalEntradas / cantVentas << endl;
    } else {
        cout << "Promedio de entradas por venta: 0" << endl;
    }
    cout << "================================" << endl;
}

//se podrìa modularizar
void generarReporteRanking(ResumenPelicula stats[], int cantStats) {
    if (cantStats == 0) {
        cout << "No hay datos para generar el reporte." << endl;
        return;
    }

    // 1. Crear una copia para ordenar y no romper los índices originales
    ResumenPelicula ranking[50];
    for (int i = 0; i < cantStats; i++) {
        ranking[i] = stats[i];
    }

    // 2. Ordenar de mayor a menor por recaudación (Método Burbuja)
    for (int i = 0; i < cantStats - 1; i++) {
        for (int j = 0; j < cantStats - i - 1; j++) {
            if (ranking[j].recaudacion < ranking[j + 1].recaudacion) {
                ResumenPelicula aux = ranking[j];
                ranking[j] = ranking[j + 1];
                ranking[j + 1] = aux;
            }
        }
    }

    // 3. Mostrar en pantalla el Ranking
    cout << endl << "===== Ranking de Recaudacion =====" << endl;
    for (int i = 0; i < cantStats; i++) {
        cout << i + 1 << ". " << ranking[i].titulo << " - $" << ranking[i].recaudacion << " (" << ranking[i].entradasVendidas << " ent.)" << endl;
    }

    // 4. Pedir fecha y generar archivo secuencial
    char fecha[9];
    cout << "\nIngrese la fecha para el reporte (AAAAMMDD): ";
    cin >> fecha;

    char nombreArchivo[40];
    sprintf(nombreArchivo, "reporte_%s.dat", fecha);

    FILE* arch = fopen(nombreArchivo, "wb");
    if (arch == NULL) {
        cout << "Error al crear el archivo de reporte." << endl;
        return;
    }

    fwrite(ranking, sizeof(ResumenPelicula), cantStats, arch);
    fclose(arch);

    cout << "Reporte generado exitosamente: " << nombreArchivo << endl;
}

void consultarReporteGuardado() {
    char fecha[9];
    cout << "Ingrese la fecha del reporte a consultar (AAAAMMDD): ";
    cin >> fecha;

    char nombreArchivo[40];
    sprintf(nombreArchivo, "reporte_%s.dat", fecha);

    FILE* arch = fopen(nombreArchivo, "rb");
    if (arch == NULL) {
        cout << "No existe un reporte para la fecha " << fecha << "." << endl;
        return;
    }

    cout << endl << "===== Reporte del " << fecha << " =====" << endl;
    cout << "Codigo | Titulo | Entradas | Recaudacion" << endl;
    cout << "------------------------------------------------" << endl;

    ResumenPelicula r;
    float totalRecaudado = 0.0;

    fread(&r, sizeof(ResumenPelicula), 1, arch);
    while (!feof(arch)) {
        cout << r.codigo << " | " << r.titulo << " | " << r.entradasVendidas << " | $" << r.recaudacion << endl;
        totalRecaudado += r.recaudacion;
        fread(&r, sizeof(ResumenPelicula), 1, arch);
    }

    fclose(arch);
    cout << "------------------------------------------------" << endl;
    cout << "Total Recaudado en el reporte: $" << totalRecaudado << endl;
    cout << "================================================" << endl;
}