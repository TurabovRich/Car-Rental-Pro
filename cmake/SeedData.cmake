if(NOT DEFINED src OR NOT DEFINED dst)
  message(FATAL_ERROR "SeedData.cmake requires -Dsrc=... -Ddst=...")
endif()

file(MAKE_DIRECTORY "${dst}")

function(copy_if_missing relpath)
  if(NOT EXISTS "${dst}/${relpath}")
    get_filename_component(_dir "${dst}/${relpath}" DIRECTORY)
    file(MAKE_DIRECTORY "${_dir}")
    file(COPY "${src}/${relpath}" DESTINATION "${_dir}")
  endif()
endfunction()

# Seed CSV files only if missing
copy_if_missing("cars.csv")
copy_if_missing("customers.csv")
copy_if_missing("reservations.csv")
copy_if_missing("invoices.csv")

# Seed images folder only if missing (do not overwrite user-added images)
if(EXISTS "${src}/images" AND NOT EXISTS "${dst}/images")
  file(COPY "${src}/images" DESTINATION "${dst}")
endif()

