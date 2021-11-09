file(GLOB SRC_FILES "*.cpp")

foreach( _name_ext ${SRC_FILES})

get_filename_component(_name ${_name_ext} NAME_WLE)
add_executable(test_${_name} ${_name}.cpp)
target_link_libraries(test_${_name} PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} fmt::fmt)
add_test(${_name}_test test_${_name})

endforeach()