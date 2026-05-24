if(NOT DEFINED src OR NOT DEFINED dst)
  message(FATAL_ERROR "SeedData.cmake needs -Dsrc and -Ddst")
endif()

file(MAKE_DIRECTORY "${dst}")

function(copy_if_missing relpath)
  if(NOT EXISTS "${dst}/${relpath}")
    file(COPY "${src}/${relpath}" DESTINATION "${dst}")
  endif()
endfunction()

copy_if_missing("cars.csv")
copy_if_missing("customers.csv")
copy_if_missing("reservations.csv")
copy_if_missing("invoices.csv")
copy_if_missing("users.csv")
