#ifndef CURRENTACCOUNT_H
#define CURRENTACCOUNT_H
#include <string>
#include "Account.h"

class CurrentAccount : public Account {
    static const double MinimumBalance;
    static const double PenaltyFee;
public:
    CurrentAccount(const std::string& accountNumber,const std::string& name,const double amount,const bool status,const std::string& passcode);
    CurrentAccount(const std::string& name,const double amount,const bool status,const std::string& passcode);
    std::string getDisplay() const override;
    TransactionResult withdraw(const double amount) override;
};

#endif