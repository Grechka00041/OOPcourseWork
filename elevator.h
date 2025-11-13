#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QObject>
#include <vector>

class Elevator : public QObject
{
    Q_OBJECT

public:
    explicit Elevator(int maxCapacity, int totalFloors, QObject *parent = nullptr);

    // Состояние
    int currentFloor() const;
    QString direction() const;
    int passengerCount() const;
    bool isMoving() const;
    bool isIdle() const;

    // Управление
    void callToFloor(int floor, int passengers);
    void setDirection(const QString &dir);
    void moveStep(); // перемещение на 1 этаж
    void reset();

    signals:
        void stateChanged();

private:
    void stopAtFloor(int floor);
    void updateDirection();

    int m_maxCapacity;
    int m_totalFloors;
    int m_currentFloor;
    QString m_direction; // "up", "down", "idle"
    int m_passengerCount;
    bool m_isMoving;
    std::vector<int> m_destinations;
};

#endif // ELEVATOR_H