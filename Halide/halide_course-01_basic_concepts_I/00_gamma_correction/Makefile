CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`

ifndef PATH_IN
	PATH_IN=../inputs/camera.png
endif

ifndef GAMMA
	GAMMA=0.5
endif

ifndef PATH_OUT
	PATH_OUT=outputs/camera_g$(GAMMA).png
endif

all: outputs

clean:
	@rm -rf bin outputs

bin/main: src/main.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main
	@mkdir -p $@
	@$^ $(PATH_IN) $(GAMMA) $(PATH_OUT)

.PHONY: all clean
