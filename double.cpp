#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <functional>

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

    size_t table_size = usuarios.size() * 2;
    tabla_hash hash_table(table_size);

    // Insertar usuarios en la tabla hash
    for (const auto& user : usuarios) {
        hash_table.insert(user);
    }

    vector<int> indices_registrados;
    srand(time(nullptr)); // Seed para números aleatorios

    // Generar 5000 índices aleatorios para usuarios registrados
    for (int i = 0; i < 5000; ++i) {
        int index = rand() % usuarios.size();
        indices_registrados.push_back(index);
    }

    vector<vector<string>> tiempos_busqueda_encontrados;
    tiempos_busqueda_encontrados.push_back({"User_Name", "Tiempo_de_busqueda (ns)"});

    // Búsqueda de usuarios registrados
    for (int index : indices_registrados) {
        const User_data& user = usuarios[index];
        User_data foundUser;

        auto searchStartTime = high_resolution_clock::now();
        bool found = hash_table.search(user.user_Id, foundUser);
        auto searchEndTime = high_resolution_clock::now();
        auto searchDuration = duration_cast<nanoseconds>(searchEndTime - searchStartTime);

        if (found) {
            vector<string> tiempo_usuario = {user.user_name, to_string(searchDuration.count())};
            tiempos_busqueda_encontrados.push_back(tiempo_usuario);
        }
    }

    // Guardar los tiempos de búsqueda en un archivo CSV
    save_CSV("tiempos_busqueda_usuarios_encontrados_double.csv", tiempos_busqueda_encontrados);

    return 0;
}
