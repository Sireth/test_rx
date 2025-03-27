/**
 * @file DoubleLineEditSubject.h
 *
 * @date 27.03.2025
 * @author mltya.usov04\@gmail.com
 *
 * Copyright (c) 2025 Sireth
 */

#ifndef LINEEDITSUBJECT_H
#define LINEEDITSUBJECT_H

#include <QLineEdit>
#include <rxcpp/rx.hpp>


class QDoubleValidator;

class DoubleLineEditSubject final : public QLineEdit {
    Q_OBJECT

public:
    explicit DoubleLineEditSubject(QWidget *parent = nullptr);

    void setValue(double value) const;
    [[nodiscard]] double value() const;

    [[nodiscard]] rxcpp::observable<double> observable() const;

protected:
    bool event(QEvent *event) override;

private:
    void init();
    void setupValidator();

    void validateValue() const;

private:
    enum class EventType {
        FocusIn,
        FocusOut,
        Edited,
        EditedFinished,
        EventTypeSize
    };

    rxcpp::subjects::behavior<double> m_value{0.0};
    rxcpp::subjects::subject<EventType> m_events;

    QDoubleValidator *m_validator{};
};


#endif // LINEEDITSUBJECT_H
