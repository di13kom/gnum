Ui: Ui.c net.c
	gcc  -Wall `pkg-config --cflags gtk+-3.0` $^ -o $@ `pkg-config --libs gtk+-3.0`
debugUi: Ui.c net.c
	gcc -g -Wall `pkg-config --cflags gtk+-3.0` $^ -o $@ `pkg-config --libs gtk+-3.0` -DDEBUG
clean:
	rm -rf Ui debugUi
