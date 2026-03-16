cmake_minimum_required(VERSION 3.28)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	project(mqtt-rpc-client-qt VERSION 1.0.0 LANGUAGES CXX)
endif()

find_package(Qt6 REQUIRED COMPONENTS Network Mqtt WebSockets)

add_library(mqtt-rpc-client-qt STATIC
	src/mqtt_rpc_client_qt.cpp
	src/op_command.cpp
	src/util.cpp
	src/commands.cpp

	src/mqtt_rpc_client_qt.h
	src/op_command.h
	src/util.h
	src/commands.h
)

target_compile_definitions(mqtt-rpc-client-qt PRIVATE
	MQTTRPCCLIENTQT_LIBRARY
	QT_DEPRECATED_WARNINGS
)

target_link_libraries(mqtt-rpc-client-qt PRIVATE
	Qt6::Mqtt
	Qt6::Network
	Qt6::WebSockets
)

target_include_directories(mqtt-rpc-client-qt PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/src
)

include(GNUInstallDirs)
install(TARGETS mqtt-rpc-client-qt
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
