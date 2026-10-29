#include <iostream>
#include "Enrutador.h"

using namespace std;

int main() {
    // 1. Creamos el enrutador 'A'
    Enrutador nodoA('A');

    // 2. Conectamos sus vecinos directos
    nodoA.conectarVecino('B', 4);
    nodoA.conectarVecino('C', 10);

    cout << "PRUEBA DE ENLACES DIRECTOS\n";
    nodoA.mostrarVecinos();

    // 3. Simulamos la carga de su tabla de enrutamiento con char
    nodoA.asignarRuta('B', 4, {'A', 'B'});
    nodoA.asignarRuta('D', 5, {'A', 'B', 'D'});
    nodoA.asignarRuta('C', 7, {'A', 'B', 'D', 'C'});

    // 4. Mostramos la tabla de enrutamiento
    nodoA.mostrarTablaEnrutamiento();

    return 0;
}