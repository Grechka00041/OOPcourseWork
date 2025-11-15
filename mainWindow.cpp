#include "mainWindow.h"
#include "elevatorSystem.h"
#include "elevator.h" // <-- ВАЖНО: без этого будет ошибка "incomplete type"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, m_system(new ElevatorSystem(this))
, m_timer(nullptr)
, m_entrancesSpin(nullptr)
, m_floorsSpin(nullptr)
, m_capacitySpin(nullptr)
, m_applyButton(nullptr)
, m_callEntrance(nullptr)
, m_floorsContainer(nullptr)
, m_floorsLayout(nullptr)
, m_upButton(nullptr)
, m_downButton(nullptr)
, m_callButton(nullptr)
, m_setDestinationsButton(nullptr)
, m_stateLabel(nullptr)
, m_floorLabel(nullptr)
, m_directionLabel(nullptr)
, m_passengerCountLabel(nullptr)
, m_totalPassengersLabel(nullptr)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ========== ЛЕВАЯ КОЛОНКА: Окно параметров ==========
    QGroupBox *paramGroup = new QGroupBox("Окно параметров", this);
    QVBoxLayout *paramLayout = new QVBoxLayout(paramGroup);
    paramLayout->setSpacing(8);

    QHBoxLayout *entranceLayout = new QHBoxLayout();
    entranceLayout->addWidget(new QLabel("Число подъездов:", this));
    m_entrancesSpin = new QSpinBox(this);
    for (int i = 1; i <= m_entrancesSpin->value(); ++i) {
        m_elevatorMemory[i] = QMap<int, int>();
    }
    m_entrancesSpin->setRange(1, 10);
    m_entrancesSpin->setValue(2);
    entranceLayout->addWidget(m_entrancesSpin);
    paramLayout->addLayout(entranceLayout);

    QHBoxLayout *floorsLayout = new QHBoxLayout();
    floorsLayout->addWidget(new QLabel("Количество этажей:", this));
    m_floorsSpin = new QSpinBox(this);
    m_floorsSpin->setRange(2, 20);
    m_floorsSpin->setValue(9);
    floorsLayout->addWidget(m_floorsSpin);
    paramLayout->addLayout(floorsLayout);

    QHBoxLayout *capacityLayout = new QHBoxLayout();
    capacityLayout->addWidget(new QLabel("Грузоподъемность лифта:", this));
    m_capacitySpin = new QSpinBox(this);
    m_capacitySpin->setRange(1, 20);
    m_capacitySpin->setValue(4);
    capacityLayout->addWidget(m_capacitySpin);
    paramLayout->addLayout(capacityLayout);

    m_applyButton = new QPushButton("Применить", this);
    paramLayout->addWidget(m_applyButton);
    mainLayout->addWidget(paramGroup);

    // ========== СРЕДНЯЯ КОЛОНКА: Окно управления ==========
    QGroupBox *callGroup = new QGroupBox("Окно управления", this);
    QVBoxLayout *callLayout = new QVBoxLayout(callGroup);
    callLayout->setSpacing(10);

    QHBoxLayout *entranceCallLayout = new QHBoxLayout();
    entranceCallLayout->addWidget(new QLabel("Выбор подъезда:", this));
    m_callEntrance = new QSpinBox(this);
    m_callEntrance->setRange(1, 10);
    m_callEntrance->setValue(1);
    entranceCallLayout->addWidget(m_callEntrance);
    callLayout->addLayout(entranceCallLayout);
    callLayout->addWidget(new QLabel("Этажи:", this));

    m_floorsContainer = new QWidget(this);
    m_floorsLayout = new QGridLayout(m_floorsContainer);
    m_floorsLayout->setSpacing(4);
    m_floorsLayout->setContentsMargins(0, 0, 0, 0);
    callLayout->addWidget(m_floorsContainer);

    QHBoxLayout *dirLayout = new QHBoxLayout();
    dirLayout->addWidget(new QLabel("Направление движения:", this));
    m_upButton = new QPushButton("Вверх", this);
    m_downButton = new QPushButton("Вниз", this);
    m_upButton->setCheckable(true);
    m_downButton->setCheckable(true);
    connect(m_upButton, &QPushButton::clicked, this, [this](){
        m_upButton->setChecked(true);
        m_downButton->setChecked(false);
    });
    connect(m_downButton, &QPushButton::clicked, this, [this](){
        m_downButton->setChecked(true);
        m_upButton->setChecked(false);
    });
    dirLayout->addWidget(m_upButton);
    dirLayout->addWidget(m_downButton);
    callLayout->addLayout(dirLayout);

    QHBoxLayout *passLayout = new QHBoxLayout();
    passLayout->addWidget(new QLabel("Кол-во пассажиров:", this));
    m_passengerSpin = new QSpinBox(this);
    m_passengerSpin->setRange(1, m_capacitySpin->value());
    m_passengerSpin->setValue(qMin(3, m_capacitySpin->value()));
    passLayout->addWidget(m_passengerSpin);
    callLayout->addLayout(passLayout);

    m_callButton = new QPushButton("Вызвать лифт", this);
    callLayout->addWidget(m_callButton);

    m_setDestinationsButton = new QPushButton("Указать этажи назначения", this);
    m_setDestinationsButton->setEnabled(false);
    callLayout->addWidget(m_setDestinationsButton);

    mainLayout->addWidget(callGroup);

    // ========== ПРАВАЯ КОЛОНКА: Окно отображения ==========
    QGroupBox *displayGroup = new QGroupBox("Окно отображения", this);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);
    displayLayout->setSpacing(8);

    QHBoxLayout *stateLayout = new QHBoxLayout();
    stateLayout->addWidget(new QLabel("Состояние:", this));
    m_stateLabel = new QLabel("OFF", this);
    stateLayout->addWidget(m_stateLabel);
    displayLayout->addLayout(stateLayout);

    QHBoxLayout *floorLayout = new QHBoxLayout();
    floorLayout->addWidget(new QLabel("Текущий этаж:", this));
    m_floorLabel = new QLabel("1", this);
    floorLayout->addWidget(m_floorLabel);
    displayLayout->addLayout(floorLayout);

    QHBoxLayout *dirDisplayLayout = new QHBoxLayout();
    dirDisplayLayout->addWidget(new QLabel("Направление движения:", this));
    m_directionLabel = new QLabel("остановлен", this);
    dirDisplayLayout->addWidget(m_directionLabel);
    displayLayout->addLayout(dirDisplayLayout);

    QHBoxLayout *passDisplayLayout = new QHBoxLayout();
    passDisplayLayout->addWidget(new QLabel("Кол-во пассажиров:", this));
    m_passengerCountLabel = new QLabel("0", this);
    passDisplayLayout->addWidget(m_passengerCountLabel);
    displayLayout->addLayout(passDisplayLayout);

    mainLayout->addWidget(displayGroup);

    // ========== Подключение сигналов ==========
    connect(m_applyButton, &QPushButton::clicked, this, &MainWindow::onApply);
    connect(m_callButton, &QPushButton::clicked, this, &MainWindow::onCall);
    connect(m_callEntrance, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &MainWindow::updateDisplay);
    connect(m_setDestinationsButton, &QPushButton::clicked, this, &MainWindow::onSetDestinations);
    connect(m_upButton, &QPushButton::clicked, this, &MainWindow::onUp);
    connect(m_downButton, &QPushButton::clicked, this, &MainWindow::onDown);
    connect(m_system, &ElevatorSystem::elevatorStateChanged, this, &MainWindow::updateDisplay);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        Elevator *e = m_system->getElevator(m_callEntrance->value());
        if (e && e->isMoving()) {
            e->moveStep();
        }
    });

    updateFloorCheckboxes();
    updateDisplay();
    setWindowTitle("Имитация лифта");
}

MainWindow::~MainWindow() = default;
void MainWindow::onApply() {
    m_system->configure(
        m_entrancesSpin->value(),
        m_floorsSpin->value(),
        m_capacitySpin->value()
    );
    m_callEntrance->setRange(1, m_entrancesSpin->value());

    // Обновляем память лифтов
    m_elevatorMemory.clear();
    for (int i = 1; i <= m_entrancesSpin->value(); ++i) {
        m_elevatorMemory[i] = QMap<int, int>();
    }

    updateFloorCheckboxes();
    QMessageBox::information(this, "Параметры", "Применены новые параметры.");
}
void MainWindow::updateFloorCheckboxes() {
    // Очистка старых чекбоксов и спинбоксов
    if (m_floorsLayout) {
        QLayoutItem *child;
        while ((child = m_floorsLayout->takeAt(0)) != nullptr) {
            if (child->widget()) {
                delete child->widget();
            }
            delete child;
        }
    }

    m_floorBoxes.clear();
    m_passengerSpins.clear();

    // +++ ПРОВЕРКА НА НАЛИЧИЕ СИСТЕМЫ +++
    if (!m_system) return;

    const int floors = m_system->totalFloors(); // +++ ТЕПЕРЬ ПЕРЕМЕННАЯ ОПРЕДЕЛЕНА +++
    const int cols = 3;

    for (int i = 0; i < floors; ++i) {
        int floorNumber = i + 1;

        // Создаем контейнер для этажа
        QWidget *floorWidget = new QWidget(this);
        QHBoxLayout *floorLayout = new QHBoxLayout(floorWidget);
        floorLayout->setContentsMargins(0, 0, 0, 0);
        floorLayout->setSpacing(5);

        // Чекбокс этажа
        QCheckBox *cb = new QCheckBox(QString::number(floorNumber), this);
        m_floorBoxes.append(cb);
        floorLayout->addWidget(cb);

        // SpinBox для пассажиров
        QSpinBox *passSpin = new QSpinBox(this);
        passSpin->setRange(0, m_system->capacity());
        passSpin->setValue(0);
        passSpin->setEnabled(false);
        passSpin->setFixedWidth(50);
        m_passengerSpins.append(passSpin);
        floorLayout->addWidget(passSpin);
        floorLayout->addWidget(new QLabel("чел."));

        // Подключаем сигнал чекбокса
        connect(cb, &QCheckBox::toggled, this, [this, passSpin](bool checked) {
            if (passSpin) {
                passSpin->setEnabled(checked);
                if (!checked) {
                    passSpin->setValue(0);
                }
            }
            this->onFloorSelectionChanged();
        });

        // Подключаем сигнал SpinBox
        connect(passSpin, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &MainWindow::onFloorSelectionChanged);

        // Добавляем в сетку
        if (m_floorsLayout) {
            static_cast<QGridLayout*>(m_floorsLayout)->addWidget(floorWidget, i / cols, i % cols);
        }
    }
}
void MainWindow::onCall() {
    int entrance = m_callEntrance->value();

    // Находим этаж вызова (первый выбранный этаж)
    int callFloor = -1;
    for (int i = 0; i < m_floorBoxes.size(); ++i) {
        if (m_floorBoxes[i] && m_floorBoxes[i]->isChecked()) {
            callFloor = i + 1;
            break;
        }
    }

    if (callFloor == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите этаж вызова!");
        return;
    }

    // Проверка направления
    QString direction;
    if (m_upButton->isChecked()) {
        direction = "вверх";
    } else if (m_downButton->isChecked()) {
        direction = "вниз";
    } else {
        QMessageBox::warning(this, "Ошибка", "Выберите направление движения!");
        return;
    }

    // Количество пассажиров из спинбокса
    int passengers = m_passengerSpin->value();

    // Вызываем лифт (только вызов, без этажей назначения)
    m_system->callElevator(entrance, callFloor, passengers, direction);

    // Активируем кнопку для указания этажей назначения
    m_setDestinationsButton->setEnabled(true);

    // Сбрасываем выбор этажей, но запоминаем какой этаж был вызовом
    for (int i = 0; i < m_floorBoxes.size(); ++i) {
        if (m_floorBoxes[i]) {
            m_floorBoxes[i]->setChecked(false);
        }
        if (m_passengerSpins[i]) {
            m_passengerSpins[i]->setValue(0);
            m_passengerSpins[i]->setEnabled(false);
        }
    }

    // Показываем сообщение о успешном вызове
    QMessageBox::information(this, "Лифт вызван",
        QString("Лифт вызван на %1 этаж. Теперь укажите этажи назначения.").arg(callFloor));

    updateDisplay();
}
void MainWindow::onFloorSelectionChanged() {
    if (m_floorBoxes.isEmpty() || m_passengerSpins.isEmpty()) {
        return;
    }

    int totalPassengers = 0;
    int selectedFloors = 0;

    // Считаем общее количество пассажиров по выбранным этажам
    for (int i = 0; i < m_floorBoxes.size(); ++i) {
        if (m_floorBoxes[i] && m_floorBoxes[i]->isChecked()) {
            selectedFloors++;
            if (m_passengerSpins[i]) {
                totalPassengers += m_passengerSpins[i]->value();
            }
        }
    }

    // Блокируем кнопку если превышено количество пассажиров в лифте
    Elevator *e = m_system->getElevator(m_callEntrance->value());
    if (m_setDestinationsButton && e) {
        bool withinLimit = (totalPassengers <= e->passengerCount()) && (selectedFloors > 0);
        m_setDestinationsButton->setEnabled(withinLimit);

        if (totalPassengers > e->passengerCount()) {
            m_setDestinationsButton->setToolTip(
                QString("Превышено количество пассажиров! В лифте: %1, выбрано: %2")
                .arg(e->passengerCount()).arg(totalPassengers));
        } else {
            m_setDestinationsButton->setToolTip("");
        }
    }
}

void MainWindow::onSetDestinations() {
    Elevator *e = m_system->getElevator(m_callEntrance->value());
    if (!e || e->passengerCount() == 0) return;

    // Собираем этажи назначения с количеством пассажиров
    QMap<int, int> floorPassengers;
    int totalDestinationPassengers = 0;

    for (int i = 0; i < m_floorBoxes.size(); ++i) {
        if (m_floorBoxes[i] && m_floorBoxes[i]->isChecked()) {
            int floor = i + 1;
            int passengers = m_passengerSpins[i] ? m_passengerSpins[i]->value() : 0;

            if (floor != e->currentFloor() && passengers > 0) {
                floorPassengers[floor] = passengers;
                totalDestinationPassengers += passengers;
            }
        }
    }

    // Проверка: сумма пассажиров по этажам не должна превышать пассажиров в лифте
    if (totalDestinationPassengers > e->passengerCount()) {
        QMessageBox::warning(this, "Ошибка",
            QString("Сумма пассажиров по этажам (%1) превышает количество пассажиров в лифте (%2)!")
            .arg(totalDestinationPassengers).arg(e->passengerCount()));
        return;
    }

    if (floorPassengers.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите этажи назначения с пассажирами!");
        return;
    }

    // СОХРАНЯЕМ состояние для текущего подъезда
    int currentEntrance = m_callEntrance->value();
    m_elevatorMemory[currentEntrance] = floorPassengers;

    // Устанавливаем этажи назначения
    e->setDestinationFloors(floorPassengers);
    m_setDestinationsButton->setEnabled(false);

    if (!m_timer->isActive()) m_timer->start(1000);
    updateDisplay();
}

void MainWindow::onUp() {
    Elevator *e = m_system->getElevator(m_callEntrance->value());
    if (e) e->setDirection("вверх");
}

void MainWindow::onDown() {
    Elevator *e = m_system->getElevator(m_callEntrance->value());
    if (e) e->setDirection("вниз");
}

void MainWindow::updateDisplay() {
    Elevator *e = m_system->getElevator(m_callEntrance->value());
    if (!e) return;

    m_stateLabel->setText(e->isMoving() ? "ON" : "OFF");
    m_floorLabel->setText(QString::number(e->currentFloor()));
    m_directionLabel->setText(e->direction());
    m_passengerCountLabel->setText(QString::number(e->passengerCount()));

    // Восстанавливаем сохраненные этажи назначения для текущего подъезда
    int currentEntrance = m_callEntrance->value();
    if (m_elevatorMemory.contains(currentEntrance)) {
        QMap<int, int> savedFloors = m_elevatorMemory[currentEntrance];

        // Сбрасываем все чекбоксы
        for (int i = 0; i < m_floorBoxes.size(); ++i) {
            if (m_floorBoxes[i]) {
                m_floorBoxes[i]->setChecked(false);
            }
            if (m_passengerSpins[i]) {
                m_passengerSpins[i]->setValue(0);
                m_passengerSpins[i]->setEnabled(false);
            }
        }

        // Восстанавливаем сохраненные этажи
        for (auto it = savedFloors.begin(); it != savedFloors.end(); ++it) {
            int floor = it.key();
            int passengers = it.value();

            if (floor - 1 < m_floorBoxes.size() && m_floorBoxes[floor - 1]) {
                m_floorBoxes[floor - 1]->setChecked(true);
                if (floor - 1 < m_passengerSpins.size() && m_passengerSpins[floor - 1]) {
                    m_passengerSpins[floor - 1]->setValue(passengers);
                    m_passengerSpins[floor - 1]->setEnabled(true);
                }
            }
        }
    }
}