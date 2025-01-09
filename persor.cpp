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

    // Set tab order for keyboard navigation
    viewProfileButton->setFocusPolicy(Qt::StrongFocus);
    buyTicketButton->setFocusPolicy(Qt::StrongFocus);
    myTicketsButton->setFocusPolicy(Qt::StrongFocus);
    logoutButton->setFocusPolicy(Qt::StrongFocus);

    userWindow->setTabOrder(viewProfileButton, buyTicketButton);
    userWindow->setTabOrder(buyTicketButton, myTicketsButton);
    userWindow->setTabOrder(myTicketsButton, logoutButton);

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

        // Add horizontal layout for buttons
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* confirmButton = new QPushButton("Confirm Purchase");
        QPushButton* cancelButton = new QPushButton("Cancel");
        buttonLayout->addWidget(confirmButton);
        buttonLayout->addWidget(cancelButton);
        dialogLayout->addLayout(buttonLayout);

        // Set tab order
        confirmButton->setFocusPolicy(Qt::StrongFocus);
        cancelButton->setFocusPolicy(Qt::StrongFocus);
        
        QObject::connect(cancelButton, &QPushButton::clicked, buyTicketDialog, &QDialog::reject);

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
    
    // Add new buttons and modify existing ones
    QPushButton* manageUsersButton = new QPushButton("Manage Users");
    QPushButton* viewLogsButton = new QPushButton("View Logs");
    QPushButton* ticketManagementButton = new QPushButton("Ticket Management");
    QPushButton* displayDataButton = new QPushButton("Display All Users Data");
    QPushButton* systemStatsButton = new QPushButton("System Statistics");
    QPushButton* logoutButton = new QPushButton("Logout");

    // Set tab order for keyboard navigation
    manageUsersButton->setFocusPolicy(Qt::StrongFocus);
    viewLogsButton->setFocusPolicy(Qt::StrongFocus);
    ticketManagementButton->setFocusPolicy(Qt::StrongFocus);
    displayDataButton->setFocusPolicy(Qt::StrongFocus);
    systemStatsButton->setFocusPolicy(Qt::StrongFocus);
    logoutButton->setFocusPolicy(Qt::StrongFocus);

    adminWindow->setTabOrder(manageUsersButton, viewLogsButton);
    adminWindow->setTabOrder(viewLogsButton, ticketManagementButton);
    adminWindow->setTabOrder(ticketManagementButton, displayDataButton);
    adminWindow->setTabOrder(displayDataButton, systemStatsButton);
    adminWindow->setTabOrder(systemStatsButton, logoutButton);

    // Add new ticket management functionality
    QObject::connect(ticketManagementButton, &QPushButton::clicked, [adminWindow, &db]() {
        QDialog* ticketManageDialog = new QDialog(adminWindow);
        ticketManageDialog->setWindowTitle("Ticket Management");
        QVBoxLayout* dialogLayout = new QVBoxLayout(ticketManageDialog);

        QPushButton* viewAllTicketsBtn = new QPushButton("View All Active Tickets");
        QPushButton* modifyPricingBtn = new QPushButton("Modify Ticket Pricing");
        QPushButton* addTicketTypeBtn = new QPushButton("Add New Ticket Type");
        QPushButton* cancelBtn = new QPushButton("Close");

        dialogLayout->addWidget(viewAllTicketsBtn);
        dialogLayout->addWidget(modifyPricingBtn);
        dialogLayout->addWidget(addTicketTypeBtn);
        dialogLayout->addWidget(cancelBtn);

        // Set tab order
        viewAllTicketsBtn->setFocusPolicy(Qt::StrongFocus);
        modifyPricingBtn->setFocusPolicy(Qt::StrongFocus);
        addTicketTypeBtn->setFocusPolicy(Qt::StrongFocus);
        cancelBtn->setFocusPolicy(Qt::StrongFocus);

        QObject::connect(viewAllTicketsBtn, &QPushButton::clicked, [&db, ticketManageDialog]() {
            QSqlQuery query(db);
            query.exec("SELECT COUNT(*) FROM TICKETS");
            query.next();
            int ticketCount = query.value(0).toInt();
            
            QString message = QString("Total active tickets: %1\n").arg(ticketCount);
            QMessageBox::information(ticketManageDialog, "Ticket Statistics", message);
        });

        QObject::connect(cancelBtn, &QPushButton::clicked, ticketManageDialog, &QDialog::accept);

        ticketManageDialog->exec();
    });

    // Add system statistics functionality
    QObject::connect(systemStatsButton, &QPushButton::clicked, [adminWindow, &db]() {
        QSqlQuery query(db);
        
        // Get user count
        query.exec("SELECT COUNT(*) FROM USERS");
        query.next();
        int userCount = query.value(0).toInt();

        // Get ticket count
        query.exec("SELECT COUNT(*) FROM TICKETS");
        query.next();
        int ticketCount = query.value(0).toInt();

        QString stats = QString("System Statistics:\n\n"
                              "Total Users: %1\n"
                              "Total Active Tickets: %2\n"
                              "System Version: 1.0.0\n"
                              "Database Status: Connected").arg(userCount).arg(ticketCount);

        QMessageBox::information(adminWindow, "System Statistics", stats);
    });

    // Add logout functionality
    QObject::connect(logoutButton, &QPushButton::clicked, [adminWindow]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(adminWindow, "Logout", "Are you sure you want to logout?",
                                    QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            adminWindow->close();
        }
    });

    adminLayout->addWidget(manageUsersButton);
    adminLayout->addWidget(viewLogsButton);
    adminLayout->addWidget(ticketManagementButton);
    adminLayout->addWidget(displayDataButton);
    adminLayout->addWidget(systemStatsButton);
    adminLayout->addWidget(logoutButton);

    adminWindow->setLayout(adminLayout);
    adminWindow->setFixedSize(300, 400);  // Increased height for new buttons
    adminWindow->show();
}