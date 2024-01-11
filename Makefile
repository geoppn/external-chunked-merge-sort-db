sort:
	@echo " Compile sort_main ...";
	rm -rf ./build/sort_main
	rm -rf ./data.db
	rm -rf ./out0.db
	rm -rf ./out1.db
	gcc -g -I ./include/ -L ./lib/ -Wl,-rpath,./lib/ ./examples/sort_main.c ./src/record.c ./src/sort.c ./src/merge.c ./src/chunk.c -lbf -lhp_file -o ./build/sort_main -O2
