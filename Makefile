hp:
	rm -f data.db
	@echo " Compile hp_main ...";
	gcc -I ./include/ -L ./lib/ -Wl,-rpath,./lib/ ./examples/hp_main.c ./src/record.c ./src/hp_file.c -lbf -o ./build/hp_main -O2
	./build/hp_main

bf:
	@echo " Compile bf_main ...";
	gcc -I ./include/ -L ./lib/ -Wl,-rpath,./lib/ ./examples/bf_main.c ./src/record.c -lbf -o ./build/bf_main -O2;

test:
	rm -f data.db
	gcc -I ./include/ -L ./lib/ -Wl,-rpath,./lib/ ./examples/test_main.c ./src/record.c ./src/hp_file.c -lbf -o ./build/test_main -O2
	./build/test_main 10000