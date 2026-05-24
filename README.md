# CarRentalPro

Desktop car rental app built with **Qt Widgets** and **C++17**.

- **Admin** — cars, customers, reservations, returns, revenue report  
- **User** — search available cars, reserve, return, profile, rental history  

All records are saved as CSV files in a `data/` folder (created next to the built binary on first run).

## Default admin account

| Field    | Value  |
|----------|--------|
| Username | `admin` |
| Password | `admin` |

On first launch the app creates this account if no admin exists yet.  
User registration adds a row to `customers.csv` and a linked account in `users.csv` (passwords stored as SHA-256 hex, not plain text).

## Data files

| File              | Contents                          |
|-------------------|-----------------------------------|
| `cars.csv`        | Fleet                             |
| `customers.csv`   | Customer profiles                 |
| `reservations.csv`| Bookings                          |
| `invoices.csv`    | Charges after a return            |
| `users.csv`       | Login accounts                    |

## Build and run

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./CarRentalPro
```

Requires **CMake 3.16+** and **Qt 5 or 6** with the Widgets module.
