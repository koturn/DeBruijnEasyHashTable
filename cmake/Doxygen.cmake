find_package(Doxygen)

set(DOXYGEN_CURRENT_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}")

if(DOXYGEN_FOUND)
  add_custom_target(doxygen)
endif()


function(add_doxygen)
  if(NOT DOXYGEN_FOUND)
    return()
  endif()

  set(options)
  set(oneValueArgs
    NAME
    OUTPUT_DIRECTORY)
  set(multiValueArgs
    TARGETS
    QT_TARGETS
    SOURCES
    INCLUDE_DIRECTORIES
    COMPILE_DEFINITIONS
    GENERATE_LATEX
    HAVE_DOT)
  cmake_parse_arguments(DOXYGEN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT DOXYGEN_NAME)
    if(DEFINED PROJECT_NAME AND NOT "${PROJECT_NAME}" STREQUAL "")
      set(DOXYGEN_NAME "${PROJECT_NAME}")
    else()
      message(FATAL_ERROR "Name of document is empty")
    endif()
  endif()

  if(NOT DEFINED DOXYGEN_OUTPUT_DIRECTORY)
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/document")
  endif()

  if(NOT DEFINED DOXYGEN_GENERATE_LATEX)
    set(DOXYGEN_GENERATE_LATEX "NO")
  endif()
  if(NOT DEFINED DOXYGEN_HAVE_DOT)
    set(DOXYGEN_HAVE_DOT "NO")
  endif()

  foreach(target ${DOXYGEN_TARGETS} ${DOXYGEN_QT_TARGETS})
    get_property(sources
      TARGET ${target}
      PROPERTY SOURCES)
    list(APPEND DOXYGEN_SOURCES ${sources})

    get_property(incdirs
      TARGET ${target}
      PROPERTY INCLUDE_DIRECTORIES)
    list(APPEND DOXYGEN_INCLUDE_DIRECTORIES ${incdirs})

    get_property(defs
      DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      PROPERTY COMPILE_DEFINITIONS)
    list(APPEND DOXYGEN_COMPILE_DEFINITIONS ${defs})
  endforeach(target)

  # Convert to absolute path.
  set(abs_source_paths "")
  foreach(source ${DOXYGEN_SOURCES})
    string(TOLOWER "${source}" source_lower)
    if("${source_lower}" MATCHES "\\.(c|cc|cxx|cpp|c\\+\\+|ii|ixx|ipp|i\\+\\+|inl|h|hh|hxx|hpp|h\\+\\+|inc)")
      if(IS_ABSOLUTE "${source}")
        list(APPEND abs_source_paths "${source}")
      else()
        get_filename_component(abs_source "${source}" ABSOLUTE)
        list(APPEND abs_source_paths "${abs_source}")
      endif()
    endif()
  endforeach(source)
  list(REMOVE_DUPLICATES abs_source_paths)

  set(abs_incdir_header_paths "")
  foreach(incdir ${DOXYGEN_INCLUDE_DIRECTORIES})
    file(GLOB headers
      ${incdir}/*.ii
      ${incdir}/*.ixx
      ${incdir}/*.ipp
      ${incdir}/*.i++
      ${incdir}/*.inl
      ${incdir}/*.h
      ${incdir}/*.hh
      ${incdir}/*.hxx
      ${incdir}/*.hpp
      ${incdir}/*.h++
      ${incdir}/*.inc)
    list(APPEND abs_incdir_header_paths ${headers})
  endforeach(incdir)
  list(REMOVE_DUPLICATES abs_incdir_header_paths)

  set(abs_input_dir_paths "")
  foreach(input_dir ${DOXYGEN_INPUT_DIRECTORIES})
    if(IS_ABSOLUTE "${input_dir}")
      list(APPEND abs_input_dir_paths "${input_dir}")
    else()
      get_filename_component(abs_input_dir "${input_dir}" ABSOLUTE)
      list(APPEND abs_input_dir_paths "${abs_input_dir}")
    endif()
  endforeach()

  set(qt_autogen_targets "")
  foreach(target ${DOXYGEN_QT_TARGETS})
    list(APPEND qt_autogen_targets "${target}_autogen")
  endforeach(target)

  # Convert lists to whitespace separated strings
  string(REPLACE ";" " " abs_source_paths_ws_splited "${abs_source_paths}")
  string(REPLACE ";" " " abs_incdir_header_paths_ws_splited "${abs_incdir_header_paths}")
  string(REPLACE ";" " " abs_incdir_paths_ws_splited "${INCLUDE_DIRECTORIES}")
  string(REPLACE ";" " " defs_ws_splited "${DOXYGEN_COMPILE_DEFINITIONS}")

  # Create output directory
  add_custom_command(
    OUTPUT "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}"
    COMMENT "Creating documentation directory for ${DOXYGEN_NAME}...")

  # Create Doxyfile from Doxyfile.in.
  add_custom_command(
    OUTPUT "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}/Doxyfile"
    COMMAND ${CMAKE_COMMAND}
      -D "DOXYGEN_SCRIPT_DOXYFILE_TEMPLATE=${CMAKE_CURRENT_BINARY_DIR}/CMakeDoxyfile.in"
      -D "DOXYGEN_SCRIPT_DOXYFILE_DEFAULTS=${CMAKE_CURRENT_BINARY_DIR}/CMakeDoxygenDefaults.cmake"
      -D "DOXYGEN_SCRIPT_OUTPUT_DOXYFILE=${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}/Doxyfile"
      -D "DOXYGEN_SCRIPT_QT_TARGETS=${DOXYGEN_QT_TARGETS}"
      -D "DOXYGEN_SCRIPT_QT_AUTOGEN_DIR=${CMAKE_CURRENT_BINARY_DIR}"
      -D "DOXYGEN_GENERATE_LATEX=${DOXYGEN_GENERATE_LATEX}"
      -D "DOXYGEN_HAVE_DOT=${DOXYGEN_HAVE_DOT}"
      -D "DOXYGEN_PROJECT_NAME=${DOXYGEN_NAME}"
      -D "DOXYGEN_OUTPUT_DIRECTORY=${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}"
      -D "DOXYGEN_STRIP_FROM_PATH=${CMAKE_CURRENT_SOURCE_DIR}"
      -D "DOXYGEN_WARN_LOGFILE=${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}.warnings"
      -D "DOXYGEN_INPUT=${abs_source_paths_ws_splited} ${abs_incdir_header_paths_ws_splited}"
      -D "DOXYGEN_INCLUDE_PATH=${abs_incdir_paths_ws_splited}"
      -D "DOXYGEN_PREDEFINED=${defs_ws_splited}"
      -D "DOXYGEN_DOCUMENTATION_OUTPUT_PATH=${DOXYGEN_OUTPUT_DIRECTORY}"
      -P "${DOXYGEN_CURRENT_LIST_DIR}/DoxygenScript.cmake"
    DEPENDS "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}" ${qt_autogen_targets}
    WORKING_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}"
    COMMENT "Generating Doxyfile for ${DOXYGEN_NAME}...")

  # Execute doxygen.
  add_custom_command(
    OUTPUT "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}/index.html"
    COMMAND ${DOXYGEN_EXECUTABLE}
    DEPENDS "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}/Doxyfile"
    WORKING_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}"
    COMMENT "Creating HTML documentation for ${DOXYGEN_NAME}...")

  add_custom_target(doxygen-${DOXYGEN_NAME}
    DEPENDS "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_NAME}/index.html")

  add_dependencies(doxygen doxygen-${DOXYGEN_NAME})
endfunction()
