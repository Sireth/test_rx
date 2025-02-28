#include <QApplication>
#include <QVBoxLayout>
#include <QValidator>
#include <QLineEdit>
#include <QLabel>

#include <rxcpp/rx.hpp>


enum class EventType {
    FocusIn,
    FocusOut,
    Edited,
    EditedFinished,
    EventTypeSize
};

class EventFilter final : public QObject {
    Q_OBJECT

public:
    explicit EventFilter(QObject *parent, rxcpp::subscriber<EventType> subscriber)
        : QObject(parent)
        , m_subscriber(std::move(subscriber)) { }

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    rxcpp::subscriber<EventType> m_subscriber;
};

bool EventFilter::eventFilter(QObject *watched, QEvent *event) {
    switch (event->type()) {
        case QEvent::FocusIn:
            m_subscriber.on_next(EventType::FocusIn);
            break;
        case QEvent::FocusOut:
            m_subscriber.on_next(EventType::FocusOut);
            break;
        default: {
            break;
        }
    }
    return QObject::eventFilter(watched, event);
}

class MainWindow final : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr)
        : QWidget(parent) {
        // Создание UI элементов
        auto *layout = new QVBoxLayout(this);

        aEdit  = new QLineEdit("0", this);
        bEdit  = new QLineEdit("0", this);
        cLabel = new QLabel("0", this);

        layout->addWidget(new QLabel("a:"));
        layout->addWidget(aEdit);
        layout->addWidget(new QLabel("b:"));
        layout->addWidget(bEdit);
        layout->addWidget(new QLabel("c:"));
        layout->addWidget(cLabel);

        // Инициализация Subjects
        aSubject.get_subscriber().on_next(0.0);
        bSubject.get_subscriber().on_next(0.0);

        auto source1 = aSubject.get_observable();
        auto source2 = bSubject.get_observable();

        auto a_events = rxcpp::subjects::subject<EventType>();
        auto b_events = rxcpp::subjects::subject<EventType>();

        aEdit->installEventFilter(new EventFilter(this, a_events.get_subscriber()));
        bEdit->installEventFilter(new EventFilter(this, b_events.get_subscriber()));

        auto sum = source1.combine_latest([](const double a, const double b) { return a + b; }, source2)
                       .distinct_until_changed();


        connect(aEdit, &QLineEdit::textEdited, [this] { setSubject(aEdit, aSubject); });

        connect(bEdit, &QLineEdit::textEdited, [this] { setSubject(bEdit, bSubject); });

        auto aObs = aSubject.get_observable().distinct_until_changed();

        auto to_update_a = a_events.get_observable()
                               .filter([=](const EventType event) {
                                   return event == EventType::EditedFinished || event == EventType::FocusOut;
                               })
                               .map([=](EventType) {
                                   return aObs.take_until(a_events.get_observable().filter(
                                       [](const EventType event) { return event == EventType::FocusIn; }));
                               })
                               .switch_on_next();

        aObs.subscribe([this](const double a) { bSubject.get_subscriber().on_next(a * 2); });

        auto bObs = bSubject.get_observable().distinct_until_changed();

        auto to_update_b = b_events.get_observable()
                               .filter([=](const EventType event) {
                                   return event == EventType::EditedFinished || event == EventType::FocusOut;
                               })
                               .map([=](EventType) {
                                   return bObs.take_until(b_events.get_observable().filter(
                                       [](const EventType event) { return event == EventType::FocusIn; }));
                               })
                               .switch_on_next();

        bObs.subscribe([this](const double a) { aSubject.get_subscriber().on_next(a / 2); });

        to_update_a.subscribe([this](const double a) { aEdit->setText(QString::number(a)); });

        to_update_b.subscribe([this](const double a) { bEdit->setText(QString::number(a)); });

        sum.subscribe([this](const double c) { cLabel->setText(QString::number(c)); });
        a_events.get_subscriber().on_next(EventType::FocusOut);
        b_events.get_subscriber().on_next(EventType::FocusOut);
    }

private:
    QLineEdit *aEdit, *bEdit;
    QLabel *cLabel;

    rxcpp::subjects::behavior<double> aSubject{0.0};
    rxcpp::subjects::behavior<double> bSubject{0.0};

    static void setSubject(const QLineEdit *edit, const rxcpp::subjects::behavior<double> &subject) {
        auto validator = QDoubleValidator();
        validator.setRange(-100'000, 100'000);
        validator.setLocale(QLocale::C);
        int pos     = 0;
        QString tmp = edit->text();
        if (validator.validate(tmp, pos) == QValidator::Acceptable) {
            subject.get_subscriber().on_next(tmp.toDouble());
        } else {
            subject.get_subscriber().on_next(0.0);
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(300, 200);
    window.show();
    return QApplication::exec();
}

#include "main.moc"
