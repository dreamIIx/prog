#.PHONY: all clean

CXX = g++
DEBUGFLAGS = -g
RELEASEFLAGS = -O3

ifeq ($(OS), Windows_NT)
	SHELL = CMD
	CFLAGS = -c -Wall -std=c++20
	LDFLAGS =
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
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
		CFLAGS = -c -Wall -std=c++20 -ftree-vectorize -mavx
		LDFLAGS = 
#			-lboost_iostreams -lboost_system -lboost_filesystem  
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
    endif
endif

$(info init..)
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

./Source/vmain1.o: ./Source/vmain1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/vmain1.d.o: ./Source/vmain1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Source/vmain2.o: ./Source/vmain2.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) -I./ $(CFLAGS) $< -o $@
	
./Source/vmain2.d.o: ./Source/vmain2.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) -I./ $(CFLAGS) $< -o $@

./Source/vmain3.o: ./Source/vmain3.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) -I./ $(CFLAGS) $< -o $@
	
./Source/vmain3.d.o: ./Source/vmain3.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) -I./ $(CFLAGS) $< -o $@

./Source/emain1.o: ./Source/excmain1.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/emain1.d.o: ./Source/excmain1.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Source/emain2.o: ./Source/excmain2.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/emain2.d.o: ./Source/excmain2.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Source/emain3.o: ./Source/excmain3.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/emain3.d.o: ./Source/excmain3.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Source/emain4.o: ./Source/excmain4.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/emain4.d.o: ./Source/excmain4.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@

./Source/emain5.o: ./Source/excmain5.cpp
	$(info processing....)
	$(CXX) $(RELEASEFLAGS) $(CFLAGS) $< -o $@
	
./Source/emain5.d.o: ./Source/excmain5.cpp
	$(info processing....)
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) $< -o $@
	
$(EXE_vmain1d): ./Source/vmain1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain1): ./Source/vmain1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain2d): ./Source/vmain2.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain2): ./Source/vmain2.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain3d): ./Source/vmain3.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_vmain3): ./Source/vmain3.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain1d): ./Source/emain1.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain1): ./Source/emain1.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain2d): ./Source/emain2.d.o 
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain2): ./Source/emain2.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain3d): ./Source/emain3.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain3): ./Source/emain3.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain4d): ./Source/emain4.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain4): ./Source/emain4.o
	$(info processing......)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EXE_emain5d): ./Source/emain5.d.o
	$(info processing......)
	$(CXX) $(LDFLAGS) -lboost_thread $^ -o $@

$(EXE_emain5): ./Source/emain5.o
	$(info processing......)
	$(CXX) $(LDFLAGS) -lboost_thread $^ -o $@

clean:
	rm ./Source/*.o
#ifeq ($(OS), Windows_NT)
#	copy .\Source\*.o .\*.o
#	del .\Source\*.o
#else
#    UNAME_S := $(shell uname -s)
#    ifeq ($(UNAME_S), Linux)
#		rm *.o
#    endif
#endif