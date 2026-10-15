
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "rideshare.h"

class QStackedWidget;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QTableWidget;
class QSpinBox;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    QVector<Ride> rides;
    int activeRideIndex = -1;
    int elapsedSeconds = 0;

    // Ride persistence
    QString storageFilePath() const;
    void saveRides();
    void loadRides();

    QStackedWidget *pages = nullptr;
    QLabel *fareValue = nullptr;
    QLabel *distanceValue = nullptr;
    QLabel *rideCountValue = nullptr;
    QLabel *activeStatusValue = nullptr;
    QLabel *timerValue = nullptr;
    QLabel *tripSummaryValue = nullptr;

    QLineEdit *nameInput = nullptr;
    QLineEdit *phoneInput = nullptr;
    QLineEdit *pickupInput = nullptr;
    QLineEdit *destinationInput = nullptr;
    QComboBox *vehicleInput = nullptr;
    QTableWidget *historyTable = nullptr;
    QTimer *journeyTimer = nullptr;

    QWidget *makeDashboardPage();
    QWidget *makeBookingPage();
    QWidget *makeHistoryPage();
    QWidget *makeJourneyPage();

    void navigateTo(int index);
    void updateFare();
    void updateDashboard();
    void refreshHistory();

    double estimateDistance(
        const QString &pickup,
        const QString &destination
    ) const;

    const Vehicle *selectedVehicle(QString &type) const;

    void bookRide();
    void cancelSelectedRide();
    void showRideDetails(int index);
    void startJourney();
    void finishJourney();
    void updateJourneyTimer();
};

#endif
