CC = gcc
TARGET = schedr

# Compiler flags
debug_flags = -g -Wall -pedantic -Werror
release_flags = -O3
test_flags = $(debug_flags) -DTEST --coverage

# Directories
debug_target_dir = bin/debug
release_target_dir = bin/release
test_target_dir = bin/debug/test
target_dir = $(debug_target_dir)
include_dir = src/include
src_dir = src/main
test_dir = src/test
object_dir = obj
test_object_dir = obj/test
test_resource_dir = res/debug/test

# Files
sources := $(shell find $(src_dir) -name '*.c')
objects := $(patsubst $(src_dir)/%.c, $(object_dir)/%.o, $(sources))
headers := $(shell find $(include_dir) -name '*.h')
tests := $(shell find $(test_dir) -name '*.c')
test_targets := $(patsubst $(test_dir)/%.c, $(test_target_dir)/%, $(tests))
test_objects := $(filter-out $(object_dir)/main.o, $(objects))

.PHONY: all
all: debug

.PHONY: release
release: CFLAGS=$(release_flags)
release: target_dir=$(release_target_dir)
release: create_dirs
	$(CC) $(CFLAGS) -I $(include_dir) $(sources) -o $(target_dir)/$(TARGET)

.PHONY: check
check:
	$(CC) -DTEST -fsyntax-only -I $(include_dir) $(sources) $(tests)
	cppcheck -q --enable=all -I src/include --language=c --platform=unix64 --std=c11 --suppress=missingIncludeSystem src

.PHONY: debug
debug: CFLAGS=$(debug_flags)
debug: target_dir=$(debug_target_dir)
debug: build

.PHONY: clean
clean:
	rm -rf obj/* bin/debug/* bin/release/*

.PHONY: create_dirs
create_dirs: $(test_object_dir) $(test_target_dir) $(release_target_dir)

$(test_object_dir):
	mkdir -p $(test_object_dir)

$(test_target_dir):
	mkdir -p $(test_target_dir)

$(release_target_dir):
	mkdir -p $(release_target_dir)

.PHONY: build
build: create_dirs $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(target_dir)/$(TARGET)

$(object_dir)/%.o: $(src_dir)/%.c $(headers)
	$(CC) $(CFLAGS) -I $(include_dir) -c $< -o $@

.PHONY: test
test: CFLAGS=$(test_flags)
test: create_dirs $(test_targets)
test:
	@gcov $(object_dir)/*.gcno

# Compile each test
$(test_target_dir)/%: $(test_object_dir)/%.o $(test_objects)
	$(CC) $(CFLAGS) $< $(test_objects) -o $@ 
	@valgrind -q --track-origins=yes --leak-check=full $@

$(test_object_dir)/%.o: $(test_dir)/%.c $(headers)
	$(CC) $(CFLAGS) -I $(include_dir) -c $< -o $@
