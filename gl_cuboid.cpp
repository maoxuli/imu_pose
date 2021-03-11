#include "gl_cuboid.h"
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <opencv2/opencv.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>

GlCuboid* GlCuboid::instance = NULL;

GlCuboid::GlCuboid(int* argc, char** argv)
{
    GlCuboid::instance = this; 
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Cuboid");
    glutReshapeFunc(GlCuboid::reshape_callback); 
    glutDisplayFunc(GlCuboid::display_callback); 
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    CameraView = CameraViews::Front; 
    CameraDistance = 50.0; 

    halfDimension[0] = 3; 
    halfDimension[1] = 2; 
    halfDimension[2] = 1; 

#if USE_TRANSFORM_MATRIX
    transform[0] = 1.0; transform[1] = 0.0;  transform[2] = 0.0; transform[3] = 0.0; 
    transform[4] = 0.0; transform[5] = 1.0;  transform[6] = 0.0; transform[7] = 0.0; 
    transform[8] = 0.0; transform[9] = 0.0;  transform[10] = 1.0; transform[11] = 0.0; 
    transform[12] = 0.0; transform[13] = 0.0;  transform[14] = 0.0; transform[15] = 1.0; 
#else 
    position[0] = 0; position[1] = 0; position[2] = 0;  
    rotation[0] = 0; rotation[1] = 0; rotation[2] = 0;  
#endif 

    imageFolder = "images"; 
    imageFiles[0] = "Right.png"; 
    imageFiles[1] = "Left.png"; 
    imageFiles[2] = "Back.png"; 
    imageFiles[3] = "Front.png"; 
    imageFiles[4] = "Top.png"; 
    imageFiles[5] = "Bottom.png";
    LoadTextureFromImage();

    // start gl thread 
    _gl_thread = std::thread(std::bind(&GlCuboid::gl_thread, this)); 
}

GlCuboid::~GlCuboid()
{
    glutLeaveMainLoop(); 
    _gl_thread.join();  
}

void GlCuboid::LoadTextureFromImage()
{
    int numOfPic = 6;
    glGenTextures(numOfPic, textures);
    for (int i = 0; i < numOfPic; i++)
    {
        cv::Mat image = cv::imread(imageFolder + "/" + imageFiles[i]); 
        cv::flip(image, image, 0); 
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, image.channels(), image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
    }
}

void GlCuboid::gl_thread() 
{
    glutMainLoop();
}

void GlCuboid::reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(10, (float)w / (float)h, 1.0, 250);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void GlCuboid::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPolygonMode(GL_FRONT, GL_FILL);

    std::unique_lock<std::mutex> lock(_gl_mutex); 

    // Set camera view and distance
    glTranslatef(0, 0, -1.0 * CameraDistance);
    switch (CameraView)
    {
        case CameraViews::Right:
            glRotatef(-90, 0, 1, 0);
            glRotatef(-90, 1, 0, 0);
            break;
        case CameraViews::Left:
            glRotatef(90, 0, 1, 0);
            glRotatef(-90, 1, 0, 0);
            break;
        case CameraViews::Back:
            glRotatef(90, 1, 0, 0);
            glRotatef(180, 0, 1, 0);
            break;
        case CameraViews::Front:
            glRotatef(-90, 1, 0, 0);
            break;
        case CameraViews::Top:
            break;
        case CameraViews::Bottom:
            glRotatef(180, 1, 0, 0);
            break;
    }

#if USE_TRANSFORM_MATRIX
    glPushMatrix();
    glMultMatrixf(transform);
#else 
    glTranslatef(position[0], position[1], position[2]); 
    glRotatef(rotation[0], 1, 0, 0);
    glRotatef(rotation[1], 0, 1, 0);
    glRotatef(rotation[2], 0, 0, 1);
#endif 

    lock.unlock(); 

    // +'ve x face
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0); glTexCoord2f(0, 0); glVertex3f(halfDimension[0], -halfDimension[1], -halfDimension[2]);
    glNormal3f(1, 0, 0); glTexCoord2f(0, 1); glVertex3f(halfDimension[0], -halfDimension[1], halfDimension[2]);
    glNormal3f(1, 0, 0); glTexCoord2f(1, 1); glVertex3f(halfDimension[0], halfDimension[1], halfDimension[2]);
    glNormal3f(1, 0, 0); glTexCoord2f(1, 0); glVertex3f(halfDimension[0], halfDimension[1], -halfDimension[2]);
    glEnd();

    // -'ve x face
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0); glTexCoord2f(1, 0); glVertex3f(-halfDimension[0], -halfDimension[1], -halfDimension[2]);
    glNormal3f(-1, 0, 0); glTexCoord2f(1, 1); glVertex3f(-halfDimension[0], -halfDimension[1], halfDimension[2]);
    glNormal3f(-1, 0, 0); glTexCoord2f(0, 1); glVertex3f(-halfDimension[0], halfDimension[1], halfDimension[2]);
    glNormal3f(-1, 0, 0); glTexCoord2f(0, 0); glVertex3f(-halfDimension[0], halfDimension[1], -halfDimension[2]);
    glEnd();

    // +'ve y face
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0); glTexCoord2f(1, 0); glVertex3f(-halfDimension[0], halfDimension[1], -halfDimension[2]);
    glNormal3f(0, 1, 0); glTexCoord2f(1, 1); glVertex3f(-halfDimension[0], halfDimension[1], halfDimension[2]);
    glNormal3f(0, 1, 0); glTexCoord2f(0, 1); glVertex3f(halfDimension[0], halfDimension[1], halfDimension[2]);
    glNormal3f(0, 1, 0); glTexCoord2f(0, 0); glVertex3f(halfDimension[0], halfDimension[1], -halfDimension[2]);
    glEnd();

    // -'ve y face
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0); glTexCoord2f(0, 0); glVertex3f(-halfDimension[0], -halfDimension[1], -halfDimension[2]);
    glNormal3f(0, -1, 0); glTexCoord2f(0, 1); glVertex3f(-halfDimension[0], -halfDimension[1], halfDimension[2]);
    glNormal3f(0, -1, 0); glTexCoord2f(1, 1); glVertex3f(halfDimension[0], -halfDimension[1], halfDimension[2]);
    glNormal3f(0, -1, 0); glTexCoord2f(1, 0); glVertex3f(halfDimension[0], -halfDimension[1], -halfDimension[2]);
    glEnd();

    // +'ve z face
    glBindTexture(GL_TEXTURE_2D, textures[4]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1); glTexCoord2f(0, 0); glVertex3f(-halfDimension[0], -halfDimension[1], halfDimension[2]);
    glNormal3f(0, 0, 1); glTexCoord2f(1, 0); glVertex3f(halfDimension[0], -halfDimension[1], halfDimension[2]);
    glNormal3f(0, 0, 1); glTexCoord2f(1, 1); glVertex3f(halfDimension[0], halfDimension[1], halfDimension[2]);
    glNormal3f(0, 0, 1); glTexCoord2f(0, 1); glVertex3f(-halfDimension[0], halfDimension[1], halfDimension[2]);
    glEnd();

    // -'ve z face
    glBindTexture(GL_TEXTURE_2D, textures[5]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1); glTexCoord2f(0, 1); glVertex3f(-halfDimension[0], -halfDimension[1], -halfDimension[2]);
    glNormal3f(0, 0, -1); glTexCoord2f(1, 1); glVertex3f(halfDimension[0], -halfDimension[1], -halfDimension[2]);
    glNormal3f(0, 0, -1); glTexCoord2f(1, 0); glVertex3f(halfDimension[0], halfDimension[1], -halfDimension[2]);
    glNormal3f(0, 0, -1); glTexCoord2f(0, 0); glVertex3f(-halfDimension[0], halfDimension[1], -halfDimension[2]);
    glEnd();

#if USE_TRANSFORM_MATRIX
    glPopMatrix();
#endif 
    glFlush();
    glutSwapBuffers();
}

void GlCuboid::update_display() 
{
    glutPostRedisplay();
}

void GlCuboid::update_position(float x, float y, float z)
{
    std::unique_lock<std::mutex> lock(_gl_mutex); 
#if USE_TRANSFORM_MATRIX
    transform[12] = x;
    transform[13] = y;
    transform[14] = z;
#else 
    position[0] = x; 
    position[1] = y; 
    position[2] = z; 
#endif 
    lock.unlock();

    update_display(); 
}

void GlCuboid::update_rotation(float qw, float qx, float qy, float qz)
{
#if USE_TRANSFORM_MATRIX
    // float sqw = qw * qw;
    // float sqx = qx * qx;
    // float sqy = qy * qy;
    // float sqz = qz * qz;
    // float invs = 1.0f / (sqx + sqy + sqz + sqw);

	// float m00 = ( sqx - sqy - sqz + sqw) * invs;
    // float m11 = (-sqx + sqy - sqz + sqw) * invs;
    // float m22 = (-sqx - sqy + sqz + sqw) * invs;
    
    // float tmp1 = qx * qy;
    // float tmp2 = qz * qw;
    // float m10 = 2.0 * (tmp1 + tmp2) * invs;
    // float m01 = 2.0 * (tmp1 - tmp2) * invs;
    
    // tmp1 = qx * qz;
    // tmp2 = qy * qw;
    // float m20 = 2.0 * (tmp1 - tmp2) * invs;
    // float m02 = 2.0 * (tmp1 + tmp2) * invs;

    // tmp1 = qy * qz;
    // tmp2 = qx * qw;
    // float m21 = 2.0 * (tmp1 + tmp2) * invs;
    // float m12 = 2.0 * (tmp1 - tmp2) * invs; 

    float xx = qx * qx;
    float xy = qx * qy;
    float xz = qx * qz;
    float xw = qx * qw;

    float yy = qy * qy;
    float yz = qy * qz;
    float yw = qy * qw;

    float zz = qz * qz;
    float zw = qz * qw;

    float m00 = 1 - 2 * ( yy + zz );
    float m01 =     2 * ( xy - zw );
    float m02 =     2 * ( xz + yw );

    float m10 =     2 * ( xy + zw );
    float m11 = 1 - 2 * ( xx + zz );
    float m12 =     2 * ( yz - xw );

    float m20 =     2 * ( xz - yw );
    float m21 =     2 * ( yz + xw );
    float m22 = 1 - 2 * ( xx + yy );

    std::unique_lock<std::mutex> lock(_gl_mutex); 
    transform[0] = m00; transform[4] = m01; transform[8] = m02;
    transform[1] = m10; transform[5] = m11; transform[9] = m12;
    transform[2] = m20; transform[6] = m21; transform[10] = m22;
    lock.unlock();
#else 
    std::unique_lock<std::mutex> lock(_gl_mutex); 

    // roll (x-axis rotation)
    float sinr_cosp = 2 * (qw * qx + qy * qz);
    float cosr_cosp = 1 - 2 * (qx * qx + qy * qy);
    rotation[0] = std::atan2(sinr_cosp, cosr_cosp) * 180 / M_PI;

    // pitch (y-axis rotation)
    float sinp = 2 * (qw * qy - qz * qx);
    if (std::abs(sinp) >= 1)
        rotation[1] = std::copysign(M_PI / 2, sinp) * 180 / M_PI; // use 90 degrees if out of range
    else
        rotation[1] = std::asin(sinp) * 180 / M_PI;

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (qw * qz + qx * qy);
    float cosy_cosp = 1 - 2 * (qy * qy + qz * qz);
    rotation[2] = std::atan2(siny_cosp, cosy_cosp) * 180 / M_PI;

    lock.unlock();
#endif 

    update_display(); 
}

void GlCuboid::update_rotation(float roll, float pitch, float yaw)
{
#if USE_TRANSFORM_MATRIX
    roll = roll * M_PI / 180; 
    pitch = pitch * M_PI / 180; 
    yaw = yaw * M_PI / 180; 

    float A = cos(yaw), B = sin(yaw); 
    float C = cos(pitch), D = sin(pitch); 
    float E = cos(roll), F = sin(roll); 
    float DE = D*E, DF = D*F; 

    float m00 = A*C;  
    float m01 = A*DF - B*E;  
    float m02 = B*F + A*DE;

    float m10 = B*C;  
    float m11 = A*E + B*DF;  
    float m12 = B*DE - A*F;

    float m20 = -D;   
    float m21 = C*F;         
    float m22 = C*E; 

    std::unique_lock<std::mutex> lock(_gl_mutex); 
    transform[0] = m00; transform[4] = m01; transform[8] = m02;
    transform[1] = m10; transform[5] = m11; transform[9] = m12;
    transform[2] = m20; transform[6] = m21; transform[10] = m22;
    lock.unlock();
#else 
    std::unique_lock<std::mutex> lock(_gl_mutex); 
    rotation[0] = roll; 
    rotation[1] = pitch; 
    rotation[2] = yaw; 
    lock.unlock();
#endif 

    update_display(); 
}
