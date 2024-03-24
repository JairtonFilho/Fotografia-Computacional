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
	PATH_IN=../inputs/horiz.png
endif

ifndef PATH_OUT
	PATH_OUT=outputs/dt.png
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
	
bin/distance_transform.generator: src/generators/HalideDT.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/distance_transform.o: bin/distance_transform.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f distance_transform -g distance_transform \
		target=$(TARGET) $(AUTOSCHEDULER_PARAMS)

bin/main: src/main.cpp bin/distance_transform.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main
	@mkdir -p outputs
	@$^ $(PATH_IN) $(PATH_OUT)
	@echo ""

.PHONY: all clean
