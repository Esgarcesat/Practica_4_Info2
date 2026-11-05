#ifndef RED_H
#define RED_H

#include "Enrutador.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <set>
#include <queue>
#include <vector>
#include <list>
#include <limits>
#include <iomanip>

using namespace std;

class Red {
private:
    // Contenedor 1: std::map
    map<char, Enrutador> enrutadores;

    void calcularDijkstraDesde(char idOrigen);

public:
    Red();

    // --- Gestion dinamica de Enrutadores (Nodos) ---
    bool existeEnrutador(char id) const;
    bool agregarEnrutador(char id);
    bool eliminarEnrutador(char id);

    // --- Gestion dinamica de Enlaces (Conexiones fisicas) ---
    bool agregarOActualizarEnlace(char idOrigen, char idDestino, int costo);
    bool eliminarEnlace(char idOrigen, char idDestino);

    // --- Algoritmo de Enrutamiento y Consultas ---
    void actualizarTablas();
    void consultarRutaYCosto(char idOrigen, char idDestino) const;

    // --- Visualizacion de Topologia y Tablas ---
    void mostrarDiagramaRed() const;
    void mostrarMatrizCostosMinimos() const;
    void mostrarTablaEnrutador(char id) const;
    void mostrarTodasLasTablas() const;

    // --- Archivos y Generacion Aleatoria (Fase 4) ---
    bool cargarDesdeArchivo(const char* nombreArchivo);
    bool guardarEnArchivo(const char* nombreArchivo) const;
    void generarRedAleatoria(int cantidadNodos, int costoMaximo, const char* archivoSalida = nullptr);

    void limpiarRed();
};

#endif // RED_H