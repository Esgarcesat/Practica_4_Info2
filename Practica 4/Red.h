#ifndef RED_H
#define RED_H

#include "Enrutador.h"
#include <iostream>
#include <map>
#include <iomanip>

using namespace std;

class Red {
private:
    // Mapa principal de la red: <ID_Enrutador, Objeto Enrutador>
    map<char, Enrutador> enrutadores;

public:
    Red();

    // --- Gestion dinamica de Enrutadores (Nodos) ---
    bool existeEnrutador(char id) const;
    bool agregarEnrutador(char id);
    bool eliminarEnrutador(char id);

    // --- Gestion dinamica de Enlaces (Conexiones fisicas) ---
    bool agregarOActualizarEnlace(char idOrigen, char idDestino, int costo);
    bool eliminarEnlace(char idOrigen, char idDestino);

    // --- Visualizacion de Topologia y Tablas ---
    void mostrarDiagramaRed() const;
    void mostrarTablaEnrutador(char id) const;
    void mostrarTodasLasTablas() const;

    // Metodo auxiliar para limpiar toda la red (util al cargar archivos)
    void limpiarRed();
};

#endif // RED_H