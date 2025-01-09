#include "persor.cpp"

static void createUsersTableAndInsertData(QSqlDatabase& db) {
    QSqlQuery query(db);

    // Create the USERS table if it doesn't exist
    QString createUsersTableSQL = "CREATE TABLE IF NOT EXISTS USERS ("
                                  "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "USERNAME TEXT NOT NULL UNIQUE, "
                                  "PASSWORD TEXT NOT NULL, "
                                  "ROLE TEXT NOT NULL)";
    if (!query.exec(createUsersTableSQL)) {
        qDebug() << "Error creating USERS table:" << query.lastError().text();
        return;
    }
	
    QString insertUsersDataSQL = "INSERT INTO USERS (USERNAME, PASSWORD, ROLE) "
                                 "VALUES ('admin', 'admin123', 'admin'), "
                                 "('user', 'user123', 'user'), "
                                 "('Jan', 'Janek123', 'user')";
    if (!query.exec(insertUsersDataSQL)) {
        qDebug() << "Error inserting data into USERS table:" << query.lastError().text();
    } else {
        qDebug() << "Sample users inserted!";
    }

    // Create the TICKET_LIFETIME table if it doesn't exist
    QString createTicketLifetimeTableSQL = "CREATE TABLE IF NOT EXISTS TICKET_LIFETIME ("
                                           "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                           "DURATION TEXT NOT NULL, "
                                           "STUDENT_PRICE REAL NOT NULL, "
                                           "PENSIONER_PRICE REAL NOT NULL, "
                                           "NORMAL_PRICE REAL NOT NULL)";
    if (!query.exec(createTicketLifetimeTableSQL)) {
        qDebug() << "Error creating TICKET_LIFETIME table:" << query.lastError().text();
        return;
    }

    // Insert predefined ticket lifetimes and pricing
    QString insertTicketLifetimeDataSQL = "INSERT INTO TICKET_LIFETIME (DURATION, STUDENT_PRICE, PENSIONER_PRICE, NORMAL_PRICE) "
                                          "VALUES "
                                          "('1 day', 10.00, 12.00, 15.00), "
                                          "('3 days', 25.00, 30.00, 35.00), "
                                          "('5 days', 40.00, 45.00, 50.00), "
                                          "('7 days', 55.00, 60.00, 65.00), "
                                          "('14 days', 70.00, 75.00, 80.00), "
                                          "('30 days', 96.00, 110.00, 120.00), "
                                          "('90 days', 250.00, 270.00, 300.00), "
                                          "('semester', 450.00, 480.00, 500.00)";
    if (!query.exec(insertTicketLifetimeDataSQL)) {
        qDebug() << "Error inserting data into TICKET_LIFETIME table:" << query.lastError().text();
    } else {
        qDebug() << "Ticket lifetime data inserted!";
    }

    // Create the TICKETS table if it doesn't exist
    QString createTicketsTableSQL = "CREATE TABLE IF NOT EXISTS TICKETS ("
                                    "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "USER_ID INTEGER NOT NULL, "
                                    "ACTIVATION_DATE TEXT NOT NULL, "
                                    "TICKET_LIFETIME_ID INTEGER NOT NULL, "
                                    "TICKET_TYPE TEXT NOT NULL, "
                                    "FOREIGN KEY (USER_ID) REFERENCES USERS(ID), "
                                    "FOREIGN KEY (TICKET_LIFETIME_ID) REFERENCES TICKET_LIFETIME(ID))";
    if (!query.exec(createTicketsTableSQL)) {
        qDebug() << "Error creating TICKETS table:" << query.lastError().text();
        return;
    }

    qDebug() << "TICKET_LIFETIME and TICKETS tables created successfully!";
}

void insertNewUser(QSqlDatabase& db, const QString& username, const QString& password) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO USERS (USERNAME, PASSWORD, ROLE) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue("user");  // Automatically assign the 'user' role

    if (!query.exec()) {
        qDebug() << "Error inserting new user:" << query.lastError().text();
        return;
    }

    qDebug() << "New user inserted successfully!";
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
	Admin ppp (user_name + "'s dashboard", db);    
}

void showUserDashboard(QString user_name, QSqlDatabase& db) {
	// acces db to user for simple user usage HERE
	User uuu (user_name, db);
}
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qDebug() << "Error: Unable to open database";
        return 1;
    }
    qDebug() << "Database opened successfully";

    createUsersTableAndInsertData(db);

    // Create login window
    QWidget* loginWindow = new QWidget();
    loginWindow->setWindowTitle("Login");
    loginWindow->setFixedWidth(400);
    loginWindow->setFixedHeight(220);

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

    QPushButton* signUpButton = new QPushButton("Sign Up");
    loginLayout->addWidget(signUpButton);

    // Login validation function
    auto tryLogin = [&]() {
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        QString role;

        if (validateLogin(db, username, password, role)) {
            if (role == "admin") {
                showAdminDashboard(username, db);
            } else {
                showUserDashboard(username, db);
            }
            loginWindow->close(); // Close login window
        } else {
            QMessageBox::warning(loginWindow, "Login Failed", "Invalid username or password.");
        }
    };

    QObject::connect(loginButton, &QPushButton::clicked, tryLogin);

    // Sign Up window logic
    QObject::connect(signUpButton, &QPushButton::clicked, [&]() {
        QWidget* signUpWindow = new QWidget();
        signUpWindow->setWindowTitle("Sign Up");
        signUpWindow->setFixedWidth(400);
        signUpWindow->setFixedHeight(300);  // Increased height for new fields

        QVBoxLayout* signUpLayout = new QVBoxLayout;

        QLineEdit* newUsernameEdit = new QLineEdit;
        newUsernameEdit->setPlaceholderText("New Username (3-20 characters)");
        signUpLayout->addWidget(newUsernameEdit);

        QLineEdit* newPasswordEdit = new QLineEdit;
        newPasswordEdit->setPlaceholderText("New Password (min 6 characters)");
        newPasswordEdit->setEchoMode(QLineEdit::Password);
        signUpLayout->addWidget(newPasswordEdit);

        QLineEdit* confirmPasswordEdit = new QLineEdit;
        confirmPasswordEdit->setPlaceholderText("Confirm Password");
        confirmPasswordEdit->setEchoMode(QLineEdit::Password);
        signUpLayout->addWidget(confirmPasswordEdit);

        QPushButton* signUpSubmitButton = new QPushButton("Submit");
        signUpLayout->addWidget(signUpSubmitButton);

        QPushButton* cancelSubmitButton = new QPushButton("Cancel");
        signUpLayout->addWidget(cancelSubmitButton);
        
        // Add validation function
        auto validateSignUp = [](const QString& username, const QString& password, const QString& confirmPass) -> QString {
            if (username.length() < 3 || username.length() > 20) {
                return "Username must be between 3 and 20 characters.";
            }
            
            // Check username for valid characters (letters, numbers, underscore)
            QRegExp usernameRegex("^[a-zA-Z0-9_]+$");
            if (!usernameRegex.exactMatch(username)) {
                return "Username can only contain letters, numbers, and underscores.";
            }

            if (password.length() < 6) {
                return "Password must be at least 6 characters long.";
            }

            if (password != confirmPass) {
                return "Passwords do not match.";
            }

            return QString(); // Empty string means validation passed
        };

        QObject::connect(cancelSubmitButton, &QPushButton::clicked, [signUpWindow, loginWindow]() {
            signUpWindow->deleteLater();  // Properly delete the window
            loginWindow->show();
        });

        QObject::connect(signUpSubmitButton, &QPushButton::clicked, [=, &db]() {
            QString newUsername = newUsernameEdit->text();
            QString newPassword = newPasswordEdit->text();
            QString confirmPassword = confirmPasswordEdit->text();

            // Validate input
            QString validationError = validateSignUp(newUsername, newPassword, confirmPassword);
            
            if (!validationError.isEmpty()) {
                QMessageBox::warning(signUpWindow, "Sign Up Failed", validationError);
                return;
            }

            // Check if username already exists
            QSqlQuery checkQuery(db);
            checkQuery.prepare("SELECT COUNT(*) FROM USERS WHERE USERNAME = ?");
            checkQuery.addBindValue(newUsername);
            
            if (checkQuery.exec() && checkQuery.next()) {
                if (checkQuery.value(0).toInt() > 0) {
                    QMessageBox::warning(signUpWindow, "Sign Up Failed", "Username already exists.");
                    return;
                }
            }

            // If all validation passes, insert the new user
            insertNewUser(db, newUsername, newPassword);
            QMessageBox::information(signUpWindow, "Success", "Account created successfully!");
            signUpWindow->deleteLater();  // Properly delete the window
            loginWindow->show();
        });

        signUpWindow->setLayout(signUpLayout);
        signUpWindow->show();
        loginWindow->close();
    });

    QObject::connect(usernameEdit, &QLineEdit::returnPressed, tryLogin);
    QObject::connect(passwordEdit, &QLineEdit::returnPressed, tryLogin);

    loginWindow->setLayout(loginLayout);
    loginWindow->show();

    return app.exec();  // Run the application
}