cmake_minimum_required(VERSION 3.3)

######################################################################
# Machine Config
######################################################################
if(MACHINE_CONFIG)
  if(EXISTS ${MACHINE_CONFIG})
    message(STATUS "Loading config in " ${MACHINE_CONFIG})
    include(${MACHINE_CONFIG})
  else(EXISTS ${MACHINE_CONFIG})
    message(ERROR " Machine config not found!")
  endif(EXISTS ${MACHINE_CONFIG})
endif(MACHINE_CONFIG)

if(NOT DEFAULT_BLAS_LAPACK)
    set(DEFAULT_BLAS_LAPACK auto)
endif(NOT DEFAULT_BLAS_LAPACK)

######################################################################
# Project START
######################################################################
project (MAQUIS_DMRG)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/config)

# set proper rpath for installed binaries
# @stknecht: make sure this works also for OS X with MACOS_RPATH
set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib:${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/opt/maquis" CACHE PATH "MAQUIS install prefix" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

set (ALPS_SOURCE_DIR "alps")

set(BUILD_SYMMETRIES "TwoU1PG;SU2U1PG" CACHE STRING "List of symmetry
to include in the build objects. U1, TwoU1, TwoU1PG, NU1, Z2, SU2U1, SU2U1PG, U1DG and NONE are currently available.")
set(DMRG_NUMSYMM 6 CACHE STRING "Maximum number of U1 symmetries.")
option(ENABLE_OMP "Enable OpenMP" ON)
# Disabled
# option(ENABLE_ALPS_MODELS "Enable binding with ALPS lattices and models" OFF)
option(LAPACK_64_BIT "Build for 64-bit LAPACK library" ON)

# DMRG-related flags
option (BUILD_INIT "Build dmrg_init binary (deprecated)" OFF)
option (BUILD_CIDEAS "Build CIDEAS binary (deprecated)" OFF)
option (BUILD_MPS_ROTATE "Build mps_rotate binary for RASSI counterrotation (deprecated)" OFF)
option (BUILD_MPS_COMPRESS "Build mps_compress binary for MPS compression (not working currently)" OFF)
option (BUILD_MPS_OVERLAP "Build MPS Overlap calculator" OFF)
option (BUILD_ONETDM_DIAG "Build 1-TDM diagonal elements calculator (deprecated by dmrg_meas and the binary interface)" OFF)
option (BUILD_MPS2CI "Build CI coefficient reconstruction facilities" OFF)
option (BUILD_DEAS "Build s1 and I printer tool" OFF)
option (BUILD_MPS_TRANSFORM "Build MPS SU2->2U1 symmetry group transformation tool" OFF)
option (BUILD_DMRG_EVOLVE "Builds the time-evolution DMRG module" OFF)
option (BUILD_PREBO "Build PreBO model" OFF)

# Time-evolution TD-DMRG
if(BUILD_DMRG_EVOLVE)
  list(APPEND DMRG_DEFINITIONS -DDMRG_TD)
  set(DMRG_TD TRUE)
endif(BUILD_DMRG_EVOLVE)

option (ENABLE_COLLECTOR "Enable profiling through DataCollector object" OFF)
mark_as_advanced(ENABLE_COLLECTOR)

option (BUILD_MANUAL "Enable pdf manual building. Requires pdflatex" OFF)
set(BENCHMARKS_DIR "../../benchmarks" CACHE PATH "Location of benchmarks directory.")
option(QCMAQUIS_TESTS      "Build and run tests" OFF)

# OpenMOLCAS interface
option (BUILD_OPENMOLCAS_INTERFACE "Build OpenMOLCAS interface" OFF)
if (BUILD_OPENMOLCAS_INTERFACE)
    include(CMakeDependentOption)
    cmake_dependent_option(BUILD_OPENMOLCAS_MPI "Build with OpenMOLCAS MPI support" OFF "BUILD_OPENMOLCAS_INTERFACE" OFF)
    mark_as_advanced(BUILD_OPENMOLCAS_MPI)
    if (BUILD_OPENMOLCAS_MPI)
        if (NOT GA_INCLUDE_DIR)
            message(FATAL_ERROR "Compilation with OpenMOLCAS MPI support requires Global Arrays include directory")
        endif ()
        # if (NOT MPI_LIBRARIES)
        #     message(FATAL_ERROR "Compilation with OpenMOLCAS MPI support requires specifying MPI libraries")
        # endif ()
        if (NOT GA_LIBRARIES)
            message(FATAL_ERROR "Compilation with OpenMOLCAS MPI support requires specifying Global Arrays libraries")
        endif ()
    endif()
endif()

set(BLAS_LAPACK_SELECTOR "auto" CACHE STRING "Linear algebra library selector")
set_property(CACHE BLAS_LAPACK_SELECTOR PROPERTY STRINGS auto manual mkl_sequential mkl_parallel veclib openblas)

# QCMaquis manual
if (BUILD_MANUAL)
    find_package(LATEX)
    set (DOC_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/share/doc/qcmaquis CACHE PATH "QCMaquis documentation path" )
    add_subdirectory(doc)
endif()

######################################################################
# How to build
######################################################################

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    include("config/clang_fixes.cmake")
endif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
# Set a default build type if none was specified
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "RelWithDebugInfo")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-Wno-unknown-pragmas ${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "CMAKE_CXX_FLAGS_RELEASE: Flags used by the compiler during release builds" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Wno-unknown-pragmas ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "CMAKE_CXX_FLAGS_RELWITHDEBINFO: Flags used by the compiler during Release with Debug Info builds." FORCE)
set(CMAKE_C_FLAGS_RELEASE " ${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "CMAKE_C_FLAGS_RELEASE: Flags used by the compiler during release builds" FORCE)


######################################################################
# Symmetries management
######################################################################

macro(get_symm_suffix RET SYMM)
  set(${RET} ${SYMM})
  string(REGEX REPLACE "^U1$" "u1" ${RET} ${${RET}})
  string(REGEX REPLACE "^TWOU1$" "2u1" ${RET} ${${RET}})
  string(REGEX REPLACE "^TwoU1$" "2u1" ${RET} ${${RET}})
  string(REGEX REPLACE "^NU1$"  "nu1" ${RET} ${${RET}})
  string(REGEX REPLACE "^TWOU1PG$" "2u1pg" ${RET} ${${RET}})
  string(REGEX REPLACE "^TwoU1PG$" "2u1pg" ${RET} ${${RET}})
  string(REGEX REPLACE "^NONE$" "none" ${RET} ${${RET}})
  string(REGEX REPLACE "^Z2$" "Ztwo" ${RET} ${${RET}})
  string(REGEX REPLACE "^SU2U1$" "su2u1" ${RET} ${${RET}})
  string(REGEX REPLACE "^SU2U1PG$" "su2u1pg" ${RET} ${${RET}})
  string(REGEX REPLACE "^U1DG$" "u1dg" ${RET} ${${RET}})
endmacro(get_symm_suffix)

macro(get_symm_group_name RET SYMM)
  set(${RET} ${SYMM})
  string(REGEX REPLACE "^U1$" "U1" ${RET} ${${RET}})
  string(REGEX REPLACE "^TWOU1$" "TwoU1" ${RET} ${${RET}})
  string(REGEX REPLACE "^TwoU1$" "TwoU1" ${RET} ${${RET}})
  string(REGEX REPLACE "^NU1$"  "NU1" ${RET} ${${RET}})
  string(REGEX REPLACE "^TWOU1PG$" "TwoU1PG" ${RET} ${${RET}})
  string(REGEX REPLACE "^TwoU1PG$" "TwoU1PG" ${RET} ${${RET}})
  string(REGEX REPLACE "^NONE$" "TrivialGroup" ${RET} ${${RET}})
  string(REGEX REPLACE "^Z2$" "Ztwo" ${RET} ${${RET}})
  string(REGEX REPLACE "^SU2U1$" "SU2U1" ${RET} ${${RET}})
  string(REGEX REPLACE "^SU2U1PG$" "SU2U1PG" ${RET} ${${RET}})
  string(REGEX REPLACE "^U1DG$" "U1DG" ${RET} ${${RET}})
endmacro(get_symm_group_name)

macro(get_symm_files TYPE RET FILEBASE)
  if(NOT ${TYPE} STREQUAL "APPEND")
    set(${RET} "")
  endif(NOT ${TYPE} STREQUAL "APPEND")
  foreach(SYMM ${BUILD_SYMMETRIES})
    get_symm_suffix(SYMM_SUFFIX ${SYMM})
    string(REPLACE "{SYMM}" ${SYMM_SUFFIX} SYMM_FILE ${FILEBASE})
    list(APPEND ${RET} ${SYMM_FILE})
  endforeach(SYMM)
endmacro(get_symm_files)

# Note that here we assume that NU1 is managed with a special treatment
macro(configure_symm_file INPUT OUTBASE VARNAME)
  foreach(SYMM ${BUILD_SYMMETRIES})
    if (NOT "${SYMM}" STREQUAL "NU1")
      get_symm_suffix(SYMM_SUFFIX ${SYMM})
      get_symm_group_name(${VARNAME} ${SYMM})
      string(REPLACE "{SYMM}" ${SYMM_SUFFIX} SYMM_FILE ${OUTBASE})
      configure_file(${INPUT} ${SYMM_FILE})
    endif()
  endforeach(SYMM)
endmacro(configure_symm_file)

# Adding symmetries definitions
message(STATUS "Enabled symmetries: ${BUILD_SYMMETRIES}")
foreach(SYMM ${BUILD_SYMMETRIES})
  get_symm_group_name(SYMM_NAME ${SYMM})
  list(APPEND DMRG_DEFINITIONS -DHAVE_${SYMM_NAME})
  set(DMRG_HAS_${SYMM_NAME} TRUE)
endforeach(SYMM)

if((BUILD_PREBO))
  if((NOT DMRG_HAS_NU1))
    message(FATAL_ERROR "For pre-BO module, activate NU1 symmetry!")
  else()
    list(APPEND DMRG_DEFINITIONS -DDMRG_PREBO)
    set(DMRG_PREBO TRUE)
  endif()
endif()

##### Dependencies #####
# GSL
# find GNU Scientific Library if using SU2 Groups
if((DMRG_HAS_SU2U1) OR (DMRG_HAS_SU2U1PG))
  find_package(GSL REQUIRED)
  list(APPEND DMRG_LIBRARIES ${GSL_LIBRARIES})
endif()

# BLAS/LAPACK selector
find_package(MAQUISLapack)
message(STATUS "Lapack include:   ${MAQUISLapack_INCLUDE_DIRS}")
message(STATUS "Lapack lib dirs:  ${MAQUISLapack_LIB_DIRS}")
message(STATUS "Lapack libs:      ${MAQUISLapack_LIBRARIES}")
message(STATUS "Lapack CXX flags: ${MAQUISLapack_CXX_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${MAQUISLapack_CXX_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS}")
list(APPEND DMRG_LIBRARIES ${MAQUISLapack_LIBRARIES})
# ILP64 definition
if(LAPACK_64_BIT)
  add_definitions(-DBIND_FORTRAN_INTEGER_8)
endif()

# HDF5
find_package(HDF5 REQUIRED)
list(APPEND DMRG_LIBRARIES ${HDF5_LIBRARIES})

# Boost
set(Boost_requirements program_options filesystem system serialization thread)
set (Boost_NO_BOOST_CMAKE ON)

find_package(Boost 1.56 COMPONENTS ${Boost_requirements})
if(Boost_FOUND)
    list(APPEND DMRG_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
    list(APPEND DMRG_LIBRARY_DIRS ${Boost_LIBRARY_DIRS})
    list(APPEND DMRG_LIBRARIES ${Boost_LIBRARIES})
    if(${Boost_MINOR_VERSION} VERSION_LESS 60)
        add_definitions(-DLEGACY_BOOST)
    endif()
else(Boost_FOUND)
    message(FATAL_ERROR "Boost libraries are required for QCMaquis")
endif(Boost_FOUND)

# ALPS
add_subdirectory(${ALPS_SOURCE_DIR})
list(APPEND DMRG_LIBRARIES alps)

# Eigen
# find_package(Eigen3 3.3 REQUIRED NO_MODULE)
# message("Eigen directory " ${EIGEN3_INCLUDE_DIR})
# list(APPEND DMRG_LIBRARIES Eigen3::Eigen)

# OpenMolcas interface
if (BUILD_OPENMOLCAS_INTERFACE)
    project (MAQUIS_DMRG CXX C Fortran)
    add_subdirectory(lib/interfaces/openmolcas)
endif()

######################################################################
# Version information
######################################################################
set(DMRG_YEAR 2021)
set(DMRG_VERSION_MAJOR 3)
set(DMRG_VERSION_MINOR 1)
set(DMRG_VERSION_PATCH 1)
set(DMRG_VERSION_BUILD "")

if(NOT DMRG_VERSION_BUILD AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)
  FIND_PACKAGE(Subversion)
  IF(Subversion_FOUND)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} DMRG)
    string(REPLACE ${DMRG_WC_ROOT} "" DMRG_BRANCH ${DMRG_WC_URL})
    set(DMRG_VERSION_BUILD "r${DMRG_WC_REVISION} (${DMRG_BRANCH})")
  ENDIF(Subversion_FOUND)
endif(NOT DMRG_VERSION_BUILD AND EXISTS ${PROJECT_SOURCE_DIR}/.svn)

if(EXISTS ${PROJECT_SOURCE_DIR}/../.git)
    find_package(Git)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list --max-count=1 HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_REVISION
            ERROR_QUIET
            )
        if(NOT ${GIT_REVISION} STREQUAL "")
            string(STRIP ${GIT_REVISION} GIT_REVISION)
        endif()

        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
            ERROR_QUIET
            )
        if(NOT ${GIT_BRANCH} STREQUAL "")
            string(STRIP ${GIT_BRANCH} GIT_BRANCH)
        endif()

        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --abbrev=0 --tags
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE SVN_REV
            ERROR_QUIET
            )

        if(NOT ${SVN_REV} STREQUAL "")
            string(STRIP ${SVN_REV} SVN_REV)
        endif()

        set(DMRG_VERSION_BUILD "${GIT_REVISION} (${GIT_BRANCH}) ${SVN_REV}")
    endif()
endif(EXISTS ${PROJECT_SOURCE_DIR}/../.git)

if(DMRG_VERSION_BUILD)
  set(DMRG_VERSION "${DMRG_VERSION_MAJOR}.${DMRG_VERSION_MINOR}.${DMRG_VERSION_PATCH}-${DMRG_VERSION_BUILD}")
else(DMRG_VERSION_BUILD)
  set(DMRG_VERSION "${DMRG_VERSION_MAJOR}.${DMRG_VERSION_MINOR}.${DMRG_VERSION_PATCH}")
endif(DMRG_VERSION_BUILD)
set(DMRG_VERSION_STRING "DMRG version ${DMRG_VERSION}")
MESSAGE(STATUS "DMRG version: ${DMRG_VERSION}")

# OpenMP
set(OpenMP_LIBRARIES "")
if(ENABLE_OMP)
  find_package(OpenMP)
  if(OPENMP_FOUND)
    add_definitions(-DMAQUIS_OPENMP)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(OpenMP_LIBRARIES "${OpenMP_CXX_LIBRARIES}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  else(OPENMP_FOUND)
    message("OpenMP not found")
  endif(OPENMP_FOUND)
endif(ENABLE_OMP)


######################################################################
# Include / link directories
######################################################################
set (ALPS_INCLUDE_DIRS ${ALPS_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/${ALPS_SOURCE_DIR}/src)
list(APPEND DMRG_INCLUDE_DIRS ${MATRICES_DIR} ${HDF5_INCLUDE_DIR} ${ALPS_INCLUDE_DIRS})
list(APPEND DMRG_LIBRARY_DIRS ${ALPS_LIBRARY_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/framework ${PROJECT_BINARY_DIR}/framework ${PROJECT_SOURCE_DIR}/lib/maquis_dmrg ${PROJECT_BINARY_DIR}/lib/maquis_dmrg ${DMRG_INCLUDE_DIRS})
link_directories(${DMRG_LIBRARY_DIRS})
add_definitions(${DMRG_DEFINITIONS})

set(DMRG_FRAMEWORK_DIR ${PROJECT_SOURCE_DIR}/framework)
if(CMAKE_EXE_LINKER_FLAGS MATCHES "^ $")
    set(CMAKE_EXE_LINKER_FLAGS ${MAQUISLapack_LINKER_FLAGS})
else(CMAKE_EXE_LINKER_FLAGS MATCHES "^ $")
    list(APPEND CMAKE_EXE_LINKER_FLAGS ${MAQUISLapack_LINKER_FLAGS})
endif(CMAKE_EXE_LINKER_FLAGS MATCHES "^ $")


######################################################################
# Configure files
######################################################################

configure_file(framework/dmrg/version.h.in ${PROJECT_BINARY_DIR}/framework/dmrg/version.h)


######################################################################
# Targets
######################################################################

# DataCollector
if(ENABLE_COLLECTOR)
  add_definitions(-DENABLE_DATACOLLECTORS)
  add_library(maquis_collector framework/utils/data_collector.cpp)
  list(APPEND DMRG_LIBRARIES maquis_collector)
endif(ENABLE_COLLECTOR)

# *** libraries / utilities
add_subdirectory(framework/dmrg)

# *** shared library
add_subdirectory(lib/maquis_dmrg)

# *** applications
add_subdirectory(applications)

# *** tests
if(QCMAQUIS_TESTS)
  message(STATUS "Enabling tests")
  enable_testing()
  add_subdirectory(tests)
  add_test(NAME Test1 COMMAND test1)
  add_test(NAME Test2 COMMAND test2)
  if(BUILD_PREBO)
    add_test(NAME PreBO_Test1 COMMAND prebo_test1)
    add_test(NAME PreBO_Test2 COMMAND prebo_test2)
    add_test(NAME PreBO_Test_Model COMMAND prebo_test_model)
    add_test(NAME PreBO_Test_SymbolicOperator COMMAND test_prebo_symbolic_operator)
    add_test(NAME PreBO_Test_SymbolicJordanWigner COMMAND test_prebo_symbolic_jw)
  endif(BUILD_PREBO)
  add_test(NAME Test_Relativistic COMMAND test_rel)
  add_test(NAME Test_Integral_Map COMMAND test_integral_map)
  add_test(NAME Test_HiRDM COMMAND test_hirdm)
  add_test(NAME Test_MPS_MPO_Ops_TwoU1 COMMAND test_mps_mpo_ops_TwoU1)
  add_test(NAME Test_MPS_MPO_Ops_Electronic COMMAND test_mps_mpo_ops_electronic)
  add_test(NAME Test_MPS_Overlap_Electronic COMMAND test_mps_overlap_electronic)
  add_test(NAME Test_SiteProblem COMMAND test_siteproblem)
  add_test(NAME Test_MPS_Join COMMAND test_mpsjoin)
  add_test(NAME Test_Wigner COMMAND test_wigner)
  add_test(NAME Test_Block_Matrix COMMAND test_block_matrix)
  if(BUILD_DMRG_EVOLVE)
    add_test(NAME Test_Time_Evolver COMMAND test_time_evolvers)
    add_test(NAME Test_Site_Shifter COMMAND test_site_shifter)
    add_test(NAME Test_Time_Evolution_Sweep COMMAND test_time_evolution_sweep)
    add_test(NAME Test_RealTime_Evolution COMMAND test_realtime)
    add_test(NAME Test_ImaginaryTime_Evolution COMMAND test_imaginarytime)
  endif(BUILD_DMRG_EVOLVE)
endif(QCMAQUIS_TESTS)

######################################################################
# DMRG exports
######################################################################

## For build tree
set(CONF_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/framework ${PROJECT_BINARY_DIR}/framework ${DMRG_INCLUDE_DIRS})
configure_file(MAQUIS_DMRGConfig.cmake.in ${PROJECT_BINARY_DIR}/MAQUIS_DMRGConfig.cmake @ONLY)

## For install
set(CONF_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include ${DMRG_INCLUDE_DIRS})
configure_file(MAQUIS_DMRGConfig.cmake.in ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MAQUIS_DMRGConfig.cmake @ONLY)

install(FILES ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MAQUIS_DMRGConfig.cmake DESTINATION share/cmake  COMPONENT dev)
install(EXPORT DMRGTargets DESTINATION share/cmake COMPONENT dev)


######################################################################
# Install
######################################################################

install(FILES ${PROJECT_BINARY_DIR}/framework/dmrg/version.h
        DESTINATION include/framework/dmrg COMPONENT headers)

install(FILES
            lib/maquis_dmrg/maquis_dmrg.h
            lib/maquis_dmrg/mpssi_interface.h
            lib/maquis_dmrg/maquis_dmrg_detail.h
            lib/maquis_dmrg/integral_interface.h
            lib/maquis_dmrg/maquis_cinterface.h
            lib/maquis_dmrg/mpssi_cinterface.h
            lib/maquis_dmrg/starting_guess.h
        DESTINATION include
        COMPONENT headers
        )

install(FILES
            framework/dmrg/utils/align.h
            framework/dmrg/utils/DmrgParameters.h
            framework/dmrg/utils/DmrgOptions.h
            framework/dmrg/utils/BaseParameters.h
            framework/dmrg/utils/results_collector.h
        DESTINATION include/framework/dmrg/utils
        COMPONENT headers
    )



install(DIRECTORY lib/python DESTINATION lib  COMPONENT python
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "*.py" PATTERN "*.pyc" EXCLUDE
        PATTERN ".svn" EXCLUDE PATTERN "CVS" EXCLUDE)
