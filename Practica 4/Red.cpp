#include "Red.h"

Red::Red() {}

bool Red::existeEnrutador(char id) const {
    return enrutadores.find(id) != enrutadores.end();
}

bool Red::agregarEnrutador(char id) {
    if (existeEnrutador(id)) {
        return false; // Ya existe un enrutador con esa letra
    }
    enrutadores[id] = Enrutador(id);
    // Nota: En la Fase 3 conectaremos aqui actualizarTablas()
    return true;
}

bool Red::eliminarEnrutador(char id) {
    if (!existeEnrutador(id)) {
        return false; // No existe en la red
    }

    // 1. Eliminar cualquier enlace fisico que apunte hacia este nodo en los demas enrutadores
    for (auto& par : enrutadores) {
        par.second.desconectarVecino(id);
    }

    // 2. Borrar el enrutador del mapa de la red
    enrutadores.erase(id);

    // Nota: En la Fase 3 conectaremos aqui actualizarTablas()
    return true;
}

bool Red::agregarOActualizarEnlace(char idOrigen, char idDestino, int costo) {
    if (idOrigen == idDestino || costo <= 0) {
        return false; // No se permiten auto-enlaces con costo ni costos negativos/cero
    }

    // Si alguno de los enrutadores no existe aun, lo creamos automaticamente
    if (!existeEnrutador(idOrigen)) {
        enrutadores[idOrigen] = Enrutador(idOrigen);
    }
    if (!existeEnrutador(idDestino)) {
        enrutadores[idDestino] = Enrutador(idDestino);
    }

    // Enlace bidireccional (Grafo no dirigido)
    enrutadores[idOrigen].conectarVecino(idDestino, costo);
    enrutadores[idDestino].conectarVecino(idOrigen, costo);

    // Nota: En la Fase 3 conectaremos aqui actualizarTablas()
    return true;
}

bool Red::eliminarEnlace(char idOrigen, char idDestino) {
    if (!existeEnrutador(idOrigen) || !existeEnrutador(idDestino)) {
        return false;
    }

    // Desconectamos en ambos sentidos
    enrutadores[idOrigen].desconectarVecino(idDestino);
    enrutadores[idDestino].desconectarVecino(idOrigen);

    // Nota: En la Fase 3 conectaremos aqui actualizarTablas()
    return true;
}

void Red::mostrarDiagramaRed() const {
    cout << "\n====================================================================\n";
    cout << "                   DIAGRAMA TOPOLOGICO DE LA RED\n";
    cout << "====================================================================\n";

    if (enrutadores.empty()) {
        cout << "  [La red no tiene enrutadores registrados actualmente]\n";
        cout << "====================================================================\n";
        return;
    }

    // 1. Representacion tipo esquema de enlaces por nodo
    cout << "--- 1. Esquema de Enlaces Fisicos Directos ---\n";
    for (const auto& par : enrutadores) {
        cout << "  ";
        par.second.mostrarVecinos();
    }

    // 2. Representacion matricial de enlaces fisicos (Topologia directa)
    cout << "\n--- 2. Matriz de Adyacencia (Costos de enlaces directos) ---\n";
    cout << "      ";
    for (const auto& col : enrutadores) {
        cout << setw(6) << col.first;
    }
    cout << "\n    --";
    for (size_t i = 0; i < enrutadores.size(); ++i) {
        cout << "------";
    }
    cout << endl;

    for (const auto& fila : enrutadores) {
        char idFila = fila.first;
        const map<char, int>& vecinosFila = fila.second.getVecinos();

        cout << "  " << idFila << " | ";
        for (const auto& col : enrutadores) {
            char idCol = col.first;
            if (idFila == idCol) {
                cout << setw(6) << 0;
            } else {
                auto it = vecinosFila.find(idCol);
                if (it != vecinosFila.end()) {
                    cout << setw(6) << it->second;
                } else {
                    cout << setw(6) << '-'; // '-' indica que no hay cable directo
                }
            }
        }
        cout << endl;
    }
    cout << "====================================================================\n";
}

void Red::mostrarTablaEnrutador(char id) const {
    auto it = enrutadores.find(id);
    if (it != enrutadores.end()) {
        it->second.mostrarTablaEnrutamiento();
    } else {
        cout << "\n[!] Error: El enrutador [" << id << "] no existe en la red.\n";
    }
}

void Red::mostrarTodasLasTablas() const {
    if (enrutadores.empty()) {
        cout << "\n[!] La red esta vacia.\n";
        return;
    }
    for (const auto& par : enrutadores) {
        par.second.mostrarTablaEnrutamiento();
    }
}

void Red::limpiarRed() {
    enrutadores.clear();
}