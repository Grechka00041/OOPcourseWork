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
    updateFloorCheckboxes(); // Обновить чекбоксы под новое число этажей
    QMessageBox::information(this, "Параметры", "Применены новые параметры.");
}

void MainWindow::updateFloorCheckboxes() {
    // Очистка старых чекбоксов
    QLayoutItem *child;
    while ((child = m_floorsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    m_floorBoxes.clear();

    const int floors = m_system->totalFloors();
    const int cols = 5; // Максимум 5 чекбоксов в строке

    for (int i = 0; i < floors; ++i) {
        QCheckBox *cb = new QCheckBox(QString::number(i + 1), this);
        m_floorBoxes.append(cb);
        static_cast<QGridLayout*>(m_floorsLayout)->addWidget(cb, i / cols, i % cols);
    }
}

void MainWindow::onCall() {
    int entrance = m_callEntrance->value();
    int passengers = m_passengerSpin->value();

    QList<int> selectedFloors;
    for (int i = 0; i < m_floorBoxes.size(); ++i) {
        if (m_floorBoxes[i]->isChecked()) {
            selectedFloors.append(i + 1);
        }
    }
    if (selectedFloors.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите этаж вызова!");
        return;
    }
    if (passengers > m_system->capacity()) {
        QMessageBox::warning(this, "Ошибка", "Превышена грузоподъёмность!");
        return;
    }
    QString dir;
    if (m_upButton->isChecked()) dir = "вверх";
    else if (m_downButton->isChecked()) dir = "вниз";
    else {
        QMessageBox::warning(this, "Ошибка", "Укажите направление!");
        return;
    }

    int targetFloor = selectedFloors.first();
    m_system->callElevator(entrance, targetFloor, passengers, dir);

    m_setDestinationsButton->setEnabled(true);
    updateDisplay();
}

void MainWindow::onSetDestinations() {
    Elevator *e = m_system->getElevator(m_callEntrance->value());
    if (!e || e->passengerCount() == 0) return;

    QVector<int> dests;
    for (int i = 0; i < m_floorBoxes.size(); ++i) {
        if (m_floorBoxes[i]->isChecked()) {
            int f = i + 1;
            if (f != e->currentFloor()) {
                dests.append(f);
            }
        }
    }
    if (dests.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите хотя бы один этаж назначения!");
        return;
    }

    e->setDestinationFloors(dests);
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
}