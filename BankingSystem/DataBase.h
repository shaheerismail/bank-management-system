#ifndef DATABASE_H
#define DATABASE_H
#include "Account.h"
#include <vector>
#include <string>

class DataBase {
    std::vector<Account*> accounts;
    std::string fileName;
public:
    DataBase(const std::string& file);
    ~DataBase();
    Account* operator[](const std::string& accountNo) const;
    Account* operator[](const int idx) const;
    bool saveData() const;
    bool loadData();
    void addAccount(const std::string& name,const double amount,const bool status,const std::string& passcode); //adds Current Account
    void addAccount(const std::string& name,const std::string& passcode,const double amount,const bool status);    //adds Savings Account
    void deleteAccount(const Account* ptr);
    void applyInterest();
    int getSize() const;
};

#endif
