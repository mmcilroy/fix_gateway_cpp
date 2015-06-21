add_executable( scratch scratch.cpp )

add_executable( main main.cpp )
target_link_libraries( main lua5.2 boost_thread boost_system pthread )

add_executable( fix_script fix_script.cpp )
target_link_libraries( fix_script lua5.2 boost_thread boost_system pthread )
