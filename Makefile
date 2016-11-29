# by dancal

CC = gcc
CPP = g++
LD = g++

CC = /opt/rh/devtoolset-3/root/usr/bin/gcc
CPP = /opt/rh/devtoolset-3/root/usr/bin/g++
LD = /opt/rh/devtoolset-3/root/usr/bin/g++

#debug
#DEBUG = -g3 -fno-inline -pg -Wall
#OPT_STATIC_LIBS = -L/usr/local/lib -lboost_system -lboost_thread -lboost_filesystem -ljansson -lpthread -lrt -lc -lmecab -lstdc++ -lrt -lc -lm

#release
#OPTIMISATIONS = -g -Wall -DUSE_JEMALLOC
OPTIMISATIONS = -O2 -DNDEBUG -DPIC -fPIC -msse3 -pipe -fomit-frame-pointer -frerun-cse-after-loop -frerun-loop-opt -fprefetch-loop-arrays
OPT_STATIC_LIBS = -L/usr/lib64 -lpthread -lstdc++ -lrt -lc -lm -ldl -lz -lssl -lcrypto -ljemalloc

CPP_INCLUDE = -I./libs/aerospike-client-c/target/Linux-x86_64/include -I./libs/jansson/src -I./libs/boost_1_62_0

CPP_EXT_FLAGS = ./libs/aerospike-client-c/target/Linux-x86_64/lib/libaerospike.a \
				./libs/jansson/src/.libs/libjansson.a \
				./libs/boost_1_62_0/stage/lib/libboost_filesystem.a \
				./libs/boost_1_62_0/stage/lib/libboost_thread.a \
				./libs/boost_1_62_0/stage/lib/libboost_system.a \
				./libs/boost_1_62_0/stage/lib/libboost_regex.a \
				./libs/boost_1_62_0/stage/lib/libboost_iostreams.a \
				./libs/boost_1_62_0/stage/lib/libboost_timer.a \
				-lpthread -ljemalloc

# default flags
CFLAGS = ${DEBUG} ${OPTIMISATIONS}
CPPFLAGS = ${DEBUG} ${OPTIMISATIONS} ${CPP_INCLUDE}
LDFLAGS = ${OPT_STATIC_LIBS} ${CPP_EXT_FLAGS}

#target 
AS_PROXY_SERVER = ./bin/as_proxyd

OBJ_AS_PROXYD_CPP = \
	src/connection.opp  \
	src/mime_types.opp  \
	src/reply.opp  \
	src/request_handler.opp  \
	src/request_parser.opp  \
	src/util.opp \
	src/server.opp \
	src/INIReader.opp \
	src/microtime.opp \
	src/as_gzip.opp \
	src/wp_base64.opp \
	src/main.opp 

OBJ_AS_PROXYD_C = src/ini.o
		
OBJ_CPP = ${OBJ_AS_PROXYD_CPP}

OBJ_C = ${OBJ_AS_PROXYD_C}

OBJ = ${OBJ_CPP}  ${OBJ_C}

TARGETS = ${AS_PROXY_SERVER}

all:	${TARGETS}

${AS_PROXY_SERVER}:	${OBJ_AS_PROXYD_CPP} ${OBJ_AS_PROXYD_C}
	${LD} ${CPPFLAGS} -o $@ $^ ${LDFLAGS}

# compile c files
	
%.o:	%.c
	${CC} ${CFLAGS} -c $< -MD -MF $(<:%.c=%.dep) -o $@

# compile c++ files
	
%.opp:	%.cpp
	${CPP} ${CPPFLAGS} -c $< -MD -MF $(<:%.cpp=%.dep) -o $@

#misc

.PHONY:	clean all
clean:
	rm -f ${TARGETS}
	rm -f *.[hc]pp~ Makefile~ core
	rm -f ${OBJ}
	rm -f $(OBJ_C:%.o=%.dep) $(OBJ_CPP:%.opp=%.dep)

-include $(OBJ_C:%.o=%.dep) $(OBJ_CPP:%.opp=%.dep)
