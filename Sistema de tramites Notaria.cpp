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
            cout << "Error: no ingrese numeros en este campo.\n";
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

void leerFecha(const char* msg, char* destino) {      // dd/mm/aaaa
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

    nuevo->costo = leerCosto("Costo del tramite: S/ ");

    leerTexto("Descripcion del tramite: ", nuevo->descripcion, 100);

    nuevo->siguiente = inicioLista;
    inicioLista = nuevo;

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

//Menus
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

// ======================= PROGRAMA PRINCIPAL =======================
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







