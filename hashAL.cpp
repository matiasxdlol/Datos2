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
        return vector<User_data>(tabla[index].begin(), tabla[index].end());
    }

    // Método para obtener el tamaño de la tabla hash en MB
    double tamaño_MB() const {
        double size_element = sizeof(list<User_data>) + sizeof(User_data); // Suponiendo un tamaño fijo por elemento
        double total_size = size_element * tabla_size;
        return total_size / (1024 * 1024); // Convertir a MB
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

    // Crear el archivo para guardar los tiempos de llenado de la tabla hash
    ofstream fill_times_file("tiempos_llenado_tabla_hash.csv");
    fill_times_file << "Ciclo, Tiempo (microsegundos)" << endl;

    // Realizar 500 ciclos para medir el tiempo de llenado de la tabla hash
    for (int ciclo = 0; ciclo < 500; ++ciclo) {
        // Crear la tabla hash con un tamaño adecuado
        size_t table_size = users.size() * 2; // Doble del número de usuarios para disminuir colisiones
        tabla_hash hash_table(table_size);

        // Medir el tiempo de llenado de la tabla hash
        auto start = high_resolution_clock::now();
        for (const auto& user : users) {
            hash_table.insert(user);
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        // Guardar el tiempo en el archivo
        fill_times_file << ciclo + 1 << ", " << duration.count() << endl;
    }

    // Cerrar el archivo de tiempos de llenado de la tabla hash
    fill_times_file.close();

    cout << "Tiempos de llenado de la tabla hash guardados en 'tiempos_llenado_tabla_hash.csv'." << endl;

    return 0;
}
