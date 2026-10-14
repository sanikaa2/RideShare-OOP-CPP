
#include <iostream>
#include <string>
using namespace std;

// Passenger class
class Passenger {
private:
    string name;
    string phone;

public:
    // Parameterized constructor
    Passenger(string n, string p) {
        name = n;
        phone = p;
    }

    void displayDetails() {
        cout << "\n--- Passenger Details ---\n";
        cout << "Name: " << name << endl;
        cout << "Phone: " << phone << endl;
    }
};

// Driver class
class Driver {
private:
    string name;
    string vehicleNumber;
    string vehicleType;

public:
    // Parameterized constructor
    Driver(string n, string number, string type) {
        name = n;
        vehicleNumber = number;
        vehicleType = type;
    }

    void displayDetails() {
        cout << "\n--- Driver Details ---\n";
        cout << "Name: " << name << endl;
        cout << "Vehicle Number: " << vehicleNumber << endl;
        cout << "Vehicle Type: " << vehicleType << endl;
    }
};

int main() {
    string passengerName;
    string passengerPhone;

    cout << "=================================\n";
    cout << "       RIDESHARE SYSTEM\n";
    cout << "=================================\n";

    cout << "\nEnter passenger name: ";
    getline(cin, passengerName);

    cout << "Enter passenger phone: ";
    getline(cin, passengerPhone);

    Passenger passenger(passengerName, passengerPhone);

    Driver driver("Rahul Sharma", "MH-12-AB-1234", "Mini Car");

    passenger.displayDetails();
    driver.displayDetails();

    cout << "\nPassenger and driver setup complete!\n";

    return 0;
}
