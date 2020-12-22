PROJECT := GefiLTe
BUILDDIR := $(CURDIR)/build
OUT := $(BUILDDIR)/$(PROJECT)

SOURCES += \
	$(wildcard src/*.c)

INCLUDES += \
	include/

ifeq ($(OS),WINDOWS)
	INCLUDES += glfw.win/include/
	GL_LIBS = -L$(CURDIR)/glfw.win32/lib-mingw/ -lglfw3 -lopengl32 -lgdi32
include glad32/glad.mk
else
	GL_LIBS = -L/usr/local/lib -lglfw3 -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp  -lGL
	GL_CFLAGS = -I/usr/local/include
include glad/glad.mk
endif

main: $(BUILDDIR)
	gcc $(GL_CFLAGS) -o $(OUT) $(SOURCES) $(addprefix -I, $(INCLUDES)) $(addprefix -D, $(DEFINES)) $(GL_LIBS)

ifeq ($(OS),WINDOWS)
$(BUILDDIR):
	mkdir $(subst /,\,$@)
else
$(BUILDDIR):
	mkdir -p $@
endif