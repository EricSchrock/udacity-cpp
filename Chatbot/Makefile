.PHONY: all
all: build test

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make

.PHONY: test
test:
	cd build && \
	./membot

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make && \
	gdb ./membot

.PHONY: grind
grind:
	cd build && \
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind.log ./membot
	echo
	grep -A 2 "HEAP SUMMARY" build/valgrind.log
	echo
	grep -A 8 "LEAK SUMMARY" build/valgrind.log
	echo
	grep "ERROR SUMMARY" build/valgrind.log

.PHONY: clean
clean:
	rm -rf build
