
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

// 1. Passenger class - Encapsulation
class Passenger {
private:
    string name, phone;

public:
    Passenger(string n = "Guest", string p = "Not provided") {
        name = n;
        phone = p;
    }

    string getName() { return name; }
    string getPhone() { return phone; }

    void display() {
        cout << "\nPassenger: " << name
             << "\nPhone: " << phone << endl;
    }
};

// 2. Driver class - Encapsulation and constructor
class Driver {
private:
    string name, vehicleNumber;

public:
    Driver(string n, string v) {
        name = n;
        vehicleNumber = v;
    }

    void display() {
        cout << "Driver: " << name
             << "\nVehicle Number: " << vehicleNumber << endl;
    }
};

// 3. Abstract class - Abstraction
class Vehicle {
protected:
    string vehicleName;
    double ratePerKm;

public:
    Vehicle(string name, double rate) {
        vehicleName = name;
        ratePerKm = rate;
    }

    virtual double calculateFare(double distance) = 0;

    virtual void displayVehicle() {
        cout << vehicleName;
    }

    virtual ~Vehicle() {}
};

// 4. Single inheritance and runtime polymorphism
class Mini : public Vehicle {
public:
    Mini() : Vehicle("Mini", 12.0) {}

    double calculateFare(double distance) override {
        return 40 + distance * ratePerKm;
    }
};

class Sedan : public Vehicle {
public:
    Sedan() : Vehicle("Sedan", 18.0) {}

    double calculateFare(double distance) override {
        return 60 + distance * ratePerKm;
    }
};

class SUV : public Vehicle {
public:
    SUV() : Vehicle("SUV", 25.0) {}

    double calculateFare(double distance) override {
        return 100 + distance * ratePerKm;
    }
};

// 5. Ride class
class Ride {
private:
    Passenger passenger;
    Driver driver;
    string pickup, destination;
    double distance, fare;
    Vehicle* vehicle;
    bool booked;

    // 6. Static data member
    static int totalRides;

public:
    Ride() : passenger(), driver("Rahul Sharma", "MH-12-AB-1234") {
        distance = 0;
        fare = 0;
        vehicle = nullptr;
        booked = false;
    }

    void bookRide() {
        if (booked) {
            cout << "\nPlease cancel the current ride first.\n";
            return;
        }

        string name, phone;

        cout << "\nEnter passenger name: ";
        getline(cin >> ws, name);

        cout << "Enter phone number: ";
        getline(cin, phone);

        cout << "Enter pickup location: ";
        getline(cin, pickup);

        cout << "Enter destination: ";
        getline(cin, destination);

        cout << "Enter distance in km: ";
        cin >> distance;

        if (distance <= 0 || distance > 1000) {
            cout << "Invalid distance. Enter 0 to 1000 km, excluding 0.\n";
            return;
        }

        int choice;
        cout << "\n--- Choose Your Vehicle ---\n";
        cout << "1. Mini  - Rs. 12/km + Rs. 40 base fare\n";
        cout << "2. Sedan - Rs. 18/km + Rs. 60 base fare\n";
        cout << "3. SUV   - Rs. 25/km + Rs. 100 base fare\n";
        cout << "Enter choice: ";
        cin >> choice;

        Mini mini;
        Sedan sedan;
        SUV suv;

        switch (choice) {
            case 1: vehicle = &mini; break;
            case 2: vehicle = &sedan; break;
            case 3: vehicle = &suv; break;
            default:
                cout << "Invalid vehicle choice.\n";
                vehicle = nullptr;
                return;
        }

        passenger = Passenger(name, phone);
        fare = vehicle->calculateFare(distance);
        booked = true;
        totalRides++;

        cout << fixed << setprecision(2);
        cout << "\n========== RIDE CONFIRMED ==========\n";
        cout << "Passenger: " << passenger.getName() << endl;
        cout << "Pickup: " << pickup << endl;
        cout << "Destination: " << destination << endl;
        cout << "Distance: " << distance << " km\n";
        cout << "Vehicle: ";
        vehicle->displayVehicle();
        cout << "\nFare: Rs. " << fare << endl;
        cout << "====================================\n";
    }

    void showRide() {
        if (!booked) {
            cout << "\nNo active ride. Please book a ride first.\n";
            return;
        }

        cout << "\n========== RIDE DETAILS ==========\n";
        passenger.display();
        driver.display();
        cout << "Pickup: " << pickup
             << "\nDestination: " << destination
             << "\nDistance: " << distance << " km\n";
        cout << "Vehicle: ";
        vehicle->displayVehicle();
        cout << "\nFare: Rs. " << fixed << setprecision(2)
             << fare << endl;

        // 8. Friend function
        showFareDetails(*this);

        cout << "==================================\n";
    }

    void cancelRide() {
        if (!booked) {
            cout << "\nNo active ride to cancel.\n";
            return;
        }

        booked = false;
        vehicle = nullptr;
        cout << "\nRide cancelled successfully.\n";
    }

    // 7. Static member function
    static void showTotalRides() {
        cout << "\nTotal rides booked during this session: "
             << totalRides << endl;
    }

    // Friend function can access private members
    friend void showFareDetails(Ride r);
};

int Ride::totalRides = 0;

// Friend function
void showFareDetails(Ride r) {
    cout << "Fare breakdown: Base fare + distance-based charge\n";
    cout << "Final payable amount: Rs. "
         << fixed << setprecision(2) << r.fare << endl;
}

// Main menu
int main() {
    Ride ride;
    int choice;

    do {
        cout << "\n";
        cout << "====================================\n";
        cout << "          RIDESHARE APP\n";
        cout << "      Your Ride, Your Way!\n";
        cout << "====================================\n";
        cout << "1. Book a Ride\n";
        cout << "2. View Ride Details\n";
        cout << "3. Cancel Ride\n";
        cout << "4. Show Total Rides\n";
        cout << "5. Exit\n";
        cout << "------------------------------------\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a valid number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                ride.bookRide();
                break;
            case 2:
                ride.showRide();
                break;
            case 3:
                ride.cancelRide();
                break;
            case 4:
                Ride::showTotalRides();
                break;
            case 5:
                cout << "\nThank you for using RideShare!\n";
                break;
            default:
                cout << "\nInvalid choice. Try again.\n";
        }

    } while (choice != 5);

    return 0;
}
