#! /usr/bin/env bash 

# compile 
echo "Compiling"
g++ -c -Wall -I/usr/include/eigen3 `pkg-config --cflags opencv` -I./MadgwickAHRS -I./posiiton MadgwickAHRS/MadgwickAHRS.cpp gl_cuboid.cpp main.cpp
if [ $? -ne 0 ]; then
  echo "Compile error!"
  exit
fi

# link 
echo "Linking"
g++ MadgwickAHRS.o gl_cuboid.o main.o -lm -lGL -lGLU -lglut -lpthread `pkg-config --libs opencv` -o imu_pose 
if [ $? -ne 0 ]; then
  echo "Link error!"
  exit
fi

# clean
rm *.o
echo "Build done!"
