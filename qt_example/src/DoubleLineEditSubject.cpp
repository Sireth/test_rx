/**
 * @file LineEditSubject.cpp
 *
 * @date 27.03.2025
 * @author mltya.usov04\@gmail.com
 *
 * Copyright (c) 2025 Sireth
 */

#include "DoubleLineEditSubject.h"

#include <QEvent>
#include <QDoubleValidator>

DoubleLineEditSubject::DoubleLineEditSubject(QWidget *parent)
    : QLineEdit(parent) {
    init();

    connect(this, &DoubleLineEditSubject::textEdited, this, &DoubleLineEditSubject::validateValue);
    /// Создаем событие, что сейчас пользователь не в режиме редактирования
    m_events.get_subscriber().on_next(EventType::FocusOut);
}

void DoubleLineEditSubject::setValue(double value) const { m_value.get_subscriber().on_next(value); }

double DoubleLineEditSubject::value() const { return m_value.get_value(); }

rxcpp::observable<double> DoubleLineEditSubject::observable() const {
    return m_value.get_observable().distinct_until_changed();
}

bool DoubleLineEditSubject::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::FocusIn:
            m_events.get_subscriber().on_next(EventType::FocusIn);
            break;
        case QEvent::FocusOut:
            m_events.get_subscriber().on_next(EventType::FocusOut);
            break;
        default: {
            break;
        }
    }
    return QLineEdit::event(event);
}

void DoubleLineEditSubject::init() {
    setText("0");
    /// Значения которые обновляются программно, а не пользователем во время ввода
    const auto to_update =
        m_events.get_observable()
            .filter([=](const EventType event) {
                return event == EventType::EditedFinished || event == EventType::FocusOut;
            })
            /// Значения, пока пользователь не перешел в режимм редакторования
            .map([=](EventType) {
                return m_value.get_observable().distinct_until_changed().take_until(m_events.get_observable().filter(
                    [](const EventType event) { return event == EventType::FocusIn; }));
            })
            .switch_on_next();

    /// Обновляем текст новыми значениями
    to_update.subscribe([this](const double a) { setText(QString::number(a)); });
    setupValidator();
}

void DoubleLineEditSubject::setupValidator() {
    m_validator = new QDoubleValidator(this);

    m_validator->setRange(-100'000, 100'000);
    QLocale locale(QLocale::C);
    locale.setNumberOptions(QLocale::RejectGroupSeparator); // Запретить разделители тысяч (',')
    m_validator->setLocale(locale);
    setValidator(m_validator);
}

void DoubleLineEditSubject::validateValue() const {
    int pos     = 0;
    QString tmp = text();
    if (m_validator->validate(tmp, pos) == QValidator::Acceptable) {
        m_value.get_subscriber().on_next(tmp.toDouble());
    } else {
        m_value.get_subscriber().on_next(0.0);
    }
}
