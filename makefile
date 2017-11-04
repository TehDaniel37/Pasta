CC = gcc
TARGET = pasta

# Debug variables
debug_flags = -g -Wall -pedantic
debug_target_dir = bin/debug

# Release variables
release_flags = -O
release_target_dir = bin/release

target_dir = $(debug_target_dir)
object_dir = obj
src_dir = src/main
include_dir = src/include
sources := $(shell find $(src_dir) -name '*.c')
objects := $(patsubst $(src_dir)/%.c, $(object_dir)/%.o, $(sources))
headers := $(shell find $(include_dir) -name '*.h')

# Test variables
test_target_dir = bin/debug/test
test_object_dir = obj/test
test_dir = src/test
test_resource_dir = res/debug/test
tests := $(shell find $(test_dir) -name '*.c')
test_targets := $(patsubst $(test_dir)/%.c, $(test_target_dir)/%, $(tests))
test_objects := $(filter-out $(object_dir)/main.o, $(objects))

.PHONY: all
all: debug

.PHONY: release
release: CFLAGS=$(release_flags)
release: target_dir=$(release_target_dir)
release: build

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
test: CFLAGS=$(debug_flags)
test: create_dirs $(test_objects) $(test_targets)
	@for file in $(test_targets); do ./$$file; done

# Compile each test
$(test_target_dir)/test_%: $(test_object_dir)/test_%.o
	$(CC) $(CFLAGS) $< $(test_objects) -o $@

$(test_object_dir)/%.o: $(test_dir)/%.c $(include_dir)/test.h
	$(CC) $(CFLAGS) -I $(include_dir) -c $< -o $@
