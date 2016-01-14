INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_SNR snr)

FIND_PATH(
    SNR_INCLUDE_DIRS
    NAMES snr/api.h
    HINTS $ENV{SNR_DIR}/include
        ${PC_SNR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SNR_LIBRARIES
    NAMES gnuradio-snr
    HINTS $ENV{SNR_DIR}/lib
        ${PC_SNR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SNR DEFAULT_MSG SNR_LIBRARIES SNR_INCLUDE_DIRS)
MARK_AS_ADVANCED(SNR_LIBRARIES SNR_INCLUDE_DIRS)

