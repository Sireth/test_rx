/**
 * @file MainWindow.h
 *
 * @date 27.03.2025
 * @author mltya.usov04\@gmail.com
 *
 * Copyright (c) 2025 Sireth
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>


class QLabel;
class DoubleLineEditSubject;

class MainWindow final : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    DoubleLineEditSubject *m_aEdit;
    DoubleLineEditSubject *m_bEdit;

    QLabel *m_result;
};


#endif // MAINWINDOW_H
