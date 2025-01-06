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


class User {
     private:
	     
     public:
	User(const QString &user_name);
};

User::User(const QString &user_name) {
    QWidget* userWindow = new QWidget();
    userWindow->setWindowTitle(user_name + "'s Dashboard");

    QVBoxLayout *userLayout = new QVBoxLayout;
    QPushButton* viewProfileButton = new QPushButton("View Profile");
    userLayout->addWidget(viewProfileButton);
    userLayout->addWidget(new QPushButton("Change Password"));
    userLayout->addWidget(new QPushButton("User Settings"));
    userWindow->setLayout(userLayout);
    userWindow->setFixedSize(300, 200);
    userWindow->show();

    // Connect Enter key to View Profile button
    QObject::connect(viewProfileButton, &QPushButton::clicked, [userWindow]() {
        // Action for viewing profile
        QMessageBox::information(userWindow, "Profile", "Viewing Profile...");
    });
    
    // Detect Enter key press
    QObject::connect(userWindow, &QWidget::keyPressEvent, [viewProfileButton](QKeyEvent *event) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            viewProfileButton->click();
        }
    });
}

class Admin {
     private:
	     
     public:
	Admin(const QString &user_name, QSqlDatabase &db);	
};


Admin::Admin (const QString &user_name, QSqlDatabase &db) {
    QWidget* adminWindow = new QWidget();
    adminWindow->setWindowTitle(user_name + "'s Dashboard");

    QVBoxLayout *adminLayout = new QVBoxLayout;
    QPushButton* manageUsersButton = new QPushButton("Manage Users");
    adminLayout->addWidget(manageUsersButton);
    adminLayout->addWidget(new QPushButton("View Logs"));
    adminLayout->addWidget(new QPushButton("Admin Settings"));
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
    QObject::connect(adminWindow, &QWidget::keyPressEvent, [manageUsersButton](QKeyEvent *event) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            manageUsersButton->click();
        }
    });
}
