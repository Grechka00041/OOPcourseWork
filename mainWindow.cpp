#include "mainWindow.h"
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

    // Основная ГОРИЗОНТАЛЬНАЯ раскладка — для трёх колонок
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ========== ЛЕВАЯ КОЛОНКА: Окно параметров (всё вертикально) ==========
    QGroupBox *paramGroup = new QGroupBox("Окно параметров", this);
    QVBoxLayout *paramLayout = new QVBoxLayout(paramGroup);
    paramLayout->setSpacing(8);

    // Число подъездов
    QHBoxLayout *entranceLayout = new QHBoxLayout();
    entranceLayout->addWidget(new QLabel("Число подъездов:", this));
    m_entrancesSpin = new QSpinBox(this);
    m_entrancesSpin->setRange(1, 10);
    m_entrancesSpin->setValue(2);
    entranceLayout->addWidget(m_entrancesSpin);
    paramLayout->addLayout(entranceLayout);

    // Количество этажей
    QHBoxLayout *floorsLayout = new QHBoxLayout();
    floorsLayout->addWidget(new QLabel("Количество этажей:", this));
    m_floorsSpin = new QSpinBox(this);
    m_floorsSpin->setRange(2, 20);
    m_floorsSpin->setValue(9);
    floorsLayout->addWidget(m_floorsSpin);
    paramLayout->addLayout(floorsLayout);

    // Грузоподъёмность
    QHBoxLayout *capacityLayout = new QHBoxLayout();
    capacityLayout->addWidget(new QLabel("Грузоподъемность лифта:", this));
    m_capacitySpin = new QSpinBox(this);
    m_capacitySpin->setRange(1, 20);
    m_capacitySpin->setValue(4);
    capacityLayout->addWidget(m_capacitySpin);
    paramLayout->addLayout(capacityLayout);

    // Кнопка "Применить"
    m_applyButton = new QPushButton("Применить", this);
    paramLayout->addWidget(m_applyButton);

    // Добавляем левую колонку
    mainLayout->addWidget(paramGroup);

    // ========== СРЕДНЯЯ КОЛОНКА: Окно управления (всё вертикально) ==========
    QGroupBox *callGroup = new QGroupBox("Окно управления", this);
    QVBoxLayout *callLayout = new QVBoxLayout(callGroup);
    callLayout->setSpacing(10);

    // Выбор подъезда
    QHBoxLayout *entranceCallLayout = new QHBoxLayout();
    entranceCallLayout->addWidget(new QLabel("Выбор подъезда:", this));
    m_callEntrance = new QSpinBox(this);
    m_callEntrance->setRange(1, 10);
    m_callEntrance->setValue(1);
    entranceCallLayout->addWidget(m_callEntrance);
    callLayout->addLayout(entranceCallLayout);

    // Заголовок для этажей
    callLayout->addWidget(new QLabel("Этажи вызова:", this));

    // Чекбоксы этажей — размещаем в сетке 3x3 для компактности
    QGridLayout *floorGrid = new QGridLayout();
    for (int i = 0; i < 9; ++i) {
        QCheckBox *cb = new QCheckBox(QString::number(i + 1), this);
        m_floorBoxes.append(cb);
        floorGrid->addWidget(cb, i / 3, i % 3); // 3 столбца
    }
    callLayout->addLayout(floorGrid);

    // Направление движения
    QHBoxLayout *dirLayout = new QHBoxLayout();
    dirLayout->addWidget(new QLabel("Направление движения:", this));
    m_upButton = new QPushButton("Вверх", this);
    m_downButton = new QPushButton("Вниз", this);
    dirLayout->addWidget(m_upButton);
    dirLayout->addWidget(m_downButton);
    callLayout->addLayout(dirLayout);

    // Количество пассажиров
    QHBoxLayout *passLayout = new QHBoxLayout();
    passLayout->addWidget(new QLabel("Кол-во пассажиров:", this));
    m_passengerSpin = new QSpinBox(this);
    m_passengerSpin->setRange(1, 20);
    m_passengerSpin->setValue(3);
    passLayout->addWidget(m_passengerSpin);
    callLayout->addLayout(passLayout);

    // Кнопка "Вызвать лифт"
    m_callButton = new QPushButton("Вызвать лифт", this);
    callLayout->addWidget(m_callButton);

    // Добавляем среднюю колонку
    mainLayout->addWidget(callGroup);

    // ========== ПРАВАЯ КОЛОНКА: Окно отображения (всё вертикально) ==========
    QGroupBox *displayGroup = new QGroupBox("Окно отображения", this);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);
    displayLayout->setSpacing(8);

    // Состояние
    QHBoxLayout *stateLayout = new QHBoxLayout();
    stateLayout->addWidget(new QLabel("Состояние:", this));
    m_stateLabel = new QLabel("OFF", this);
    stateLayout->addWidget(m_stateLabel);
    displayLayout->addLayout(stateLayout);

    // Текущий этаж
    QHBoxLayout *floorLayout = new QHBoxLayout();
    floorLayout->addWidget(new QLabel("Текущий этаж:", this));
    m_floorLabel = new QLabel("1", this);
    floorLayout->addWidget(m_floorLabel);
    displayLayout->addLayout(floorLayout);

    // Направление движения
    QHBoxLayout *dirDisplayLayout = new QHBoxLayout();
    dirDisplayLayout->addWidget(new QLabel("Направление движения:", this));
    m_directionLabel = new QLabel("остановлен", this);
    dirDisplayLayout->addWidget(m_directionLabel);
    displayLayout->addLayout(dirDisplayLayout);

    // Количество пассажиров
    QHBoxLayout *passDisplayLayout = new QHBoxLayout();
    passDisplayLayout->addWidget(new QLabel("Кол-во пассажиров:", this));
    m_passengerCountLabel = new QLabel("0", this);
    passDisplayLayout->addWidget(m_passengerCountLabel);
    displayLayout->addLayout(passDisplayLayout);

    // Добавляем правую колонку
    mainLayout->addWidget(displayGroup);

    // ========== Подключение сигналов ==========
    connect(m_applyButton, &QPushButton::clicked, this, &MainWindow::onApply);
    connect(m_callButton, &QPushButton::clicked, this, &MainWindow::onCall);
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

    updateDisplay();
    setWindowTitle("Имитация лифта");
}

// ... остальной код (слоты и деструктор) ОСТАЁТСЯ БЕЗ ИЗМЕНЕНИЙ

MainWindow::~MainWindow() = default;

// --- Слоты ---
void MainWindow::onApply() {
    m_system->configure(
        m_entrancesSpin->value(),
        m_floorsSpin->value(),
        m_capacitySpin->value()
    );
    // Обновляем максимум для подъездов
    m_callEntrance->setRange(1, m_entrancesSpin->value());
    QMessageBox::information(this, "Параметры", "Применены новые параметры.");
}

void MainWindow::onCall() {
    int entrance = m_callEntrance->value();
    int passengers = m_passengerSpin->value();
    int floor = -1;

    for (int i = 0; i < m_floorBoxes.size(); ++i) {
        if (m_floorBoxes[i]->isChecked()) {
            floor = i + 1; // этажи с 1
            break;
        }
    }

    if (floor == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите этаж!");
        return;
    }

    if (passengers > m_system->capacity()) {
        QMessageBox::warning(this, "Ошибка", "Слишком много пассажиров!");
        return;
    }

    m_system->callElevator(entrance, floor, passengers);
    m_timer->start(1000);
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