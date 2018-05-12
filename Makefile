myeditor: myeditor.o
	gcc -o myeditor myeditor.c

clean:
	rm -f myeditor*.o
