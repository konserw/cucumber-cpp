#!/bin/sh
set -e #break script on non-zero exitcode from any command
set -x #display command being executed
export DISPLAY=:99

startXvfb () {
# Xvfb sends SIGUSR1 to its parent when it finished startup, this causes the 'wait' below to stop waiting
# Starting Xvfb hangs on OSX, that's why we do this on Linux only now
    if [[ "${TRAVIS_OS_NAME}" = "linux" ]]; then
        trap : USR1
        (trap '' USR1; Xvfb $DISPLAY -screen 0 640x480x8 -nolisten tcp > /dev/null 2>&1) &
        XVFBPID=$!
        wait || :
        trap '' USR1
        if ! kill -0 $XVFBPID 2> /dev/null; then
            echo "Xvfb failed to start" >&2
            exit 1
        fi
    else
        sudo Xvfb $DISPLAY -screen 0 640x480x8 -nolisten tcp > /dev/null 2>&1 &
        XVFBPID=$!
        sleep 5 
    fi
}

killXvfb () {
    if [ -n "${XVFBPID:-}" ]; then
        # Stop virtual X display server
        sudo kill $XVFBPID
        wait
    fi
}

CTEST_OUTPUT_ON_FAILURE=ON
export CTEST_OUTPUT_ON_FAILURE

cmake -E make_directory build
cmake -E chdir build cmake \
    -G Ninja \
    -DCUKE_ENABLE_EXAMPLES=on \
    ${CMAKE_PREFIX_PATH:+"-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"} \
    ${COVERALLS_SERVICE_NAME:+"-DCMAKE_BUILD_TYPE=Debug"} \
    ${COVERALLS_SERVICE_NAME:+"-DCMAKE_CXX_FLAGS='--coverage'"} \
    ${VALGRIND_TESTS:+"-DVALGRIND_TESTS=${VALGRIND_TESTS}"} \
    ${GMOCK_PATH:-"-DGMOCK_VER=${GMOCK_VER}"} \
    ${GMOCK_PATH:+"-DGMOCK_SRC_DIR=${GMOCK_PATH}"} \
    ..
cmake --build build
cmake --build build --target test
mkdir /home/runner/cucumber-cpp/build/tmp/
cmake --build build --target features

startXvfb # Start virtual X display server

for TEST in \
    build/examples/Calc/GTestCalculatorSteps \
    build/examples/Calc/QtTestCalculatorSteps \
    build/examples/Calc/BoostCalculatorSteps \
    build/examples/Calc/FuncArgsCalculatorSteps \
; do
    if [ -f "${TEST}" ]; then
        "${TEST}" > /dev/null &
        sleep 1
        cucumber examples/Calc
        wait %
    fi
done

for TEST in \
    build/examples/CalcQt/GTestCalculatorQtSteps \
    build/examples/CalcQt/QtTestCalculatorQtSteps \
    build/examples/CalcQt/BoostCalculatorQtSteps \
; do
    if [ -f "${TEST}" -a -n "${DISPLAY:-}" ]; then
        "${TEST}" 2> /dev/null &
        sleep 1
        cucumber examples/CalcQt
        wait %
    fi
done

# Test unix sockets
SOCK=cucumber.wire.sock
TEST=build/examples/FeatureShowcase/FeatureShowcaseSteps
if [ -f "${TEST}" ]; then
    echo "unix: ${SOCK}" > examples/FeatureShowcase/features/step_definitions/cucumber.wire
    "${TEST}" --unix "${SOCK}" > /dev/null &
    cucumber examples/FeatureShowcase
    wait %
fi

killXvfb