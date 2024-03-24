CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h,schedule
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`
AUTOSCHEDULER_PARAMS = autoscheduler=Mullapudi2016 -p $(HALIDE_ROOT)/lib/libautoschedule_mullapudi2016.so


ifndef PATH_IN
	PATH_IN=../inputs/camera.png
endif

ifndef SIGMA
	SIGMA=0.5
endif

ifndef K
	K=7
endif

ifndef ROUGHNESS
	ROUGHNESS=0.7
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
	
bin/dog.generator: src/generators/HalideDoG.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/dog.o: bin/dog.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f dog -g dog \
		target=$(TARGET) sigma=$(SIGMA) k=$(K) roughness=$(ROUGHNESS) \
		$(AUTOSCHEDULER_PARAMS)

bin/main: src/main.cpp bin/dog.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main
	@mkdir -p outputs
	@$^ $(PATH_IN) $(PATH_OUT)

.PHONY: all clean
