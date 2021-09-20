CC           = gcc
CFLAGS       = -O2 -Wall -Werror -pedantic -std=gnu99 -pthread
#CFLAGS       = -O0 -g

#set the following variables for custom libraries and/or other objects
EXTOBJS      =
LIBS         = -lm -lpthread
LIBPATHS     = 
INCLUDEPATHS = 

OBJ         = ./.build

PROGRAM     = netster
SRC         = $(wildcard ./*.c)
OBJS        = $(patsubst ./%.c,$(OBJ)/%.o,$(SRC))

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $(OBJS)

$(OBJ)/%.o: %.c netster.h
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: part1
part1:
	cp -n .templates/chat.* .
	cp .templates/part1.h netster.h

.PHONY: part2
part2:
	cp -n .templates/file.* .
	cp .templates/part2.h netster.h

.PHONY: part3
part3:
	cp -n .templates/stopandwait.* .
	cp .templates/part3.h netster.h

.PHONY: part4
part4:
	cp -n .templates/gobackn.* .
	cp .templates/part4.h netster.h

.PHONY: clean
clean:
	-rm -f $(OBJS) $(PROGRAM)
	rm -rf $(OBJ)
