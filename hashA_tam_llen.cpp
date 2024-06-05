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
void read_CSV(const string& filename, vector<User_data>& users, int num_lines) {
    ifstream file(filename);
    string line;

    // Ignorar la primera línea (encabezado)
    getline(file, line);

    // Leer cada línea del archivo CSV y almacenar los datos en el vector de usuarios
    for (int i = 0; i < num_lines; ++i) {
        if (!getline(file, line))
            break;

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
    vector<int> iteraciones = {1000, 5000, 10000, 21070}; // 21070 para leer todo el archivo

    ofstream time_file("tiempos_llenado_tabla_hash.csv");
    time_file << "Numero de Lineas,Tiempo (microsegundos)" << endl;

    ofstream size_file("tamaño_tabla_hash.csv");
    size_file << "Numero de Lineas,Tamaño (MB)" << endl;

    for (int i = 0; i < iteraciones.size(); ++i) {
        int iteracion = iteraciones[i];

        // Crear la tabla hash con un tamaño adecuado
        size_t table_size = iteracion * 2; // Doble del número de usuarios para disminuir colisiones
        tabla_hash hash_table(table_size);

        // Leer las iteraciones especificadas
        vector<User_data> users;
        read_CSV("universities_followers.csv", users, iteracion);

        // Insertar los usuarios en la tabla hash y medir el tiempo
        auto start = high_resolution_clock::now();
        for (const auto& user : users) {
            hash_table.insert(user);
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        // Guardar el tiempo y el tamaño en archivos CSV
        time_file << iteracion << "," << duration.count() << endl;
        size_file << iteracion << "," << hash_table.tamaño_MB() << endl;

        // Limpiar el vector de usuarios para la siguiente lectura
        users.clear();
    }

    time_file.close();
    size_file.close();

    return 0;
}

