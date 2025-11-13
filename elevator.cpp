#include "elevator.h"
#include <algorithm>
#include <QDebug>

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
    m_destinations.clear();
    emit stateChanged();
}

void Elevator::callToFloor(int floor, int passengers) {
    if (floor < 1 || floor > m_totalFloors) return;
    if (passengers > m_maxCapacity) return;

    m_destinations.push_back(floor);
    m_passengerCount = passengers;
    m_isMoving = true;

    // Определяем начальное направление
    if (floor > m_currentFloor) m_direction = "up";
    else if (floor < m_currentFloor) m_direction = "down";
    else m_direction = "idle";

    emit stateChanged();
}

void Elevator::setDirection(const QString &dir) {
    if (dir == "вверх") m_direction = "up";
    else if (dir == "вниз") m_direction = "down";
    emit stateChanged();
}

void Elevator::moveStep() {
    if (!m_isMoving || m_destinations.empty()) {
        m_isMoving = false;
        if (m_passengerCount == 0) m_direction = "idle";
        emit stateChanged();
        return;
    }

    // Движение
    if (m_direction == "up" && m_currentFloor < m_totalFloors) m_currentFloor++;
    else if (m_direction == "down" && m_currentFloor > 1) m_currentFloor--;
    else {
        // Разворот
        m_direction = (m_direction == "up") ? "down" : "up";
        moveStep(); // рекурсивно продолжить в этом же шаге
        return;
    }

    // Проверка остановки
    auto it = std::find(m_destinations.begin(), m_destinations.end(), m_currentFloor);
    if (it != m_destinations.end()) {
        m_destinations.erase(it);
        // На первом или последнем этаже — все выходят
        if (m_currentFloor == 1 || m_currentFloor == m_totalFloors) {
            m_passengerCount = 0;
            m_destinations.clear();
        } else {
            m_passengerCount = std::max(0, m_passengerCount - 1);
        }
    }

    // Если назначений нет — останавливаемся
    if (m_destinations.empty()) {
        m_isMoving = false;
        if (m_passengerCount == 0) m_direction = "idle";
    }

    emit stateChanged();
}