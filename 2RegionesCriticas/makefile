CC= gcc -Wall
HEADER_FILES_DIR = .
INCLUDES = -I $(HEADER_FILES_DIR)
LIBS = -lm -lpthread
APELLIDOS = CardamaSantiago
NOMBRE = FranciscoJavier
PRACTICA = 1
MAIN= buffer
SRCS = $(wildcard *.c)
DEPS = $(HEADER_FILES_DIR)/$(wildcard *.h)
OBJS = $(SRCS:.c=.o) 

$(MAIN): $(OBJS)
	$(CC) -o $(MAIN) $(OBJS) $(LIBS) 

%.o: %.c $(DEPS)
	$(CC) -c $< $(INCLUDES)

cleanall: clean
	rm -f $(MAIN)
clean:
	rm -f *.o *~
	
zip:
	zip $(APELLIDOS)$(NOMBRE)_$(PRACTICA) *.c $(HEADER_FILES_DIR)/*.h
