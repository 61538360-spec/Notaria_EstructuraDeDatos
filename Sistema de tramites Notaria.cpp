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

//Lista

void insertarTramite() {
    Tramite* nuevo = new Tramite;

    cout << "\n--- REGISTRAR TRAMITE ---\n";

    leerTexto("Codigo del tramite: ", nuevo->codigo, 15);

    if (buscarPorCodigo(nuevo->codigo) != NULL) {
        cout << "Error: el codigo ya existe.\n";
        delete nuevo;
        return;
    }

    leerDni("DNI del cliente: ", nuevo->dni);
    leerTextoLetras("Nombre del cliente: ", nuevo->nombre, 50);
    leerTextoLetras("Tipo de tramite: ", nuevo->tipo, 40);
    leerFecha("Fecha de registro (DD/MM/AAAA): ", nuevo->fecha);

    strcpy(nuevo->estado, "Pendiente");

    leerCosto("Costo del tramite: ", nuevo->costo);

    leerTexto("Descripcion del tramite: ", nuevo->descripcion, 100);

    nuevo->siguiente = inicioLista;
    inicioLista = nuevo;

    guardarTramites();

    cout << "Tramite registrado correctamente.\n";
}

void eliminarTramite() {
    char codigo[15];

    cout << "\n--- ELIMINAR TRAMITE ---\n";
    leerTexto("Ingrese el codigo del tramite: ", codigo, 15);

    Tramite* actual = inicioLista;
    Tramite* anterior = NULL;

    while (actual != NULL &&
           strcmp(actual->codigo, codigo) != 0) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == NULL) {
        cout << "Error: no se encontro el tramite.\n";
        return;
    }

    if (strcmp(actual->estado, "En proceso") == 0) {
        cout << "No se puede eliminar un tramite en proceso.\n";
        return;
    }

    Turno* turno = frente;

    while (turno != NULL) {
        if (strcmp(turno->codigoTramite, codigo) == 0) {
            cout << "No se puede eliminar: el tramite tiene un turno pendiente.\n";
            return;
        }

        turno = turno->siguiente;
    }

    char respuesta;
    cout << "¿Desea eliminar el tramite? (S/N): ";
    cin >> respuesta;
    cin.ignore(1000, '\n');

    if (respuesta != 'S' && respuesta != 's') {
        cout << "Eliminacion cancelada.\n";
        return;
    }

    if (anterior == NULL) {
        inicioLista = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    delete actual;

    purgarHistorial(codigo);
    guardarTramites();

    cout << "Tramite eliminado correctamente.\n";
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

//Pilas

void apilarModificacion(Tramite* t, const char* cambio) {
    if (t == NULL) {
        return;
    }

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

    cout << "Modificacion registrada en la pila correctamente.\n";
}

void eliminarUltimaModificacion() {
    if (cima == NULL) {
        cout << "La pila de modificaciones esta vacia.\n";
        return;
    }

    Modificacion* temp = cima;

    cima = cima->siguiente;

    delete temp;

    cout << "Ultima modificacion eliminada correctamente.\n";
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
 

void consultarUltimaModificacion() {
    cout << "\n=== ULTIMA MODIFICACION ===\n";
    if (pilaVacia()) { cout << "El historial esta vacio.\n"; return; }
    mostrarModificacion(cima);
}

//Colas

void registrarTurno() {
    char codigo[15];
    char dni[9];

    cout << "\n--- REGISTRAR TURNO ---\n";

    leerTexto("Codigo del tramite: ", codigo, 15);

    Tramite* t = buscarPorCodigo(codigo);

    if (t == NULL) {
        cout << "Error: no se encontro el tramite.\n";
        return;
    }

    leerDni("DNI del cliente: ", dni);

    if (strcmp(t->dni, dni) != 0) {
        cout << "Error: el DNI no corresponde al tramite.\n";
        return;
    }

    if (strcmp(t->estado, "Finalizado") == 0) {
        cout << "No se puede registrar turno para un tramite finalizado.\n";
        return;
    }

    Turno* aux = frente;

    while (aux != NULL) {
        if (strcmp(aux->codigoTramite, codigo) == 0) {
            cout << "Este tramite ya tiene un turno pendiente.\n";
            return;
        }

        aux = aux->siguiente;
    }

    Turno* nuevo = new Turno;

    ultimoNumero++;
    nuevo->numero = ultimoNumero;

    strcpy(nuevo->dni, dni);
    strcpy(nuevo->codigoTramite, codigo);
    nuevo->siguiente = NULL;

    if (frente == NULL) {
        frente = nuevo;
        fin = nuevo;
    } else {
        fin->siguiente = nuevo;
        fin = nuevo;
    }

    guardarTurnos();

    cout << "Turno registrado correctamente.\n";
    cout << "Numero de turno: " << nuevo->numero << "\n";
}

void atenderSiguienteTurno() {
    if (frente == NULL) {
        cout << "No hay turnos pendientes.\n";
        return;
    }

    Turno* temp = frente;

    cout << "\n--- ATENDER TURNO ---\n";
    cout << "Numero de turno: " << temp->numero << "\n";
    cout << "DNI del cliente: " << temp->dni << "\n";
    cout << "Codigo del tramite: " << temp->codigoTramite << "\n";

    Tramite* t = buscarPorCodigo(temp->codigoTramite);

    if (t != NULL) {
        cout << "Nombre del cliente: " << t->nombre << "\n";
        cout << "Tipo de tramite: " << t->tipo << "\n";
    }

    frente = frente->siguiente;

    if (frente == NULL) {
        fin = NULL;
    }

    delete temp;

    guardarTurnos();

    cout << "Turno atendido y eliminado de la cola.\n";
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
