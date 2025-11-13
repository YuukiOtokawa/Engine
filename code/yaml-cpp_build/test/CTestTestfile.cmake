# CMake generated Testfile for 
# Source directory: C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test
# Build directory: C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp_build/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[yaml-cpp::test]=] "C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp_build/test/Debug/yaml-cpp-tests.exe")
  set_tests_properties([=[yaml-cpp::test]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test/CMakeLists.txt;56;add_test;C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[yaml-cpp::test]=] "C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp_build/test/Release/yaml-cpp-tests.exe")
  set_tests_properties([=[yaml-cpp::test]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test/CMakeLists.txt;56;add_test;C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[yaml-cpp::test]=] "C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp_build/test/MinSizeRel/yaml-cpp-tests.exe")
  set_tests_properties([=[yaml-cpp::test]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test/CMakeLists.txt;56;add_test;C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[yaml-cpp::test]=] "C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp_build/test/RelWithDebInfo/yaml-cpp-tests.exe")
  set_tests_properties([=[yaml-cpp::test]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test/CMakeLists.txt;56;add_test;C:/Users/yuuki/Desktop/OtokawaEngine/code/yaml-cpp-master/test/CMakeLists.txt;0;")
else()
  add_test([=[yaml-cpp::test]=] NOT_AVAILABLE)
endif()
subdirs("prefix")
