#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H
#include <string>
#include "Account.h"

class SavingsAccount : public Account {
    static const double IntrestRate;
    static const double MinimumBalance;
public:
    SavingsAccount(const std::string& accountNumber,const std::string& name,const double amount,const bool status,const std::string& passcode);
    SavingsAccount(const std::string& name,const double amount,const bool status,const std::string& passcode);
    std::string getDisplay() const override;
    TransactionResult withdraw(const double amount) override;
    void updateBalance();
};

#endif