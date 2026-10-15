# RideShare — C++ OOP Project

A desktop ride-booking prototype built with **C++17** and **Qt Widgets**. It demonstrates core object-oriented programming concepts through passenger, vehicle, and ride management.

## Features
- Dashboard, ride booking, ride history, and journey monitoring screens
- Mini, Sedan, and SUV fare calculation
- Input validation and exception handling
- Ride status updates: Booked, In Progress, Completed, Cancelled, and Interrupted
- Local ride-history persistence using a JSON file in the application's data directory

## OOP concepts demonstrated
- **Class and object:** `Passenger`, `Vehicle`, `Mini`, `Sedan`, `SUV`, `Ride`, and `MainWindow`
- **Encapsulation:** private data members with public getters and controlled status updates
- **Inheritance:** `Mini`, `Sedan`, and `SUV` inherit from `Vehicle`
- **Abstraction:** `Vehicle` declares pure virtual functions
- **Runtime polymorphism:** vehicle fare and description functions are overridden and called through `Vehicle`
- **Constructors:** parameterized constructors initialize passenger and ride data
- **Static data member and static member function:** `Ride::totalBookings` and `Ride::getTotalRides()`
- **Friend function:** `getRideFareDetails(const Ride&)` accesses private ride data
- **Exception handling:** booking validation uses `try`, `throw`, and `catch`

## Requirements
- Qt 5 or Qt 6 with the **Widgets** component
- CMake 3.16 or later
- A compatible C++17 compiler

## Open and run in Qt Creator
1. Extract the project folder.
2. Open `CMakeLists.txt` in Qt Creator.
3. Select a Qt kit with a compatible compiler.
4. Configure the project, then Build and Run.

The project is CMake-based. Do not upload generated `build/` output or Qt Creator's local user settings to source control.

## Data storage note
Ride history is saved locally in the operating system's application-data directory as `rides.json`. It is not cloud-synced.

## Main source files
- `main.cpp` — starts the Qt application and displays the main window
- `mainwindow.h` / `mainwindow.cpp` — UI, navigation, booking workflow, journey timer, validation, and persistence
- `rideshare.h` — passenger, vehicle hierarchy, ride model, fare breakdown, and OOP examples
- `CMakeLists.txt` — build configuration
- `.gitignore` — excludes generated build output and local Qt Creator settings
