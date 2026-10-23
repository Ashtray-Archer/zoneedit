cmake_minimum_required(VERSION 3.22)
file(READ "${SOURCE}/app/src/main/c/navigation.c" original)
string(REPLACE "!strcmp(host, \"zoneedit.com\") ||"
  "strstr(host, \"zoneedit.com\") != NULL ||" insecure "${original}")
if(insecure STREQUAL original)
  message(FATAL_ERROR "Host-boundary mutation was not applied")
endif()
file(WRITE "${BUILD}/mutant-navigation.c" "${insecure}")
execute_process(COMMAND "${NDK}/clang" --target=x86_64-linux-gnu --sysroot=/ -fuse-ld=lld
  -std=c17 -Wall -Wextra -Werror "-I${SOURCE}/app/src/main/c"
  "${SOURCE}/tests/navigation.c" "${BUILD}/mutant-navigation.c" -o "${BUILD}/bad-navigation"
  RESULT_VARIABLE compiled)
if(NOT compiled EQUAL 0)
  message(FATAL_ERROR "Security mutant did not compile; no rejection evidence")
endif()
execute_process(COMMAND "${BUILD}/bad-navigation" RESULT_VARIABLE status ERROR_VARIABLE diagnostic)
if(NOT status EQUAL 1 OR NOT diagnostic MATCHES "FAIL policy case 3")
  message(FATAL_ERROR "URL tests failed to reject the hostile-host mutant: ${status} ${diagnostic}")
endif()
message(STATUS "PASS: hostile-host acceptance mutant rejected by URL policy tests")
