CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`


ifndef PATH_IN
	PATH_IN=../inputs/monarch.png
endif

ifndef PATH_OUT_UPDATE
	PATH_OUT_UPDATE=outputs/result_update.png
endif

ifndef PATH_OUT_SELECT
	PATH_OUT_SELECT=outputs/result_select.png
endif

ifndef PATH_OUT_MUX
	PATH_OUT_MUX=outputs/result_mux.png
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
	
bin/rgb2hsl.generator: src/generators/HalideRGB2HSL.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/rgb2hsl_update.o: bin/rgb2hsl.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f rgb2hsl_update -g rgb2hsl target=$(TARGET) merge_mode=update

bin/rgb2hsl_select.o: bin/rgb2hsl.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f rgb2hsl_select -g rgb2hsl target=$(TARGET) merge_mode=select

bin/rgb2hsl_mux.o: bin/rgb2hsl.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f rgb2hsl_mux -g rgb2hsl target=$(TARGET) merge_mode=mux

bin/main: src/main.cpp bin/rgb2hsl_update.o bin/rgb2hsl_select.o bin/rgb2hsl_mux.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main 
	@mkdir -p outputs
	@$^ $(PATH_IN) $(PATH_OUT_UPDATE) $(PATH_OUT_SELECT) $(PATH_OUT_MUX)

.PHONY: all clean
