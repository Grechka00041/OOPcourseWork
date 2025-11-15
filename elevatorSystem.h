#ifndef ELEVATORSYSTEM_H
#define ELEVATORSYSTEM_H

#include <QObject>
#include <QVector>

class Elevator;

class ElevatorSystem : public QObject
{
    Q_OBJECT

public:
    explicit ElevatorSystem(QObject *parent = nullptr);
    void configure(int entrances, int floors, int capacity);

    void callElevator(int entrance, int floor, int passengers, const QString &direction);
    Elevator* getElevator(int entrance) const;

    int totalEntrances() const;
    int totalFloors() const;
    int capacity() const;

    signals:
        void elevatorStateChanged();
    void configurationChanged();

private:
    int m_entrances;
    int m_floors;
    int m_capacity;
    QVector<Elevator*> m_elevators;
};

#endif // ELEVATORSYSTEM_H