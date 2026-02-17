add_executable(testWindow)

target_sources(testWindow
PRIVATE
testWindow.cpp
)

target_link_libraries(testWindow
PRIVATE
window
)

function(make_test op)
add_test(
NAME ${op}
COMMAND testWindow ${op}
)
endfunction()
