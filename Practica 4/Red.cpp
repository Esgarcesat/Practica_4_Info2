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
    // 1. Verificar que ambos enrutadores existan en la topologia
    if (!existeEnrutador(idOrigen) || !existeEnrutador(idDestino)) {
        return false;
    }

    // 2. Verificar que realmente exista un enlace fisico directo entre ellos
    if (!enrutadores[idOrigen].tieneVecino(idDestino)) {
        return false;
    }

    enrutadores[idOrigen].desconectarVecino(idDestino);
    enrutadores[idDestino].desconectarVecino(idOrigen);

    actualizarTablas(); // Recalculo automatico al romper el enlace
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
        cout << "  Error: El enrutador origen [" << idOrigen << "] no existe en la red.\n";
        return;
    }
    if (!existeEnrutador(idDestino)) {
        cout << "  Error: El enrutador destino [" << idDestino << "] no existe en la red.\n";
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

    cout << " 1. Esquema de Enlaces Fisicos Directos \n";
    for (const auto& par : enrutadores) {
        cout << "  ";
        par.second.mostrarVecinos();
    }

    cout << "\n 2. Matriz de Adyacencia (Costos de enlaces directos) \n";
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
        cout << "\n Error: El enrutador [" << id << "] no existe en la red.\n";
    }
}

void Red::mostrarTodasLasTablas() const {
    if (enrutadores.empty()) {
        cout << "\n La red esta vacia.\n";
        return;
    }
    for (const auto& par : enrutadores) {
        par.second.mostrarTablaEnrutamiento();
    }
}

void Red::limpiarRed() {
    enrutadores.clear();
}

// ARCHIVOS Y GENERACION ALEATORIA

bool Red::cargarDesdeArchivo(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "\n  Error critico: No se encontro o no se pudo abrir el archivo '" << nombreArchivo << "'.\n";
        return false;
    }

    limpiarRed();

    char linea[256];
    int numLinea = 0;
    int enlacesValidos = 0;
    bool huboErrores = false;

    while (archivo.getline(linea, sizeof(linea))) {
        numLinea++;

        // Ignorar lineas vacias o de solo espacios
        int i = 0;
        while (linea[i] == ' ' || linea[i] == '\t' || linea[i] == '\r') {
            i++;
        }
        if (linea[i] == '\0') {
            continue;
        }

        char origStr[50], destStr[50], extraStr[50];
        int costo = 0;

        int leidos = sscanf(linea, "%49s %49s %d %49s", origStr, destStr, &costo, extraStr);

        if (leidos < 3) {
            cout << "   Error en " << nombreArchivo << " (Linea " << numLinea
                 << "): Formato incompleto o el costo no es un numero entero -> \"" << linea << "\"\n";
            huboErrores = true;
            continue;
        }
        if (leidos > 3) {
            cout << "   Error en " << nombreArchivo << " (Linea " << numLinea
                 << "): Demasiados datos en la misma linea -> \"" << linea << "\"\n";
            huboErrores = true;
            continue;
        }

        if (origStr[1] != '\0' || !isalpha(origStr[0])) {
            cout << "   Error en " << nombreArchivo << " (Linea " << numLinea
                 << "): El origen '" << origStr << "' no es una letra valida (A-Z).\n";
            huboErrores = true;
            continue;
        }
        if (destStr[1] != '\0' || !isalpha(destStr[0])) {
            cout << "   Error en " << nombreArchivo << " (Linea " << numLinea
                 << "): El destino '" << destStr << "' no es una letra valida (A-Z).\n";
            huboErrores = true;
            continue;
        }

        char origen = toupper(origStr[0]);
        char destino = toupper(destStr[0]);

        if (origen == destino) {
            cout << "   Error en " << nombreArchivo << " (Linea " << numLinea
                 << "): Auto-enlace no permitido (Origen '" << origen << "' es igual a Destino '" << destino << "').\n";
            huboErrores = true;
            continue;
        }

        if (costo <= 0) {
            cout << "   Error en " << nombreArchivo << " (Linea " << numLinea
                 << "): El costo (" << costo << ") entre [" << origen << "] y [" << destino << "] debe ser mayor a 0.\n";
            huboErrores = true;
            continue;
        }

        // Solo guardamos en memoria si no ha habido errores previos,
        // pero seguimos recorriendo el archivo para reportar TODOS los errores de una vez
        if (!huboErrores) {
            if (!existeEnrutador(origen)) {
                enrutadores[origen] = Enrutador(origen);
            }
            if (!existeEnrutador(destino)) {
                enrutadores[destino] = Enrutador(destino);
            }
            enrutadores[origen].conectarVecino(destino, costo);
            enrutadores[destino].conectarVecino(origen, costo);
            enlacesValidos++;
        }
    }

    archivo.close();

    // Si hubo al menos un error en cualquier linea, rechazamos todo el archivo
    if (huboErrores) {
        cout << "\n   Carga abortada: El archivo '" << nombreArchivo
             << "' contiene errores de diseno. Corrijalos antes de continuar.\n";
        limpiarRed();
        return false;
    }

    if (enlacesValidos == 0) {
        cout << "\n   Topologia invalida: El archivo '" << nombreArchivo << "' esta vacio.\n";
        limpiarRed();
        return false;
    }

    actualizarTablas();
    return true;
}

bool Red::guardarEnArchivo(const char* nombreArchivo) const {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        return false;
    }

    for (const auto& par : enrutadores) {
        char origen = par.first;
        const map<char, int>& vecinos = par.second.getVecinos();

        for (const auto& enlace : vecinos) {
            char destino = enlace.first;
            int costo = enlace.second;

            // Como la red es bidireccional, guardamos solo cuando origen < destino
            // para no duplicar el mismo cable dos veces en el archivo .txt
            if (origen < destino) {
                archivo << origen << " " << destino << " " << costo << "\n";
            }
        }
    }

    archivo.close();
    return true;
}

void Red::generarRedAleatoria(int cantidadNodos, int costoMaximo, const char* archivoSalida) {
    if (cantidadNodos < 2) cantidadNodos = 2;
    if (cantidadNodos > 26) cantidadNodos = 26; // Maximo de 'A' a 'Z'
    if (costoMaximo < 1) costoMaximo = 10;

    limpiarRed();

    // Contenedor 5: std::vector<char> para acceder a los nodos por indice aleatorio
    vector<char> listaNodos;
    for (int i = 0; i < cantidadNodos; ++i) {
        char id = 'A' + i;
        enrutadores[id] = Enrutador(id);
        listaNodos.push_back(id);
    }

    // 1. Garantizar que la red sea conexa (arbol base) conectando cada nuevo nodo con uno anterior
    for (int i = 1; i < cantidadNodos; ++i) {
        int indicePrevio = rand() % i;
        char u = listaNodos[i];
        char v = listaNodos[indicePrevio];
        int costo = 1 + (rand() % costoMaximo);

        enrutadores[u].conectarVecino(v, costo);
        enrutadores[v].conectarVecino(u, costo);
    }

    // 2. Agregar enlaces adicionales al azar para crear caminos alternativos (ciclos)
    int enlacesExtra = cantidadNodos / 2 + (rand() % cantidadNodos);
    for (int i = 0; i < enlacesExtra; ++i) {
        char u = listaNodos[rand() % cantidadNodos];
        char v = listaNodos[rand() % cantidadNodos];

        if (u != v && !enrutadores[u].tieneVecino(v)) {
            int costo = 1 + (rand() % costoMaximo);
            enrutadores[u].conectarVecino(v, costo);
            enrutadores[v].conectarVecino(u, costo);
        }
    }

    // Si se especifico un nombre de archivo, guardamos la topologia y la recargamos desde el archivo
    // cumpliendo literalmente con la sugerencia del punto 5 de la guia
    if (archivoSalida != nullptr) {
        guardarEnArchivo(archivoSalida);
        cargarDesdeArchivo(archivoSalida);
    } else {
        actualizarTablas();
    }
}