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
        size_t index = funcion_hash(user_id, tabla_size); //usa user_id como key
        vector<User_data> results;
        for (const auto& user : tabla[index]) {
            if (user.user_id == user_id) {
                results.push_back(user);
            }
        }
        return results;
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

//crea la table hash
    size_t table_size = users.size() * 2;
    tabla_hash hash_table(table_size);

//incerta los usuarios 
    for (const auto& user : users) {
        hash_table.insert(user);
    }

//crea el archivo para escribir los tiempos de búsqueda
    ofstream outfile("tiempos_busqueda.csv");
    outfile << "Usuario, Tiempo (ms)" << endl;

//hace una busqueda aleatoria en el vertor user
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, users.size() - 1);

    for (int i = 0; i < 5000; ++i) {
//selecciona un usuario aleatorio
        int index = dist(gen);
        const User_data& user = users[index];

//realiza la búsqueda y medir el tiempo
        auto start = high_resolution_clock::now();
        vector<User_data> results = hash_table.search(user.user_id);  // Buscar por user_id
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

//escribe el tiempo en el csv
        outfile << user.user_name << ", " << duration.count() << endl;
    }

//cierra el archivo
    outfile.close();

    cout << "Tiempos de búsqueda guardados en 'tiempos_busqueda.csv'." << endl;

    return 0;
}
