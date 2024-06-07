//no se ocupa
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

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

    // Linear probing insertion
    void insert(const User_data& data) {
        size_t index = funcion_hash(data.user_Id, tabla_size);
        while (ocupado[index]) {
            index = (index + 1) % tabla_size;
        }
        tabla[index] = data;
        ocupado[index] = true;
        num_elements++;
    }

    // Linear probing search
    bool search(const string& user_Id, User_data& result) {
        size_t index = funcion_hash(user_Id, tabla_size);
        size_t start_index = index;
        while (ocupado[index]) {
            if (tabla[index].user_Id == user_Id) {
                result = tabla[index];
                return true;
            }
            index = (index + 1) % tabla_size;
            if (index == start_index) {
                break;
            }
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
void save_CSV(const string& filename, double size_MB) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "Tamaño de la tabla (MB)" << endl;
        file << size_MB << endl;
        file.close();
        cout << "El tamaño de la tabla se ha guardado exitosamente en el archivo: " << filename << endl;
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

    // Calcular el tamaño de la tabla en MB cuando está llena
    double size_MB = hash_table.size_MB();

    // Guardar el tamaño de la tabla en un archivo CSV
    save_CSV("tamaño_tabla_linear_probing.csv", size_MB);

    return 0;
}
