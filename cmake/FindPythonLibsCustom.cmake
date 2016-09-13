# FindPythonLibsCustom
# --------------------
#
# This module locates Python libraries.
#
# This code sets the non-deprecated subset of the variables set by CMake's
# built-in FindPythonLibs:
#
# PYTHONLIBS_FOUND - have the Python libs been found
# PYTHON_LIBRARIES - path to the Python library
# PYTHON_INCLUDE_DIRS - path to where Python.h is found
# PYTHONLIBS_VERSION_STRING - version of the Python libs found
#
# If calling both `find_package(PythonInterp)` and
# `find_package(PythonLibsCustom)`, call `find_package(PythonInterp)` first.

include(FindPackageHandleStandardArgs)

find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
  # Get paths directly from the Python interpreter.
  find_file(_Python_pyinfo_py pyinfo.py PATHS ${CMAKE_MODULE_PATH})
  if(_Python_pyinfo_py)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} ${_Python_pyinfo_py} OUTPUT_VARIABLE _Python_pyinfo)
    unset(_Python_pyinfo_py)
    if(_Python_pyinfo)
      string(REGEX REPLACE "^.*\ninc_dir:([^\n]*)\n.*$" "\\1" PYTHON_INCLUDE_DIRS ${_Python_pyinfo})
      string(REGEX REPLACE "^.*\nlib_name:([^\n]*)\n.*$" "\\1" _Python_lib_name ${_Python_pyinfo})
      string(REGEX REPLACE "^.*\nlib_ver:([^\n]*)\n.*$" "\\1" PYTHONLIBS_VERSION_STRING ${_Python_pyinfo})
      string(REGEX REPLACE "^.*\nlib_dir:([^\n]*)\n.*$" "\\1" _Python_lib_dir ${_Python_pyinfo})
      unset(_Python_pyinfo)
      find_library(PYTHON_LIBRARIES NAMES ${_Python_lib_name} HINTS ${_Python_lib_dir})
      unset(_Python_lib_name)
      unset(_Python_lib_dir)
    endif(_Python_pyinfo)
  else(_Python_pyinfo_py)
    message(SEND_ERROR "failed to find pyinfo.py")
  endif(_Python_pyinfo_py)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(PythonLibs DEFAULT_MSG PYTHON_LIBRARIES PYTHON_INCLUDE_DIRS PYTHONLIBS_VERSION_STRING)
else(PYTHONINTERP_FOUND)
  # Fall back to FindPythonLibs if we don't have an interpreter to query.
  find_package(PythonLibs)
endif(PYTHONINTERP_FOUND)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PythonLibsCustom DEFAULT_MSG PYTHONLIBS_FOUND)
