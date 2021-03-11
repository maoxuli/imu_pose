#ifndef CUBOID_H__
#define CUBOID_H__

#include <vector>
#include <string> 
#include <thread> 
#include <mutex> 
#include <cassert>

#define USE_TRANSFORM_MATRIX 1

class GlCuboid 
{
public: 
    enum CameraViews {Right, Left, Back, Front, Top, Bottom};

    GlCuboid(int* argc, char** argv); 
    ~GlCuboid(); 

    void set_view(CameraViews view) { CameraView = view; }; 
    void set_distance(float distance) { CameraDistance = distance; }; 

    void set_dimension(float x, float y, float z)
    {
        halfDimension[0] = x/2; 
        halfDimension[1] = y/2;
        halfDimension[2] = z/2; 
    }

    void update_position(float x, float y, float z);
    void update_rotation(float qw, float qx, float qy, float qz);
    void update_rotation(float roll, float pitch, float yaw); 

    void update_display(); 

private: 
    // parameters for view 
    CameraViews CameraView; 
    float CameraDistance;

    // parameters for cuboid 
    float halfDimension[3];

#if USE_TRANSFORM_MATRIX
    float transform[16];
#else 
    float position[3]; 
    float rotation[3]; 
#endif

    // surface textures  
    uint textures[6];
    std::string imageFolder; 
    std::string imageFiles[6];
    void LoadTextureFromImage();

    // thread for gl 
    void gl_thread(); 
    std::thread _gl_thread; 
    std::mutex _gl_mutex; 

    void display(); 
    void reshape(int w, int h);

    static GlCuboid* instance;
    static void display_callback() 
    {
        assert(instance);
        instance->display(); 
    }

    static void reshape_callback(int w, int h)
    {
        assert(instance); 
        instance->reshape(w, h); 
    }
}; 

#endif 
