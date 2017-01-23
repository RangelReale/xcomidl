FILE(GLOB sources *.cpp)
ADD_LIBRARY(xcomidl_cppgen SHARED ${sources})
SET_TARGET_PROPERTIES(xcomidl_cppgen PROPERTIES PREFIX "")
INSTALL(TARGETS xcomidl_cppgen LIBRARY DESTINATION /lib)
