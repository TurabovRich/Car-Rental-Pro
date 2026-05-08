# OOP Requirements Mapping

This file maps course requirements to concrete code in this project.

## 1) Classes & Objects
- Domain classes: `src/domain/Vehicle.h`, `src/domain/Customer.h`, `src/domain/Reservation.h`, `src/domain/Invoice.h`, `src/domain/UserAccount.h`
- Service classes: `src/service/RentalService.h`, `src/service/AuthService.h`
- UI classes: `src/ui/MainWindow.h`, `src/ui/tabs/*`, `src/ui/dialogs/*`

## 2) Constructors & Destructors
- Constructors are used throughout domain/service/ui classes.
- Virtual destructors:
  - `Vehicle::~Vehicle()` in `src/domain/Vehicle.h`
  - `Insurable::~Insurable()` in `src/domain/Vehicles.h`

## 3) Function Overloading
- `RentalService::findVehicle(int)` and `RentalService::findVehicle(const QString&)`
- `RentalService::processReturn(int, int, double)` and `RentalService::processReturn(int, const Date&, double)`

## 4) Operator Overloading
- `Customer::operator==` in `src/domain/Customer.h`
- `operator<<` for `Vehicle` and `Customer`
- `Date::operator<` and `Date::operator==` in `src/utils/Date.h/.cpp`

## 5) Polymorphism (Runtime & Compile-time)
- Runtime polymorphism:
  - `Vehicle::dailyRate()` overridden by `Sedan/SUV/Truck/Electric/PremiumSUV`
- Compile-time polymorphism:
  - Templated CSV helpers in `src/utils/CsvUtils.h` (`toCell<T>`, `makeRow(...)`)

## 6) Inheritance
- Single + hierarchical:
  - `Vehicle` -> `Sedan/SUV/Truck/Electric`
- Multiple inheritance:
  - `PremiumSUV : public SUV, public Insurable` in `src/domain/Vehicles.h`

## 7) Static Members
- `Reservation::nextId` in `src/domain/Reservation.h` and initialized in `src/service/RentalService.cpp`

## 8) File Handling
- CSV read/write in `src/storage/FileManager.cpp`
- Local files: `data/cars.csv`, `data/customers.csv`, `data/reservations.csv`, `data/invoices.csv`, `data/users.csv`

## 9) Exception Handling
- Custom exceptions: `src/utils/Exceptions.h`
- try/catch in UI actions (`dialogs`, `tabs`) and startup in `src/main.cpp`

## 10) GUI
- Qt Widgets app:
  - Auth dialog (`src/ui/dialogs/AuthDialog.*`)
  - Role-based main window (`src/ui/MainWindow.*`)
  - Admin tabs + User tabs (`src/ui/tabs/*`)

