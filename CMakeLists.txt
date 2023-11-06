cmake_minimum_required( VERSION 3.16 )

project(EPIFIRE VERSION 2.0.0)

if (DEFINED ${CMAKE_PROJECT_NAME}_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${${CMAKE_PROJECT_NAME}_SHARED_LIBS}")
endif ()

include(FindGSL)

add_library(epifire ${CMAKE_CURRENT_SOURCE_DIR}/src/Network.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Utility.cpp)
add_library(${CMAKE_PROJECT_NAME}::epifire ALIAS epifire)

target_link_libraries(epifire PUBLIC GSL::gsl GSL::gslcblas)

set_target_properties(epifire PROPERTIES
                      VERSION ${${CMAKE_PROJECT_NAME}_VERSION}
                      SOVERSION ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR})

target_include_directories(epifire PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")

target_compile_features(epifire PUBLIC cxx_std_17)

target_compile_definitions(
    epifire PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:EPIFIRE_STATIC_DEFINE>")
target_include_directories(
    epifire PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

target_link_libraries(epifire PUBLIC GSL::gsl GSL::gslcblas)

# add_executable( EpiFire src/main.cpp )
# target_link_libraries( PLS pls )

install(TARGETS epifire
#  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
