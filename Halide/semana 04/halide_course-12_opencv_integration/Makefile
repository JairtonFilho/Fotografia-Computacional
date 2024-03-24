CXX = g++
CXX_FLAGS = -std=c++20 -O3

GENERATOR_DEPS = $(HALIDE_ROOT)/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h,stmt
INCLUDES = -I$(HALIDE_ROOT)/include -I$(HALIDE_ROOT)/tools -Iinclude -Ibin
LIBS = -L$(HALIDE_ROOT)/lib 
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo `pkg-config --libs --cflags opencv4`
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`


TARGET=host
ifdef PROFILE
	ifeq ($(PROFILE), true)
		TARGET+=-profile
	endif
endif


ifndef AUTO_SCHEDULE
	AUTO_SCHEDULE=true
endif
ifeq ($(AUTO_SCHEDULE), true)
	ifndef AUTO_SCHEDULER
		AUTO_SCHEDULER=Mullapudi2016
		AUTO_SCHEDULER_PATH=$(HALIDE_ROOT)/lib/libautoschedule_mullapudi2016.so
	endif
	AUTO_SCHEDULER_PARS=autoscheduler=$(AUTO_SCHEDULER) -p $(AUTO_SCHEDULER_PATH)
	ifdef MACHINE_PARAMS
		AUTO_SCHEDULER_PARS+=machine_params=$(MACHINE_PARAMS)
	endif
endif

all: run_cv2_integration

clean:
	@rm -rf bin *.png


bin/gradient.generator: src/registration/HalideGradient.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/gradient.o: bin/gradient.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f gradient -g gradient \
		target=$(TARGET) $(AUTOSCHEDULER_PARAMS)

bin/main: src/main.cpp bin/gradient.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

run_cv2_integration: bin/main
	@$^
	@echo "Success!"

.PHONY: all clean run_cv2_integration
.PRECIOUS: bin/%
