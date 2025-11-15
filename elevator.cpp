#include "elevator.h"
#include <algorithm>

Elevator::Elevator(int maxCapacity, int totalFloors, QObject *parent)
    : QObject(parent)
    , m_maxCapacity(maxCapacity)
    , m_totalFloors(totalFloors)
    , m_currentFloor(1)
    , m_direction("idle")
    , m_passengerCount(0)
    , m_isMoving(false)
{
}

int Elevator::currentFloor() const { return m_currentFloor; }

QString Elevator::direction() const {
    if (m_direction == "up") return "вверх";
    if (m_direction == "down") return "вниз";
    return "остановлен";
}

int Elevator::passengerCount() const { return m_passengerCount; }

bool Elevator::isMoving() const { return m_isMoving; }

bool Elevator::isIdle() const { return !m_isMoving && m_passengerCount == 0; }

void Elevator::reset() {
    m_currentFloor = 1;
    m_direction = "idle";
    m_passengerCount = 0;
    m_isMoving = false;
    m_destinationFloors.clear();
    emit stateChanged();
}

void Elevator::callToFloor(int floor, int passengers, const QString &direction) {
    if (floor < 1 || floor > m_totalFloors || passengers <= 0 || passengers > m_maxCapacity)
        return;
    if (!isIdle())
        return;

    // === ИСПРАВЛЕНО: мгновенное перемещение на этаж вызова ===
    m_currentFloor = floor;
    m_passengerCount = passengers;
    m_isMoving = false;
    m_direction = "idle";
    m_destinationFloors.clear();
    emit stateChanged();
}

void Elevator::setDestinationFloors(const QVector<int> &floors) {
    if (m_passengerCount == 0 || m_isMoving)
        return;

    m_destinationFloors.clear();
    for (int f : floors) {
        if (f >= 1 && f <= m_totalFloors && f != m_currentFloor) {
            m_destinationFloors.append(f);
        }
    }
    std::sort(m_destinationFloors.begin(), m_destinationFloors.end());
    m_destinationFloors.erase(
        std::unique(m_destinationFloors.begin(), m_destinationFloors.end()),
        m_destinationFloors.end()
    );

    if (!m_destinationFloors.isEmpty()) {
        m_isMoving = true;
        m_direction = (m_destinationFloors.first() > m_currentFloor) ? "up" : "down";
    }
    emit stateChanged();
}

void Elevator::setDirection(const QString &dir) {
    if (dir == "вверх") m_direction = "up";
    else if (dir == "вниз") m_direction = "down";
    emit stateChanged();
}

void Elevator::moveStep() {
    if (!m_isMoving || m_destinationFloors.isEmpty())
        return;

    if (m_direction == "up" && m_currentFloor < m_totalFloors) {
        m_currentFloor++;
    } else if (m_direction == "down" && m_currentFloor > 1) {
        m_currentFloor--;
    } else {
        // Достиг границы — все выходят
        m_passengerCount = 0;
        m_destinationFloors.clear();
        m_isMoving = false;
        m_direction = "idle";
        emit stateChanged();
        return;
    }

    // === ПРАВИЛО: на 1-м и последнем этаже ВСЕ выходят ===
    if (m_currentFloor == 1 || m_currentFloor == m_totalFloors) {
        m_passengerCount = 0;
        m_destinationFloors.clear();
        m_isMoving = false;
        m_direction = "idle";
        emit stateChanged();
        return;
    }

    // Проверка: выходит ли кто-то
    auto it = std::find(m_destinationFloors.begin(), m_destinationFloors.end(), m_currentFloor);
    if (it != m_destinationFloors.end()) {
        m_destinationFloors.erase(it);
        m_passengerCount = std::max(0, m_passengerCount - 1);
    }

    // Остановка, если некуда ехать
    if (m_destinationFloors.isEmpty()) {
        m_isMoving = false;
        if (m_passengerCount == 0) m_direction = "idle";
    }

    emit stateChanged();
}