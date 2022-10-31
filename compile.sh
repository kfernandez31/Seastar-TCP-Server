c++ \
src/validation.cc \
src/parsing.cc \
src/store.cc \
src/server.cc \
src/server_main.cc \
`pkg-config --cflags --libs --static $SEASTAR/build/release/seastar.pc`