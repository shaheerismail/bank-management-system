#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include "BankManager.h"
#include "Account.h"
#include "CurrentAccount.h"
#include "SavingsAccount.h"
#include "DataBase.h"
#include <QDebug>

int Account::StartAccountNo=1001;
const double CurrentAccount::MinimumBalance=5000;
const double CurrentAccount::PenaltyFee=50;
const double SavingsAccount::IntrestRate=0.03;
const double SavingsAccount::MinimumBalance=1000;

void Account::updateStartAccountNo(const int accNo) {
    StartAccountNo=accNo;
}

bool Account::ValidateAccountNo(const std::string& accNo){
    if(accNo.length()==5 && std::isalpha(accNo.at(0)) && std::isdigit(accNo.at(1)) && std::isdigit(accNo.at(2)) && std::isdigit(accNo.at(3)) && std::isdigit(accNo.at(4))){
        return true;
    }
    else{
        return false;
    }
}

bool Account::ValidateBalance(const double amount) {
    if(amount>0){
        return true;
    }
    else{
        return false;
    }
}

bool Account::ValidatePin(const std::string& passcode){
    if(passcode.length()==4 && std::isdigit(passcode.at(0)) && std::isdigit(passcode.at(1)) && std::isdigit(passcode.at(2)) && std::isdigit(passcode.at(3))){
        return true;
    }
    else{
        return false;
    }
}

Account::Account(const std::string& accNo,const std::string& name,const double amount,const bool status,const std::string& passcode) : accountNo(accNo), holderName(name), balance(amount), active(status), pin(passcode) {}

Account::TransactionResult Account::deposit(const double amount){
    if(!ValidateBalance(amount)){
        return TransactionResult::INVALID_AMOUNT;
    }
    else {
        balance+=amount;
        return TransactionResult::SUCCESS;
    }
}

std::string Account::getAccountNo() const {
    return accountNo;
}

bool Account::matchPin(const std::string& Pin) {
    return pin==Pin;
}

void Account::updateName(const std::string& name){
    holderName=name;
}

bool Account::updatePin(const std::string& oldPin,const std::string& newPin){
    if(pin!=oldPin || !ValidatePin(newPin)){
        return false;
    }
    else{
        pin=newPin;
        return true;
    }
}

void Account::updateStatus(const bool status){
    active=status;
}

std::string Account::getDataToSave() const {
    return accountNo + "," + holderName + "," + std::to_string(balance) + "," + std::to_string(active) + "," + pin + "\n";  
}

CurrentAccount::CurrentAccount(const std::string& accountNumber,const std::string& name,const double amount,const bool status,const std::string& passcode) : Account(accountNumber,name,amount,status,passcode) {}

CurrentAccount::CurrentAccount(const std::string& name,const double amount,const bool status,const std::string& passcode) : Account("C"+std::to_string(StartAccountNo++),name,amount,status,passcode) {}

std::string CurrentAccount::getDisplay() const {
    std::ostringstream out;
    out << std::fixed <<  std::setprecision(2) << balance; 
    return "Account Number: " + accountNo + "\nAccount Holder: " + holderName + "\nBalance: PKR " +  out.str() + "\nMinimum Balance Requirment: PKR " + std::to_string(MinimumBalance) + "\n";  
}

Account::TransactionResult CurrentAccount::withdraw(const double amount) {
    if(!ValidateBalance(amount)){
        return TransactionResult::INVALID_AMOUNT;
    }
    else if(balance-amount<0){
        return TransactionResult::INSUFFICIENT_FUNDS;
    }
    else if(balance-amount<MinimumBalance){
        if(balance-amount-PenaltyFee<0){
            return TransactionResult::INSUFFICIENT_FUNDS;
        }
        else{
            balance-=amount+PenaltyFee;
            return TransactionResult::SUCCESS;
        }
    }
    else{
        balance-=amount;
        return TransactionResult::SUCCESS;
    }
}

SavingsAccount::SavingsAccount(const std::string& accountNumber,const std::string& name,const double amount,const bool status,const std::string& passcode) : Account(accountNumber,name,amount,status,passcode) {}

SavingsAccount::SavingsAccount(const std::string& name,const double amount,const bool status,const std::string& passcode) : Account("S"+std::to_string(StartAccountNo++),name,amount,status,passcode) {}

std::string SavingsAccount::getDisplay() const {
    std::ostringstream out1,out2;
    out1 << std::fixed <<  std::setprecision(2) << balance; 
    out2 << std::fixed << std::setprecision(2) << IntrestRate*100;
    return "Account Number: " + accountNo + "\nAccount Holder: " + holderName + "\nBalance: PKR " +  out1.str() + "\nIntrest Rate: " + out2.str() + "%\n"; 
}

void SavingsAccount::updateBalance() {
    if(balance>=MinimumBalance) {
        balance=balance*(1+IntrestRate);
    }
}

Account::TransactionResult SavingsAccount::withdraw(const double amount) {
    if(!ValidateBalance(amount)){
        return TransactionResult::INVALID_AMOUNT;
    }
    else if(balance-amount<0 || balance-amount<MinimumBalance){
        return TransactionResult::INSUFFICIENT_FUNDS;
    }
    else {
        balance-=amount;
        return TransactionResult::SUCCESS;
    }
}

DataBase::DataBase(const std::string& file) : fileName(file) {}

DataBase::~DataBase() {
    for(const Account* account : accounts) {
        delete account;
    }
}

Account* DataBase::operator[](const std::string& accountNo) const { 
    qDebug() << "Current Vector Size during search:" << accounts.size();
    if(!Account::ValidateAccountNo(accountNo)){
        return nullptr;
    }
    for(Account* account : accounts) {
        if(account->getAccountNo()==accountNo){
            return account;
        }
    }
    return nullptr;
}

bool DataBase::saveData() const {
    std::ofstream myFile(fileName);
    if(!myFile){
        return false;
    }
    for(const Account* account : accounts) {
        myFile << account->getDataToSave();
    }
    return true;
}

bool DataBase::loadData() {
    std::ifstream myFile(fileName);
    if(!myFile) {
        return false;
    }
    std::string buffer;
    std::stringstream out;
    while(std::getline(myFile,buffer)){
        out.str(buffer);
        std::string accountNo,holderName,pin,temp;
        double balance;
        bool active;
        std::getline(out,accountNo,',');
        std::getline(out,holderName,',');
        std::getline(out,temp,',');
        std::stringstream(temp) >> balance;
        std::getline(out,temp,',');
        std::stringstream(temp) >> active;
        std::getline(out,pin);
        accountNo.erase(accountNo.find_last_not_of(" \n\r\t") + 1);
        pin.erase(pin.find_last_not_of(" \n\r\t") + 1);
        if(!accountNo.empty() && accountNo.at(0)=='S'){
            accounts.push_back(new SavingsAccount(accountNo,holderName,balance,active,pin));
            Account::updateStartAccountNo(std::stoi(accountNo.substr(1))+1);
        }
        else if(!accountNo.empty() && accountNo.at(0)=='C') {
            accounts.push_back(new CurrentAccount(accountNo,holderName,balance,active,pin));
            Account::updateStartAccountNo(std::stoi(accountNo.substr(1))+1);
        }
        else {
            continue;
        }
        out.str("");
        out.clear();
    }

    myFile.close();
    return true;
}

Account* DataBase::operator[](const int idx) const {
    if(idx>=0 && idx<accounts.size()) {
        return accounts[idx];
    }
    else {
        return nullptr;
    }
}

void DataBase::addAccount(const std::string& name,const double amount,const bool status,const std::string& passcode) {
    qDebug() << "DEBUG: Inside CURRENT path";
    accounts.push_back(new CurrentAccount(name,amount,status,passcode));
}

void DataBase::addAccount(const std::string& name,const std::string& passcode,const double amount,const bool status,bool value) {
    qDebug() << "DEBUG: Inside SAVINGS path";
    accounts.push_back(new SavingsAccount(name,amount,status,passcode));
}

void DataBase::deleteAccount(const Account* ptr) {
    auto it=std::find(accounts.begin(),accounts.end(),ptr);
    if(it!=accounts.end()) {
        delete *it;
        accounts.erase(it);
        return ;
    }
}

int DataBase::getSize() const {
    return accounts.size();
}

void DataBase::applyInterest() {
    for(Account* ptr : accounts) {
        SavingsAccount* account=dynamic_cast<SavingsAccount*>(ptr);
        if(account!=nullptr) {
            account->updateBalance();
        }
    }
}

BankManager::BankManager(const std::string& file) : db(file), currentAccount(nullptr) {}

bool BankManager::initialize() {
    return db.loadData();
}

bool BankManager::closeProcedure() const {
    return db.saveData();
}

bool BankManager::login(const std::string& accountNumber,const std::string& pin) {
    Account *ptr=db[accountNumber];
    if(ptr!=nullptr && ptr->matchPin(pin)) {
        currentAccount=ptr;
        return true;
    }
    else{
        return false;
    }
}




Account::TransactionResult BankManager::processWithdraw(const double amount) {
    if(currentAccount==nullptr) {
        return Account::TransactionResult::SIGNED_OUT;
    }
    else {
        return currentAccount->withdraw(amount);
    }
}

Account::TransactionResult BankManager::processDeposit(const double amount) {
    if(currentAccount==nullptr) {
        return Account::TransactionResult::SIGNED_OUT;
    }
    else {
        return currentAccount->deposit(amount);
    }
}

void BankManager::createAccount(const std::string& name,const double amount,const bool status,const std::string& passcode) {
    db.addAccount(name,amount,status,passcode);
}

void BankManager::createAccount(const std::string& name,const std::string& passcode,const double amount,const bool status) {
    db.addAccount(name,passcode,amount,status);
}

void BankManager::removeAccount() {
    if(currentAccount!=nullptr) {
        db.deleteAccount(currentAccount);
        currentAccount=nullptr;
        db.saveData();
    }
}

std::string BankManager::viewAccount() const {
    if(currentAccount!=nullptr) {
        return currentAccount->getDisplay();
    }
    else {
        return "User not logged in\n";
    }
}

bool BankManager::setNewPin(const std::string& oldPin,const std::string& newPin) {
    if(currentAccount!=nullptr) {
        return currentAccount->updatePin(oldPin,newPin);
    }
    else {
        return false;
    }
}

void BankManager::setStatus(const bool status) {
    if(currentAccount!=nullptr) { 
        currentAccount->updateStatus(status);
    }
}

void BankManager::runEndOfMonthProcess() {
    db.applyInterest();
}

std::string BankManager::getLatestSummary() {
    int lastIdx = db.getSize() - 1;
    const Account* ptr=db[lastIdx];
    if(ptr!=nullptr) {
        return ptr->getDisplay();
    }
    else {
        return "Error in account creation";
    }
}
