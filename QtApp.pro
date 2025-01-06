QT += core gui sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtApp
TEMPLATE = app

SOURCES += main.cpp \
           persor.cpp  # Ensure persor.cpp is included here