# Unit tests and benchmarks for Psychic

add_executable(unit_tests unit_tests.cpp)
target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)
if(TARGET dsp_engine_static)
    target_link_libraries(unit_tests PRIVATE dsp_engine_static)
elseif(TARGET dsp_engine)
    target_link_libraries(unit_tests PRIVATE dsp_engine)
endif()

add_test(NAME unit_tests COMMAND unit_tests)

add_executable(benchmarks benchmarks.cpp)
target_include_directories(benchmarks PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)
if(TARGET dsp_engine_static)
    target_link_libraries(benchmarks PRIVATE dsp_engine_static)
elseif(TARGET dsp_engine)
    target_link_libraries(benchmarks PRIVATE dsp_engine)
endif()
