# CarRentalPro architecture (Qt Widgets, CSV persistence)

## Big picture

This app is intentionally "single-process, local-first":

- UI: Qt Widgets (`src/ui/**`)
- Domain models: simple value types (`src/domain/**`)
- Business logic: `RentalService` and `AuthService` (`src/service/**`)
- Persistence: CSV files in a `data/` directory (`src/storage/FileManager.*`)

There is no server. All state is loaded from CSV on startup and saved back to CSV
after each write operation (and again on exit as a safety net).

## Data flow

### Startup

`main.cpp`:

- Builds the `dataDir` (either next to the binary or `./data`).
- Constructs `FileManager`, then `RentalService` and `AuthService`.
- Loads vehicles/customers/reservations/invoices via `RentalService::load()`.
- Loads user accounts via `AuthService::load()` and ensures at least one admin exists.
- Shows `AuthDialog` (Login/Register).
- Routes the user to:
  - **Admin window** if role is `Admin`
  - **User window** if role is `User` (linked to one `Customer` by `customerId`)

### CRUD + booking

`RentalService` is the only place that mutates rental data:

- Vehicles: add/update/delete
- Customers: add/update/delete
- Reservations: create (with overlap checks)
- Returns: process return → generates an `Invoice` and marks reservation as returned

Any operation that changes state calls `save()` to persist immediately.

### Authentication

`AuthService` owns `users.csv` and does:

- `login(username, password)` by comparing SHA-256(password) to the stored hash
- `registerUser(...)` which also creates a linked `Customer`
- `ensureDefaultAdmin()` which creates `admin/admin` if no admin exists yet

Passwords are never stored; only SHA-256 hex hashes are persisted.

## Persistence format

All CSV files live in `data/`:

- `cars.csv`
- `customers.csv`
- `reservations.csv`
- `invoices.csv`
- `users.csv` (accounts; password hashes only)

The `cmake/SeedData.cmake` step copies the `data/` folder next to the built binary
only if missing, so developers can build/run without manual setup while keeping
local changes intact.

## Key invariants / rules

- **Availability**:
  - `Vehicle.available` is a manual flag (e.g., "in service" or "disabled").
  - Date availability is determined by *overlap* against `Active` reservations.
- **Reservation overlap**:
  - Range overlap rule: two ranges overlap if `start < other.end && other.start < end`.
- **User mode isolation**:
  - A normal user account is linked to exactly one `Customer` via `customerId`.
  - User UI must not allow switching to other customers.

