EXT_INC = 
EXTERNALS_INC = 
EXT_LIBS =# -lgsl -lgslcblas 
CC = gcc
CPP = g++
CFLAGS = -g3 -Wall -pedantic -std=iso9899:1999 -I. -I$(EXT_INC) -I${EXTERNALS_INC} -O0 -fopenmp
#CFLAGS = -g3 -Wall -pedantic -std=iso9899:1999 -Wtraditional -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wconversion  -I. -I$(EXT_INC) -I${EXTERNALS_INC} -O0 -fopenmp
LDLIBS= -lm
LDFLAGS= -L${LM_ROOT}/ext/lib-x86_64/


SRCS =  knn_search_test.c \
	kdtree.c

OBJS = $(SRCS:%.c=%.o)

BIN_NAME = test

all: $(BIN_NAME)

$(BIN_NAME): $(OBJS)
	$(CC) ${CFLAGS} -o $@ $^ $(LDFLAGS) $(LDLIBS) $(EXT_LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $^ $(LDFLAGS) $(LDLIBS) $(EXT_LIBS)

clean: 
	rm -f *.o
	rm -f $(BIN_NAME)

