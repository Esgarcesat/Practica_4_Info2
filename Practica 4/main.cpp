#include <iostream>
#include "Red.h"

using namespace std;

int main() {
    Red miRed;

    // 1. Construimos la topologia de la Figura 1 de la guia
    // Enlaces: A-B (4), A-C (10), B-D (1), C-D (2)
    miRed.agregarOActualizarEnlace('A', 'B', 4);
    miRed.agregarOActualizarEnlace('A', 'C', 10);
    miRed.agregarOActualizarEnlace('B', 'D', 1);
    miRed.agregarOActualizarEnlace('C', 'D', 2);

    cout << ">>> ESTADO INICIAL (FIGURA 1 DE LA GUIA):";
    miRed.mostrarDiagramaRed();

    // 2. Simulamos prueba de sustentacion: Agregar nodo 'E' conectado a 'C' con costo 5
    cout << "\n>>> PRUEBA 1: Agregando nodo [E] conectado con [C] (costo 5)...";
    miRed.agregarEnrutador('E');
    miRed.agregarOActualizarEnlace('C', 'E', 5);
    miRed.mostrarDiagramaRed();

    // 3. Simulamos prueba de sustentacion: Eliminar el nodo 'D' de la red
    cout << "\n>>> PRUEBA 2: Eliminando el nodo [D] de la topologia...";
    miRed.eliminarEnrutador('D');
    miRed.mostrarDiagramaRed();

    return 0;
}