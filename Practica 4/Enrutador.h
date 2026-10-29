#ifndef ENRUTADOR_H
#define ENRUTADOR_H

#include <iostream>
#include <map>
#include <list>
#include <iomanip>

using namespace std;

class Enrutador {
private:
    char id; // Identificador

    // Enlaces fisicos directos: <ID_Vecino, Costo_Enlace>
    map<char, int> vecinos;

    // Tabla de enrutamiento (Costos minimos): <ID_Destino, Costo_Total_Minimo>
    map<char, int> tablaCostos;

    // Tabla de enrutamiento (Caminos): <ID_Destino, Lista_de_nodos_visitados>
    map<char, list<char>> tablaCaminos;

public:
    // Constructores
    Enrutador();
    Enrutador(char _id);

    // Getter del ID
    char getId() const;

    // Metodos para gestionar vecinos directos (Enlaces fisicos)
    void conectarVecino(char idVecino, int costo);
    void desconectarVecino(char idVecino);
    bool tieneVecino(char idVecino) const;
    const map<char, int>& getVecinos() const;

    // Metodos para gestionar la Tabla de Enrutamiento
    void limpiarTablaEnrutamiento();
    void asignarRuta(char idDestino, int costoTotal, const list<char>& camino);
    int obtenerCostoA(char idDestino) const;
    list<char> obtenerCaminoA(char idDestino) const;

    // Metodos de visualizacion
    void mostrarVecinos() const;
    void mostrarTablaEnrutamiento() const;
};

#endif // ENRUTADOR_H