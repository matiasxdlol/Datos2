#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Estructura para almacenar los datos de cada usuario
struct User_data {
    string university;
    string user_Id;
    string user_name;
    int number_tweets;
    int friends_count;
    int followers_count;
    string created_at;
};

// Función hash simple
size_t funcion_hash(const string& key, size_t tabla_size) {
    size_t hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % tabla_size;
    }
    return hash;
}

// Función hash secundaria
size_t funcion_hash2(const string& key, size_t tabla_size) {
    size_t hash = 0;
    for (char c : key) {
        hash = (hash * 33 + c) % tabla_size;
    }
    return hash;
}

class tabla_hash {
private:
    vector<User_data> tabla;
    vector<bool> ocupado;
    size_t tabla_size;
    size_t num_elements;

public:
    tabla_hash(size_t size) : tabla_size(size), num_elements(0) {
        tabla.resize(tabla_size);
        ocupado.resize(tabla_size, false);
    }

    // Double hashing insertion
    void insert(const User_data& data) {
        size_t index1 = funcion_hash(data.user_Id, tabla_size);
        size_t index2 = funcion_hash2(data.user_Id, tabla_size);
        size_t index = index1;
        size_t i = 1;
        while (ocupado[index]) {
            index = (index1 + i * index2) % tabla_size;
            i++;
        }
        tabla[index] = data;
        ocupado[index] = true;
        num_elements++;
    }

    // Double hashing search
    bool search(const string& user_Id, User_data& result) {
        size_t index1 = funcion_hash(user_Id, tabla_size);
        size_t index2 = funcion_hash2(user_Id, tabla_size);
        size_t index = index1;
        size_t i = 1;
        while (ocupado[index]) {
            if (tabla[index].user_Id == user_Id) {
                result = tabla[index];
                return true;
            }
            index = (index1 + i * index2) % tabla_size;
            i++;
        }
        return false;
    }

    // Calcular el tamaño de la tabla en MB
    double size_MB() const {
        return (tabla_size * sizeof(User_data) + tabla_size * sizeof(bool)) / (1024.0 * 1024.0);
    }
};

// Función para dividir una cadena en tokens utilizando un delimitador
vector<string> split(const string &s, char delimit) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimit)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Función para convertir una cadena en un número
double convert_numero(const string& str) {
    stringstream ss(str);
    double number;
    ss >> number;
    return number;
}

// Función para guardar los datos en un archivo CSV
void save_CSV(const string& filename, const vector<vector<string>>& data) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& row : data) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i != row.size() - 1) file << ",";
            }
            file << endl;
        }
        file.close();
        cout << "Los datos se han guardado exitosamente en el archivo: " << filename << endl;
    } else {
        cout << "No se pudo abrir el archivo " << filename << " para escribir." << endl;
    }
}

int main() {
    string filename = "universities_followers.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "No se pudo abrir el archivo " << filename << endl;
        return 1;
    }

    vector<User_data> usuarios;
    string line;
    while (getline(file, line)) {
        vector<string> tokens = split(line, ',');
        if (tokens.size() != 7) {
            cout << "Error: línea inválida en el archivo CSV" << endl;
            continue;
        }
        User_data user;
        user.university = tokens[0];
        user.user_Id = tokens[1];
        user.user_name = tokens[2];
        user.number_tweets = convert_numero(tokens[3]);
        user.friends_count = convert_numero(tokens[4]);
        user.followers_count = convert_numero(tokens[5]);
        user.created_at = tokens[6];
        usuarios.push_back(user);
    }
    file.close();

    // Vectores para almacenar los tiempos de inserción y tamaños de tabla
    vector<long long> insertion_times;
    vector<double> table_sizes;

    // Insertar para 1000, 5000, 10000 y total
    vector<int> iteraciones = {1000, 5000, 10000, static_cast<int>(usuarios.size())};

    for (int iteracion : iteraciones) {
        size_t table_size = iteracion * 2;
        tabla_hash hash_table(table_size);

        auto start_insertion = high_resolution_clock::now();
        // Insertar usuarios en la tabla hash
        for (int i = 0; i < iteracion; ++i) {
            hash_table.insert(usuarios[i]);
        }
        auto stop_insertion = high_resolution_clock::now();
        auto insertion_duration = duration_cast<milliseconds>(stop_insertion - start_insertion);

        // Calcular el tamaño de la tabla
        double size_mb = hash_table.size_MB();
        table_sizes.push_back(size_mb);

        // Guardar el tiempo de inserción en el vector
        insertion_times.push_back(insertion_duration.count());
    }

    // Guardar los tiempos de inserción en un archivo CSV
    vector<vector<string>> insertion_data;
    insertion_data.push_back({"Iteración", "Tiempo de inserción (ms)"});
    for (size_t i = 0; i < iteraciones.size(); ++i) {
        insertion_data.push_back({to_string(iteraciones[i]), to_string(insertion_times[i])});
    }
    save_CSV("tiempos_insercion_double_hashing.csv", insertion_data);

    // Guardar los tamaños de la tabla en un archivo CSV
    vector<vector<string>> table_size_data;
    table_size_data.push_back({"Iteración", "Tamaño de la tabla (MB)"});
    for (size_t i = 0; i < iteraciones.size(); ++i) {
        table_size_data.push_back({to_string(iteraciones[i]), to_string(table_sizes[i])});
    }
    save_CSV("tamanio_tabla_double_hashing.csv", table_size_data);

    return 0;
}




