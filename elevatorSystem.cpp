#include "elevatorSystem.h"
#include "elevator.h"

ElevatorSystem::ElevatorSystem(QObject *parent)
    : QObject(parent)
    , m_entrances(2)
    , m_floors(9)
    , m_capacity(4)
{
    configure(m_entrances, m_floors, m_capacity);
}

void ElevatorSystem::configure(int entrances, int floors, int capacity) {
    m_entrances = entrances;
    m_floors = floors;
    m_capacity = capacity;

    qDeleteAll(m_elevators);
    m_elevators.clear();

    for (int i = 0; i < m_entrances; ++i) {
        Elevator *e = new Elevator(m_capacity, m_floors, this);
        connect(e, &Elevator::stateChanged, this, &ElevatorSystem::elevatorStateChanged);
        m_elevators.append(e);
    }

    emit configurationChanged();
}

void ElevatorSystem::callElevator(int entrance, int floor, int passengers, const QString &direction) {
    if (entrance < 1 || entrance > m_entrances) return;
    Elevator *e = m_elevators[entrance - 1];
    e->callToFloor(floor, passengers, direction);
}

Elevator* ElevatorSystem::getElevator(int entrance) const {
    if (entrance < 1 || entrance > m_entrances) return nullptr;
    return m_elevators[entrance - 1];
}

int ElevatorSystem::totalEntrances() const { return m_entrances; }
int ElevatorSystem::totalFloors() const { return m_floors; }
int ElevatorSystem::capacity() const { return m_capacity; }