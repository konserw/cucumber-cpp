project(CalcQt)

if(TARGET Qt::Core AND TARGET Qt::Gui AND TARGET Qt::Widgets AND TARGET Qt::Test)
    add_library(libcalcqt STATIC src/CalculatorWidget.cpp src/CalculatorWidget.h)
    set_target_properties(libcalcqt PROPERTIES AUTOMOC ON)
    target_include_directories(libcalcqt INTERFACE src)
    target_link_libraries(libcalcqt
        PUBLIC
            Qt::Core
            Qt::Widgets
            Qt::Test
        PRIVATE
            Qt::Gui
    )

    add_executable(calcqt src/CalcQt.cpp)
    target_link_libraries(calcqt PRIVATE libcalcqt)

    if(TARGET Qt::Test)
        add_executable(QtTestCalculatorQtSteps features/step_definitions/QtTestCalculatorQtSteps)
        target_link_libraries(QtTestCalculatorQtSteps PRIVATE libcalcqt cucumber-cpp)
    endif()

    if(TARGET Boost::unit_test_framework)
        add_executable(BoostCalculatorQtSteps features/step_definitions/BoostCalculatorQtSteps.cpp)
        target_link_libraries(BoostCalculatorQtSteps PRIVATE libcalcqt Boost::unit_test_framework cucumber-cpp)
    endif()

    if(TARGET GTest::GTest)
        add_executable(GTestCalculatorQtSteps features/step_definitions/GTestCalculatorQtSteps)
        target_link_libraries(GTestCalculatorQtSteps PRIVATE libcalcqt cucumber-cpp GTest::GTest)
    endif()

endif()
