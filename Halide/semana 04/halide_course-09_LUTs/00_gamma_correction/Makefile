CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h,stmt_html
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin -I../3rdparty/hbpp/include
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`


ifndef PATH_IN
	PATH_IN=../inputs/camera.png
endif

ifndef GAMMA
	GAMMA=1
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
	
bin/gamma_correction.generator: src/generators/HalideGammaCorrection.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/gamma_correction.o: bin/gamma_correction.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f gamma_correction -g gamma_correction target=$(TARGET) gamma=$(GAMMA)

bin/gamma_correction_lut.o: bin/gamma_correction.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f gamma_correction_lut -g gamma_correction target=$(TARGET) gamma=$(GAMMA) mode=lut

bin/main: src/main.cpp bin/gamma_correction.o bin/gamma_correction_lut.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main
	@mkdir -p outputs
	@$^ $(PATH_IN) $(PATH_OUT)

.PHONY: all clean
