GLEW_LIBS := $(shell pkgconf glew --libs)
GLFW_LIBS := $(shell pkgconf glfw3 --libs)

MY_PATH   := ./functions/

TARGET    := test

$(TARGET): main/main.c $(MY_PATH)loadShader.c $(MY_PATH)translation.c $(MY_PATH)loadObj.c
	gcc -o $(TARGET) main/main.c \
	    $(MY_PATH)loadShader.c $(MY_PATH)translation.c $(MY_PATH)loadObj.c \
	    $(GLEW_LIBS) $(GLFW_LIBS) -lm

clean:
	rm -f $(TARGET)
