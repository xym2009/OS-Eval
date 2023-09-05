CXX=g++

INCLUDES = -I/home/libs/gdal/include  -I/home/libs/opencv460/include/opencv4 

OPENCV_DIR = -L/home/libs/opencv460/lib

OPENCV_LIBS= $(OPENCV_DIR) -lopencv_world

GDAL_DIR = -L/home/libs/gdal/lib

GDAL_LIBS= $(GDAL_DIR) -lgdal

LDFLAGS=-shared -fPIC -fopenmp 

CXXFLAGS=$(LDFLAGS) -c -O2 $(INCLUDES)

LDLIB=-L ./lib $(OPENCV_LIBS) $(GDAL_LIBS)

ALLOBJS=`find -name *.o|xargs`

default: clean OSEval

OSEval:obj/OS.o obj/matchfunc.o obj/Log.o
	$(CXX) -o $@  $^ $(LDLIB) $(CXXFLAGSHEAD) -fopenmp -Wl,-rpath=/home/libs/opencv460/lib -Wl,-rpath=/home/libs/gdal/lib

obj/matchfunc.o: src/matchFunc.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<
	
obj/OS.o: src/Main.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<
	
obj/Log.o: src/Logger.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<
	
clean:
	rm -f obj/*.o
	rm -f OSEval
