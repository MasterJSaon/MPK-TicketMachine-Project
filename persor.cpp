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
    userLayout->addWidget(new QPushButton("View Profile"));
    userLayout->addWidget(new QPushButton("Change Password"));
    userLayout->addWidget(new QPushButton("User Settings"));
    userWindow->setLayout(userLayout);
    userWindow->setFixedSize(300, 200);
    userWindow->show();

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
    QPushButton* viewLogsButton = new QPushButton("View Logs");
    QPushButton* adminSettingsButton = new QPushButton("Admin Settings");

    // Add button to display users' data
    QPushButton* displayDataButton = new QPushButton("Display All Users Data");
    QObject::connect(displayDataButton, &QPushButton::clicked, [adminWindow, &db]() {
        // Query the database and display data
        QSqlQuery query(db);
        if (!db.isOpen()) {
            QMessageBox::warning(adminWindow, "Database Error", "Database connection is not open.");
            return;
        }

        query.prepare("SELECT * FROM USERS");
        if (!query.exec()) {
            QMessageBox::warning(adminWindow, "Query Error", query.lastError().text());
            return;
        }

        QString data = "Users Data:\n";
        while (query.next()) {
            int id = query.value(0).toInt();
            QString username = query.value(1).toString();
            QString role = query.value(2).toString();
            data += QString("ID: %1, Username: %2, Role: %3\n").arg(id).arg(username).arg(role);
        }

        QMessageBox::information(adminWindow, "Users Data", data);
    });

    adminLayout->addWidget(manageUsersButton);
    adminLayout->addWidget(viewLogsButton);
    adminLayout->addWidget(adminSettingsButton);
    adminLayout->addWidget(displayDataButton);
    adminWindow->setLayout(adminLayout);
    adminWindow->setFixedSize(300, 300);
    adminWindow->show();
}
