PDKPATH=/opt/PalmPDK

EXECUTABLE=main

CXX = g++

LDFLAGS := -lSDL \
	-lpdl

CXXFLAGS := -I$(PDKPATH)/include \
	-I$(PDKPATH)/include/SDL \
	-Wno-deprecated \
	-Wno-write-strings

ifneq (,$(DEVICE))
	CXX = $(PDKPATH)/arm-gcc/bin/arm-none-linux-gnueabi-g++

	LDFLAGS := $(LDFLAGS) \
		-L$(PDKPATH)/device/lib \
		-Wl,--allow-shlib-undefined

	CXXFLAGS := $(CXXFLAGS) \
		-mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp \
		-DDEVICE
else
	LDFLAGS := $(LDFLAGS) \
		-L$(PDKPATH)/host/lib \
		-framework cocoa \
		-lSDLmain

	CXXFLAGS := $(CXXFLAGS) \
		-arch i386
endif

ifneq (,$(DEBUG))
	CXXFLAGS := $(CXXFLAGS) \
		-g \
		-DDEBUG
else
	CXXFLAGS := $(CXXFLAGS) \
		-s \
		-O3
endif


all: $(EXECUTABLE)

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

