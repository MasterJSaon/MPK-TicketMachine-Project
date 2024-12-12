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

static void createUsersTableAndInsertData(QSqlDatabase& db) {
    // Tworzenie tabeli USERS, jeśli nie istnieje, oraz dodanie przykładowych użytkowników
    QSqlQuery query(db);

    // Tworzenie tabeli USERS (z rolami: admin, user)
    QString createTableSQL = "CREATE TABLE IF NOT EXISTS USERS ("
                             "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "USERNAME TEXT NOT NULL UNIQUE, "
                             "PASSWORD TEXT NOT NULL, "
                             "ROLE TEXT NOT NULL)";
    if (!query.exec(createTableSQL)) {
        qDebug() << "Error creating table:" << query.lastError().text();
        return;
    }

    // Wstawianie przykładowych użytkowników
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
    // Walidacja danych logowania
    QSqlQuery query(db);
    query.prepare("SELECT ROLE FROM USERS WHERE USERNAME = :username AND PASSWORD = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        role = query.value(0).toString();  // Zapisujemy rolę użytkownika
        return true;
    }
    return false;
}

void showAdminDashboard(QString user_name) {
    // Admin dashboard window
    QWidget* adminWindow = new QWidget();
    adminWindow -> setWindowTitle("" + user_name + "'s Dashboard");

    QVBoxLayout *adminLayout = new QVBoxLayout;
    adminLayout->addWidget(new QPushButton("Manage Users"));
    adminLayout->addWidget(new QPushButton("View Logs"));
    adminLayout->addWidget(new QPushButton("Admin Settings"));
    adminWindow->setLayout(adminLayout);
    adminWindow->setFixedSize(300, 200);
    adminWindow->show();
}

void showUserDashboard(QString user_name) {
    // User dashboard window
    QWidget* userWindow = new QWidget();
    userWindow->setWindowTitle("" + user_name + "'s Dashboard");

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

    // Połączenie z bazą danych SQLite
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qDebug() << "Error: Unable to open database";
        return 1;
    }
    qDebug() << "Database opened successfully";

    // Tworzenie tabeli użytkowników i dodanie przykładowych danych
    createUsersTableAndInsertData(db);

    // Tworzenie okna logowania
    QWidget* loginWindow = new QWidget();
    loginWindow->setWindowTitle("Login");
    loginWindow->setFixedWidth(400);
    loginWindow->setFixedHeight(180);

    QVBoxLayout* loginLayout = new QVBoxLayout;

    // Pola do wprowadzania nazwy użytkownika i hasła
    QLineEdit* usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Username");
    loginLayout->addWidget(usernameEdit);

    QLineEdit* passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginLayout->addWidget(passwordEdit);

    QPushButton* loginButton = new QPushButton("Login");
    loginLayout->addWidget(loginButton);

    // Akcja po kliknięciu przycisku logowania
    QObject::connect(loginButton, &QPushButton::clicked, [&]() {
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        QString role;

        if (validateLogin(db, username, password, role)) {
            QMessageBox::information(loginWindow, "Login Successful", "Welcome " + username + "!");
            
            // W zależności od roli, otwieramy odpowiedni dashboard
            if (role == "admin") {
                showAdminDashboard(username);  // Show Admin Dashboard
            } else if (role == "user") {
                showUserDashboard(username);  // Show User Dashboard
            }

            loginWindow->close();  // Close the login window after successful login
        } else {
            QMessageBox::warning(loginWindow, "Login Failed", "Invalid username or password.");
        }
    });

    loginWindow->setLayout(loginLayout);
    loginWindow->show();

    return app.exec();  // Keep the application running
}
