set(DOXYGEN_CREATE_SUBDIRS "YES")
set(DOXYGEN_EXTRACT_ALL "YES")
set(DOXYGEN_EXTRACT_PRIVATE "YES")
set(DOXYGEN_EXTRACT_PACKAGE "YES")
set(DOXYGEN_EXTRACT_STATIC "YES")
set(DOXYGEN_EXTRACT_ANON_NSPACES "YES")
set(DOXYGEN_FORCE_LOCAL_INCLUDES "YES")
set(DOXYGEN_WARN_NO_PARAMDOC "YES")
set(DOXYGEN_REFERENCED_BY_RELATION "YES")
set(DOXYGEN_REFERENCES_RELATION "YES")

set(abs_autogen_source_paths)
foreach(target ${DOXYGEN_SCRIPT_QT_TARGETS})
  file(GLOB_RECURSE autogen_sources
    "${DOXYGEN_SCRIPT_QT_AUTOGEN_DIR}/${target}_autogen/*.cpp"
    "${DOXYGEN_SCRIPT_QT_AUTOGEN_DIR}/${target}_autogen/*.h")
  list(APPEND abs_autogen_source_paths ${autogen_sources})
endforeach(target)
string(REPLACE ";" " " abs_autogen_source_paths_ws_splited "${abs_autogen_source_paths}")
set(DOXYGEN_INPUT "${DOXYGEN_INPUT} ${abs_autogen_source_paths_ws_splited}")

include("${DOXYGEN_SCRIPT_DOXYFILE_DEFAULTS}")
configure_file(
  "${DOXYGEN_SCRIPT_DOXYFILE_TEMPLATE}"
  "${DOXYGEN_SCRIPT_OUTPUT_DOXYFILE}"
  @ONLY)
