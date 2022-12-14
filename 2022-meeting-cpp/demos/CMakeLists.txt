cmake_minimum_required(VERSION 3.17)
project(clean-test-demos CXX)

set(CMAKE_CXX_STANDARD 20)
enable_testing()

find_package(CleanTest REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

function(add_demo src)
    set(options NO_MAIN BOOST)
    set(oneValueArgs NAME)
    set(multiValueArgs)
    cmake_parse_arguments(PARSED "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if ("${PARSED_NAME}" STREQUAL "")
      set(name "${src}")
    else()
      set(name "${PARSED_NAME}")
    endif()

    set(sources "${src}.cpp")
    add_compile_options(${PARSED_UNPARSED_ARGUMENTS})

    add_executable(${name} ${sources})
    if (PARSED_BOOST)
      target_link_libraries(${name} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    elseif (PARSED_NO_MAIN)
      target_link_libraries(${name} CleanTest::automatic)
    else()
      target_link_libraries(${name} CleanTest::main-automatic)
    endif()
    add_compile_options(-Wall -Wextra -Werror)
    #add_compile_options(-fsanitize=undefined)
    #add_link_options(-lubsan)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_demo(Overview)
add_demo(Intro)
add_demo(Intro-listing NO_MAIN)
add_demo(ExpectIntro)
add_demo(ExpectIntrospection)
add_demo(ExpectAttempt)
add_demo(ExpectCached)
add_demo(ExpectEvaluation)
add_demo(AdvancedExpectations NO_MAIN -Wno-div-by-zero)
add_demo(DataDriven NO_MAIN)
add_demo(Distance NO_MAIN)
add_demo(ConversionBefore BOOST)
add_demo(ConversionAfter)
add_demo(Observer)
add_demo(Limitations)
add_demo(Asio)