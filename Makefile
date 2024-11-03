CC = c++
SRC = $(wildcard src/*.cpp)
HR = $(wildcard include/*.h)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
BASS_DIR = ./bass

LDFLAGS = ./MLX42/build/libmlx42.a
DYLIB = echo "done"

NAME = as
UNAME = $(shell uname)

ifeq ($(UNAME), Linux)
	BASS_DIR = ./bass_linux
	LDFLAGS += -lglfw -ldl -pthread -lm $(BASS_DIR)/libs/x86_64/libbass.so
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS += -lglfw -L $(shell brew --prefix glfw)/lib -framework Cocoa -framework IOKit $(BASS_DIR)/libbass.dylib
	DYLIB = install_name_tool -change @rpath/libbass.dylib ./bass/libbass.dylib $(NAME)
endif
CFLAGS = -Iinclude -I./MLX42/include/MLX42 -I$(BASS_DIR)

all: mlx bass $(NAME)
	$(DYLIB)

bass:
	@cd $(BASS_DIR) && make 64bit

mlx:
	@cmake -B ./MLX42/build ./MLX42
	@cmake --build ./MLX42/build -j16

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

build/%.o: src/%.cpp $(HR)
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf build && rm -rf MLX42/build
	rm -rf $(BASS_DIR)/64bit

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean
