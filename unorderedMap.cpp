#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono> // Agregado para medir el tiempo
#include <algorithm> // Para usar std::remove_if y std::isspace

using namespace std;
using namespace std::chrono; // Espacio de nombres para la biblioteca chrono

// Estructura para almacenar los datos de un usuario
struct UserData {
    string university;
    string userId;
    string userName;
    int numberTweets;
    int friendsCount;
    int followersCount;
    string createdAt;
};

// Función para dividir una cadena en tokens usando un delimitador
vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        // Eliminar espacios en blanco alrededor del token
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
        tokens.push_back(token);
    }
    return tokens;
}

// Función para convertir una cadena a número, maneja la notación científica
double convertToNumber(const string& str) {
    stringstream ss(str);
    double number;
    ss >> number;
    return number;
}

// Función para buscar un usuario por user_id o user_name
UserData findUser(const unordered_map<string, UserData>& usuarios, const string& key, bool searchById) {
    if (searchById) {
        auto it = usuarios.find(key);
        if (it != usuarios.end()) {
            return it->second;
        }
    } else {
        for (const auto& pair : usuarios) {
            if (pair.second.userName == key) {
                return pair.second;
            }
        }
    }
    // Retornar un UserData vacío si el usuario no se encuentra
    return UserData();
}

// Función para calcular el tamaño en bytes de una cadena
size_t getStringSize(const string& str) {
    return sizeof(char) * str.capacity();
}

// Función para calcular el tamaño de un UserData
size_t getUserDataSize(const UserData& user) {
    size_t size = sizeof(UserData);
    size += getStringSize(user.university);
    size += getStringSize(user.userId);
    size += getStringSize(user.userName);
    size += getStringSize(user.createdAt);
    return size;
}

int main() {
    // Nombre del archivo CSV a leer
    string filename = "universities_followers.csv";

    // Intentar abrir el archivo
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "No se pudo abrir el archivo " << filename << endl;
        return 1;
    }

    // Crear un unordered_map para almacenar los datos de los usuarios utilizando user_id como clave
    unordered_map<string, UserData> usuariosPorId;

    // Crear un unordered_map adicional para almacenar los datos de los usuarios utilizando user_name como clave
    unordered_map<string, UserData> usuariosPorNombre;

    // Medir el tiempo de inserción de datos
    auto startTime = high_resolution_clock::now();

    // Leer el archivo línea por línea
    string line;
    int lineCount = 0;
    int invalidLineCount = 0;
    int validLineCount = 0;
    while (getline(file, line)) {
        lineCount++;

        // Dividir la línea en tokens usando coma como delimitador
        vector<string> tokens = split(line, ',');

        // Verificar que la línea tenga la cantidad de campos esperados
        if (tokens.size() != 7) {
            cout << "Error: línea inválida en el archivo CSV en la línea " << lineCount << ": " << line << endl;
            invalidLineCount++;
            continue;
        }

        try {
            // Crear un objeto UserData y asignar los valores desde los tokens
            UserData user;
            user.university = tokens[0];
            user.userId = tokens[1];
            user.userName = tokens[2];
            user.numberTweets = stoi(tokens[3]);
            user.friendsCount = stoi(tokens[4]);
            user.followersCount = stoi(tokens[5]);
            user.createdAt = tokens[6];

            // Insertar el usuario en el unordered_map utilizando user_id como clave
            usuariosPorId[tokens[1]] = user;

            // Insertar el usuario en el unordered_map adicional utilizando user_name como clave
            usuariosPorNombre[tokens[2]] = user;

            validLineCount++;
        } catch (const invalid_argument& e) {
            cout << "Error: argumento inválido en la línea " << lineCount << ": " << line << endl;
            invalidLineCount++;
        } catch (const out_of_range& e) {
            cout << "Error: número fuera de rango en la línea " << lineCount << ": " << line << endl;
            invalidLineCount++;
        }
    }

    // Medir el tiempo de inserción de datos
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(endTime - startTime);
    cout << "Tiempo de inserción de datos: " << duration.count() << " ms" << endl;

    // Almacenar el tamaño del vector lleno
    size_t vectorSize = usuariosPorId.size();
    cout << "Número total de usuarios añadidos: " << vectorSize << endl;
    cout << "Número de líneas válidas: " << validLineCount << endl;
    cout << "Número de líneas inválidas: " << invalidLineCount << endl;

    // Calcular el tamaño total del unordered_map en MB
    size_t totalSize = 0;
    for (const auto& pair : usuariosPorId) {
        totalSize += sizeof(pair.first) + getStringSize(pair.first);
        totalSize += getUserDataSize(pair.second);
    }
    double totalSizeMB = static_cast<double>(totalSize) / (1024 * 1024);
    cout << "Tamaño del vector lleno: " << totalSizeMB << " MB" << endl;

    // Cerrar el archivo
    file.close();

    // Interfaz de usuario para buscar usuario por user_name o user_id
    cout << "Elija cómo desea buscar al usuario:" << endl;
    cout << "1. Buscar por user_name" << endl;
    cout << "2. Buscar por user_id" << endl;
    int choice;
    cout << "Ingrese su elección (1 o 2): ";
    cin >> choice;

    string searchKey;
    bool searchById = false;
    if (choice == 1) {
        cout << "Ingrese el user_name del usuario que desea buscar: ";
        cin >> searchKey;
    } else if (choice == 2) {
        cout << "Ingrese el user_id del usuario que desea buscar: ";
        cin >> searchKey;
        searchById = true;
    } else {
        cout << "Elección no válida. Saliendo del programa." << endl;
        return 1;
    }

    // Medir el tiempo de búsqueda del usuario
    auto searchStartTime = high_resolution_clock::now();

    // Buscar al usuario
    UserData foundUser = findUser((searchById ? usuariosPorId : usuariosPorNombre), searchKey, searchById);

    auto searchEndTime = high_resolution_clock::now();
    auto searchDuration = duration_cast<milliseconds>(searchEndTime - searchStartTime);
    cout << "Tiempo de búsqueda: " << searchDuration.count() << " ms" << endl;

    if (!foundUser.university.empty()) {
        cout << "Usuario encontrado:" << endl;
        cout << "User ID: " << foundUser.userId << ", User Name: " << foundUser.userName << endl;
        cout << "University: " << foundUser.university << endl;
        cout << "Number Tweets: " << foundUser.numberTweets << endl;
        cout << "Friends Count: " << foundUser.friendsCount << endl;
        cout << "Followers Count: " << foundUser.followersCount << endl;
        cout << "Created At: " << foundUser.createdAt << endl;
    } else {
        cout << "Usuario no encontrado." << endl;
    }

    return 0;
}
