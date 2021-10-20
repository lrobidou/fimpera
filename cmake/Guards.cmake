if (${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "")
endif()

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release"
    AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug"
    AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"
    AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "Profile"
    AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "Coverage")
message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE. [Release|Debug|RelWithDebInfo|Profile|Coverage]")
endif()