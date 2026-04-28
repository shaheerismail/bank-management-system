#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "BankManager.h"
#include "Account.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loginBtn_clicked();
    void on_btnLogout_clicked();
    void on_btnSave_clicked();
    void on_btnConfirmRegister_clicked();
    void on_btnDeposit_clicked();
    void on_btnWithdraw_clicked();
    void on_btnUpdatePin_clicked();
    void on_btnDeleteAccount_clicked();
    void on_btnSignUp_clicked();
private:
    Ui::MainWindow *ui;
    BankManager manager;
    void updateDashboard();
    void showResult(Account::TransactionResult result);
};

#endif
