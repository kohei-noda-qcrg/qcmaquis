#set(COMPILER_FLAGS_VAR CMAKE_CXX_FLAGS_RELEASE) # Quite just the release mode
set(COMPILER_FLAGS_VAR CMAKE_CXX_FLAGS) # Quite everything

## be quite on some warning
set(${COMPILER_FLAGS_VAR} "-Wno-return-type-c-linkage ${${COMPILER_FLAGS_VAR}}")
set(${COMPILER_FLAGS_VAR} "-Wno-dangling-else ${${COMPILER_FLAGS_VAR}}")

## fix compilation of Boost
set(${COMPILER_FLAGS_VAR} "-ftemplate-depth-256 ${${COMPILER_FLAGS_VAR}}")

## fix problems with libc++
if(${CMAKE_OSX_DEPLOYMENT_TARGET} VERSION_GREATER "12.0")
  set(${COMPILER_FLAGS_VAR} "-stdlib=libc++ ${${COMPILER_FLAGS_VAR}}")
else()
  set(${COMPILER_FLAGS_VAR} "-stdlib=libstdc++ ${${COMPILER_FLAGS_VAR}}")
endif()
