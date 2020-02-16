CFLAGS = -std=c++11 -lXi -lGLEW -lGLU -lm -lGL -lm -lpthread -ldl -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXxf86vm -lXext -lXcursor -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp -ljpeg
Sphere = BaseApp.cpp SphericalApp.cpp SphereMain.cpp
Flat = BaseApp.cpp FlatApp.cpp FlatMain.cpp

hw3_flat:
	g++ $(Flat) -o hw3_flat $(CFLAGS)

hw3_sphere:
	g++ $(Sphere) -o hw3_sphere $(CFLAGS)