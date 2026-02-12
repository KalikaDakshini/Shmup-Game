add_executable(test_object)

target_sources(test_object
PRIVATE
test_object.cpp
)

target_link_libraries(test_object
PRIVATE
object
)

function(make_test op)
add_test(
NAME ${op}
COMMAND test_object ${op}
)
endfunction()
