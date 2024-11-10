# Makefile
CXX = g++
PKG_FLAGS = `pkg-config --cflags --libs gtk+-3.0 opencv4` -lsndfile
SRCS = main.cpp voice.cpp image.cpp utils.cpp
TARGET = noise_removal_median_filtering

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(PKG_FLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

