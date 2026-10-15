#ifndef RIDESHARE_H
#define RIDESHARE_H

#include <QString>
#include <QDateTime>
#include <QVector>

// Class and Object + Encapsulation
class Passenger
{
private:
    QString name;
    QString phone;

public:
    Passenger(const QString &n, const QString &p) : name(n), phone(p) {}
    QString getName() const { return name; }
    QString getPhone() const { return phone; }
};

// Abstraction
class Vehicle
{
public:
    virtual ~Vehicle() = default;
    virtual double calculateFare(double km) const = 0;
    virtual QString getType() const = 0;
    virtual QString getDescription() const = 0;
    virtual double getBaseFare() const = 0;
    virtual double getPerKmRate() const = 0;
};

// Inheritance + Runtime polymorphism
class Mini : public Vehicle
{
public:
    double calculateFare(double km) const override { return 40.0 + 12.0 * km; }
    QString getType() const override { return "Mini"; }
    QString getDescription() const override { return "Affordable • 4 seats"; }
    double getBaseFare() const override { return 40.0; }
    double getPerKmRate() const override { return 12.0; }
};

class Sedan : public Vehicle
{
public:
    double calculateFare(double km) const override { return 60.0 + 18.0 * km; }
    QString getType() const override { return "Sedan"; }
    QString getDescription() const override { return "Comfort • 4 seats"; }
    double getBaseFare() const override { return 60.0; }
    double getPerKmRate() const override { return 18.0; }
};

class SUV : public Vehicle
{
public:
    double calculateFare(double km) const override { return 100.0 + 25.0 * km; }
    QString getType() const override { return "SUV"; }
    QString getDescription() const override { return "Space • 6 seats"; }
    double getBaseFare() const override { return 100.0; }
    double getPerKmRate() const override { return 25.0; }
};


class Ride
{
private:
    Passenger passenger;
    QString pickup;
    QString destination;
    QString vehicleType;
    double distanceKm;
    double fare;
    QString status;
    QDateTime bookedAt;

    static int totalBookings;

public:
    // Constructor for a new ride
    Ride(const Passenger &p, const QString &from, const QString &to,
         const QString &type, double km, double amount)
        : passenger(p),
          pickup(from),
          destination(to),
          vehicleType(type),
          distanceKm(km),
          fare(amount),
          status("Booked"),
          bookedAt(QDateTime::currentDateTime())
    {
        ++totalBookings;
    }

    // Constructor for restoring a saved ride
    Ride(const Passenger &p, const QString &from, const QString &to,
         const QString &type, double km, double amount,
         const QString &savedStatus, const QDateTime &savedBookedAt)
        : passenger(p),
          pickup(from),
          destination(to),
          vehicleType(type),
          distanceKm(km),
          fare(amount),
          status(savedStatus),
          bookedAt(savedBookedAt)
    {
        ++totalBookings;
    }

    QString getPassengerName() const { return passenger.getName(); }
    QString getPhone() const { return passenger.getPhone(); }
    QString getPickup() const { return pickup; }
    QString getDestination() const { return destination; }
    QString getVehicleType() const { return vehicleType; }
    double getDistance() const { return distanceKm; }
    double getFare() const { return fare; }
    QString getStatus() const { return status; }
    QDateTime getBookedAt() const { return bookedAt; }

    void setStatus(const QString &newStatus)
    {
        status = newStatus;
    }

    QString getSummary() const
    {
        return "Passenger: " + passenger.getName()
            + "\nPhone: " + passenger.getPhone()
            + "\nPickup: " + pickup
            + "\nDestination: " + destination
            + "\nVehicle: " + vehicleType
            + "\nEstimated distance: "
            + QString::number(distanceKm, 'f', 1) + " km"
            + "\nFare: Rs. "
            + QString::number(fare, 'f', 2)
            + "\nStatus: " + status
            + "\nBooked: "
            + bookedAt.toString("dd MMM yyyy, hh:mm AP");
    }

    static int getTotalRides()
    {
        return totalBookings;
    }

    friend QString getRideFareDetails(const Ride &ride);
};

inline int Ride::totalBookings = 0;

inline QString getRideFareDetails(const Ride &ride)
{
    return "FARE BREAKDOWN\n"
        "Vehicle: " + ride.vehicleType
        + "\nEstimated distance: " + QString::number(ride.distanceKm, 'f', 1) + " km"
        + "\nTotal fare: Rs. " + QString::number(ride.fare, 'f', 2);
}

#endif
