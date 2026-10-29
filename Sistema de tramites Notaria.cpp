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
