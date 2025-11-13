#ifndef ELEVATORSYSTEM_H
#define ELEVATORSYSTEM_H

#include <QObject>
#include <QVector>
#include "elevator.h"

class ElevatorSystem : public QObject
{
    Q_OBJECT

public:
    explicit ElevatorSystem(QObject *parent = nullptr);
    void configure(int entrances, int floors, int capacity);
    void callElevator(int entrance, int floor, int passengers);

    Elevator* getElevator(int entrance) const;

    int totalEntrances() const;
    int totalFloors() const;
    int capacity() const;

    signals:
        void configurationChanged();
    void elevatorStateChanged();

private:
    int m_entrances;
    int m_floors;
    int m_capacity;
    QVector<Elevator*> m_elevators;
};

#endif // ELEVATORSYSTEM_H