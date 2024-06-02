#include <iostream>
#include <unordered_map>
#include <string>

class UserInfo {
public:
    int userID;
    std::string userName;
    std::string email;
    int age;

    UserInfo(int id, const std::string& name, const std::string& mail, int age)
        : userID(id), userName(name), email(mail), age(age) {}
};
