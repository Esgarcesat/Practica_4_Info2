#include "Red.h"

Red::Red() {}

bool Red::existeEnrutador(char id) const {
    return enrutadores.find(id) != enrutadores.end();
}

bool Red::agregarEnrutador(char id) {
    if (existeEnrutador(id)) {
        return false;
    }
    enrutadores[id] = Enrutador(id);
    actualizarTablas(); // Recalculo automatico en tiempo de ejecucion
    return true;
}

bool Red::eliminarEnrutador(char id) {
    if (!existeEnrutador(id)) {
        return false;
    }

    for (auto& par : enrutadores) {
        par.second.desconectarVecino(id);
    }

    enrutadores.erase(id);
    actualizarTablas(); // Recalculo automatico tras borrar el nodo
    return true;
}

bool Red::agregarOActualizarEnlace(char idOrigen, char idDestino, int costo) {
    if (idOrigen == idDestino || costo <= 0) {
        return false;
    }

    if (!existeEnrutador(idOrigen)) {
        enrutadores[idOrigen] = Enrutador(idOrigen);
    }
    if (!existeEnrutador(idDestino)) {
        enrutadores[idDestino] = Enrutador(idDestino);
    }

    enrutadores[idOrigen].conectarVecino(idDestino, costo);
    enrutadores[idDestino].conectarVecino(idOrigen, costo);

    actualizarTablas(); // Recalculo automatico al cambiar enlaces
    return true;
}

bool Red::eliminarEnlace(char idOrigen, char idDestino) {
    if (!existeEnrutador(idOrigen) || !existeEnrutador(idDestino)) {
        return false;
    }

    enrutadores[idOrigen].desconectarVecino(idDestino);
    enrutadores[idDestino].desconectarVecino(idOrigen);

    actualizarTablas(); // Recalculo automatico al romper un enlace
    return true;
}

// --- MOTOR DE ENRUTAMIENTO: ALGORITMO DE DIJKSTRA ---
void Red::calcularDijkstraDesde(char idOrigen) {
    Enrutador& enrutadorOrigen = enrutadores[idOrigen];
    enrutadorOrigen.limpiarTablaEnrutamiento();

    const int INF = numeric_limits<int>::max();

    // Mapas auxiliares para distancias minimas y nodo predecesor (para armar la ruta)
    map<char, int> distancias;
    map<char, char> predecesores;

    for (const auto& par : enrutadores) {
        distancias[par.first] = INF;
        predecesores[par.first] = '\0';
    }
    distancias[idOrigen] = 0;

    // Contenedor 3: std::set -> Almacena los nodos ya visitados definitivamente
    set<char> visitados;

    // Contenedores 4 y 5: std::priority_queue apoyada en std::vector
    // Guarda pares <costo_acumulado, id_nodo> ordenados de menor a mayor costo
    typedef pair<int, char> ParCostoNodo;
    priority_queue<ParCostoNodo, vector<ParCostoNodo>, greater<ParCostoNodo>> colaPrioridad;

    colaPrioridad.push({0, idOrigen});

    while (!colaPrioridad.empty()) {
        char actual = colaPrioridad.top().second;
        int distActual = colaPrioridad.top().first;
        colaPrioridad.pop();

        // Si el nodo ya fue visitado en el set, lo ignoramos
        if (visitados.find(actual) != visitados.end()) {
            continue;
        }
        visitados.insert(actual); // Marcamos el nodo como visitado

        // Explorar todos los vecinos fisicos del nodo actual
        const map<char, int>& vecinos = enrutadores[actual].getVecinos();
        for (const auto& enlace : vecinos) {
            char vecino = enlace.first;
            int pesoEnlace = enlace.second;

            // Relajacion de arista si encontramos un camino mas corto hacia el vecino
            if (visitados.find(vecino) == visitados.end() && distActual != INF) {
                int nuevaDistancia = distActual + pesoEnlace;
                if (nuevaDistancia < distancias[vecino]) {
                    distancias[vecino] = nuevaDistancia;
                    predecesores[vecino] = actual;
                    colaPrioridad.push({nuevaDistancia, vecino});
                }
            }
        }
    }

    // Reconstruir los caminos con std::list y guardarlos en la tabla del enrutador
    for (const auto& par : enrutadores) {
        char destino = par.first;
        if (distancias[destino] == INF) {
            // El destino es inalcanzable (red desconectada)
            enrutadorOrigen.asignarRuta(destino, -1, list<char>());
        } else {
            // Contenedor 2: std::list -> Reconstruccion inversa usando push_front
            list<char> caminoOptimo;
            char paso = destino;
            while (paso != '\0') {
                caminoOptimo.push_front(paso);
                paso = predecesores[paso];
            }
            enrutadorOrigen.asignarRuta(destino, distancias[destino], caminoOptimo);
        }
    }
}

void Red::actualizarTablas() {
    for (const auto& par : enrutadores) {
        calcularDijkstraDesde(par.first);
    }
}

void Red::consultarRutaYCosto(char idOrigen, char idDestino) const {
    cout << "\n--------------------------------------------------------------------\n";
    cout << "  CONSULTA DE PAQUETE: Origen [" << idOrigen << "] ---> Destino [" << idDestino << "]\n";
    cout << "--------------------------------------------------------------------\n";

    if (!existeEnrutador(idOrigen)) {
        cout << "  [!] Error: El enrutador origen [" << idOrigen << "] no existe en la red.\n";
        return;
    }
    if (!existeEnrutador(idDestino)) {
        cout << "  [!] Error: El enrutador destino [" << idDestino << "] no existe en la red.\n";
        return;
    }

    const Enrutador& origen = enrutadores.at(idOrigen);
    int costo = origen.obtenerCostoA(idDestino);
    list<char> camino = origen.obtenerCaminoA(idDestino);

    if (costo == -1 || camino.empty()) {
        cout << "  Estado: DESTINO INALCANZABLE (No existe camino entre ["
             << idOrigen << "] y [" << idDestino << "]).\n";
    } else {
        cout << "  * Costo total minimo : " << costo << "\n";
        cout << "  * Cantidad de saltos : " << (camino.size() - 1) << "\n";
        cout << "  * Nodos visitados    : ";
        for (auto it = camino.begin(); it != camino.end(); ++it) {
            cout << "[" << *it << "]";
            if (next(it) != camino.end()) {
                cout << " -> ";
            }
        }
        cout << "\n";
    }
    cout << "--------------------------------------------------------------------\n";
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

    cout << "--- 1. Esquema de Enlaces Fisicos Directos ---\n";
    for (const auto& par : enrutadores) {
        cout << "  ";
        par.second.mostrarVecinos();
    }

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
                    cout << setw(6) << '-';
                }
            }
        }
        cout << endl;
    }
    cout << "====================================================================\n";
}

void Red::mostrarMatrizCostosMinimos() const {
    cout << "\n====================================================================\n";
    cout << "         MATRIZ GLOBAL DE ENRUTAMIENTO (COSTOS MAS CORTOS)\n";
    cout << "====================================================================\n";

    if (enrutadores.empty()) {
        cout << "  [La red esta vacia]\n";
        cout << "====================================================================\n";
        return;
    }

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
        cout << "  " << idFila << " | ";
        for (const auto& col : enrutadores) {
            char idCol = col.first;
            int costoOptimo = fila.second.obtenerCostoA(idCol);
            if (costoOptimo == -1) {
                cout << setw(6) << 'X'; // X = Inalcanzable
            } else {
                cout << setw(6) << costoOptimo;
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