#include <iostream>
#include "Red.h"

using namespace std;

int main() {
    Red miRed;

    // 1. Cargamos los enlaces fisicos de la Figura 1 de la guia
    miRed.agregarOActualizarEnlace('A', 'B', 4);
    miRed.agregarOActualizarEnlace('A', 'C', 10);
    miRed.agregarOActualizarEnlace('B', 'D', 1);
    miRed.agregarOActualizarEnlace('C', 'D', 2);

    // 2. Verificamos la Matriz Global de Costos Minimos (Paginas 2 y 3 del PDF)
    miRed.mostrarMatrizCostosMinimos();

    // 3. Mostramos las tablas detalladas de A y D (como en el PDF)
    miRed.mostrarTablaEnrutador('A');
    miRed.mostrarTablaEnrutador('D');

    // 4. Consultamos el envio de un paquete de A hacia C
    miRed.consultarRutaYCosto('A', 'C');

    // 5. PRUEBA DE SUSTENTACION: ¿Que pasa con la ruta de A a C si se cae el nodo D?
    cout << "\n>>> SIMULACION DE FALLO: Eliminando el nodo [D] de la red...";
    miRed.eliminarEnrutador('D');

    miRed.mostrarTablaEnrutador('A');
    miRed.consultarRutaYCosto('A', 'C');

    return 0;
}