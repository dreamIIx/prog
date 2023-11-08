CXX = g++
DEBUGFLAGS = -g
RELEASEFLAGS = -O3

ifeq ($(OS), Windows_NT)
	SHELL = CMD
	CFLAGS = -c -Wall -std=c++20
	LDFLAGS =
	EXE_main4d = ./Debug/main4.exe
	EXE_main4 = ./Release/main4.exe
	EXE_vmain1d = ./Debug/vmain1.exe
	EXE_vmain1 = ./Release/vmain1.exe
	EXE_vmain2d = ./Debug/vmain2.exe
	EXE_vmain2 = ./Release/vmain2.exe
	EXE_vmain3d = ./Debug/vmain3.exe
	EXE_vmain3 = ./Release/vmain3.exe
	EXE_emain1d = ./Debug/emain1.exe
	EXE_emain1 = ./Release/emain1.exe
	EXE_emain2d = ./Debug/emain2.exe
	EXE_emain2 = ./Release/emain2.exe
	EXE_emain3d = ./Debug/emain3.exe
	EXE_emain3 = ./Release/emain3.exe
	EXE_emain4d = ./Debug/emain4.exe
	EXE_emain4 = ./Release/emain4.exe
	EXE_emain5d = ./Debug/emain5.exe
	EXE_emain5 = ./Release/emain5.exe
	EXE_lcmaind = ./Debug/lcmain.exe
	EXE_lcmain = ./Release/lcmain.exe
	EXE_graph1d = ./Debug/graph1.exe
	EXE_graph1 = ./Release/graph1.exe
	EXE_dztpr1d = ./Debug/dztpr1.exe
	EXE_dztpr1 = ./Release/dztpr1.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
		CFLAGS = -c -Wall -std=c++2b -ftree-vectorize -mavx
		LDFLAGS = 
#			-lboost_iostreams -lboost_system -lboost_filesystem
		EXE_main4d = ./Debug/main4
		EXE_main4 = ./Release/main4
		EXE_vmain1d = ./Debug/vmain1
		EXE_vmain1 = ./Release/vmain1
		EXE_vmain2d = ./Debug/vmain2
		EXE_vmain2 = ./Release/vmain2
		EXE_vmain3d = ./Debug/vmain3
		EXE_vmain3 = ./Release/vmain3
		EXE_emain1d = ./Debug/emain1
		EXE_emain1 = ./Release/emain1
		EXE_emain2d = ./Debug/emain2
		EXE_emain2 = ./Release/emain2
		EXE_emain3d = ./Debug/emain3
		EXE_emain3 = ./Release/emain3
		EXE_emain4d = ./Debug/emain4
		EXE_emain4 = ./Release/emain4
		EXE_emain5d = ./Debug/emain5
		EXE_emain5 = ./Release/emain5
		EXE_lcmaind = ./Debug/lcmain
		EXE_lcmain = ./Release/lcmain
		EXE_graph1d = ./Debug/graph1
		EXE_graph1 = ./Release/graph1
		EXE_dztpr1d = ./Debug/dztpr1
		EXE_dztpr1 = ./Release/dztpr1
    endif
endif

$(info init..)
.PHONY: main4d main4 vmain1d vmain1 vmain2d vmain2 vmain3d vmain3 emain1d emain1		\
	emain2d emain2 emain3d emain3 emain4d emain4 emain5d emain5 lcmaind					\
	lcmain graph1d graph1 dztpr1d dztpr1
main4d: ./Source/main4.cpp $(EXE_main4d)
main4: ./Source/main4.cpp $(EXE_main4)
vmain1d: ./Source/vmain1.cpp $(EXE_vmain1d)
vmain1: ./Source/vmain1.cpp $(EXE_vmain1)
vmain2d: ./Source/vmain2.cpp $(EXE_vmain2d)
vmain2: ./Source/vmain2.cpp $(EXE_vmain2)
vmain3d: ./Source/vmain3.cpp $(EXE_vmain3d)
vmain3: ./Source/vmain3.cpp $(EXE_vmain3)
emain1d: ./Source/excmain1.cpp $(EXE_emain1d)
emain1: ./Source/excmain1.cpp $(EXE_emain1)
emain2d: ./Source/excmain2.cpp $(EXE_emain2d)
emain2: ./Source/excmain2.cpp $(EXE_emain2)
emain3d: ./Source/excmain3.cpp $(EXE_emain3d)
emain3: ./Source/excmain3.cpp $(EXE_emain3)
emain4d: ./Source/excmain4.cpp $(EXE_emain4d)
emain4: ./Source/excmain4.cpp $(EXE_emain4)
emain5d: ./Source/excmain5.cpp $(EXE_emain5d)
emain5: ./Source/excmain5.cpp $(EXE_emain5)
lcmaind: ./Source/lc_main.cpp $(EXE_lcmaind)
lcmain: ./Source/lc_main.cpp $(EXE_lcmain)
graph1d: ./Source/graph1.cpp $(EXE_graph1d)
graph1: ./Source/graph1.cpp $(EXE_graph1)
dztpr1d: ./Source/dztpr1.cpp $(EXE_dztpr1d)
dztpr1: ./Source/dztpr1.cpp $(EXE_dztpr1)

./Build/main4.o: ./Source/main4.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/main4.d.o: ./Source/main4.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/vmain1.o: ./Source/vmain1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/vmain1.d.o: ./Source/vmain1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/vmain2.o: ./Source/vmain2.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) -I./ $(CFLAGS) $< -o $@

./Build/vmain2.d.o: ./Source/vmain2.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) -I./ $(CFLAGS) $< -o $@

./Build/vmain3.o: ./Source/vmain3.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) -I./ $(CFLAGS) $< -o $@

./Build/vmain3.d.o: ./Source/vmain3.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) -I./ $(CFLAGS) $< -o $@

./Build/emain1.o: ./Source/excmain1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/emain1.d.o: ./Source/excmain1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/emain2.o: ./Source/excmain2.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/emain2.d.o: ./Source/excmain2.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/emain3.o: ./Source/excmain3.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/emain3.d.o: ./Source/excmain3.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/emain4.o: ./Source/excmain4.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/emain4.d.o: ./Source/excmain4.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/emain5.o: ./Source/excmain5.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/emain5.d.o: ./Source/excmain5.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/lc_main.o: ./Source/lc_main.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/lc_main.d.o: ./Source/lc_main.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/graph1.o: ./Source/graph1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/graph1.d.o: ./Source/graph1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Build/dztpr1.o: ./Source/dztpr1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@

./Build/dztpr1.d.o: ./Source/dztpr1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

$(EXE_main4d): ./Build/main4.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_main4): ./Build/main4.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain1d): ./Build/vmain1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain1): ./Build/vmain1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain2d): ./Build/vmain2.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain2): ./Build/vmain2.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain3d): ./Build/vmain3.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain3): ./Build/vmain3.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain1d): ./Build/emain1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain1): ./Build/emain1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain2d): ./Build/emain2.d.o 
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain2): ./Build/emain2.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain3d): ./Build/emain3.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain3): ./Build/emain3.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain4d): ./Build/emain4.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain4): ./Build/emain4.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain5d): ./Build/emain5.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) -lboost_thread $^ -o $@

$(EXE_emain5): ./Build/emain5.o
	$(info processing......)
	$(CXX) $(LDFLAGS) -lboost_thread $^ -o $@

$(EXE_lcmaind): ./Build/lc_main.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_lcmain): ./Build/lc_main.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_graph1d): ./Build/graph1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_graph1): ./Build/graph1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_dztpr1d): ./Build/dztpr1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_dztpr1): ./Build/dztpr1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

.PHONY: clean clean_with_bin
clean:
	rm -f ./Source/*.o
	rm -f ./Build/*.o
clean_with_bin:
	rm -f ./Source/*.o
	rm -f ./Build/*.o
	rm -f ./Debug/*
	rm -f ./Release/*
#ifeq ($(OS), Windows_NT)
#	copy .\Source\*.o .\*.o
#	del .\Source\*.o
#else
#    UNAME_S := $(shell uname -s)
#    ifeq ($(UNAME_S), Linux)
#		rm *.o
#    endif
#endif