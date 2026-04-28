#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
#include "DataBase.h"

class Account {
    static bool ValidatePin(const std::string& passcode);
protected:
    static int StartAccountNo;
    const std::string accountNo;
    std::string holderName;
    double balance;
    bool active;
    std::string pin;
    static bool ValidateBalance(const double amount);
public:
    static void updateStartAccountNo(const int accNo);
    static bool ValidateAccountNo(const std::string& accNo);
    enum class TransactionResult {SUCCESS,INVALID_AMOUNT,INSUFFICIENT_FUNDS,SIGNED_OUT};
    Account(const std::string& accNo,const std::string& name,const double amount,const bool status,const std::string& passcode);
    virtual ~Account(){}
    virtual std::string getDisplay() const = 0;
    TransactionResult deposit(const double amount);
    virtual TransactionResult withdraw(const double amount) = 0;
    std::string getAccountNo() const;
    void updateName(const std::string& name);
    bool matchPin(const std::string& Pin);
    bool updatePin(const std::string& oldPin,const std::string& newPin);
    void updateStatus(const bool status);
    std::string getDataToSave() const;
};

#endif