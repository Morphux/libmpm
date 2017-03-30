################################### LICENSE ####################################
#                            Copyright 2016 Morphux                            #
#                                                                              #
#        Licensed under the Apache License, Version 2.0 (the "License");       #
#        you may not use this file except in compliance with the License.      #
#                  You may obtain a copy of the License at                     #
#                                                                              #
#                 http://www.apache.org/licenses/LICENSE-2.0                   #
#                                                                              #
#      Unless required by applicable law or agreed to in writing, software     #
#       distributed under the License is distributed on an "AS IS" BASIS,      #
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  #
#        See the License for the specific language governing permissions and   #
#                       limitations under the License.                         #
################################################################################

# Names and binaries
NAME =		libmpm.a
CC =		gcc
LIB =		ar

# Include Flags
IFLAGS =	-I inc/ \
			-I lib/libmorphux/inc/ \
			-I lib/curl/lib/ \
			-I lib/json-c/ \
			-I lib/sqlite \
			-I lib/libconfuse/src/

# Optimisation Flags
OFLAGS =	-std=gnu99 -g -O3

# Severity Flags
SFLAGS =	-Wall -Wextra -Werror -Wno-unused-result

# Library Flags
LFLAGS =	-L lib/libmorphux -lmorphux \
			-L ./lib/curl/lib/.libs/ -lcurl \
			-L ./lib/json-c/.libs -ljson-c \
			-L ./lib/libconfuse/.libs -lconfuse
CFLAGS =	$(SFLAGS) $(IFLAGS) $(OFLAGS) $(LFLAGS)
ARFLAGS =	-cq

# Sources
SRCS =		$(wildcard src/*.c)
SQLITE =	lib/sqlite/sqlite3.o
OBJS =		$(SRCS:%.c=%.o)

# Library Merge
OBJ_DIR =	objs/
L_MORPHUX =	../lib/libmorphux/libmorphux.a
L_CURL = 	../lib/curl/lib/.libs/libcurl.a
L_JSON = 	../lib/json-c/.libs/libjson-c.a
L_CONF =	../lib/libconfuse/src/.libs/libconfuse.a

all: $(NAME)

%.o: %.c
	@echo "CC\t\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $^

$(NAME): $(SQLITE) $(OBJS)
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
	@cp $(OBJS) $(OBJ_DIR)
	@cp $(SQLITE) $(OBJ_DIR)
	@echo "AR\t\t$(L_MORPHUX) $(L_CURL) $(L_JSON) $(L_CONF)"
	@cd $(OBJ_DIR) && ar -x $(L_MORPHUX) && ar -x $(L_CURL) && ar -x $(L_JSON) && ar -x $(L_CONF)
	@echo "CCLD\t\t$(NAME)"
	@$(LIB) $(ARFLAGS) $(NAME) objs/*.o
	@rm -rf $(OBJ_DIR)

single: $(SQLITE) $(OBJS)
	@echo "LD\t\t$(NAME)"
	@$(LIB) $(ARFLAGS) $(NAME) $(OBJS)

init:
	make -C lib/libmorphux
	cd lib/curl/ && ./buildconf && ./configure --disable-shared --enable-debug --enable-maintainer-mode && make
	cd lib/json-c/ && sh autogen.sh && ./configure --disable-shared && make
	cd lib/libconfuse/ && sh autogen.sh && ./configure && make

doc:
	doxygen docs/doxyfile

check:
	make -C lib/libmorphux/ test
	$(MAKE) fclean all OFLAGS="-DCOMPILE_WITH_TEST -Wno-error"
	make -C tests clean check

coverage:
	make -C lib/libmorphux/ test
	$(MAKE) fclean all OFLAGS="-std=gnu99 -g -O0 -coverage -lcov -DCOMPILE_WITH_TEST"
	make -C tests coverage check
	gcov -o src/ $(SRCS)
	make -C tests fclean

clean:
	rm -f $(OBJS)
	rm -f *.gcov
	rm -f src/*.gcno
	rm -f src/*.gcda

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: fclean clean all docs
