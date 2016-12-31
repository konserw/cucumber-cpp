git submodule init
git submodule update
call gem install bundle
call bundle install
REM bundle env
call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" %platform% 
cmake -E make_directory build
cmake -E chdir build cmake -G "NMake Makefiles" -DCUKE_ENABLE_EXAMPLES=on -DBOOST_ROOT="%BOOST_ROOT%" -DBOOST_INCLUDEDIR="%BOOST_INCLUDEDIR%" -DBOOST_LIBRARYDIR="%BOOST_LIBRARYDIR%" -DQT_QMAKE_EXECUTABLE="%QT_DIR%/bin/qmake.exe" ..
cmake --build build
cmake --build build --target test
cmake --build build --target features
