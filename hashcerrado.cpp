#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <unordered_map>
#include <chrono> 
#include <vector>

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

    unordered_map<string, User_data> usuariosPorId; // Tabla hash cerrada para almacenar usuarios por ID

    string line;
    // Leer datos del archivo CSV y almacenarlos en la tabla hash
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
        usuariosPorId[user.user_Id] = user;
    }
    file.close();

    vector<int> indices_registrados;
    srand(time(nullptr)); // Seed para números aleatorios

    // Generar 5000 índices aleatorios para usuarios registrados
    for (int i = 0; i < 5000; ++i) {
        int index = rand() % usuariosPorId.size();
        indices_registrados.push_back(index);
    }

    vector<vector<string>> tiempos_busqueda_encontrados;
    tiempos_busqueda_encontrados.push_back({"User_Name", "Tiempo_de_busqueda (ns)"});

    // Búsqueda de usuarios registrados
    for (int index : indices_registrados) {
        // Obtener un iterador al elemento en la tabla hash basado en un índice aleatorio
        auto it = usuariosPorId.begin();
        advance(it, index);
        const User_data& user = it->second;

        // Medir el tiempo de búsqueda
        auto searchStartTime = high_resolution_clock::now();
        auto searchEndTime = high_resolution_clock::now();
        auto searchDuration = duration_cast<nanoseconds>(searchEndTime - searchStartTime);

        // Realizar la búsqueda y medir el tiempo
        searchStartTime = high_resolution_clock::now();
        it = usuariosPorId.find(user.user_Id);
        if (it != usuariosPorId.end()) {
            searchEndTime = high_resolution_clock::now();
            searchDuration = duration_cast<nanoseconds>(searchEndTime - searchStartTime);
            vector<string> tiempo_usuario = {user.user_name, to_string(searchDuration.count())};
            tiempos_busqueda_encontrados.push_back(tiempo_usuario);
        }
    }

    // Guardar los tiempos de búsqueda en un archivo CSV
    save_CSV("tiempos_busqueda_usuarios_encontrados_cerrado.csv", tiempos_busqueda_encontrados);

    return 0;
}
