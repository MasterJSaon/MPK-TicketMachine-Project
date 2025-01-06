#pragma once
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QLabel>
#include <QKeyEvent>

class CustomWidget : public QWidget {
    Q_OBJECT
public:
    CustomWidget(QWidget *parent = nullptr) : QWidget(parent) {}

protected:
    void keyPressEvent(QKeyEvent *event) override {
        emit keyPressed(event);
    }

signals:
    void keyPressed(QKeyEvent *event);
};

class User {
private:
public:
    User(const QString &user_name);
};

User::User(const QString &user_name) {
    CustomWidget* userWindow = new CustomWidget();
    userWindow->setWindowTitle(user_name + "'s Dashboard");

    QVBoxLayout *userLayout = new QVBoxLayout;
    QPushButton* viewProfileButton = new QPushButton("View Profile");
    userLayout->addWidget(viewProfileButton);
    userLayout->addWidget(new QPushButton("Change Password"));
    userLayout->addWidget(new QPushButton("User Settings"));

    // Add Logout Button
    QPushButton* logoutButton = new QPushButton("Logout");
    userLayout->addWidget(logoutButton);

    userWindow->setLayout(userLayout);
    userWindow->setFixedSize(300, 200);
    userWindow->show();

    // Connect Enter key to View Profile button
    QObject::connect(viewProfileButton, &QPushButton::clicked, [userWindow]() {
        // Action for viewing profile
        QMessageBox::information(userWindow, "Profile", "Viewing Profile...");
    });

    // Detect Enter key press
    QObject::connect(userWindow, &CustomWidget::keyPressed, [viewProfileButton](QKeyEvent *event) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            viewProfileButton->click();
        }
    });

    // Handle Logout action
    QObject::connect(logoutButton, &QPushButton::clicked, [userWindow, loginWindow]() {
        userWindow->close();  // Close the user dashboard
        loginWindow->show();  // Show the login window again
    });
}

class Admin {
public:
    Admin(const QString &user_name, QSqlDatabase &db);
};

Admin::Admin(const QString &user_name, QSqlDatabase &db) {
    Q_UNUSED(db);  // Suppress unused parameter warning

    CustomWidget* adminWindow = new CustomWidget();
    adminWindow->setWindowTitle(user_name + "'s Dashboard");

    QVBoxLayout *adminLayout = new QVBoxLayout;
    QPushButton* manageUsersButton = new QPushButton("Manage Users");
    adminLayout->addWidget(manageUsersButton);
    adminLayout->addWidget(new QPushButton("View Logs"));
    adminLayout->addWidget(new QPushButton("Admin Settings"));

    // Add Logout Button
    QPushButton* logoutButton = new QPushButton("Logout");
    adminLayout->addWidget(logoutButton);

    QPushButton* displayDataButton = new QPushButton("Display All Users Data");
    adminLayout->addWidget(displayDataButton);
    adminWindow->setLayout(adminLayout);
    adminWindow->setFixedSize(300, 300);
    adminWindow->show();

    // Connect Enter key to Manage Users button
    QObject::connect(manageUsersButton, &QPushButton::clicked, [adminWindow]() {
        // Action for managing users
        QMessageBox::information(adminWindow, "Manage Users", "Managing Users...");
    });

    // Detect Enter key press
    QObject::connect(adminWindow, &CustomWidget::keyPressed, [manageUsersButton](QKeyEvent *event) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            manageUsersButton->click();
        }
    });

    // Handle Logout action
    QObject::connect(logoutButton, &QPushButton::clicked, [adminWindow, loginWindow]() {
        adminWindow->close();  // Close the admin dashboard
        loginWindow->show();  // Show the login window again
    });
}
