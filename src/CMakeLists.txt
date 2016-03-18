set(CUKE_SOURCES
    drivers/GenericDriver.cpp
    ContextManager.cpp
    CukeCommands.cpp
    CukeEngine.cpp
    CukeEngineImpl.cpp
    StepManager.cpp
    HookRegistrar.cpp
    Regex.cpp
    Scenario.cpp
    Table.cpp
    Tag.cpp
    connectors/wire/WireServer.cpp
    connectors/wire/WireProtocol.cpp
    connectors/wire/WireProtocolCommands.cpp
)

# Find the Core library
find_package(Qt5Core REQUIRED)
# Find the QtTest library
find_package(Qt5Test REQUIRED)

if(${Qt5Core_FOUND} AND ${Qt5Test_FOUND})
    message(STATUS "Using Qt version ${Qt5Core_VERSION_STRING} ")
    
    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    # Instruct CMake to run moc automatically when needed.
    set(CMAKE_AUTOMOC ON)

    # FIX: Qt was built with -reduce-relocations
    if (Qt5_POSITION_INDEPENDENT_CODE)
        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()

    include_directories(${Qt5Core_INCLUDE_DIRS})
    list(APPEND CUKE_SOURCES drivers/QtTestDriver.cpp)
endif()

if(GTEST_FOUND)
    include_directories(${GTEST_INCLUDE_DIRS})
    list(APPEND CUKE_SOURCES drivers/GTestDriver.cpp)
endif()

if(CPPSPEC_FOUND)
    include_directories(${CPPSPEC_INCLUDE_DIRS})
    list(APPEND CUKE_SOURCES drivers/CppSpecDriver.cpp)
endif()

if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
    list(APPEND CUKE_SOURCES drivers/BoostDriver.cpp)
endif()

if(CMAKE_EXTRA_GENERATOR OR MSVC_IDE)
    message(STATUS "Adding header files to project")
    file(GLOB_RECURSE CUKE_HEADERS "${CUKE_INCLUDE_DIR}/cucumber-cpp/*.hpp")
    if(MSVC_IDE)
        source_group("Header Files" FILES ${CUKE_HEADERS})
    endif()
    list(APPEND CUKE_SOURCES ${CUKE_HEADERS})
endif()

add_library(cucumber-cpp-nomain STATIC ${CUKE_SOURCES})
add_library(cucumber-cpp STATIC ${CUKE_SOURCES} main.cpp)

if(GMOCK_FOUND) 
    add_dependencies(cucumber-cpp gmock) 
    add_dependencies(cucumber-cpp-nomain gmock) 
endif()
