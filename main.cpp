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

static void createUsersTableAndInsertData(QSqlDatabase& db) {
    QSqlQuery query(db);
    QString createTableSQL = "CREATE TABLE IF NOT EXISTS USERS ("
                             "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "USERNAME TEXT NOT NULL UNIQUE, "
                             "PASSWORD TEXT NOT NULL, "
                             "ROLE TEXT NOT NULL)";
    if (!query.exec(createTableSQL)) {
        qDebug() << "Error creating table:" << query.lastError().text();
        return;
    }

    QString insertDataSQL = "INSERT INTO USERS (USERNAME, PASSWORD, ROLE) "
                            "VALUES ('admin', 'admin123', 'admin'), "
                            "('user', 'user123', 'user'), ('Jan', 'Janek123', 'user')";
    if (!query.exec(insertDataSQL)) {
        qDebug() << "Error inserting data:" << query.lastError().text();
    } else {
        qDebug() << "Sample users inserted!";
    }
}

bool validateLogin(QSqlDatabase& db, const QString& username, const QString& password, QString& role) {
    QSqlQuery query(db);
    query.prepare("SELECT ROLE FROM USERS WHERE USERNAME = :username AND PASSWORD = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        role = query.value(0).toString();
        return true;
    }
    return false;
}

void showAdminDashboard(QString user_name, QSqlDatabase& db) {
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

void showUserDashboard(QString user_name) {
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

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Connect to the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qDebug() << "Error: Unable to open database";
        return 1;
    }
    qDebug() << "Database opened successfully";

    // Create table and insert sample data
    createUsersTableAndInsertData(db);

    // Create login window
    QWidget* loginWindow = new QWidget();
    loginWindow->setWindowTitle("Login");
    loginWindow->setFixedWidth(400);
    loginWindow->setFixedHeight(180);

    QVBoxLayout* loginLayout = new QVBoxLayout;
    QLineEdit* usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Username");
    loginLayout->addWidget(usernameEdit);

    QLineEdit* passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginLayout->addWidget(passwordEdit);

    QPushButton* loginButton = new QPushButton("Login");
    loginLayout->addWidget(loginButton);

    // Function to handle login validation
    auto tryLogin = [&]() {
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        QString role;

        if (validateLogin(db, username, password, role)) {
            // Show dashboard based on role
            if (role == "admin") {
                showAdminDashboard(username, db);  // Show Admin Dashboard with display data button
            } else if (role == "user") {
                showUserDashboard(username);  // Show User Dashboard
            }

            loginWindow->close();  // Close login window
        } else {
            QMessageBox::warning(loginWindow, "Login Failed", "Invalid username or password.");
        }
    };

    // Handle login action on button click
    QObject::connect(loginButton, &QPushButton::clicked, tryLogin);

    // Detect Enter key press on username or password field
    QObject::connect(usernameEdit, &QLineEdit::returnPressed, tryLogin);
    QObject::connect(passwordEdit, &QLineEdit::returnPressed, tryLogin);

    loginWindow->setLayout(loginLayout);
    loginWindow->show();

    return app.exec();  // Keep the application running
}
