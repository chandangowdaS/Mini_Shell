C_FILES= mini_func.c

export LD_LIBRARY_PATH:=$(shell pwd)

dynamic: $(C_FILES)
	gcc -fPIC -shared $(C_FILES) -o libdynamic.so

Mini_Shell: dynamic
	gcc main.c -L . -ldynamic -o Mini_Shell
	@echo "Mini_Shell is the executable file"

run: Mini_Shell
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./Mini_Shell

clean: 
	rm -f Mini_Shell libdynamic.so





