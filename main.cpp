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
                            "('user', 'user123', 'user')";
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
    QWidget loginWindow;
    loginWindow.setWindowTitle("Login");

    QVBoxLayout *loginLayout = new QVBoxLayout;

    // Pola do wprowadzania nazwy użytkownika i hasła
    QLineEdit *usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Username");
    loginLayout->addWidget(usernameEdit);

    QLineEdit *passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginLayout->addWidget(passwordEdit);

    QPushButton *loginButton = new QPushButton("Login");
    loginLayout->addWidget(loginButton);

    // Akcja po kliknięciu przycisku logowania
    QObject::connect(loginButton, &QPushButton::clicked, [&]() {
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        QString role;

        if (validateLogin(db, username, password, role)) {
            QMessageBox::information(&loginWindow, "Login Successful", "Welcome " + username + "!");
            
            // W zależności od roli, otwieramy odpowiedni panel
            if (role == "admin") {
                // Okno administratora
                QWidget adminWindow;
                adminWindow.setWindowTitle("Admin Panel");

                QVBoxLayout *adminLayout = new QVBoxLayout;
                adminLayout->addWidget(new QPushButton("Admin Options"));
                adminWindow.setLayout(adminLayout);
                adminWindow.show();
            } else if (role == "user") {
                // Okno użytkownika
                QWidget userWindow;
                userWindow.setWindowTitle("User Panel");

                QVBoxLayout *userLayout = new QVBoxLayout;
                userLayout->addWidget(new QPushButton("User Options"));
                userWindow.setLayout(userLayout);
                userWindow.show();
            }

            loginWindow.close();  // Zamykamy okno logowania
        } else {
            QMessageBox::warning(&loginWindow, "Login Failed", "Invalid username or password.");
        }
    });

    loginWindow.setLayout(loginLayout);
    loginWindow.show();

    return app.exec();
}
