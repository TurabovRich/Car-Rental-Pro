# CarRentalPro Presentation Pack (10-15 min)

Use this as your speaking guide + slide content plan.

## 1) Objective Document Template (required)

Copy into a separate file and fill details:

- Project title: **CarRentalPro**
- Team members:
  - Name 1 - ID - Section
  - Name 2 - ID - Section
  - ...
- Project description:
  - A local-first C++ Qt car rental application with role-based UI:
    - Admin panel for operations
    - User app for renting/returning cars and profile/history
- Main objective:
  - Demonstrate practical OOP concepts in a complete GUI application with persistent storage.

## 2) Slide-by-slide PPT outline

### Slide 1 - Title
- CarRentalPro
- Team members + sections
- Course + instructor

### Slide 2 - Problem statement
- Manual/rudimentary rental tracking is error-prone
- Need role-based workflow for admin and customer
- Need persistent local data

### Slide 3 - Project scope
- Admin features
- User features
- Authentication and local persistence

### Slide 4 - Architecture
- 4 layers:
  - Domain (`src/domain`)
  - Services (`src/service`)
  - Storage (`src/storage`)
  - UI (`src/ui`)
- Mention `docs/ARCHITECTURE.md`

### Slide 5 - OOP coverage summary
- Reference `docs/OOP_REQUIREMENTS_MAP.md`
- Show checklist of all required OOP topics covered

### Slide 6 - Inheritance + polymorphism UML
- `Vehicle` base class
- `Sedan/SUV/Truck/Electric/PremiumSUV`
- `Insurable` interface and `PremiumSUV : SUV + Insurable`

### Slide 7 - Data model + file handling
- CSV files in `data/`
- `FileManager` read/write
- Immediate save after mutations

### Slide 8 - Auth + security
- Login/register flow
- Password hashing (SHA-256 in `users.csv`)
- Default admin creation

### Slide 9 - Demo plan (what you will show live)
- Admin login -> manage cars/customers
- User register/login
- Date-based reservation and overlap prevention
- Return flow and invoice/history

### Slide 10 - Challenges and fixes
- Date overlap logic
- Role isolation
- Local persistence consistency

### Slide 11 - Conclusion
- What was learned (OOP + GUI + persistence)
- Future improvements

### Slide 12 - Q&A

## 3) 12-minute speaking script

### 0:00-1:00 Intro
- "We built CarRentalPro, a local-first C++ Qt application."
- "It has two role-based experiences: admin and user."

### 1:00-2:00 Problem + objective
- Explain the need for reliable reservation/return tracking
- State objective: demonstrate OOP concepts in a real GUI app

### 2:00-4:00 Architecture
- Walk through Domain/Service/Storage/UI layers
- Explain startup flow from `main.cpp`

### 4:00-6:00 OOP concepts mapping
- Inheritance and runtime polymorphism in `Vehicle`
- Compile-time polymorphism with `CsvUtils` templates
- Overloading, operators, static members, exceptions

### 6:00-10:00 Live demo
- Admin login
- Add/edit car and customer
- Register a user and login as user
- Reserve car for long range and show non-overlap behavior
- Return car and show invoice/history

### 10:00-11:00 Persistence
- Show `data/*.csv`
- Explain local save behavior

### 11:00-12:00 Wrap-up
- Summarize scope and correctness
- Mention potential future enhancements

## 4) Demo checklist (run before presentation)

- Build works: `cmake --build build -j`
- App launches
- `data/` files exist:
  - `cars.csv`
  - `customers.csv`
  - `reservations.csv`
  - `invoices.csv`
  - `users.csv`
- Default admin login works
- New user registration works
- Reservation overlap prevention works
- Return flow creates invoice
- Logout/login role routing works

## 5) Team split suggestion (example: 4 members)

- Member A:
  - Problem statement, objective, architecture
- Member B:
  - OOP mapping + UML explanation
- Member C:
  - Live demo admin/user flows
- Member D:
  - Persistence, validation, conclusion, Q&A

## 6) Questions examiners may ask (practice)

- Why use role-based windows instead of one mixed window?
- How do you prevent double-booking?
- Where are passwords stored and how protected?
- Which requirement demonstrates compile-time polymorphism?
- How do you ensure data is not lost on crash/close?

## 7) Backup plan if live demo fails

- Keep screenshots ready:
  - login/register
  - admin panel
  - user rental tab
  - my rentals/history
  - CSV files after operations
- Continue explanation using diagrams and code references.

