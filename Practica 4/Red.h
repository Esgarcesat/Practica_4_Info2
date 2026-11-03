#ifndef RED_H
#define RED_H

#include "Enrutador.h"
#include <iostream>
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

    // Metodo interno que ejecuta Dijkstra para un enrutador especifico
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

    // --- Algoritmo de Enrutamiento y Consultas (Fase 3) ---
    void actualizarTablas();
    void consultarRutaYCosto(char idOrigen, char idDestino) const;

    // --- Visualizacion de Topologia y Tablas ---
    void mostrarDiagramaRed() const;
    void mostrarMatrizCostosMinimos() const;
    void mostrarTablaEnrutador(char id) const;
    void mostrarTodasLasTablas() const;

    void limpiarRed();
};

#endif // RED_H