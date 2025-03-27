/**
 * @file main.cpp
 *
 * @date 27.03.2025
 * @author mltya.usov04\@gmail.com
 *
 * Copyright (c) 2025 Sireth
 */


#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return QApplication::exec();
}
