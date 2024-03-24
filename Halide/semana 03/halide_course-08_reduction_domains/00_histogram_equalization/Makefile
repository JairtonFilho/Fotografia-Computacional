CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h,schedule
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`


ifndef PATH_IN
	PATH_IN=../inputs/camera.png
endif

ifndef PATH_OUT
	PATH_OUT=outputs/result.png
endif

TARGET=host
ifdef PROFILE
	ifeq ($(PROFILE), true)
		TARGET=host-profile
	endif
endif

all: outputs

clean:
	@rm -rf bin outputs
	
bin/rgb2gray.generator: src/generators/HalideRGB2Gray.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/histogram_equalization.generator: src/generators/HalideHistEq.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/rgb2gray.o: bin/rgb2gray.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f rgb2gray -g rgb2gray target=$(TARGET) -p $(HALIDE_ROOT)/lib/libautoschedule_mullapudi2016.so autoscheduler=Mullapudi2016

bin/histogram_equalization.o: bin/histogram_equalization.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f histogram_equalization -g histogram_equalization target=$(TARGET)  -p $(HALIDE_ROOT)/lib/libautoschedule_mullapudi2016.so autoscheduler=Mullapudi2016

bin/main: src/main.cpp bin/rgb2gray.o bin/histogram_equalization.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main
	@mkdir -p outputs
	@$^ $(PATH_IN) $(PATH_OUT)

.PHONY: all clean
