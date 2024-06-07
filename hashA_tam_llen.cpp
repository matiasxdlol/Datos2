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

//método para obtener el tamaño de la tabla hash en MB
    double tamaño_MB() const {
        double size_element = sizeof(list<User_data>) + sizeof(User_data); // Suponiendo un tamaño fijo por elemento
        double total_size = size_element * tabla_size;
        return total_size / (1024 * 1024); // Convertir a MB
    }
};

//función para leer datos del csv y llenar el vector de usuarios
void read_CSV(const string& filename, vector<User_data>& users, int num_lines) {
    ifstream file(filename);
    string line;

//ignora la primera línea (encabezado)
    getline(file, line);

//lee cada línea del csv y almacenar los datos en el vector de usuarios
    for (int i = 0; i < num_lines; ++i) {
        if (!getline(file, line))
            break;

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
    //vector que almacena la cantidad de linea a leer
    vector<int> iteraciones = {1000, 5000, 10000, 21070}; 

//crea el archivo para escribir los tiempos de búsqueda
    ofstream time_file("tiempos_llenado_tabla_hash.csv");
    time_file << "Numero de Lineas,Tiempo (microsegundos)" << endl;

//crea el archivo para escribir el tamaño de la tabla hash
    ofstream size_file("tamaño_tabla_hash.csv");
    size_file << "Numero de Lineas,Tamaño (MB)" << endl;

    for (int i = 0; i < iteraciones.size(); ++i) {
        int iteracion = iteraciones[i];

//crea la tabla hash 
        size_t table_size = iteracion * 2; 
        tabla_hash hash_table(table_size);

//Lee las iteraciones especificadas
        vector<User_data> users;
        read_CSV("universities_followers.csv", users, iteracion);

//inserta los usuarios en la tabla hash y medir el tiempo
        auto start = high_resolution_clock::now();
        for (const auto& user : users) {
            hash_table.insert(user);
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

//guarda el tiempo y el tamaño en archivos CSV
        time_file << iteracion << "," << duration.count() << endl;
        size_file << iteracion << "," << hash_table.tamaño_MB() << endl;

//Limpia el vector de usuarios para la siguiente lectura
        users.clear();
    }

    time_file.close();
    size_file.close();

    return 0;
}
