#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QObject>
#include <QVector>
#include <QMap>

class Elevator : public QObject
{
    Q_OBJECT

public:
    explicit Elevator(int maxCapacity, int totalFloors, QObject *parent = nullptr);

    int currentFloor() const;
    QString direction() const;
    int passengerCount() const;
    bool isMoving() const;
    bool isIdle() const;
    void reset();
    QMap<int, int> getFloorPassengers() const { return m_floorPassengers; }
    void callToFloor(int floor, int passengers, const QString &direction);
    void setDestinationFloors(const QMap<int, int> &floorPassengers);
    void setDirection(const QString &dir);
    void moveStep();

    signals:
        void stateChanged();

private:
    int m_maxCapacity;
    int m_totalFloors;
    int m_currentFloor;
    QString m_direction; // "up", "down", "idle"
    int m_passengerCount;
    bool m_isMoving;
    QVector<int> m_destinationFloors;
    QMap<int, int> m_floorPassengers;
};

#endif // ELEVATOR_H