CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h,stmt_html,assembly,schedule
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin -I../3rdparty/hbpp/include
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`


ifndef PATH_IN
	PATH_IN=../inputs/monarch.png
endif

ifndef THRESH
	THRESH=50
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
	
bin/gradient.generator: src/generators/HalideGradient.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/gradient.o: bin/gradient.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f gradient -g gradient target=$(TARGET) custom_schedule=$(CUSTOM_SCHEDULE)

bin/main: src/main.cpp bin/gradient.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

outputs: bin/main
	@echo "Scheduler: "$(CUSTOM_SCHEDULE)
	@mkdir -p outputs
	@$^ $(PATH_IN) $(THRESH) $(PATH_OUT)
	@echo ""

# for i in $(seq 0 12); do make -B CUSTOM_SCHEDULE=$i; done

.PHONY: all clean
