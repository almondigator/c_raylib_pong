.PHONY: build run clean

BUILD_DIR = ./build
PROJECT_NAME=game

all: build run

build:
	@cmake -S . -B $(BUILD_DIR)
	@cmake --build $(BUILD_DIR)

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf .cache/

run:
	@cd $(BUILD_DIR)/$(PROJECT_NAME) && ./$(PROJECT_NAME)