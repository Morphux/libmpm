NAME =		test
CC =		gcc

# Include Flags
IFLAGS =	-I ../inc/ \
			-I ../lib/libmorphux/inc/ \
			-I ../lib/curl/lib/ \
			-I ../lib/json-c/ \
			-I ../lib/sqlite \
			-I ../lib/libconfuse/src/

# Optimisation Flags
OFLAGS =	-std=gnu99 -g -O3

# Severity Flags
SFLAGS =	-Wall -Wextra -Wno-unused-result -DCOMPILE_WITH_TEST

# Library Flags
#LFLAGS =	-L ../lib/libmorphux -lmorphux \
			#-L ../lib/curl/.libs/ -lcurl \
			#-L ../lib/json-c/.libs -ljson-c \

LFLAGS=			-L ../ -lmpm -pthread -ldl

CFLAGS =	$(SFLAGS) $(IFLAGS) $(OFLAGS) $(LFLAGS)

SRCS =		$(wildcard *.c)
OBJS =		$(SRCS:%.c=%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(CFLAGS)

check: $(NAME)
	./$(NAME)

valgrind: $(NAME)
	valgrind --leak-check=full --error-exitcode=1 ./$(NAME)

coverage:
	$(MAKE) fclean all OFLAGS="-g -O0 -coverage -lgcov -std=c99"

clean:
	rm -f $(OBJS)
	rm -f *.gcov
	rm -f *.gcno
	rm -f *.gcda


fclean: clean
	rm -f $(NAME)

re: fclean all
