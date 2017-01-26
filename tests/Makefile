NAME =		test
CC =		gcc

# Include Flags
IFLAGS =	-I ../inc/ \
			-I ../lib/libmorphux/inc/ \
			-I ../lib/curl/lib/ \
			-I ../lib/json-c/ \
			-I ../lib/sqlite

# Optimisation Flags
OFLAGS =	-std=c99 -g -O3

# Severity Flags
SFLAGS =	-Wall -Wextra -Werror -Wno-unused-result

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
	$(MAKE) fclean all OFLAGS="-g -O0 -coverage -lgcov"

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
