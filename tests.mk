
HEADERS=$(shell find include -name *.h -o -name *.hpp)
TESTS_SRC=$(shell find tests -name test_*.cpp)
TESTS=$(subst .cpp,,$(basename $(notdir $(TESTS_SRC))))


all: run

build: $(TESTS)

run: build $(addprefix run-,$(TESTS))

clean: $(addprefix clean-,$(TESTS))




test_%: tests/test_%.cpp $(HEADERS)
	$(CXX) -I . -I include -g -o $@ $<

run-test_%: test_%
	./$<

clean-test_%:
	rm -rf $(subst clean-,,$@)

