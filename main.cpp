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

static void createTableAndInsertData(QSqlDatabase& db) {
    // Create table and insert data into SQLite
    QSqlQuery query(db);

    // Create table if not exists
    QString createTableSQL = "CREATE TABLE IF NOT EXISTS COMPANY ("
                             "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "NAME TEXT NOT NULL, "
                             "AGE INTEGER NOT NULL, "
                             "ADDRESS TEXT, "
                             "SALARY REAL)";
    if (!query.exec(createTableSQL)) {
        qDebug() << "Error creating table:" << query.lastError().text();
        return;
    }

    // Insert sample data (no need to provide the ID field)
    QString insertDataSQL = "INSERT INTO COMPANY (NAME, AGE, ADDRESS, SALARY) "
                            "VALUES ('John Doe', 30, '123 Elm St', 50000)";
    if (!query.exec(insertDataSQL)) {
        qDebug() << "Error inserting data:" << query.lastError().text();
    } else {
        qDebug() << "Data inserted successfully!";
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create SQLite database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qDebug() << "Error: Unable to open database";
        return 1;
    }
    qDebug() << "Database opened successfully";

    // Create table and insert sample data
    createTableAndInsertData(db);

    // Create main window
    QWidget window;
    window.setWindowTitle("Simple Qt Application with SQLite");

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout;

    // Create QLineEdit for user input
    QLineEdit *edit = new QLineEdit;
    edit->setPlaceholderText("Enter text here...");
    layout->addWidget(edit);

    // Create QPushButton to interact with database
    QPushButton *button = new QPushButton("Show Data");
    layout->addWidget(button);

    // Show database content when button is clicked
    QObject::connect(button, &QPushButton::clicked, [&]() {
        QSqlQuery query(db);
        query.exec("SELECT * FROM COMPANY");
        while (query.next()) {
            QString name = query.value(1).toString();
            int age = query.value(2).toInt();
            QString address = query.value(3).toString();
            double salary = query.value(4).toDouble();
            qDebug() << "Name:" << name << ", Age:" << age << ", Address:" << address << ", Salary:" << salary;
        }
    });

    // Set layout and show window
    window.setLayout(layout);
    window.show();

    return app.exec();
}
