PROJECT(xcomidl)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)

set(XCOM_INCLUDE_ROOT "/usr/include" CACHE FILEPATH "xcom include root")

if(NOT EXISTS "${XCOM_INCLUDE_ROOT}/xcom/IUnknown.hpp")
    message(SEND_ERROR "Can't find IUnknown.hpp in ${SIMGE_INCLUDE_ROOT}/xcom")
endif()

set(XCOM_LIB_ROOT "/usr/lib" CACHE FILEPATH "xcom library root")

if (WIN32)
  if(NOT EXISTS "${XCOM_LIB_ROOT}/xcom.lib")
      message(SEND_ERROR "Can't find libxcom.so in ${XCOM_LIB_ROOT}")
  endif()
else()
  if(NOT EXISTS "${XCOM_LIB_ROOT}/libxcom.so")
      message(SEND_ERROR "Can't find libxcom.so in ${XCOM_LIB_ROOT}")
  endif()
endif()

LINK_DIRECTORIES(${XCOM_LIB_ROOT})
INCLUDE_DIRECTORIES(${XCOM_INCLUDE_ROOT})
INCLUDE_DIRECTORIES(${xcomidl_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(src)
