FILE(GLOB sources *.cpp)
LINK_LIBRARIES(xcom)
if (WIN32)
  LINK_LIBRARIES(Rpcrt4 Shlwapi)
endif()
ADD_LIBRARY(xcomidl_cppgen SHARED ${sources})
SET_TARGET_PROPERTIES(xcomidl_cppgen PROPERTIES PREFIX "")
#INSTALL(TARGETS xcomidl_cppgen LIBRARY DESTINATION lib)
FILE(WRITE "${CMAKE_BINARY_DIR}/xcomidl.config" "${CMAKE_INSTALL_PREFIX}/lib\n")
#INSTALL(FILES "${CMAKE_BINARY_DIR}/xcomidl.config" DESTINATION bin)
