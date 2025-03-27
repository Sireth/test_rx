/**
 * @file MainWindow.cpp
 *
 * @date 27.03.2025
 * @author mltya.usov04\@gmail.com
 *
 * Copyright (c) 2025 Sireth
 */

#include "MainWindow.h"

#include "DoubleLineEditSubject.h"

#include <QLabel>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    m_aEdit  = new DoubleLineEditSubject(this);
    m_bEdit  = new DoubleLineEditSubject(this);
    m_result = new QLabel("0", this);

    layout->addWidget(new QLabel("a:"));
    layout->addWidget(m_aEdit);
    layout->addWidget(new QLabel("b:"));
    layout->addWidget(m_bEdit);
    layout->addWidget(new QLabel("c:"));
    layout->addWidget(m_result);

    const auto a_values = m_aEdit->observable();
    const auto b_values = m_bEdit->observable();
    const auto sum      = a_values.combine_latest([](const double a, const double b) { return a + b; }, b_values)
                         .distinct_until_changed();

    a_values.subscribe([this](const double a) { m_bEdit->setValue(a / 2); });
    b_values.subscribe([this](const double b) { m_aEdit->setValue(b * 2); });
    sum.subscribe([this](const double c) { m_result->setText(QString::number(c)); });
}
