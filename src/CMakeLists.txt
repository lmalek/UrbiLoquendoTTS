set(URBI_ROOT_DIR "/usr" CACHE PATH "Urbi root directory.")

find_package (Urbi REQUIRED)
find_package (OpenCV REQUIRED)
find_package (Boost REQUIRED)

link_directories (${BOOST_LIBRARYDIR} /opt/Loquendo/LTTS7/bin)

if (WIN32)
  add_definitions( -DBOOST_ALL_DYN_LINK )
endif (WIN32)

include_directories (${URBI_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} /opt/Loquendo/LTTS7/include)

add_library (ULoquendoTTS MODULE ULoquendoTTS.cpp)

target_link_libraries (ULoquendoTTS ${URBI_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} LoqTTS7)

set_target_properties (ULoquendoTTS PROPERTIES
  VERSION 0.0.1
  SOVERSION 0.0.1
  PREFIX "")

install (TARGETS ULoquendoTTS  DESTINATION lib/gostai/uobjects COMPONENT libraries)
