FILE(GLOB sources *.cpp)
ADD_LIBRARY(xcomidl_parser SHARED ${sources})
SET_TARGET_PROPERTIES(xcomidl_parser PROPERTIES PREFIX "")
INSTALL(TARGETS xcomidl_parser LIBRARY DESTINATION /lib)
