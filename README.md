# CarRentalPro (Qt Widgets, C++17)

A small car rental management demo app:
- Cars CRUD (add/delete)
- Customers CRUD (add/delete)
- Reservations (create)
- Returns (process return: late fee + damage fee)
- Reports (invoice list + total revenue)

Persistence: CSV files in `data/`.

## Build
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```
Run `CarRentalPro`. It loads/saves CSV automatically.

## Run
```bash 
mkdir build
cd build
cmake ..
make
./CarRentalPro# Car-Rental-Pro
