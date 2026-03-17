# CarRentalPro (Qt Widgets, C++17)

A small local-first car rental demo app with two experiences:
- **Admin panel**: cars/customers CRUD, reservations/returns, reports
- **User app**: browse available cars by date range, reserve, return, profile, history

## Docs

Start here: `docs/ARCHITECTURE.md`

## Persistence (local)

All data is stored as CSV files inside a `data/` directory (next to the built binary, or `./data` in dev):
- `cars.csv`
- `customers.csv`
- `reservations.csv`
- `invoices.csv`
- `users.csv` (accounts)

Important: `users.csv` stores **password hashes** (`SHA-256` hex), not raw passwords.

## Authentication

- On first run, the app ensures a default admin exists:
  - username: `admin`
  - password: `admin`
- Registering a normal user creates:
  - a `Customer` record in `customers.csv`
  - a `UserAccount` record in `users.csv` linked via `customerId`

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
./CarRentalPro
