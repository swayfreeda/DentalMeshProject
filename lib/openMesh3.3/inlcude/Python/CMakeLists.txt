IF(NOT DEFINED OPENMESH_BUILD_PYTHON_BINDINGS)
	SET(OPENMESH_BUILD_PYTHON_BINDINGS TRUE CACHE BOOL "Enable or disable building the Python Bindings.")
ENDIF()

IF(NOT DEFINED OPENMESH_BUILD_PYTHON_UNIT_TESTS)
	SET(OPENMESH_BUILD_PYTHON_UNIT_TESTS FALSE CACHE BOOL "Enable or disable building the Python unit tests.")
ENDIF()

IF(NOT DEFINED OPENMESH_PYTHON_VERSION)
	SET(OPENMESH_PYTHON_VERSION "2.7" CACHE STRING "Choose the Python version that is used to build the Python Bindings.")
ENDIF()

IF(OPENMESH_BUILD_PYTHON_BINDINGS)
	# Look for the python libs
	MESSAGE(STATUS "Looking for Python")
	FIND_PACKAGE(PythonLibs ${OPENMESH_PYTHON_VERSION} QUIET)

	IF(PYTHONLIBS_FOUND)
		MESSAGE(STATUS "Looking for Python -- found")

		# Determine the name of the python component
		STRING(REGEX MATCH "^[0-9]+\\.[0-9]+" PYTHON_VERSION ${PYTHONLIBS_VERSION_STRING})
		STRING(REGEX REPLACE "\\." "" PYTHON_VERSION ${PYTHON_VERSION})

		SET(BOOST_PYTHON_COMPONENT "python-py${PYTHON_VERSION}")

		# Check known boost versions for the python component
		SET(BOOST_KNOWN_VERSIONS
			"1.56.0" "1.56" "1.55.0" "1.55" "1.54.0" "1.54"
			"1.53.0" "1.53" "1.52.0" "1.52" "1.51.0" "1.51"
			"1.50.0" "1.50" "1.49.0" "1.49" "1.48.0" "1.48" "1.47.0" "1.47" "1.46.1"
			"1.46.0" "1.46" "1.45.0" "1.45" "1.44.0" "1.44" "1.43.0" "1.43" "1.42.0" "1.42"
			"1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37"
			"1.36.1" "1.36.0" "1.36" "1.35.1" "1.35.0" "1.35" "1.34.1" "1.34.0"
			"1.34" "1.33.1" "1.33.0" "1.33"
		)

		MESSAGE(STATUS "Looking for Boost Python")

		# Look for python component
		FOREACH(VERSION ${BOOST_KNOWN_VERSIONS})
			IF(Boost_FOUND)
				BREAK()
			ENDIF()
			FIND_PACKAGE(Boost ${VERSION} EXACT QUIET COMPONENTS python)
		ENDFOREACH()

		IF(Boost_FOUND)
			MESSAGE(STATUS "Looking for Boost Python -- found")

			MESSAGE(STATUS "Checking the Boost Python configuration")

			TRY_COMPILE(
				COMPILE_WORKS
				${CMAKE_CURRENT_BINARY_DIR}
				${CMAKE_CURRENT_SOURCE_DIR}/Example/
				Example
				CMAKE_FLAGS
					"-DINCLUDE_DIRECTORIES:STRING=${PYTHON_INCLUDE_DIRS};${Boost_INCLUDE_DIRS}"
					"-DLINK_LIBRARIES:STRING=${PYTHON_LIBRARIES};${Boost_LIBRARIES}"
			)

			IF(COMPILE_WORKS)
				# Look for the python interpreter to check if the example works
				FIND_PACKAGE(PythonInterp ${PYTHONLIBS_VERSION_STRING} QUIET)

				IF(PYTHONINTERP_FOUND)
					EXECUTE_PROCESS(
						COMMAND ${PYTHON_EXECUTABLE} -c "from example import *; greet(); planet = World()"
						WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
						RESULT_VARIABLE PYTHON_WORKS
						OUTPUT_QUIET
						ERROR_QUIET
					)

					IF(PYTHON_WORKS EQUAL 0)

						### EVERYTHING WORKS ###

						MESSAGE(STATUS "Checking the Boost Python configuration -- done")
						
						IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND ${Boost_VERSION} VERSION_LESS 105600)
							MESSAGE("There are known issues with Clang and Boost Python 1.55 and below.")
							MESSAGE("Please consider updating Boost Python.")
						ENDIF()

						SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build/python/)

						FILE(GLOB SOURCES *.cc *hh)
						ADD_LIBRARY(openmesh SHARED ${SOURCES})

						INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ../)

						TARGET_LINK_LIBRARIES(
							openmesh
							OpenMeshCore
							OpenMeshTools
							${Boost_LIBRARIES}
							${PYTHON_LIBRARIES}
						)

						SET_TARGET_PROPERTIES(
							openmesh
							PROPERTIES
							PREFIX ""
							DEBUG_POSTFIX ""
							RELEASE_POSTFIX ""
						)

						IF(APPLE)
							SET_TARGET_PROPERTIES(openmesh PROPERTIES SUFFIX ".so")
						ENDIF()

						IF(WIN32)
							SET_TARGET_PROPERTIES(openmesh PROPERTIES SUFFIX ".pyd")
							SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

							SET(OUTPUTS openmesh.exp openmesh.lib openmesh.pyd)

							FOREACH(FILE ${OUTPUTS})
								ADD_CUSTOM_COMMAND(
									TARGET openmesh POST_BUILD
									COMMAND ${CMAKE_COMMAND} -E copy
										${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${FILE}
										${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
								)
							ENDFOREACH()
						ENDIF()

						IF(OPENMESH_BUILD_PYTHON_UNIT_TESTS)
							SET(UNITTEST_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Python-Unittests/)

							# Copy unit tests
							FILE(GLOB UNITTESTS Unittests/*.py)
							FOREACH(TEST ${UNITTESTS})
								FILE(COPY ${TEST} DESTINATION ${UNITTEST_OUTPUT_DIRECTORY})
							ENDFOREACH()

							# Copy test files
							FILE(GLOB TESTFILES ${PROJECT_SOURCE_DIR}/src/Unittests/TestFiles/*(.off|.obj|.mtl|.stl|.ply|.om))
							FOREACH(FILE ${TESTFILES})
								FILE(COPY ${FILE} DESTINATION ${UNITTEST_OUTPUT_DIRECTORY})
							ENDFOREACH()

							# Copy library
							IF(WIN32)
								FOREACH(FILE ${OUTPUTS})
									ADD_CUSTOM_COMMAND(
										TARGET openmesh POST_BUILD
										COMMAND ${CMAKE_COMMAND} -E copy
											${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${FILE}
											${UNITTEST_OUTPUT_DIRECTORY}
									)
								ENDFOREACH()
							ELSE()
								ADD_CUSTOM_COMMAND(
									TARGET openmesh POST_BUILD
									COMMAND ${CMAKE_COMMAND} -E copy
										${CMAKE_BINARY_DIR}/Build/python/openmesh.so
										${UNITTEST_OUTPUT_DIRECTORY}
								)
							ENDIF()

							ADD_TEST(
								NAME Python_tests
								WORKING_DIRECTORY ${UNITTEST_OUTPUT_DIRECTORY}
								COMMAND ${PYTHON_EXECUTABLE} -m unittest discover --verbose
							)
						ENDIF()

					ELSE()
						MESSAGE("Checking the Boost Python configuration failed!")
						MESSAGE("Reason: An error occurred while running a small Boost Python test project.")
						MESSAGE("Make sure that your Python and Boost Python libraries match.")
						MESSAGE("Skipping Python Bindings.")
					ENDIF()
				ELSE()
					MESSAGE("Checking the Boost Python configuration failed!")
					MESSAGE("Reason: Python Interpreter ${PYTHONLIBS_VERSION_STRING} not found.")
					MESSAGE("Skipping Python Bindings.")
				ENDIF()
			ELSE()
				MESSAGE("Checking the Boost Python configuration failed!")
				MESSAGE("Reason: Building a small Boost Python test project failed.")
				MESSAGE("Make sure that your Python and Boost Python libraries match.")
				MESSAGE("Skipping Python Bindings.")
			ENDIF()
		ELSE()
			MESSAGE("Boost Python not found! Skipping Python Bindings.")
		ENDIF()
	ELSE()
		MESSAGE("Python not found! Skipping Python Bindings.")
	ENDIF()
ENDIF()
