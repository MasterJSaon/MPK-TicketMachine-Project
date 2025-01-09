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
#include <QListWidget>
#include <QDebug>
#include <QLabel>
#include <QDialog>
#include <QComboBox>
#include <QDateEdit>

class User {
     private:
	     
     public:
	User(const QString &user_name, QSqlDatabase& db);
};

User::User(const QString &user_name, QSqlDatabase& db) {
    // Store the database reference
    // this->db = db;

    // Create the user window
    QWidget* userWindow = new QWidget();
    const QString user_name_cpy = user_name;
    userWindow->setWindowTitle(user_name + "'s dashboard");

    // Create a layout for the user window
    QVBoxLayout *userLayout = new QVBoxLayout;

    // Create the buttons
    QPushButton *viewProfileButton = new QPushButton("View Profile");
    QPushButton *buyTicketButton = new QPushButton("Buy ticket");
    QPushButton *myTicketsButton = new QPushButton("My Tickets");
    QPushButton *logoutButton = new QPushButton("Logout");

    // Add the buttons to the layout
    userLayout->addWidget(viewProfileButton);
    userLayout->addWidget(buyTicketButton);
    userLayout->addWidget(myTicketsButton);
    userLayout->addWidget(logoutButton);

    // Add event listeners for each button
    QObject::connect(viewProfileButton, &QPushButton::clicked, [user_name_cpy]() {
        // Handle "View Profile" button click event
        QMessageBox::information(nullptr, "User Profile", "Logged as " + user_name_cpy);
    });

    QObject::connect(buyTicketButton, &QPushButton::clicked, [this, user_name_cpy, db]() {
        // Handle "Buy Ticket" button click event
        // Show a custom dialog for ticket purchase
        QDialog* buyTicketDialog = new QDialog();
        buyTicketDialog->setWindowTitle("Buy Ticket");

        QVBoxLayout* dialogLayout = new QVBoxLayout(buyTicketDialog);

        // Ticket Duration selection (ComboBox)
        QLabel* durationLabel = new QLabel("Select Ticket Duration:");
        QComboBox* durationComboBox = new QComboBox();
        // Fetch available durations from the TICKET_LIFETIME table
        QSqlQuery query(db);
        query.exec("SELECT DURATION FROM TICKET_LIFETIME");

        while (query.next()) {
            durationComboBox->addItem(query.value(0).toString());
        }

        dialogLayout->addWidget(durationLabel);
        dialogLayout->addWidget(durationComboBox);

        // Ticket Type selection (ComboBox)
        QLabel* typeLabel = new QLabel("Select Ticket Type:");
        QComboBox* typeComboBox = new QComboBox();
        typeComboBox->addItem("Student");
        typeComboBox->addItem("Pensioner");
        typeComboBox->addItem("Normal");

        dialogLayout->addWidget(typeLabel);
        dialogLayout->addWidget(typeComboBox);

        // Activation Date selection (DateEdit)
        QLabel* dateLabel = new QLabel("Select Activation Date:");
        QDateEdit* dateEdit = new QDateEdit;
        dateEdit->setDate(QDate::currentDate());
        dateEdit->setCalendarPopup(true);

        dialogLayout->addWidget(dateLabel);
        dialogLayout->addWidget(dateEdit);

        // Confirm Button
        QPushButton* confirmButton = new QPushButton("Confirm Purchase");
        dialogLayout->addWidget(confirmButton);
        // QPushButton* cancelButton = new QPushButton("Cancel");
        // dialogLayout->addWidget(cancelButton);

        // QObject::connect(confirmButton, &QPushButton::clicked, [this, buyTicketDialog] () {
            // buyTicketDialog->cancel();
        // });
            

        // Connect confirm button
        QObject::connect(confirmButton, &QPushButton::clicked, [this, buyTicketDialog, durationComboBox, typeComboBox, dateEdit, user_name_cpy, db]() {
            // Retrieve selected values
            QString selectedDuration = durationComboBox->currentText();
            QString selectedType = typeComboBox->currentText();
            QDate selectedDate = dateEdit->date();

            // Get the corresponding ticket lifetime ID from the database
            QSqlQuery query(db);
            query.prepare("SELECT ID FROM TICKET_LIFETIME WHERE DURATION = ?");
            query.addBindValue(selectedDuration);
            query.exec();
            query.next();
            int ticketLifetimeID = query.value(0).toInt();

            // Insert the new ticket record into the TICKETS table
            query.prepare("INSERT INTO TICKETS (USER_ID, ACTIVATION_DATE, TICKET_LIFETIME_ID, TICKET_TYPE) "
                        "VALUES ((SELECT ID FROM USERS WHERE USERNAME = ?), ?, ?, ?)");
            query.addBindValue(user_name_cpy);
            query.addBindValue(selectedDate.toString("yyyy-MM-dd"));
            query.addBindValue(ticketLifetimeID);
            query.addBindValue(selectedType);

            if (query.exec()) {
                QMessageBox::information(nullptr, "Ticket Purchased", "Your ticket has been purchased!");
            } else {
                QMessageBox::critical(nullptr, "Error", "Failed to purchase the ticket. Please try again.");
            }

            buyTicketDialog->accept();  // Close the dialog after purchase
        });

        buyTicketDialog->exec();  // Show the dialog
    });
    // View tickets functionality
    QObject::connect(myTicketsButton, &QPushButton::clicked, [this, user_name_cpy, db]() {
        // Handle "My Tickets" button click event
        // Show a dialog to display the user's tickets
        QDialog* ticketsDialog = new QDialog();
        ticketsDialog->setWindowTitle("My Tickets");

        QVBoxLayout* dialogLayout = new QVBoxLayout(ticketsDialog);

        QLabel* ticketsLabel = new QLabel("Your Tickets:");
        dialogLayout->addWidget(ticketsLabel);

        // Create a list to display the tickets
        QListWidget* ticketsListWidget = new QListWidget();
        dialogLayout->addWidget(ticketsListWidget);

        // Query the tickets for the current user
        QSqlQuery query(db);
        query.prepare("SELECT TICKET_LIFETIME.DURATION, TICKET_LIFETIME.STUDENT_PRICE, TICKET_LIFETIME.PENSIONER_PRICE, "
                    "TICKET_LIFETIME.NORMAL_PRICE, TICKETS.ACTIVATION_DATE, TICKETS.TICKET_TYPE "
                    "FROM TICKETS "
                    "JOIN TICKET_LIFETIME ON TICKETS.TICKET_LIFETIME_ID = TICKET_LIFETIME.ID "
                    "WHERE TICKETS.USER_ID = (SELECT ID FROM USERS WHERE USERNAME = ?)");
        query.addBindValue(user_name_cpy);
        query.exec();

        if (!query.next()) {
            ticketsListWidget -> addItem("No tickets yeet");
        } else {
            do {
                QString ticketDetails = "Ticket Type: " + query.value(4).toString() + "\n"
                                    "Duration: " + query.value(0).toString() + "\n"
                                    "Price: " + (query.value(4).toString() == "Student" ? query.value(1).toString() :
                                                (query.value(4).toString() == "Pensioner" ? query.value(2).toString() : query.value(3).toString())) + "\n"
                                    "Activation Date: " + query.value(4).toString();
                ticketsListWidget->addItem(ticketDetails);
            } while (query.next());
        }

        // while (query.next()) {
        //     QString ticketDetails = "Ticket Type: " + query.value(4).toString() + "\n"
        //                             "Duration: " + query.value(0).toString() + "\n"
        //                             "Price: " + (query.value(4).toString() == "Student" ? query.value(1).toString() :
        //                                         (query.value(4).toString() == "Pensioner" ? query.value(2).toString() : query.value(3).toString())) + "\n"
        //                             "Activation Date: " + query.value(4).toString();
        //     ticketsListWidget->addItem(ticketDetails);
        // }

        ticketsDialog->exec();  // Show the tickets dialog
    });

    // Connect the Logout button to a lambda function for logout action
    QObject::connect(logoutButton, &QPushButton::clicked, [userWindow]() {
        QMessageBox::StandardButton reply;
        
        reply = QMessageBox::question(nullptr, "Logout", "Are you sure you want to logout?",
                                      QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            userWindow->close();  // Close the user window
        }
    });

    // Set the layout and window properties
    userWindow->setLayout(userLayout);
    userWindow->setFixedSize(300, 250); // Adjusted size to accommodate the new button
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