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

//el struct almacena los datos
struct User_data {
    string university;
    string user_id;
    string user_name;
    int number_tweets;
    int friends_count;
    int followers_count;
    string created_at;
};

//usa el id como key y aplicamos la inversa del número áureo
size_t funcion_hash(const string& key, size_t tabla_size) {
    const double A = 0.6180339887; // Constante áurea
    size_t hash = 0;
    for (char c : key) {
        hash += c;
    }
    double hash_double = tabla_size * (hash * A - floor(hash * A));
    return static_cast<size_t>(hash_double);
}

//crea una tabla hash abierto
class tabla_hash {
private:
    size_t tabla_size;
    vector<list<User_data>> tabla;

public:
    tabla_hash(size_t size) : tabla_size(size), tabla(size) {}

//método para insertar datos en la tabla hash
    void insert(const User_data& data) {
        size_t index = funcion_hash(data.user_id, tabla_size); //usa user_id como key
        tabla[index].push_back(data);
    }

//metodo para buscar datos en la tabla hash
    vector<User_data> search(const string& user_id) {
        size_t index = funcion_hash(user_id, tabla_size);  //usa user_id como key
        return vector<User_data>(tabla[index].begin(), tabla[index].end());
    }

};

//función para leer datos del csv y llenar el vector de usuarios
void read_CSV(const string& filename, vector<User_data>& users) {
    ifstream file(filename);
    string line;

//ignorar la primera línea (encabezado)
    getline(file, line);

//lee cada línea del csv y almacenar los datos en el vector de usuarios
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
//llama a la fucion para leer el csv y crea el vector usuarios para llenarlo
    vector<User_data> users;
    read_CSV("universities_followers.csv", users);

//crea el archivo para escribir los tiempos de búsqueda
    ofstream fill_times_file("tiempos_llenado_tabla_hash.csv");
    fill_times_file << "Ciclo, Tiempo (microsegundos)" << endl;

//realiza 500 ciclos para medir el tiempo de llenado de la tabla hash
    for (int ciclo = 0; ciclo < 500; ++ciclo) {
//crea la table hash
        size_t table_size = users.size() * 2; 
        tabla_hash hash_table(table_size);

//mide el tiempo de llenado de la tabla hash
        auto start = high_resolution_clock::now();
        for (const auto& user : users) {
            hash_table.insert(user);
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

//guardar el tiempo en el csv
        fill_times_file << ciclo + 1 << ", " << duration.count() << endl;
    }

//cierra el archivo
    fill_times_file.close();

    cout << "Tiempos de llenado de la tabla hash guardados en 'tiempos_llenado_tabla_hash.csv'." << endl;

    return 0;
}
