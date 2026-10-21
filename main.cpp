#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include "funciones.hpp"
using namespace std;

int main(int argc, char** argv) {

	//Carga inicial de la cartelera (Modulo A)
	NodoPelicula* listaPeliculas = cargarPeliculas();

	// Carga inicial de las salas (Modulo B)
	Sala salas[MAX_SALAS];
	int cantSalas = 0;

	if(!cargarSalas(salas,cantSalas)){
		cout << "No se pudieron cargar las salas." << endl;
	}

	NodoCliente* colafte = NULL;
	NodoCliente* colafin = NULL;

	// --- Preparación para el Módulo C y D ---
	
	// Pila de operaciones para deshacer ventas (C.4)
	NodoVenta* pilaVentas = NULL;
	
	// C.8: Obtener el número correlativo de la próxima venta
	int proximoNroVenta = obtenerProximoNroVenta();
	
	// Módulo D: Vectores de la jornada
	int entradasPorSala[MAX_SALAS] = {0}; // Inicializa todas las salas en 0
	
	ResumenPelicula statsPeliculas[50];   // Vector de estadísticas (asumimos max 50 películas)
	int cantPeliculasStats = 0;
	inicializarEstadisticasPeliculas(listaPeliculas, statsPeliculas, cantPeliculasStats);
	
	// -------------------------------------------------------

	int opcion;

	do{
		cout << endl << "===== CineUNI - Cartelera y Boleteria =====" << endl;
		cout << "1. Listar cartelera" << endl;
		cout << "2. Alta de pelicula" << endl;
		cout << "3. Baja de pelicula" << endl;
		cout << "4. Buscar por clasificacion" << endl;
		cout << "5. Llegada de un cliente" << endl;
		cout << "6. Ver la fila" << endl;
		cout << "7. Atender al proximo cliente" << endl;
		cout << "8. Ver plano de una sala" << endl;
		cout << "9. Ver estadisticas de una sala" << endl;
		cout << "10. Buscar asientos contiguos" << endl;
		cout << "11. Cambiar pelicula de una sala" << endl;
		cout << "12. Deshacer ultima venta" << endl;
		cout << "13. Ver historial de la jornada" << endl;
		cout << "14. Ver historial completo historico" << endl;
		cout << "15. Ver estadisticas del dia" << endl;
		cout << "16. Generar reporte de recaudacion" << endl;
		cout << "17. Consultar reporte guardado" << endl;
		cout << "0. Salir" << endl;
		cout << "Opcion: ";
		cin >> opcion;
		cin.ignore(10000, '\n'); // Limpia el buffer para que no afecte la lectura de textos posterior		

		switch(opcion){
			case 1: listarCartelera(listaPeliculas); break;
			case 2: altaPelicula(listaPeliculas); break;
			case 3: bajaPelicula(listaPeliculas,salas,cantSalas); break;
			case 4: buscarPorClasificacion(listaPeliculas); break;
			case 5: llegadaCliente(colafte,colafin,listaPeliculas,salas,cantSalas); break;
			case 6: verFila(colafte,colafin); break;
			case 7: atenderProximoCliente(colafte, colafin, salas, cantSalas, listaPeliculas, pilaVentas, proximoNroVenta, entradasPorSala, statsPeliculas, cantPeliculasStats); break;
			case 8: mostrarPlanoSala(salas, cantSalas, listaPeliculas); break;
			case 9: mostrarEstadisticasSala(salas,cantSalas); break;
			case 10: consultarAsientosContiguos(salas,cantSalas); break;
			case 11: cambiarPeliculaSala(salas,cantSalas,listaPeliculas); break;
			case 12: deshacerUltimaVenta(pilaVentas, salas, cantSalas, proximoNroVenta, entradasPorSala, statsPeliculas, cantPeliculasStats); break;
			case 13: mostrarHistorialJornada(pilaVentas); break;
			case 14: mostrarHistorialCompleto(); break;
			case 15: mostrarEstadisticasDelDia(statsPeliculas, cantPeliculasStats, entradasPorSala, cantSalas, pilaVentas); break;
			case 16: generarReporteRanking(statsPeliculas, cantPeliculasStats); break;
			case 17: consultarReporteGuardado(); break;
			case 0: 
			    cout << "Cerrando el sistema..." << endl; 
			    guardarVentasYVaciarPila(pilaVentas);
			    break;
			default: cout << "Opcion invalida" << endl;
		}
	} while(opcion != 0);

	//Guardado al salir: reescribe peliculas.dat y libera la lista
	guardarYLiberarPeliculas(listaPeliculas);

	return 0;
}