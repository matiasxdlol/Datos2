#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <chrono>
#include <random>
#include <cmath>

using namespace std;
using namespace std::chrono;

// Estructura para almacenar los datos de cada línea del archivo CSV
struct User_data {
    string university;
    string user_id;
    string user_name;
    int number_tweets;
    int friends_count;
    int followers_count;
    string created_at;
};

// Función de hashing para convertir una cadena en un índice de la tabla hash
// Función de hashing mejorada utilizando el método de multiplicación
size_t funcion_hash(const string& key, size_t tabla_size) {
    const double A = 0.6180339887; // Constante áurea
    size_t hash = 0;
    for (char c : key) {
        hash += c;
    }
    double hash_double = tabla_size * (hash * A - floor(hash * A));
    return static_cast<size_t>(hash_double);
}

// Clase para la tabla hash con hashing abierto
class tabla_hash {
private:
    size_t tabla_size;
    vector<list<User_data>> tabla;

public:
    tabla_hash(size_t size) : tabla_size(size), tabla(size) {}

    // Método para insertar datos en la tabla hash
    void insert(const User_data& data) {
        size_t index = funcion_hash(data.university, tabla_size);
        tabla[index].push_back(data);
    }

    // Método para buscar datos en la tabla hash
    vector<User_data> search(const string& university) {
        size_t index = funcion_hash(university, tabla_size);
        vector<User_data> results;
        for (const auto& user : tabla[index]) {
            if (user.university == university) {
                results.push_back(user);
            }
        }
        return results;
    }
};

// Función para leer datos del archivo CSV y llenar el vector de usuarios
void read_CSV(const string& filename, vector<User_data>& users) {
    ifstream file(filename);
    string line;

    // Ignorar la primera línea (encabezado)
    getline(file, line);

    // Leer cada línea del archivo CSV y almacenar los datos en el vector de usuarios
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        User_data data;

        getline(ss, token, ',');
        data.university = token;

        getline(ss, token, ',');
        data.user_id = token;

        getline(ss, token, ',');
        data.user_name = token;

        getline(ss, token, ',');
        data.number_tweets = stoi(token);

        getline(ss, token, ',');
        data.friends_count = stoi(token);

        getline(ss, token, ',');
        data.followers_count = stoi(token);

        getline(ss, token, ',');
        data.created_at = token;

        users.push_back(data);
    }
}

int main() {
    // Leer el archivo CSV y llenar el vector de usuarios
    vector<User_data> users;
    read_CSV("universities_followers.csv", users);

    // Crear la tabla hash con un tamaño adecuado
    size_t table_size = users.size() * 2; // Doble del número de usuarios para disminuir colisiones
    tabla_hash hash_table(table_size);

    // Insertar los usuarios en la tabla hash
    for (const auto& user : users) {
        hash_table.insert(user);
    }

    // Abrir el archivo para escribir los tiempos de búsqueda
    ofstream outfile("tiempos_busqueda.csv");
    outfile << "Usuario, Tiempo (ms)" << endl;

    // Realizar búsquedas aleatorias en el vector de usuarios
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, users.size() - 1);

    for (int i = 0; i < 5000; ++i) {
        // Seleccionar un usuario aleatorio
        int index = dist(gen);
        const User_data& user = users[index];

        // Realizar la búsqueda y medir el tiempo
        auto start = high_resolution_clock::now();
        vector<User_data> results = hash_table.search(user.university);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        // Escribir el tiempo en el archivo
        outfile << user.user_name << ", " << duration.count() << endl;
    }

    // Cerrar el archivo
    outfile.close();

    cout << "Tiempos de búsqueda guardados en 'tiempos_busqueda.csv'." << endl;

    return 0;
}

