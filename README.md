# CarRentalPro

Qt Widgets app for a car rental shop (C++17).

Admin can manage cars, customers, bookings and returns.  
Users can register, log in, rent a car and see their history.

Data is stored in CSV files in the `data` folder (next to the executable after build).

Default admin login: **admin** / **admin**

## Build

```bash
mkdir build && cd build
cmake ..
make
./CarRentalPro
```

Need Qt 6 with Widgets module.
