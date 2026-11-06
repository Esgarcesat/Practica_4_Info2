#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <limits>
#include "Red.h"

using namespace std;

const char* ARCHIVO_POR_DEFECTO = "Red.txt";

void limpiarEntrada() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Obliga al usuario a ingresar estrictamente una sola letra (A-Z)
// No deja avanzar si ingresa numeros (ej: '3') o simbolos
char leerLetraNodoValida(const char* mensaje) {
    char buffer[50];
    while (true) {
        cout << mensaje;
        cin >> buffer;
        limpiarEntrada();

        if (buffer[0] != '\0' && buffer[1] == '\0' && isalpha(buffer[0])) {
            return toupper(buffer[0]);
        }
        cout << "Error: '" << buffer << "' no es valido. Debe ingresar una unica letra de la A a la Z.\n";
    }
}

// Obliga al usuario a ingresar un entero estrictamente mayor o igual a minVal
int leerEnteroPositivo(const char* mensaje, int minVal = 1) {
    char buffer[50];
    while (true) {
        cout << mensaje;
        cin >> buffer;
        limpiarEntrada();

        int valor = 0;
        char extra[50];
        // sscanf intenta leer un entero y verifica si quedo algo pegado (como ".5" o letras)
        int leidos = sscanf(buffer, "%d%49s", &valor, extra);

        if (leidos == 1 && valor >= minVal) {
            return valor;
        }
        cout << "Error: '" << buffer << "' no es valido. Debe ingresar un numero entero puro mayor o igual a " << minVal << ".\n";
    }
}

// --- MENU 1: SELECCION DEL METODO DE CARGA DE LA TOPOLOGIA ---
// Retorna true cuando hay una topologia funcional cargada, o false si el usuario decide salir (0)
bool menuCargaTopologia(Red& miRed) {
    int opcion = -1;

    while (true) {
        cout << "\n";
        cout << "SELECCIONE EL METODO DE CARGA DE LA TOPOLOGIA\n";
        cout << "\n";
        cout << "1. Cargar topologia desde archivo por defecto (" << ARCHIVO_POR_DEFECTO << ")\n";
        cout << "2. Crear topologia manualmente desde consola\n";
        cout << "3. Generar red aleatoria (y guardar en " << ARCHIVO_POR_DEFECTO << ")\n";
        cout << "0. Salir del simulador\n";
        cout << "\n";

        opcion = leerEnteroPositivo("Seleccione una opcion (0-3): ", 0);

        if (opcion == 0) {
            return false;
        }
        else if (opcion == 1) {
            cout << "\n Leyendo topologia desde '" << ARCHIVO_POR_DEFECTO << "\n";
            if (miRed.cargarDesdeArchivo(ARCHIVO_POR_DEFECTO)) {
                cout << "Topologia cargada correctamente desde '" << ARCHIVO_POR_DEFECTO << "\n";
                miRed.mostrarDiagramaRed();
                return true; // Pasa al Menu 2
            } else {
                cout << "Corrija el archivo '" << ARCHIVO_POR_DEFECTO << "' o elija otro metodo de creacion.\n";
            }
        }
        else if (opcion == 2) {
            miRed.limpiarRed();
            cout << "\n CREACION MANUAL DE TOPOLOGIA\n";
            int numEnlaces = leerEnteroPositivo("Cuantos enlaces directos desea ingresar? (minimo 1): ", 1);

            for (int i = 0; i < numEnlaces; ++i) {
                // Bucle que NO deja avanzar al siguiente enlace hasta que el actual sea 100% valido
                while (true) {
                    cout << "\n Configurando Enlace #" << (i + 1) << " de " << numEnlaces << "\n";
                    char orig = leerLetraNodoValida("  Enrutador Origen (A-Z): ");
                    char dest = leerLetraNodoValida("  Enrutador Destino (A-Z): ");

                    if (orig == dest) {
                        cout << "Error en Enlace #" << (i + 1) << ": El enrutador Origen [" << orig
                             << "] no puede ser igual al Destino [" << dest << "]. Intente de nuevo.\n";
                        continue; // Repite este mismo enlace sin incrementar 'i'
                    }

                    int costo = leerEnteroPositivo("Costo del enlace (> 0): ", 1);

                    if (miRed.agregarOActualizarEnlace(orig, dest, costo)) {
                        cout << "Enlace #" << (i + 1) << " creado: [" << orig << "] <--(" << costo << ")--> [" << dest << "]\n";
                        break; // Enlace exitoso, ahora si avanza al siguiente 'i'
                    } else {
                        cout << "Error inesperado al crear el enlace. Intente nuevamente.\n";
                    }
                }
            }
            miRed.mostrarDiagramaRed();
            return true; // Pasa al Menu 2
        }
        else if (opcion == 3) {
            cout << "\n GENERACION DE RED ALEATORIA \n";
            int cantNodos;
            while (true) {
                cantNodos = leerEnteroPositivo("Cantidad de enrutadores a generar (2 a 26): ", 2);
                if (cantNodos <= 26) break;
                cout << "Error: El maximo permitido es 26 enrutadores (A-Z).\n";
            }
            int costoMax = leerEnteroPositivo("  Costo maximo para los enlaces (>= 1): ", 1);

            miRed.generarRedAleatoria(cantNodos, costoMax, ARCHIVO_POR_DEFECTO);
            cout << "\n Red aleatoria generada y guardada automaticamente en '" << ARCHIVO_POR_DEFECTO << "'.\n";
            miRed.mostrarDiagramaRed();
            return true; // Pasa al Menu 2
        }
        else {
            cout << "\n Opcion fuera de rango. Seleccione entre 0 y 3.\n";
        }
    }
}

// --- MENU 2: MODIFICACION Y VISUALIZACION ---
// Retorna true si el usuario pide volver al Menu 1, o false si decide salir del programa
bool menuOperacionRed(Red& miRed) {
    int opcion = -1;

    while (true) {
        cout << "\n";
        cout << "MENU DE OPERACION Y CONSULTAS\n";
        cout << "\n";
        cout << "MODIFICACIONES EN TIEMPO DE EJECUCION\n";
        cout << "1. Agregar un nuevo enrutador (nodo)\n";
        cout << "2. Eliminar un enrutador existente (nodo)\n";
        cout << "3. Agregar o actualizar el costo de un enlace\n";
        cout << "4. Eliminar un enlace entre dos enrutadores\n";
        cout << "5. Guardar la topologia actual en " << ARCHIVO_POR_DEFECTO << "\n\n";
        cout << "VISUALIZACION Y CONSULTAS DE ENRUTAMIENTO\n";
        cout << "6. Mostrar Diagrama de Red (Conexiones y Matriz de Adyacencia)\n";
        cout << "7. Mostrar Matriz Global de Costos Minimos\n";
        cout << "8. Mostrar Tabla de Enrutamiento de un enrutador especifico\n";
        cout << "9. Mostrar Tablas de Enrutamiento de TODOS los enrutadores\n";
        cout << "10. Consultar envio de paquete (Costo minimo y Nodos visitados)\n\n";
        cout << "\n";
        cout << "11. Volver al menu de carga de topologia\n";
        cout << "0. Salir del simulador\n";
        cout << "\n";

        opcion = leerEnteroPositivo("Seleccione una opcion (0-11): ", 0);

        switch (opcion) {
        case 1: {
            char id = leerLetraNodoValida("\n Ingrese la letra del nuevo enrutador (A-Z): ");
            if (miRed.agregarEnrutador(id)) {
                cout << "Enrutador [" << id << "] agregado a la red y tablas actualizadas.\n";
            } else {
                cout << "El enrutador [" << id << "] ya existe en la red.\n";
            }
            break;
        }

        case 2: {
            char id = leerLetraNodoValida("\n Ingrese la letra del enrutador a ELIMINAR (A-Z): ");
            if (miRed.eliminarEnrutador(id)) {
                cout << "Enrutador [" << id << "] y sus enlaces fueron eliminados. Tablas recalculadas.\n";
            } else {
                cout << "Error: El enrutador [" << id << "] no existe en la red.\n";
            }
            break;
        }

        case 3: {
            while (true) {
                char orig = leerLetraNodoValida("\n Ingrese Enrutador Origen (A-Z): ");
                char dest = leerLetraNodoValida("Ingrese Enrutador Destino (A-Z): ");

                if (orig == dest) {
                    cout << "Error: Origen y Destino no pueden ser el mismo nodo [" << orig << "]. Intente de nuevo.\n";
                    continue;
                }

                int costo = leerEnteroPositivo("Ingrese el costo del enlace (> 0): ", 1);
                miRed.agregarOActualizarEnlace(orig, dest, costo);
                cout << "Enlace [" << orig << "] <--(" << costo << ")--> [" << dest << "] actualizado y tablas recalculadas.\n";
                break;
            }
            break;
        }

        case 4: {
            char orig = leerLetraNodoValida("\n Ingrese Enrutador Origen del enlace a borrar (A-Z): ");
            char dest = leerLetraNodoValida("Ingrese Enrutador Destino del enlace a borrar (A-Z): ");

            if (!miRed.existeEnrutador(orig) || !miRed.existeEnrutador(dest)) {
                cout << "Error: Alguno de los enrutadores ([" << orig << "] o [" << dest << "]) no existe en la red.\n";
            } else if (miRed.eliminarEnlace(orig, dest)) {
                cout << "Enlace entre [" << orig << "] y [" << dest << "] eliminado. Tablas recalculadas.\n";
            } else {
                cout << "Error: No existe un enlace fisico directo entre [" << orig << "] y [" << dest << "].\n";
            }
            break;
        }

        case 5: {
            if (miRed.guardarEnArchivo(ARCHIVO_POR_DEFECTO)) {
                cout << "\n Topologia actual guardada exitosamente en '" << ARCHIVO_POR_DEFECTO << "'.\n";
            } else {
                cout << "\n Error al guardar en '" << ARCHIVO_POR_DEFECTO << "'.\n";
            }
            break;
        }

        case 6:
            miRed.mostrarDiagramaRed();
            break;

        case 7:
            miRed.mostrarMatrizCostosMinimos();
            break;

        case 8: {
            char id = leerLetraNodoValida("\n Ingrese la letra del enrutador para ver su tabla (A-Z): ");
            miRed.mostrarTablaEnrutador(id);
            break;
        }

        case 9:
            miRed.mostrarTodasLasTablas();
            break;

        case 10: {
            char orig = leerLetraNodoValida("\n Ingrese el Enrutador Origen del paquete (A-Z): ");
            char dest = leerLetraNodoValida("Ingrese el Enrutador Destino del paquete (A-Z): ");
            miRed.consultarRutaYCosto(orig, dest);
            break;
        }

        case 11:
            cout << "\n Regresando al menu de seleccion de carga de topologia...\n";
            return true; // Regresa al Menu 1

        case 0:
            cout << "\n Saliendo del simulador...\n";
            return false; // Termina el programa

        default:
            cout << "\n Opcion fuera de rango. Seleccione entre 0 y 11.\n";
            break;
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    Red miRed;

    // Ciclo principal entre el Menu 1 (Carga) y el Menu 2 (Operacion)
    while (true) {
        bool topologiaLista = menuCargaTopologia(miRed);
        if (!topologiaLista) {
            break; // El usuario eligio 0 en el Menu 1
        }

        bool volverACargar = menuOperacionRed(miRed);
        if (!volverACargar) {
            break; // El usuario eligio 0 en el Menu 2
        }
    }

    return 0;
}