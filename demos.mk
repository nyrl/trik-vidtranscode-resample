
HEADERS=$(shell find include -name *.h -o -name *.hpp)
DEMOS_SRC=$(shell find demos -name demo_*.cpp)
DEMOS=$(subst .cpp,,$(basename $(notdir $(DEMOS_SRC))))
DEMOS_HEADERS=$(shell find demos -name *.h -o -name *.hpp)

LIBV4L2?=
LIBV4L2_INCDIR?=$(LIBV4L2)/include
LIBV4L2_LIBDIR?=$(LIBV4L2)/libv4l2/.libs/


all: build

build: $(DEMOS)

clean: $(addprefix clean-,$(DEMOS))




demo_resample_bicubic_v4l2_to_file: demos/demo_resample_bicubic_v4l2_to_file.cpp $(HEADERS) $(DEMOS_HEADERS)
	$(CXX) -I . -I include -I demos -I $(LIBV4L2_INCDIR) -L $(LIBV4L2_LIBDIR) -std=c++0x -g -o $@ $< -lv4l2

demo_%: demos/demo_%.cpp $(HEADERS) $(DEMOS_HEADERS)
	$(CXX) -I . -I include -g -o $@ $<

clean-demo_%:
	rm -rf $(subst clean-,,$@)

