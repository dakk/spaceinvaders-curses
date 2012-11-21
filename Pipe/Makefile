OUTPUT = spaceinvaders
SOURCES = main.o control.o space_ship.o missile.o alien.o bomb.o scores.o utility.o
CLAGS =
LIBS = -lncurses

spaceinvaders: $(SOURCES)
	gcc -o $(OUTPUT) $(SOURCES) $(LIBS)
	
	
main.o: main.c space_invaders.h
	gcc -c -o main.o main.c $(CFLAGS)

control.o: control.c control.h space_invaders.h utility.h
	gcc -c -o control.o control.c $(CFLAGS)
	
space_ship.o: space_ship.c space_ship.h space_invaders.h utility.h
	gcc -c -o space_ship.o space_ship.c $(CFLAGS)
	
missile.o: missile.c missile.h space_invaders.h
	gcc -c -o missile.o missile.c $(CFLAGS)
	
alien.o: alien.c alien.h space_invaders.h
	gcc -c -o alien.o alien.c $(CFLAGS)

bomb.o: bomb.c bomb.h space_invaders.h
	gcc -c -o bomb.o bomb.c $(CFLAGS)
	
scores.o: scores.c scores.h
	gcc -c -o scores.o scores.c $(CFLAGS)


utility.o: utility.c utility.h
	gcc -c -o utility.o utility.c $(CFLAGS)
	
	
clean:
	rm *.o $(OUTPUT)
