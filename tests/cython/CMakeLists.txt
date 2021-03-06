INCLUDE_DIRECTORIES(
	${PYTHON_INCLUDE_PATH}
	${PROJECT_SOURCE_DIR}/opencog/atomspace
	${PROJECT_SOURCE_DIR}/opencog/guile
	${PROJECT_SOURCE_DIR}/opencog/util
)

LINK_DIRECTORIES(
	${PROJECT_BINARY_DIR}/opencog/atomspace
	${PROJECT_BINARY_DIR}/opencog/guile
	${PROJECT_BINARY_DIR}/opencog/util
)

IF (HAVE_GUILE)
	LINK_LIBRARIES(smob)
ENDIF (HAVE_GUILE)

LINK_LIBRARIES(
	atomspace
	${GUILE_LIBRARIES}
)

# The PythonEvalUTest tests PythonEval independent of the CogServer. It depends
# on the server being around for the Scheme code but it does not message
# through a "py-eval" server request.
#
ADD_CXXTEST(PythonEvalUTest)

TARGET_LINK_LIBRARIES(PythonEvalUTest
	atomspace
	PythonEval
	atomspace_cython
	${COGUTIL_LIBRARY}
)

SET_TESTS_PROPERTIES(PythonEvalUTest
	PROPERTIES
	ENVIRONMENT "PYTHONDONTWRITEBYTECODE=1"
	ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/opencog/cython"
	ENVIRONMENT "GUILE_LOAD_PATH=${GUILE_LOAD_PATH}"
	)


# The PythonUtilitiesUTest tests C++ utility functions used from Cython bindings
# outside of those bindings so failures are easier to debug.
#
ADD_CXXTEST(PythonUtilitiesUTest)

TARGET_LINK_LIBRARIES(PythonUtilitiesUTest
	PythonEval
	utilities_cython
	atomspace_cython
	atomspace
	${COGUTIL_LIBRARY}
)
SET_TESTS_PROPERTIES(PythonUtilitiesUTest
	PROPERTIES
	ENVIRONMENT "PYTHONDONTWRITEBYTECODE=1"
	ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/opencog/cython")


IF (HAVE_NOSETESTS)

	# Basic test, just does the cython wrapper for opencog/util
	ADD_TEST(Cython ${NOSETESTS_EXECUTABLE} -vs
		${CMAKE_SOURCE_DIR}/tests/cython/)
	SET_TESTS_PROPERTIES(Cython
		PROPERTIES
		ENVIRONMENT "PYTHONDONTWRITEBYTECODE=1"
		ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/opencog/cython")

	IF (HAVE_ATOMSPACE)
		ADD_TEST(CythonAtomSpace ${NOSETESTS_EXECUTABLE} -vs
			${CMAKE_SOURCE_DIR}/tests/cython/atomspace/)
		SET_PROPERTY(TEST CythonAtomSpace
			APPEND PROPERTY ENVIRONMENT "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
				"PYTHONDONTWRITEBYTECODE=1"
				"PYTHONPATH=${PROJECT_BINARY_DIR}/opencog/cython"
				"GUILE_LOAD_PATH=${GUILE_LOAD_PATH}")
		SET_PROPERTY(TEST CythonAtomSpace
					PROPERTY DEPENDS atomspace_cython scheme_wrapper)


		# All of the subsequent cython unit tests  depend on having
		# guile working. So test guile first, and then the others.
		ADD_TEST(CythonGuile ${NOSETESTS_EXECUTABLE} -vs
			${CMAKE_SOURCE_DIR}/tests/cython/guile/)
		SET_PROPERTY(TEST CythonGuile
			APPEND PROPERTY ENVIRONMENT "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
				"PYTHONDONTWRITEBYTECODE=1"
				"PYTHONPATH=${PROJECT_BINARY_DIR}/opencog/cython"
				"GUILE_LOAD_PATH=${GUILE_LOAD_PATH}")
		SET_PROPERTY(TEST CythonGuile
			PROPERTY DEPENDS atomspace_cython scheme_wrapper)

		ADD_TEST(CythonUtilities ${NOSETESTS_EXECUTABLE} -vs
			${CMAKE_SOURCE_DIR}/tests/cython/utilities/)
		SET_TESTS_PROPERTIES(CythonUtilities
			PROPERTIES
			DEPENDS atomspace_cython
			DEPENDS logger_cython
			ENVIRONMENT "GUILE_LOAD_PATH=${GUILE_LOAD_PATH}"
			ENVIRONMENT "PYTHONDONTWRITEBYTECODE=1"
			ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/opencog/cython")

		ADD_TEST(CythonBindlink ${NOSETESTS_EXECUTABLE} -vs
			${CMAKE_SOURCE_DIR}/tests/cython/bindlink/)
		SET_TESTS_PROPERTIES(CythonBindlink
			PROPERTIES
			DEPENDS atomspace_cython
			DEPENDS bindlink_cython
			ENVIRONMENT "PYTHONDONTWRITEBYTECODE=1"
			ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/opencog/cython/:${PROJECT_SOURCE_DIR}/opencog/python/:${PROJECT_SOURCE_DIR}/tests/cython/bindlink")
	ENDIF (HAVE_ATOMSPACE)

ENDIF (HAVE_NOSETESTS)
