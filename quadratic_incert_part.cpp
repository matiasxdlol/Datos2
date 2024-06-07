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

    // Quadratic probing insertion
    void insert(const User_data& data) {
        size_t index = funcion_hash(data.user_Id, tabla_size);
        size_t i = 0;
        while (ocupado[index]) {
            index = (index + i * i) % tabla_size;
            i++;
        }
        tabla[index] = data;
        ocupado[index] = true;
        num_elements++;
    }

    // Quadratic probing search
    bool search(const string& user_Id, User_data& result) {
        size_t index = funcion_hash(user_Id, tabla_size);
        size_t start_index = index;
        size_t i = 0;
        while (ocupado[index]) {
            if (tabla[index].user_Id == user_Id) {
                result = tabla[index];
                return true;
            }
            index = (index + i * i) % tabla_size;
            i++;
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

vector<string> split(const string &s, char delimit) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimit)) {
        tokens.push_back(token);
    }
    return tokens;
}

double convert_numero(const string& str) {
    stringstream ss(str);
    double number;
    ss >> number;
    return number;
}

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

    vector<long long> insertion_times;
    vector<double> table_sizes; // Almacena los tamaños de la tabla en MB

    vector<int> insertions = {1000, 5000, 10000, static_cast<int>(usuarios.size())};

    for (int iter = 0; iter < insertions.size(); ++iter) {
        size_t table_size = insertions[iter] * 2;
        tabla_hash hash_table(table_size);

        auto start_insertion = high_resolution_clock::now();
        // Insertar usuarios en la tabla hash
        for (int i = 0; i < insertions[iter]; ++i) {
            hash_table.insert(usuarios[i]);
        }
        auto stop_insertion = high_resolution_clock::now();
        auto insertion_duration = duration_cast<milliseconds>(stop_insertion - start_insertion);

        // Guardar el tiempo de inserción en el vector
        insertion_times.push_back(insertion_duration.count());

        // Calcular y guardar el tamaño de la tabla en MB
        table_sizes.push_back(hash_table.size_MB());
    }

    // Guardar los tiempos de inserción en un archivo CSV
    vector<vector<string>> insertion_data;
    insertion_data.push_back({"Cantidad", "Tiempo de inserción (ms)"});
    for (int i = 0; i < insertions.size(); ++i) {
        insertion_data.push_back({to_string(insertions[i]), to_string(insertion_times[i])});
    }
    save_CSV("tiempos_insercion_quadratic_por_partes.csv", insertion_data);

    // Guardar los tamaños de la tabla en un archivo CSV
    vector<vector<string>> table_size_data;
    table_size_data.push_back({"Cantidad", "Tamaño de la tabla (MB)"});
    for (int i = 0; i < insertions.size(); ++i) {
        table_size_data.push_back({to_string(insertions[i]), to_string(table_sizes[i])});
    }
    save_CSV("tamaño_tabla_quadratic_por_partes.csv", table_size_data);

    return 0;
}



