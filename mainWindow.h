#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QCheckBox>

class ElevatorSystem;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpinBox;
class QPushButton;
class QLabel;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onApply();
    void onCall();
    void onUp();
    void onDown();
    void updateDisplay();
    void updateFloorCheckboxes();

private:
    ElevatorSystem *m_system;
    QTimer *m_timer;

    // Параметры
    QSpinBox *m_entrancesSpin;
    QSpinBox *m_floorsSpin;
    QSpinBox *m_capacitySpin;
    QPushButton *m_applyButton;

    // Управление
    QSpinBox *m_callEntrance;
    QVector<QCheckBox*> m_floorBoxes;
    QWidget *m_floorsContainer;
    QLayout *m_floorsLayout; // может быть QGridLayout или QVBoxLayout
    QPushButton *m_upButton;
    QPushButton *m_downButton;
    QSpinBox *m_passengerSpin;
    QPushButton *m_callButton;

    // Отображение
    QLabel *m_stateLabel;
    QLabel *m_floorLabel;
    QLabel *m_directionLabel;
    QLabel *m_passengerCountLabel;
};

#endif // MAINWINDOW_H