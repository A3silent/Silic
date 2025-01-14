# 编译器设置
CC = g++
CFLAGS = -std=c++17

# OpenGL库
LIBS = -L../lib

# 包含目录
INCLUDES = -I../include ../src/*.cpp ../src/glad.c  -lglfw3dll 

# 目标文件
TARGET = program

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) 

test:
	program.exe