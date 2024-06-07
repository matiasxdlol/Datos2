#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;
//el struct almacena los datos
struct User_data {
    string university;
    string user_Id;
    string user_name;
    int number_tweets;
    int friends_count;
    int followers_count;
    string created_at;
};
//divide una cadena s en tokens
vector<string> split(const string &s, char delimit) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimit)) {
        tokens.push_back(token);
    }
    return tokens;
}
//combierte la cadena en double
double convert_numero(const string& str) {
    stringstream ss(str);
    double number;
    ss >> number;
    return number;
}

//guarda en un vector los datos del csv, se usa para el csv de salida
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
    vector<vector<string>> tiemposInsercion;//para guardar el tiempo de incercion
    tiemposInsercion.push_back({"Iteración", "Tiempo de inserción (us)"});//para calcular el tiempo
//hace 10 ciclo de leer completamente el csv y calcula el tiempo de llenado
    for (int i = 0; i < 10; ++i) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No se pudo abrir el archivo " << filename << endl;
            return 1;
        }

        unordered_map<string, User_data> usuariosPorId;
        unordered_map<string, User_data> usuariosPorNombre;

        auto startTime = high_resolution_clock::now();

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

            usuariosPorId[tokens[1]] = user;
            usuariosPorNombre[tokens[2]] = user;
        }

        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(endTime - startTime);
        tiemposInsercion.push_back({to_string(i + 1), to_string(duration.count())});

    }

    save_CSV("tiempo_insercion.csv", tiemposInsercion);

    return 0;
}

