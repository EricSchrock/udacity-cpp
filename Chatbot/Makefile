.PHONY: all
all: build test

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make

./PHONY: test
test:
	cd build && \
	./membot

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make

.PHONY: clean
clean:
	rm -rf build
