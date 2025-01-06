#include "persor.cpp"

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

void showAdminDashboard(QString user_name, QSqlDatabase& db, QWidget* loginWindow) {
    Admin ppp(user_name + "'s dashboard", db, loginWindow);    
}

void showUserDashboard(QString user_name, QWidget* loginWindow) {
    User uuu(user_name + "'s dashboard", loginWindow);
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
                showAdminDashboard(username, db, loginWindow);  // Show Admin Dashboard with display data button
            } else if (role == "user") {
                showUserDashboard(username, loginWindow);  // Show User Dashboard
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
