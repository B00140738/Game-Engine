CXX = g++
CXXFLAGS = -std=c++17 -Ilib/imgui -Ilib/imgui/backends
LDFLAGS = -lglfw -lGLEW -lGL

SRC = main.cpp \
      lib/imgui/imgui.cpp \
      lib/imgui/imgui_draw.cpp \
      lib/imgui/imgui_tables.cpp \
      lib/imgui/imgui_widgets.cpp \
      lib/imgui/backends/imgui_impl_glfw.cpp \
      lib/imgui/backends/imgui_impl_opengl3.cpp

OBJ = $(SRC:.cpp=.o)

main: $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o lib/imgui/*.o lib/imgui/backends/*.o main
