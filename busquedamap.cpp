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
//calcula el tamaño en MB
size_t get_string_size(const string& str) {
    return sizeof(char) * str.capacity();
}

size_t get_user_data_size(const User_data& user) {
    size_t size = sizeof(User_data);
    size += get_string_size(user.university);
    size += get_string_size(user.user_Id);
    size += get_string_size(user.user_name);
    size += get_string_size(user.created_at);
    return size;
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
    //ablre el archivo csv
    string filename = "universities_followers.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "No se pudo abrir el archivo " << filename << endl;
        return 1;
    }
    //lee cada línea del archivo, la divide en tokens y crea una estructura User_data que luego agrega al vector usuarios.
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
    //genera 5000 índices aleatorios dentro del rango del tamaño del vector usuarios y los almacena en indices_registrados.
    vector<int> indices_registrados;
    
    srand(time(nullptr)); //seed para números aleatorios

    
    for (int i = 0; i < 5000; ++i) {
        int index = rand() % usuarios.size();
        indices_registrados.push_back(index);
    }

    vector<vector<string>> tiempos_busqueda_encontrados;
    tiempos_busqueda_encontrados.push_back({"User_Name", "Tiempo_de_busqueda (ns)"});

    unordered_map<string, User_data> usuariosPorId;
    for (const auto& user : usuarios) {
        usuariosPorId[user.user_Id] = user;
    }

    // Búsqueda de usuarios registrados
    for (int index : indices_registrados) {
        User_data user = usuarios[index];
        auto searchStartTime = high_resolution_clock::now();
        auto it = usuariosPorId.find(user.user_Id);
        User_data foundUser;
        if (it != usuariosPorId.end()) {
            foundUser = it->second;
            auto searchEndTime = high_resolution_clock::now();
            auto searchDuration = duration_cast<nanoseconds>(searchEndTime - searchStartTime);
            vector<string> tiempo_usuario = {user.user_name, to_string(searchDuration.count())};
            tiempos_busqueda_encontrados.push_back(tiempo_usuario);
        }
    }
    //guarda en un csv el tiempo de busqueda
    save_CSV("tiempos_busqueda_usuarios_encontrados.csv", tiempos_busqueda_encontrados);

    return 0;
}

