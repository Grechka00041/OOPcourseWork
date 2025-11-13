#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "elevatorSystem.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onApply();
    void onCall();
    void onUp();
    void onDown();
    void updateDisplay();

private:
    // Верхняя панель — параметры
    QSpinBox *m_entrancesSpin;
    QSpinBox *m_floorsSpin;
    QSpinBox *m_capacitySpin;
    QPushButton *m_applyButton;

    // Средняя панель — вызов
    QSpinBox *m_callEntrance;
    QVector<QCheckBox*> m_floorBoxes;
    QPushButton *m_upButton;
    QPushButton *m_downButton;
    QSpinBox *m_passengerSpin;
    QPushButton *m_callButton;

    // Нижняя панель — отображение
    QLabel *m_stateLabel;
    QLabel *m_floorLabel;
    QLabel *m_directionLabel;
    QLabel *m_passengerCountLabel;

    ElevatorSystem *m_system;
    QTimer *m_timer;
};

#endif // MAINWINDOW_H