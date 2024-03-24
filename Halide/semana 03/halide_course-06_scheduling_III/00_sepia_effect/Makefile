CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h,stmt_html
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin -I../3rdparty/hbpp/include
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`


ifndef PATH_IN
	PATH_IN=../inputs/monarch.png
endif

ifndef PATH_OUT
	PATH_OUT=outputs/halide_result.png
endif

ifndef CUSTOM_SCHEDULE
	CUSTOM_SCHEDULE=-1
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
	
bin/sepia.generator: src/generators/HalideSepia.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/sepia.o: bin/sepia.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f sepia -g sepia target=$(TARGET) custom_schedule=$(CUSTOM_SCHEDULE)

bin/main: src/main.cpp bin/sepia.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main
	@echo "Scheduler: "$(CUSTOM_SCHEDULE)
	@mkdir -p outputs
	@$^ $(PATH_IN) $(PATH_OUT)
	@echo ""

.PHONY: all clean
