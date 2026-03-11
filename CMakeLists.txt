cmake_minimum_required(VERSION 3.16)
project(CarRentalPro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)
if (Qt6_FOUND)
  set(QT_LIBS Qt6::Widgets)
else()
  find_package(Qt5 COMPONENTS Widgets REQUIRED)
  set(QT_LIBS Qt5::Widgets)
endif()

add_executable(CarRentalPro
  src/main.cpp
  src/ui/MainWindow.h src/ui/MainWindow.cpp
  src/ui/Theme.h src/ui/Theme.cpp
  src/ui/delegates/StatusBadgeDelegate.h src/ui/delegates/StatusBadgeDelegate.cpp
  src/ui/delegates/NumericDelegate.h
  src/ui/tabs/CarsTab.h src/ui/tabs/CarsTab.cpp
  src/ui/tabs/CustomersTab.h src/ui/tabs/CustomersTab.cpp
  src/ui/tabs/ReservationsTab.h src/ui/tabs/ReservationsTab.cpp
  src/ui/tabs/ReturnsTab.h src/ui/tabs/ReturnsTab.cpp
  src/ui/tabs/ReportsTab.h src/ui/tabs/ReportsTab.cpp
  src/ui/dialogs/AddCarDialog.h src/ui/dialogs/AddCarDialog.cpp
  src/ui/dialogs/AddCustomerDialog.h src/ui/dialogs/AddCustomerDialog.cpp
  src/ui/dialogs/NewReservationDialog.h src/ui/dialogs/NewReservationDialog.cpp
  src/ui/dialogs/ReturnDialog.h src/ui/dialogs/ReturnDialog.cpp
  src/ui/dialogs/CustomerHistoryDialog.h src/ui/dialogs/CustomerHistoryDialog.cpp
  src/domain/Vehicle.h src/domain/Vehicles.h
  src/domain/Customer.h src/domain/Reservation.h src/domain/Invoice.h
  src/service/RentalService.h src/service/RentalService.cpp
  src/storage/FileManager.h src/storage/FileManager.cpp
  src/utils/Exceptions.h
  src/utils/Validation.h src/utils/Validation.cpp
  src/utils/Date.h src/utils/Date.cpp
)

target_include_directories(CarRentalPro PRIVATE src)
target_link_libraries(CarRentalPro PRIVATE ${QT_LIBS})

add_custom_command(TARGET CarRentalPro POST_BUILD
  # Seed default data only once; never overwrite user's local changes in build/data
  COMMAND ${CMAKE_COMMAND}
    -Dsrc=${CMAKE_SOURCE_DIR}/data
    -Ddst=$<TARGET_FILE_DIR:CarRentalPro>/data
    -P ${CMAKE_SOURCE_DIR}/cmake/SeedData.cmake
)
