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

void Elevator::setDestinationFloors(const QMap<int, int> &floorPassengers) {
    if (m_passengerCount == 0 || m_isMoving)
        return;

    m_destinationFloors.clear();
    m_floorPassengers.clear();

    // Заполняем этажи назначения и количество пассажиров
    for (auto it = floorPassengers.begin(); it != floorPassengers.end(); ++it) {
        int floor = it.key();
        int passengers = it.value();

        if (floor >= 1 && floor <= m_totalFloors && floor != m_currentFloor && passengers > 0) {
            m_destinationFloors.append(floor);
            m_floorPassengers[floor] = passengers;
        }
    }

    // Сортируем этажи
    std::sort(m_destinationFloors.begin(), m_destinationFloors.end());

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

    // Двигаемся
    if (m_direction == "up" && m_currentFloor < m_totalFloors) {
        m_currentFloor++;
    } else if (m_direction == "down" && m_currentFloor > 1) {
        m_currentFloor--;
    } else {
        // Достиг границы — все выходят
        m_passengerCount = 0;
        m_destinationFloors.clear();
        m_floorPassengers.clear();
        m_isMoving = false;
        m_direction = "idle";
        emit stateChanged();
        return;
    }

    // Проверка: выходят ли пассажиры на текущем этаже
    if (m_floorPassengers.contains(m_currentFloor)) {
        int exitingPassengers = m_floorPassengers[m_currentFloor];
        m_passengerCount = std::max(0, m_passengerCount - exitingPassengers);
        m_floorPassengers.remove(m_currentFloor);

        // Удаляем этаж из destinationFloors
        m_destinationFloors.removeAll(m_currentFloor);
    }

    // ПРАВИЛО: на 1-м и последнем этаже ВСЕ выходят
    if (m_currentFloor == 1 || m_currentFloor == m_totalFloors) {
        m_passengerCount = 0;
        m_destinationFloors.clear();
        m_floorPassengers.clear();
        m_isMoving = false;
        m_direction = "idle";
        emit stateChanged();
        return;
    }

    // Остановка, если некуда ехать или нет пассажиров
    if (m_destinationFloors.isEmpty() || m_passengerCount == 0) {
        m_isMoving = false;
        if (m_passengerCount == 0) {
            m_direction = "idle";
            m_floorPassengers.clear();
        }
        emit stateChanged();
        return;
    }

    // Обновляем направление если нужно
    if (!m_destinationFloors.isEmpty()) {
        if (m_direction == "up" && m_currentFloor > m_destinationFloors.last()) {
            m_direction = "down";
        } else if (m_direction == "down" && m_currentFloor < m_destinationFloors.first()) {
            m_direction = "up";
        }
    }

    emit stateChanged();
}