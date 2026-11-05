#include <iostream>
#include <fstream>
#include <ctime>
#include "Red.h"

using namespace std;

int main() {
    // Semilla para numeros aleatorios
    srand(static_cast<unsigned int>(time(nullptr)));

    Red miRed;

    // --- PRUEBA 1: Creacion y lectura desde un archivo .txt ---
    // Creamos un archivo de prueba con la Figura 1 de la guia
    ofstream archivoPrueba("red_guia.txt");
    archivoPrueba << "A B 4\n";
    archivoPrueba << "A C 10\n";
    archivoPrueba << "B D 1\n";
    archivoPrueba << "C D 2\n";
    archivoPrueba.close();

    cout << ">>> PRUEBA 1: Cargando topologia desde 'red_guia.txt'...\n";
    if (miRed.cargarDesdeArchivo("red_guia.txt")) {
        miRed.mostrarDiagramaRed();
        miRed.mostrarMatrizCostosMinimos();
    } else {
        cout << "[!] Error al abrir el archivo.\n";
    }

    // --- PRUEBA 2: Generacion de red aleatoria + volcado a archivo ---
    cout << "\n>>> PRUEBA 2: Generando red aleatoria de 5 nodos (guardada en 'red_aleatoria.txt')...\n";
    miRed.generarRedAleatoria(5, 15, "red_aleatoria.txt");

    miRed.mostrarDiagramaRed();
    miRed.mostrarMatrizCostosMinimos();
    miRed.consultarRutaYCosto('A', 'E');

    return 0;
}