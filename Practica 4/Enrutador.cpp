#include "Enrutador.h"

Enrutador::Enrutador() : id('\0') {}

Enrutador::Enrutador(char _id) : id(_id) {
    // El costo hacia si mismo siempre inicia en 0
    tablaCostos[id] = 0;
    tablaCaminos[id] = {id};
}

char Enrutador::getId() const {
    return id;
}

void Enrutador::conectarVecino(char idVecino, int costo) {
    if (idVecino != id && costo > 0) {
        vecinos[idVecino] = costo; // Crea el enlace o actualiza su costo
    }
}

void Enrutador::desconectarVecino(char idVecino) {
    vecinos.erase(idVecino);
}

bool Enrutador::tieneVecino(char idVecino) const {
    return vecinos.find(idVecino) != vecinos.end();
}

const map<char, int>& Enrutador::getVecinos() const {
    return vecinos;
}

void Enrutador::limpiarTablaEnrutamiento() {
    tablaCostos.clear();
    tablaCaminos.clear();
    tablaCostos[id] = 0;
    tablaCaminos[id] = {id};
}

void Enrutador::asignarRuta(char idDestino, int costoTotal, const list<char>& camino) {
    tablaCostos[idDestino] = costoTotal;
    tablaCaminos[idDestino] = camino;
}

int Enrutador::obtenerCostoA(char idDestino) const {
    auto it = tablaCostos.find(idDestino);
    if (it != tablaCostos.end()) {
        return it->second;
    }
    return -1; // -1 es inalcanzable o inexistente
}

list<char> Enrutador::obtenerCaminoA(char idDestino) const {
    auto it = tablaCaminos.find(idDestino);
    if (it != tablaCaminos.end()) {
        return it->second;
    }
    return list<char>(); // Lista vacia si no hay camino
}

void Enrutador::mostrarVecinos() const {
    cout << "\nEnrutador [" << id << "] conectado directamente con: ";
    if (vecinos.empty()) {
        cout << "(Sin conexiones directas)";
    } else {
        for (const auto& par : vecinos) {
            cout << "--" << par.second << "--> [" << par.first << "]   ";
        }
    }
    cout << endl;
}

void Enrutador::mostrarTablaEnrutamiento() const {
    cout << "\n          TABLA DE ENRUTAMIENTO DEL ENRUTADOR [" << id << "]\n\n";
    cout << left << setw(10) << "Destino"
         << setw(15) << "Costo Minimo"
         << setw(18) << "Siguiente Salto"
         << "Nodos Visitados (Ruta)" << endl;
    cout << "--------------------------------------------------------------------\n";

    for (const auto& par : tablaCostos) {
        char destino = par.first;
        int costo = par.second;
        const list<char>& camino = tablaCaminos.at(destino);

        // Determina el caracter del siguiente salto
        char siguienteSalto = '-';
        if (camino.size() > 1) {
            auto it = camino.begin();
            ++it; // Segundo elemento de la lista
            siguienteSalto = *it;
        } else if (destino == id) {
            siguienteSalto = id;
        }

        cout << left << setw(10) << destino;
        if (costo == -1) {
            cout << setw(15) << "Inalcanzable" << setw(18) << '-' << "Sin ruta" << endl;
        } else {
            cout << setw(15) << costo << setw(18) << siguienteSalto;

            // Imprimir nodos visitados
            for (auto it = camino.begin(); it != camino.end(); ++it) {
                cout << *it;
                if (next(it) != camino.end()) {
                    cout << " -> ";
                }
            }
            cout << endl;
        }
    }
    cout << "--------------------------------------------------------------------\n";
}