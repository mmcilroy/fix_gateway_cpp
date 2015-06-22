add_executable( app app.cpp )
target_link_libraries( app lua5.2 boost_thread boost_system pthread )
