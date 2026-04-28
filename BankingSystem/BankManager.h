#ifndef BANKMANAGER_H
#define BANKMANAGER_H
#include <string>
#include "DataBase.h"
#include "Account.h"
#include <string>

class BankManager {
    DataBase db;
    Account* currentAccount;
public:
    BankManager(const std::string& file);
    bool login(const std::string& accountNumber,const std::string& pin);
    bool initialize();
    bool closeProcedure() const;
    void createAccount(const std::string& name,const double amount,const bool status,const std::string& passcode);
    void createAccount(const std::string& name,const std::string& passcode,const double amount,const bool status);
    void removeAccount();
    bool setNewPin(const std::string& oldPin,const std::string& newPin);
    void setStatus(const bool status);
    std::string viewAccount() const;
    Account::TransactionResult processWithdraw(const double amount);
    Account::TransactionResult processDeposit(const double amount);
    std::string getLatestSummary();
    void runEndOfMonthProcess();
};

#endif
