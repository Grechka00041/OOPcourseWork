#ifndef PASSENGER_H
#define PASSENGER_H

enum Direction { FIRST, LAST }; // FIRST = вниз, LAST = вверх

class Passenger {
public:
    Passenger(int destFloor, Direction dir) : m_destFloor(destFloor), m_dir(dir) {}
    int getDestFloor() const { return m_destFloor; }
    Direction getDirection() const { return m_dir; }

private:
    int m_destFloor;
    Direction m_dir;
};

#endif // PASSENGER_H