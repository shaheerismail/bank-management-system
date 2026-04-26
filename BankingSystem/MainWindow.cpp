#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include "BankManager.h"
#include <string>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), manager("accounts.txt") {

    ui->setupUi(this);
    ui->mainStack->setCurrentIndex(0); 
    if (manager.initialize()) {
        manager.runEndOfMonthProcess();
        manager.closeProcedure();
    } else {
        ui->loginBtn->setEnabled(false);
        ui->accNumInput->setDisabled(true);
        ui->pinInput->setDisabled(true);
        ui->accNumInput->setPlaceholderText("DB ERROR");

        ui->statusbar->setStyleSheet("background-color: red; color: white;");
        ui->statusbar->showMessage("CRITICAL ERROR: Database Offline", 0);

        QMessageBox::critical(this, "System Failure", "The database file could not be loaded.");
    }
}

MainWindow::~MainWindow() {
    manager.closeProcedure();
    delete ui;
}

void MainWindow::on_btnSave_clicked() {
    if (manager.closeProcedure()) {
        ui->statusbar->setStyleSheet("color: green;");
        ui->statusbar->showMessage("Data saved successfully to accounts.txt", 3000);
    } else {
        ui->statusbar->setStyleSheet("color: red;");
        ui->statusbar->showMessage("Error: Could not save data!", 3000);
    }
}

void MainWindow::on_loginBtn_clicked() {
    std::string acc = ui->accNumInput->text().toStdString();
    std::string pin = ui->pinInput->text().toStdString();

    if (manager.login(acc, pin)) {
        this->setWindowTitle("Login Successful");
        ui->statusbar->showMessage("Authorized Access", 5000);
        ui->txtAmount->clear();
        ui->statusbar->setStyleSheet("");

        ui->mainStack->setCurrentIndex(1);
        updateDashboard();
    } else {
        ui->statusbar->setStyleSheet("color: red;");
        ui->statusbar->showMessage("Access Denied: Check Credentials", 3000);
    }
}

void MainWindow::showResult(Account::TransactionResult result) {
    switch(result) {
        case Account::TransactionResult::SUCCESS:
            ui->statusbar->setStyleSheet("color: green;");
            ui->statusbar->showMessage("Transaction Successful", 3000);
            updateDashboard();
            break;
        case Account::TransactionResult::INVALID_AMOUNT:
            ui->statusbar->setStyleSheet("color: red;");
            ui->statusbar->showMessage("Error: Invalid Amount", 3000);
            break;
        case Account::TransactionResult::INSUFFICIENT_FUNDS:
            ui->statusbar->setStyleSheet("color: red;");
            ui->statusbar->showMessage("Error: Insufficient Balance", 3000);
            break;
        case Account::TransactionResult::SIGNED_OUT:
            ui->mainStack->setCurrentIndex(0);
            break;
    }
}

void MainWindow::on_btnDeposit_clicked() {
    bool ok;
    double amount = ui->txtAmount->text().toDouble(&ok);

    if (ok) {
        Account::TransactionResult res = manager.processDeposit(amount);
        showResult(res);
        ui->txtAmount->clear();
    } else {
        ui->statusbar->setStyleSheet("color: red;");
        ui->statusbar->showMessage("Please enter a valid numeric amount", 3000);
    }
}

void MainWindow::on_btnLogout_clicked() {
    ui->mainStack->setCurrentIndex(0);
    ui->accNumInput->clear();
    ui->pinInput->clear();
    ui->statusbar->setStyleSheet("");
    ui->statusbar->showMessage("Logged out", 3000);
}

void MainWindow::on_btnUpdatePin_clicked() {
    std::string oldP = ui->txtOldPin->text().toStdString();
    std::string newP = ui->txtNewPin->text().toStdString();

    if (manager.setNewPin(oldP, newP)) {
        manager.closeProcedure();
        ui->statusbar->setStyleSheet("color: green;");
        ui->statusbar->showMessage("PIN successfully updated", 3000);
        ui->txtOldPin->clear();
        ui->txtNewPin->clear();
    } else {
        ui->statusbar->setStyleSheet("color: red;");
        ui->statusbar->showMessage("Error: Invalid PIN update", 3000);
    }
}

void MainWindow::on_btnWithdraw_clicked() {
    bool ok;
    double amount = ui->txtAmount->text().toDouble(&ok);

    if (ok) {

        Account::TransactionResult result = manager.processWithdraw(amount);
        showResult(result);

        ui->txtAmount->clear();
    } else {
        ui->statusbar->setStyleSheet("color: red;");
        ui->statusbar->showMessage("Invalid input: Please enter a number", 3000);
    }
}


void MainWindow::updateDashboard() {
    std::string details = manager.viewAccount();
    ui->lblDisplay->setText(QString::fromStdString(details));
}

void MainWindow::on_btnDeleteAccount_clicked() {
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Delete Account",
        "Are you sure? This action is permanent.", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        manager.removeAccount(); // Deletes from vector and saves file
        ui->mainStack->setCurrentIndex(0);
        ui->statusbar->showMessage("Account Deleted Successfully");
    }
}

void MainWindow::on_btnSignUp_clicked() {
    ui->mainStack->setCurrentIndex(2);
}

void MainWindow::on_btnConfirmRegister_clicked() {
    std::string name = ui->txtNewName->text().toStdString();
    std::string pass = ui->txtNewPasscode->text().toStdString();
    const double initialBalance = 0.0;
    const bool isActive = true;

    if (name.empty() || pass.empty()) {
        QMessageBox::warning(this, "Error", "Fields cannot be empty.");
        return;
    }
    if (ui->cmbType->currentText() == "Savings") {
        manager.createAccount(name, pass, initialBalance, isActive);
    } else {
        manager.createAccount(name, initialBalance, isActive, pass);
    }

    std::string summary = manager.getLatestSummary();

    QMessageBox::information(this, "Success",
        "Account Created!\n\n" + QString::fromStdString(summary));

    ui->txtNewName->clear();
    ui->txtNewPasscode->clear();
    manager.closeProcedure();
    ui->mainStack->setCurrentIndex(0);
}
