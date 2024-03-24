CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h,stmt_html,schedule
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin -I../3rdparty/hbpp/include
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`
AUTOSCHEDULER_PARAMS = autoscheduler=Mullapudi2016 -p $(HALIDE_ROOT)/lib/libautoschedule_mullapudi2016.so


ifndef PATH_IN
	PATH_IN=../inputs/monarch.png
endif

ifndef THRESH
	THRESH=50
endif

ifndef PATH_OUT
	PATH_OUT=outputs/magnitude.png
endif

ifndef PATH_OUT_ORI
	PATH_OUT_ORI=outputs/orientation.png
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
	
bin/gradient.generator: src/generators/HalideGradient.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/gradient.o: bin/gradient.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f gradient -g gradient \
		target=$(TARGET) $(AUTOSCHEDULER_PARAMS) output_orientation=true

bin/main: src/main.cpp bin/gradient.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main
	@mkdir -p outputs
	@$^ $(PATH_IN) $(THRESH) $(PATH_OUT) $(PATH_OUT_ORI)
	@echo ""

.PHONY: all clean
