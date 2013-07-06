
INCDIR=./ ../include
HEADERS=$(shell find $(INCDIR) -name \*.h -o -name \*.hpp)
DEMOS_SRC=$(shell find ./ -name \*.cpp)
DEMOS=$(addprefix demo-,$(subst .cpp,,$(notdir $(basename $(DEMOS_SRC)))))

CFLAGS+=-std=c++0x -g $(addprefix -I,$(INCDIR))




all: build

build: $(DEMOS)

clean: $(addprefix clean-,$(DEMOS))




demo-resample_bicubic_v4l2_to_file: resample_bicubic_v4l2_to_file.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -o $@ $< -lv4l2

demo-%: %.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -g -o $@ $<

clean-demo-%:
	rm -rf $(subst clean-,,$@)

