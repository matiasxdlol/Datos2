#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
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

    // Vectores para almacenar los tiempos de inserción y los tamaños del vector
    vector<vector<string>> tiemposInsercion;
    vector<vector<string>> tamañosVector;
    tiemposInsercion.push_back({"Iteración", "Tamaño (MB)"});
    tamañosVector.push_back({"Iteración", "Tiempo de inserción (us)"});

    // Números de iteraciones
    vector<int> iteraciones = {1000, 5000, 10000, 21070}; // 21070 para leer todo el archivo

    for (int i = 0; i < iteraciones.size(); ++i) {
        int iteracion = iteraciones[i];

        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No se pudo abrir el archivo " << filename << endl;
            return 1;
        }

        unordered_map<string, User_data> usuariosPorId;
        unordered_map<string, User_data> usuariosPorNombre;

        auto startTime = high_resolution_clock::now();

        string line;
        int count = 0;
        while (getline(file, line) && (iteracion == 21070 || count < iteracion)) {
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

            usuariosPorId[tokens[1]] = user;
            usuariosPorNombre[tokens[2]] = user;

            count++;
        }

        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(endTime - startTime);

        // Calcular el tamaño total del vector en MB
        size_t totalSize = 0;
        for (const auto& pair : usuariosPorId) {
            totalSize += sizeof(pair.first) + sizeof(User_data);
            totalSize += pair.second.university.size() + pair.second.user_Id.size() +
                         pair.second.user_name.size() + pair.second.created_at.size();
        }
        double totalSizeMB = static_cast<double>(totalSize) / (1024 * 1024);

        // Agregar los datos al vector correspondiente
        tiemposInsercion.push_back({to_string(iteracion), to_string(totalSizeMB)});
        tamañosVector.push_back({to_string(iteracion), to_string(duration.count())});

        // Cerrar el archivo
        file.close();
    }

    // Guardar los tiempos de inserción en un archivo CSV
    save_CSV("tiempos_insercion.csv", tamañosVector);

    // Guardar los tamaños del vector en un archivo CSV
    save_CSV("tamaños_vector.csv", tiemposInsercion);

    return 0;
}
