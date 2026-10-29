#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

struct Tramite {                       
    char codigo[15];
    char dni[9];
    char nombre[50];
    char tipo[40];
    char fecha[11];
    char estado[15];                   
    float costo;
    char descripcion[100];
    Tramite* siguiente;
};

struct Turno {                         
    int numero;
    char dni[9];
    char codigoTramite[15];
    Turno* siguiente;
};

struct Modificacion {                  
    char codigoTramite[15];
    char dni[9];                       
    char nombre[50];
    char tipo[40];
    char fecha[11];
    char estado[15];
    float costo;
    char descripcion[100];
    char cambio[100];                  
    Modificacion* siguiente;
};

Tramite* inicioLista = NULL;           
Turno* frente = NULL;                  
Turno* fin = NULL;                    
int ultimoNumero = 0;                  
Modificacion* cima = NULL;             

const char* ARCH_TRAMITES = "tramites.txt";
const char* ARCH_TURNOS = "turnos.txt";
const char* ARCH_HISTORIAL = "historial.txt";

bool soloDigitos(const char* s) {
    int n = strlen(s);
    if (n == 0) return false;
    for (int i = 0; i < n; i++)
        if (s[i] < '0' || s[i] > '9') return false;
    return true;
}

void leerTexto(const char* msg, char* destino, int max) {
    while (true) {
        cout << msg;
        cin.getline(destino, max);
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Texto demasiado largo.\n";
        } else if (strlen(destino) == 0 || strchr(destino, '|') != NULL) {
            cout << "  Dato invalido (no puede estar vacio ni contener '|').\n";
        } else {
            return;
        }
    }
}

void leerTextoLetras(const char* msg, char* destino, int max) {
    bool tieneNumero;

    do {
        leerTexto(msg, destino, max);
        tieneNumero = false;

        for (int i = 0; destino[i] != '\0'; i++) {
            if (destino[i] >= '0' && destino[i] <= '9') {
                tieneNumero = true;
                break;
            }
        }

        if (tieneNumero) {
            cout << "No ingrese numeros en este campo\n";
        }

    } while (tieneNumero);
}


void leerDni(const char* msg, char* destino) {
    char aux[50];
    while (true) {
        cout << msg;
        cin.getline(aux, 50);
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); }
        else if (strlen(aux) == 8 && soloDigitos(aux)) { strcpy(destino, aux); return; }
        cout << "  DNI invalido. Debe tener exactamente 8 digitos.\n";
    }
}

void leerFecha(const char* msg, char* destino) {      
    char aux[50];
    while (true) {
        cout << msg;
        cin.getline(aux, 50);
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); }
        else if (strlen(aux) == 10 && aux[2] == '/' && aux[5] == '/') {
            char d[3] = {aux[0], aux[1], '\0'};
            char m[3] = {aux[3], aux[4], '\0'};
            char a[5] = {aux[6], aux[7], aux[8], aux[9], '\0'};
            if (soloDigitos(d) && soloDigitos(m) && soloDigitos(a)) {
                int dia = atoi(d), mes = atoi(m);
                if (dia >= 1 && dia <= 31 && mes >= 1 && mes <= 12) {
                    strcpy(destino, aux);
                    return;
                }
            }
        }
        cout << "  Fecha invalida. Use el formato dd/mm/aaaa.\n";
    }
}

float leerCosto(const char* msg) {
    char aux[50];
    while (true) {
        cout << msg;
        cin.getline(aux, 50);
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); }
        else {
            int n = strlen(aux), puntos = 0;
            bool ok = (n > 0);
            for (int i = 0; i < n; i++) {
                if (aux[i] == '.') puntos++;
                else if (aux[i] < '0' || aux[i] > '9') ok = false;
            }
            if (ok && puntos <= 1 && n > puntos) return atof(aux);
        }
        cout << "  Costo invalido. Ingrese un numero positivo (ej. 45.50).\n";
    }
}

int leerOpcion(const char* msg) {
    char aux[50];
    cout << msg;
    cin.getline(aux, 50);
    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return -1; }
    if (!soloDigitos(aux)) return -1;
    return atoi(aux);
}

void leerEstado(char* destino) {
    while (true) {
        cout << "  1. Pendiente\n  2. En proceso\n  3. Finalizado\n";
        int op = leerOpcion("  Seleccione el estado: ");
        if (op == 1) { strcpy(destino, "Pendiente"); return; }
        if (op == 2) { strcpy(destino, "En proceso"); return; }
        if (op == 3) { strcpy(destino, "Finalizado"); return; }
        cout << "  Opcion inexistente.\n";
    }
}


Tramite* buscarPorCodigo(const char* codigo) {
    for (Tramite* aux = inicioLista; aux != NULL; aux = aux->siguiente)
        if (strcmp(aux->codigo, codigo) == 0) return aux;
    return NULL;
}

void mostrarTramite(Tramite* t) {
    cout << "  --------------------------------------\n";
    cout << "  Codigo      : " << t->codigo << endl;
    cout << "  DNI         : " << t->dni << endl;
    cout << "  Cliente     : " << t->nombre << endl;
    cout << "  Tipo        : " << t->tipo << endl;
    cout << "  Fecha       : " << t->fecha << endl;
    cout << "  Estado      : " << t->estado << endl;
    cout << "  Costo       : S/ " << t->costo << endl;
    cout << "  Descripcion : " << t->descripcion << endl;
}

int contarTramites() {
    int n = 0;
    for (Tramite* aux = inicioLista; aux != NULL; aux = aux->siguiente) n++;
    return n;
}

bool pilaVacia() { return cima == NULL; }

void mostrarModificacion(Modificacion* m) {
    cout << "  --------------------------------------\n";
    cout << "  Tramite modificado : " << m->codigoTramite << endl;
    cout << "  Cambio realizado   : " << m->cambio << endl;
    cout << "  Datos anteriores:\n";
    cout << "    DNI         : " << m->dni << endl;
    cout << "    Cliente     : " << m->nombre << endl;
    cout << "    Tipo        : " << m->tipo << endl;
    cout << "    Fecha       : " << m->fecha << endl;
    cout << "    Estado      : " << m->estado << endl;
    cout << "    Costo       : S/ " << m->costo << endl;
    cout << "    Descripcion : " << m->descripcion << endl;
}

void apilarModificacion(Tramite* t, const char* cambio) {
    Modificacion* m = new Modificacion;
    strcpy(m->codigoTramite, t->codigo);
    strcpy(m->dni, t->dni);
    strcpy(m->nombre, t->nombre);
    strcpy(m->tipo, t->tipo);
    strcpy(m->fecha, t->fecha);
    strcpy(m->estado, t->estado);
    m->costo = t->costo;
    strcpy(m->descripcion, t->descripcion);
    strcpy(m->cambio, cambio);
    m->siguiente = cima;       
    cima = m;                  
}

void eliminarUltimaModificacion() {
    cout << "\n=== ELIMINAR ULTIMA MODIFICACION ===\n";
    if (pilaVacia()) { cout << "El historial esta vacio.\n"; return; }
    Modificacion* temp = cima;
    cout << "Modificacion eliminada del tramite: " << temp->codigoTramite << endl;
    cima = cima->siguiente;
    delete temp;
    cout << "Operacion retirada correctamente.\n";
}

void deshacerUltimaModificacion() {
    cout << "\n=== DESHACER ULTIMA MODIFICACION ===\n";
    if (pilaVacia()) { cout << "No hay modificaciones para deshacer.\n"; return; }
    Modificacion* m = cima;
    cout << "Se recuperaran los datos anteriores:\n";
    mostrarModificacion(m);

    Tramite* t = buscarPorCodigo(m->codigoTramite);
    if (t == NULL) {
        cout << "El tramite ya no existe en la lista. Se descarta la modificacion.\n";
    } else {
        strcpy(t->dni, m->dni);
        strcpy(t->nombre, m->nombre);
        strcpy(t->tipo, m->tipo);
        strcpy(t->fecha, m->fecha);
        strcpy(t->estado, m->estado);
        t->costo = m->costo;
        strcpy(t->descripcion, m->descripcion);
        cout << "\nTramite restaurado en la lista:\n";
        mostrarTramite(t);
    }
    cima = cima->siguiente;
    delete m;
    cout << "Modificacion deshecha y retirada del historial.\n";
}

void mostrarHistorial() {
    cout << "\n=== HISTORIAL DE MODIFICACIONES ===\n";
    if (pilaVacia()) { cout << "El historial esta vacio.\n"; return; }
    int pos = 1;
    for (Modificacion* aux = cima; aux != NULL; aux = aux->siguiente, pos++) {
        cout << "  [" << pos << "]" << (pos == 1 ? " (MAS RECIENTE)" : "") << endl;
        mostrarModificacion(aux);
    }
}

void consultarUltimaModificacion() {
    cout << "\n=== ULTIMA MODIFICACION ===\n";
    if (pilaVacia()) { cout << "El historial esta vacio.\n"; return; }
    mostrarModificacion(cima);
}

void buscarModificacion() {
    cout << "\n=== BUSCAR MODIFICACION ===\n";
    if (pilaVacia()) { cout << "El historial esta vacio.\n"; return; }
    char codigo[15];
    leerTexto("Codigo del tramite: ", codigo, 15);
    int n = 0;
    for (Modificacion* aux = cima; aux != NULL; aux = aux->siguiente)
        if (strcmp(aux->codigoTramite, codigo) == 0) { mostrarModificacion(aux); n++; }
    if (n == 0) cout << "No hay modificaciones para ese tramite.\n";
    else cout << "\nModificaciones encontradas: " << n << endl;
}

int contarModificaciones() {
    int n = 0;
    for (Modificacion* aux = cima; aux != NULL; aux = aux->siguiente) n++;
    return n;
}

void verificarHistorial() {
    if (pilaVacia()) cout << "No existen modificaciones disponibles.\n";
    else cout << "El historial contiene modificaciones registradas.\n";
}

void purgarHistorial(const char* codigo) {
    Modificacion* actual = cima;
    Modificacion* anterior = NULL;
    while (actual != NULL) {
        if (strcmp(actual->codigoTramite, codigo) == 0) {
            Modificacion* borrar = actual;
            actual = actual->siguiente;
            if (anterior == NULL) cima = actual;
            else anterior->siguiente = actual;
            delete borrar;
        } else {
            anterior = actual;
            actual = actual->siguiente;
        }
    }
}

bool colaVacia() { return frente == NULL; }

void mostrarTurno(Turno* t) {
    cout << "  Turno No. " << t->numero << " | DNI: " << t->dni
         << " | Tramite: " << t->codigoTramite << endl;
}

bool tieneTurnosPendientes(const char* codigo) {
    for (Turno* aux = frente; aux != NULL; aux = aux->siguiente)
        if (strcmp(aux->codigoTramite, codigo) == 0)
    return false;
}

void registrarTurno() {
    cout << "\n=== REGISTRAR TURNO ===\n";
    char codigo[15], dni[9];
    leerTexto("Codigo del tramite: ", codigo, 15);
    Tramite* t = buscarPorCodigo(codigo);
    if (t == NULL) { cout << "Error: el tramite no esta registrado.\n"; return; }
    leerDni("DNI del cliente (8 digitos): ", dni);
    if (strcmp(t->dni, dni) != 0) {
        cout << "Error: el DNI no coincide con el cliente del tramite.\n";
        return;
    }
    if (strcmp(t->estado, "Finalizado") == 0) {
        cout << "Error: el tramite ya esta finalizado y no requiere turno.\n";
        return;
    }
    if (tieneTurnosPendientes(codigo)) {
        cout << "Error: ese tramite ya tiene un turno pendiente.\n";
        return;
    }

    Turno* nuevo = new Turno;
    ultimoNumero++;
    nuevo->numero = ultimoNumero;
    strcpy(nuevo->dni, dni);
    strcpy(nuevo->codigoTramite, codigo);
    nuevo->siguiente = NULL;
    if (colaVacia()) { frente = nuevo; fin = nuevo; }
    else { fin->siguiente = nuevo; fin = nuevo; }
    cout << "Turno registrado correctamente. Numero asignado: " << nuevo->numero << endl;
}

void atenderSiguienteTurno() {
    cout << "\n=== ATENDER SIGUIENTE TURNO ===\n";
    if (colaVacia()) { cout << "No hay turnos pendientes.\n"; return; }
    Turno* atendido = frente;
    cout << "Atendiendo a:\n";
    mostrarTurno(atendido);
    Tramite* t = buscarPorCodigo(atendido->codigoTramite);
    if (t != NULL) mostrarTramite(t);
    frente = frente->siguiente;
    if (frente == NULL) fin = NULL;
    delete atendido;
    cout << "Turno atendido y retirado de la cola.\n";
}

void mostrarTurnos() {
    cout << "\n=== TURNOS PENDIENTES ===\n";
    if (colaVacia()) { cout << "No hay turnos pendientes.\n"; return; }
    for (Turno* aux = frente; aux != NULL; aux = aux->siguiente) mostrarTurno(aux);
}

void consultarSiguienteTurno() {
    cout << "\n=== SIGUIENTE TURNO ===\n";
    if (colaVacia()) { cout << "No hay turnos pendientes.\n"; return; }
    mostrarTurno(frente);
}

void buscarTurno() {
    cout << "\n=== BUSCAR TURNO ===\n";
    if (colaVacia()) { cout << "No hay turnos pendientes.\n"; return; }
    cout << "1. Por numero de turno\n2. Por DNI\n";
    int op = leerOpcion("Opcion: ");
    bool encontrado = false;
    if (op == 1) {
        int n = leerOpcion("Numero de turno: ");
        for (Turno* aux = frente; aux != NULL; aux = aux->siguiente)
            if (aux->numero == n) { mostrarTurno(aux); encontrado = true; }
    } else if (op == 2) {
        char d[9];
        leerDni("DNI: ", d);
        for (Turno* aux = frente; aux != NULL; aux = aux->siguiente)
            if (strcmp(aux->dni, d) == 0) { mostrarTurno(aux); encontrado = true; }
    } else {
        cout << "Opcion inexistente.\n";
        return;
    }
    if (!encontrado) cout << "El turno no existe.\n";
}

void cancelarTurno() {
    cout << "\n=== CANCELAR TURNO ===\n";
    if (colaVacia()) { cout << "No hay turnos pendientes.\n"; return; }
    int n = leerOpcion("Numero de turno a cancelar: ");
    Turno* actual = frente;
    Turno* anterior = NULL;
    while (actual != NULL && actual->numero != n) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual == NULL) { cout << "El turno no existe.\n"; return; }
    if (anterior == NULL) frente = actual->siguiente;
    else anterior->siguiente = actual->siguiente;
    if (actual == fin) fin = anterior;
    delete actual;
    cout << "Turno cancelado correctamente.\n";
}

int contarTurnos() {
    int n = 0;
    for (Turno* aux = frente; aux != NULL; aux = aux->siguiente) n++;
    return n;
}

void verificarTurnos() {
    if (colaVacia()) cout << "No existen clientes pendientes de atencion.\n";
    else cout << "Hay clientes esperando atencion.\n";
}

void visualizarOrden() {
    cout << "\n=== ORDEN DE ATENCION ===\n";
    if (colaVacia()) { cout << "No hay turnos pendientes.\n"; return; }
    int pos = 1;
    for (Turno* aux = frente; aux != NULL; aux = aux->siguiente, pos++) {
        cout << "  Posicion " << pos << " -> Turno No. " << aux->numero
             << " | DNI: " << aux->dni << " | Tramite: " << aux->codigoTramite;
        if (pos == 1) cout << "  (SIGUIENTE)";
        cout << endl;
    }
}

void insertarTramite() {
    Tramite* nuevo = new Tramite;

    cout << "\n--- REGISTRAR TRAMITE ---\n";
    leerTexto("Codigo del tramite: ", nuevo->codigo, 15);
    if (buscarPorCodigo(nuevo->codigo) != NULL) {
        cout << "El codigo ya existe.\n";
        delete nuevo;
        return;
    }

    leerDni("DNI del cliente: ", nuevo->dni);
    leerTextoLetras("Nombre del cliente: ", nuevo->nombre, 50);
    leerTextoLetras("Tipo de tramite: ", nuevo->tipo, 40);
    leerFecha("Fecha de registro (DD/MM/AAAA): ", nuevo->fecha);
    strcpy(nuevo->estado, "Pendiente");
    nuevo->costo = leerCosto("Costo del tramite: S/ ");
    leerTexto("Descripcion del tramite: ", nuevo->descripcion, 100);

    nuevo->siguiente = inicioLista;
    inicioLista = nuevo;

    cout << "Tramite registrado.\n";
}

void mostrarTramites() {
    cout << "\n=== LISTA DE TRAMITES ===\n";
    if (inicioLista == NULL) { cout << "La lista esta vacia.\n"; return; }
    for (Tramite* aux = inicioLista; aux != NULL; aux = aux->siguiente) mostrarTramite(aux);
}

void buscarTramite() {
    cout << "\n=== BUSCAR TRAMITE ===\n";
    if (inicioLista == NULL) { cout << "La lista esta vacia.\n"; return; }
    cout << "1. Por codigo\n2. Por DNI\n3. Por nombre del cliente\n";
    int op = leerOpcion("Opcion: ");
    bool encontrado = false;
    if (op == 1) {
        char c[15];
        leerTexto("Codigo: ", c, 15);
        Tramite* t = buscarPorCodigo(c);
        if (t != NULL) { mostrarTramite(t); encontrado = true; }
    } else if (op == 2) {
        char d[9];
        leerDni("DNI: ", d);
        for (Tramite* aux = inicioLista; aux != NULL; aux = aux->siguiente)
            if (strcmp(aux->dni, d) == 0) { mostrarTramite(aux); encontrado = true; }
    } else if (op == 3) {
        char n[50];
        leerTexto("Nombre (o parte del nombre): ", n, 50);
        for (Tramite* aux = inicioLista; aux != NULL; aux = aux->siguiente)
            if (strstr(aux->nombre, n) != NULL) { mostrarTramite(aux); encontrado = true; }
    } else {
        cout << "Opcion inexistente.\n";
        return;
    }
    if (!encontrado) cout << "No se encontro ningun tramite.\n";
}

void modificarTramite() {
    cout << "\n=== MODIFICAR TRAMITE ===\n";
    if (inicioLista == NULL) { cout << "La lista esta vacia.\n"; return; }
    char codigo[15];
    leerTexto("Codigo del tramite a modificar: ", codigo, 15);
    Tramite* t = buscarPorCodigo(codigo);
    if (t == NULL) { cout << "Tramite no encontrado.\n"; return; }
    mostrarTramite(t);
    cout << "\n1. Estado\n2. Costo\n3. Tipo de tramite\n4. Descripcion\n5. Cancelar\n";
    int op = leerOpcion("Que desea modificar: ");
    switch (op) {
        case 1:
            apilarModificacion(t, "Cambio de estado");
            leerEstado(t->estado);
            break;
        case 2:
            apilarModificacion(t, "Cambio de costo");
            t->costo = leerCosto("Nuevo costo: S/ ");
            break;
        case 3:
            apilarModificacion(t, "Cambio de tipo de tramite");
            leerTexto("Nuevo tipo: ", t->tipo, 40);
            break;
        case 4:
            apilarModificacion(t, "Cambio de descripcion");
            leerTexto("Nueva descripcion: ", t->descripcion, 100);
            break;
        case 5: cout << "Modificacion cancelada.\n"; return;
        default: cout << "Opcion inexistente.\n"; return;
    }
    cout << "Tramite modificado. Los datos anteriores se guardaron en el historial.\n";
}

void eliminarTramite() {
    cout << "\n=== ELIMINAR TRAMITE ===\n";
    if (inicioLista == NULL) { cout << "La lista esta vacia.\n"; return; }
    char codigo[15];
    leerTexto("Codigo del tramite a eliminar: ", codigo, 15);

    Tramite* actual = inicioLista;
    Tramite* anterior = NULL;
    while (actual != NULL && strcmp(actual->codigo, codigo) != 0) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual == NULL) { cout << "Tramite no encontrado.\n"; return; }

    if (strcmp(actual->estado, "En proceso") == 0) {
        cout << "No se puede eliminar: el tramite esta EN PROCESO.\n";
        return;
    }
    if (tieneTurnosPendientes(codigo)) {
        cout << "No se puede eliminar: tiene un turno pendiente. Cancele el turno primero.\n";
        return;
    }
    mostrarTramite(actual);
    char conf[10];
    leerTexto("Confirmar eliminacion (S/N): ", conf, 10);
    if (strcmp(conf, "S") != 0 && strcmp(conf, "s") != 0) {
        cout << "Operacion cancelada.\n";
        return;
    }
    if (anterior == NULL) inicioLista = actual->siguiente;
    else anterior->siguiente = actual->siguiente;
    delete actual;
    purgarHistorial(codigo);       
    cout << "Tramite eliminado correctamente.\n";
}

void filtrarTramites() {
    cout << "\n=== FILTRAR TRAMITES ===\n";
    if (inicioLista == NULL) { cout << "La lista esta vacia.\n"; return; }
    cout << "1. Por estado\n2. Por tipo de tramite\n3. Por fecha de registro\n";
    int op = leerOpcion("Opcion: ");
    char criterio[50];
    if (op == 1) leerEstado(criterio);
    else if (op == 2) leerTexto("Tipo de tramite (o parte): ", criterio, 40);
    else if (op == 3) leerFecha("Fecha de registro (dd/mm/aaaa): ", criterio);
    else { cout << "Opcion inexistente.\n"; return; }

    int n = 0;
    for (Tramite* aux = inicioLista; aux != NULL; aux = aux->siguiente) {
        bool coincide;
        if (op == 1) coincide = (strcmp(aux->estado, criterio) == 0);
        else if (op == 2) coincide = (strstr(aux->tipo, criterio) != NULL);
        else coincide = (strcmp(aux->fecha, criterio) == 0);
        if (coincide) { mostrarTramite(aux); n++; }
    }
    if (n == 0) cout << "No hay tramites que cumplan el criterio.\n";
    else cout << "\nTramites encontrados: " << n << endl;
}

void guardarTodo() {
    ofstream f1(ARCH_TRAMITES);
    if (f1) {
        for (Tramite* t = inicioLista; t != NULL; t = t->siguiente)
            f1 << t->codigo << '|' << t->dni << '|' << t->nombre << '|' << t->tipo << '|'
               << t->fecha << '|' << t->estado << '|' << t->costo << '|' << t->descripcion << '\n';
        f1.close();
    }
    ofstream f2(ARCH_TURNOS);
    if (f2) {
        f2 << ultimoNumero << '\n';
        for (Turno* t = frente; t != NULL; t = t->siguiente)
            f2 << t->numero << '|' << t->dni << '|' << t->codigoTramite << '\n';
        f2.close();
    }
    ofstream f3(ARCH_HISTORIAL);
    if (f3) {
        for (Modificacion* m = cima; m != NULL; m = m->siguiente)
            f3 << m->codigoTramite << '|' << m->dni << '|' << m->nombre << '|' << m->tipo << '|'
               << m->fecha << '|' << m->estado << '|' << m->costo << '|' << m->descripcion << '|'
               << m->cambio << '\n';
        f3.close();
    }
    cout << "Informacion guardada (tramites, turnos e historial).\n";
}

void liberarTodo() {
    while (inicioLista != NULL) {
        Tramite* t = inicioLista;
        inicioLista = inicioLista->siguiente;
        delete t;
    }
    while (frente != NULL) {
        Turno* t = frente;
        frente = frente->siguiente;
        delete t;
    }
    fin = NULL;
    while (cima != NULL) {
        Modificacion* m = cima;
        cima = cima->siguiente;
        delete m;
    }
}

void cargarTodo() {
    char costo[20];

    ifstream f1(ARCH_TRAMITES);
    if (f1) {
        Tramite* ultimo = NULL;
        while (true) {
            Tramite* t = new Tramite;
            if (!f1.getline(t->codigo, 15, '|')) { delete t; break; }
            f1.getline(t->dni, 9, '|');
            f1.getline(t->nombre, 50, '|');
            f1.getline(t->tipo, 40, '|');
            f1.getline(t->fecha, 11, '|');
            f1.getline(t->estado, 15, '|');
            f1.getline(costo, 20, '|');
            f1.getline(t->descripcion, 100);
            t->costo = atof(costo);
            t->siguiente = NULL;
            if (inicioLista == NULL) inicioLista = t; else ultimo->siguiente = t;
            ultimo = t;
        }
        f1.close();
    }

    ifstream f2(ARCH_TURNOS);
    if (f2) {
        f2 >> ultimoNumero;
        f2.ignore(1000, '\n');
        char num[10];
        while (f2.getline(num, 10, '|')) {
            Turno* t = new Turno;
            t->numero = atoi(num);
            f2.getline(t->dni, 9, '|');
            f2.getline(t->codigoTramite, 15);
            t->siguiente = NULL;
            if (frente == NULL) frente = t; else fin->siguiente = t;
            fin = t;
        }
        f2.close();
    }

    ifstream f3(ARCH_HISTORIAL);
    if (f3) {
        Modificacion* ultimo = NULL;
        while (true) {
            Modificacion* m = new Modificacion;
            if (!f3.getline(m->codigoTramite, 15, '|')) { delete m; break; }
            f3.getline(m->dni, 9, '|');
            f3.getline(m->nombre, 50, '|');
            f3.getline(m->tipo, 40, '|');
            f3.getline(m->fecha, 11, '|');
            f3.getline(m->estado, 15, '|');
            f3.getline(costo, 20, '|');
            f3.getline(m->descripcion, 100, '|');
            f3.getline(m->cambio, 100);
            m->costo = atof(costo);
            m->siguiente = NULL;
            if (cima == NULL) cima = m; else ultimo->siguiente = m;
            ultimo = m;
        }
        f3.close();
    }
    cout << "Datos cargados: " << contarTramites() << " tramite(s), "
         << contarTurnos() << " turno(s), " << contarModificaciones()
         << " modificacion(es).\n";
}

void menuTramites() {
    int op;
    do {
        cout << "\n--- TRAMITES (LISTA ENLAZADA) ---\n";
        cout << "1. Registrar tramite\n2. Mostrar tramites\n3. Buscar tramite\n";
        cout << "4. Modificar tramite\n5. Eliminar tramite\n";
        cout << "6. Filtrar tramites (estado, tipo o fecha)\n7. Contar tramites\n8. Volver\n";
        op = leerOpcion("Seleccione una opcion: ");
        switch (op) {
            case 1: insertarTramite(); break;
            case 2: mostrarTramites(); break;
            case 3: buscarTramite(); break;
            case 4: modificarTramite(); break;
            case 5: eliminarTramite(); break;
            case 6: filtrarTramites(); break;
            case 7: cout << "\nTotal de tramites: " << contarTramites() << endl; break;
            case 8: break;
            default: cout << "Opcion inexistente.\n";
        }
    } while (op != 8);
}

void menuTurnos() {
    int op;
    do {
        cout << "\n--- TURNOS DE ATENCION (COLA) ---\n";
        cout << "1. Registrar turno\n2. Atender siguiente turno\n3. Mostrar turnos pendientes\n";
        cout << "4. Consultar siguiente turno\n5. Buscar turno\n6. Cancelar turno\n";
        cout << "7. Contar turnos pendientes\n8. Verificar turnos pendientes\n";
        cout << "9. Visualizar orden de atencion\n10. Volver\n";
        op = leerOpcion("Seleccione una opcion: ");
        switch (op) {
            case 1: registrarTurno(); break;
            case 2: atenderSiguienteTurno(); break;
            case 3: mostrarTurnos(); break;
            case 4: consultarSiguienteTurno(); break;
            case 5: buscarTurno(); break;
            case 6: cancelarTurno(); break;
            case 7: cout << "\nTurnos pendientes: " << contarTurnos() << endl; break;
            case 8: verificarTurnos(); break;
            case 9: visualizarOrden(); break;
            case 10: break;
            default: cout << "Opcion inexistente.\n";
        }
    } while (op != 10);
}

void menuHistorial() {
    int op;
    do {
        cout << "\n--- HISTORIAL DE MODIFICACIONES (PILA) ---\n";
        cout << "(Las modificaciones se registran solas al modificar un tramite)\n";
        cout << "1. Deshacer ultima modificacion\n2. Eliminar ultima modificacion\n";
        cout << "3. Mostrar historial\n4. Consultar ultima modificacion\n";
        cout << "5. Buscar modificacion por tramite\n6. Contar modificaciones\n";
        cout << "7. Verificar historial vacio\n8. Volver\n";
        op = leerOpcion("Seleccione una opcion: ");
        switch (op) {
            case 1: deshacerUltimaModificacion(); break;
            case 2: eliminarUltimaModificacion(); break;
            case 3: mostrarHistorial(); break;
            case 4: consultarUltimaModificacion(); break;
            case 5: buscarModificacion(); break;
            case 6: cout << "\nModificaciones almacenadas: " << contarModificaciones() << endl; break;
            case 7: verificarHistorial(); break;
            case 8: break;
            default: cout << "Opcion inexistente.\n";
        }
    } while (op != 8);
}

int main() {
    cargarTodo();
    int op;
    do {
        cout << "\n==========================================\n";
        cout << "        SISTEMA DE GESTION NOTARIAL\n";
        cout << "==========================================\n";
        cout << "1. Tramites notariales (Lista)\n";
        cout << "2. Turnos de atencion (Cola)\n";
        cout << "3. Historial de modificaciones (Pila)\n";
        cout << "4. Guardar informacion\n";
        cout << "5. Salir\n";
        op = leerOpcion("Seleccione una opcion: ");
        switch (op) {
            case 1: menuTramites(); break;
            case 2: menuTurnos(); break;
            case 3: menuHistorial(); break;
            case 4: guardarTodo(); break;
            case 5: guardarTodo(); liberarTodo(); cout << "Saliendo del sistema...\n"; break;
            default: cout << "Opcion inexistente.\n";
        }
    } while (op != 5);
    return 0;
}
