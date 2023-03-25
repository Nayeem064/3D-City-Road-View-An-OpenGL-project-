#include<GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <bits/stdc++.h>
using namespace std;

GLfloat width = 900;
GLfloat height = 700;

GLfloat floor_width = 40;
GLfloat floor_height = 50;

float rotation = 0;

GLfloat leftLightHight = 135;
GLfloat rightLightHight = 40;
GLfloat spotLightHight = 10;

bool light1_on = true;
bool light2_on = true;
bool leftLight_on = true;
bool rightLight_on = true;
bool spotLight_on = true;
bool amb_on = true;
bool dif_on = true;
bool spe_on = true;
float spt_cutoff = 30;

bool redSignal = false;
bool tramSignal=false;
GLfloat carStep = 0.25;
GLfloat car1Initial = 0;
GLfloat car2Initial = 0;
GLfloat car3Initial = 0;
GLfloat busInitial = 4;

GLfloat bus2Initial = 4;

GLfloat bogi1Initial = 0;
GLfloat bogiStep = 0.1;

GLfloat busStep = 0.25;

bool planeMovement = true;
GLfloat planePosition = 55;
GLfloat planeStepSize = 0.5;

bool cloudMovement = true;
GLfloat cloudPosition = 100;
GLfloat cloudStepSize = 0.12;

GLfloat eyeX = 0;
GLfloat eyeY = 5;
GLfloat eyeZ = -30;

GLfloat lookX = 0;
GLfloat lookY = 5;
GLfloat lookZ = 30;

vector<int> v;
unsigned int ID;


//GLfloat renr=0.0, reng=0.0, renb=0.0;
GLfloat renr=0.88, reng=0.96, renb=1.0;
//.88, .96 1.0
GLfloat ux = 0, uy = 1, uz = 0;
GLfloat dx, dy, dz, dxyz;
//GLfloat look_x = 0.0, look_y = 6.0, look_z = 0.0,  eye_x = 0.0, eye_z = 10.0, eye_y = 6.0;
double roll_value = 0.2, pi = acos(-1), cs_angle=cos(pi/180), sn_angle=sin(pi/180);

static void res(int width, int height)
{
    glViewport(0, 0, width, height);
}

static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte c_ind[6][4] =
{
    {3,1,5,7},  //front
    {6,4,0,2},  //back
    {2,3,7,6},  //top
    {1,0,4,5},  //bottom
    {7,5,4,6},  //right
    {2,0,1,3}   //left
};

static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

void cube(float R=0.5, float G=0.5, float B=0.5, float val=1)
{
    //GLfloat m_no[] = {0, 0, 0, 1.0};
    GLfloat m_amb[] = {R,G,B,1};
    GLfloat m_diff[] = {R,G,B,1};
    GLfloat m_spec[] = {1,1,1,1};
    GLfloat m_sh[] = {30};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);
        glTexCoord2f(0,val);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(0,0);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(val,0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(val,val);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);
    }
    glEnd();
}


/// Texture
class BmpLoader
{
public:
    unsigned char* textureData;
    int iWidth, iHeight;

    BmpLoader(const char*);
    ~BmpLoader();

private:
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
};

BmpLoader::BmpLoader(const char* filename)
{
    FILE *file=0;
    file=fopen(filename, "rb");
    if(!file)
        cout<<"File not found"<<endl;
    fread(&bfh, sizeof(BITMAPFILEHEADER),1,file);
    if(bfh.bfType != 0x4D42)
        cout<<"Not a valid bitmap"<<endl;
    fread(&bih, sizeof(BITMAPINFOHEADER),1,file);
    if(bih.biSizeImage==0)
        bih.biSizeImage=bih.biHeight*bih.biWidth*3;
    textureData = new unsigned char[bih.biSizeImage];
    fseek(file, bfh.bfOffBits, SEEK_SET);
    fread(textureData, 1, bih.biSizeImage, file);
    unsigned char temp;
    for(int i=0; i<bih.biSizeImage; i+=3)
    {
        temp = textureData[i];
        textureData[i] = textureData[i+2];
        textureData[i+2] = temp;
    }

    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);
}

BmpLoader::~BmpLoader()
{
    delete [] textureData;
}

void LoadTexture(const char*filename)
{
    /*glBindTexture(GL_TEXTURE_2D, TextureColorbufferName);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
 glBindTexture(GL_TEXTURE_2D, 0);*/



    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}
void texture_image()
{

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\z_dir.bmp"); //0

    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\x_dir.bmp"); //1
    v.push_back(ID);


    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\crossing.bmp"); //2
    v.push_back(ID);


    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\left_building.bmp"); //3
    v.push_back(ID);


    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\building_roof.bmp"); //4
    v.push_back(ID);


    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\grass.bmp"); //5
    v.push_back(ID);


    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\building_2.bmp"); //6
    v.push_back(ID);


    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\img5.bmp"); //7
    v.push_back(ID);


    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\img11.bmp");//8
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\car10.bmp");//9
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\ground4.bmp");//10
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\tram_line2.bmp");//11
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\building_3.bmp");//12
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\building3_1_side.bmp");//13
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\train.bmp");//14
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\train_top.bmp");//15
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\train_front.bmp");//16
    v.push_back(ID);


    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\bus_side.bmp");//17
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\bus_side2.bmp");//18
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\bus_back.bmp");//19
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\bus_front.bmp");//20
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\bus_top.bmp");//21
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\img5_2.bmp");//22
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\traffic2.bmp");//23
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\traffic3.bmp");//24
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\green.bmp");//25
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\red2.bmp");//26
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\light_red.bmp");//27
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\light_green.bmp");//28
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\yellow.bmp");//29
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\traffic.bmp");//30
    v.push_back(ID);
    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\bus5_side.bmp");//31
    v.push_back(ID);

    LoadTexture("G:\\4_2\\CSE_4208\\lab 1\\project_half\\sun.bmp");//32
    v.push_back(ID);
}

/// Lighting
void leftLight()
{
    //light
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.6, 0.6, 0.6, 1.0};
    GLfloat l_dif[] = {1,1,1,1};
    GLfloat l_spec[] = {1,1,1,1};
    GLfloat l_pos[] = {74, leftLightHight, 38.0};

    glEnable(GL_LIGHT0);

    if(leftLight_on)
    {
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_no);
    }

    /*if(amb_on && leftLight_on) glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT0, GL_AMBIENT, l_no);

    if(dif_on && leftLight_on) glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT0, GL_DIFFUSE, l_no);

    if(spe_on && leftLight_on) glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT0, GL_SPECULAR, l_no);*/

    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
}

void leftLightIndicator()
{
    glPushMatrix();
        glTranslatef(74, leftLightHight+1, 38.0);
        glScalef(.1, .1, .1);
        //glTranslatef(0, 0, 40.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void rightLight()
{
    //light
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat l_dif[] = {.8,.8,.8,1};
    GLfloat l_spec[] = {1,1,1,1};
    GLfloat l_pos[] = {-40.5, rightLightHight, -40.5};

    glEnable(GL_LIGHT1);

    if(rightLight_on)
    {
        glLightfv(GL_LIGHT1, GL_AMBIENT, l_amb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT1, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT1, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT1, GL_SPECULAR, l_no);
    }

    /*if(amb_on && rightLight_on) glLightfv(GL_LIGHT1, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT1, GL_AMBIENT, l_no);

    if(dif_on && rightLight_on) glLightfv(GL_LIGHT1, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT1, GL_DIFFUSE, l_no);

    if(spe_on && rightLight_on) glLightfv(GL_LIGHT1, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT1, GL_SPECULAR, l_no);
        */

    glLightfv(GL_LIGHT1, GL_POSITION, l_pos);
}

void rightLightIndicator()
{
    glPushMatrix();
        glTranslatef(-10, rightLightHight+1, 0);
        glScalef(1, 1, 1);
        glTranslatef(-30.5, 0, -40.5);
        cube(1.000, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void spotLight()
{
    //light
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat l_dif[] = {1,0,0,1};
    GLfloat l_spec[] = {1,0,0,1};
    GLfloat l_pos[] = {0, spotLightHight, -60.5};

    glEnable(GL_LIGHT2);

    if(spotLight_on)
    {
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_amb);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_no);
    }

    /*if(amb_on && spotLight_on) glLightfv(GL_LIGHT2, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT2, GL_AMBIENT, l_no);

    if(dif_on && spotLight_on) glLightfv(GL_LIGHT2, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT2, GL_DIFFUSE, l_no);

    if(spe_on && spotLight_on) glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT2, GL_SPECULAR, l_no);*/

    glLightfv(GL_LIGHT2, GL_POSITION, l_pos);

    GLfloat l_spt[] = {0, -1, 0, 1};
    GLfloat spt_ct[] = {spt_cutoff};
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l_spt);
    glLightfv(GL_LIGHT2, GL_SPOT_CUTOFF, spt_ct);
}

void spotLightIndicator()
{
    glPushMatrix();
        glTranslatef(0, spotLightHight+1, 0);
        glScalef(1, 1, 1);
        glTranslatef(0.0, 0, -60.5);
        cube(1.000, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void eyeIndicator()
{
    glPushMatrix();
        glTranslatef(eyeX, eyeY, eyeZ);
        glScalef(1, 1, 1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 0, 0, 1.0);
    glPopMatrix();
}

void lookAtPointIndicator()
{
    glPushMatrix();
        glTranslatef(lookX, lookY, lookZ);
        glScalef(1, 1, 1);
        glTranslatef(-0.5, 0, -0.5);
        cube(0, 1.0, 0, 1.0);
    glPopMatrix();
}

void sun()
{
    GLfloat m_amb[] = {1,1,1,1};
    GLfloat m_diff[] = {1,1,1,1};
    GLfloat m_spec[] = {1,1,1,1};
    GLfloat m_sh[] = {50};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[32]);
    glPushMatrix();
        glTranslatef(20, 80, 40);
        glutSolidSphere(2 ,40, 40);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}


void cloud()
{
    GLfloat m_amb[] = {1,1,1,1};
    GLfloat m_diff[] = {1,1,1,1};
    GLfloat m_spec[] = {1,1,1,1};
    GLfloat m_sh[] = {30};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glPushMatrix();
        glTranslated(-18.0, 40, 47);
        glutSolidSphere(3, 30, 30);
    glPopMatrix();
    glPushMatrix();
        glTranslated(-18.0, 40, 50);
        glutSolidSphere(2, 30, 34);
    glPopMatrix();
    glPushMatrix();
        glTranslated(-21.0, 40, 48);
        glutSolidSphere(2, 30, 34);
    glPopMatrix();
    glPushMatrix();
        glTranslated(-15.0, 42, 50);
        glutSolidSphere(2.5, 30, 30);
    glPopMatrix();
}

void clouds()
{
    glPushMatrix();
        if(cloudMovement)   glTranslated(cloudPosition -= cloudStepSize, 30, 0);
        else    glTranslated(cloudPosition, 30, 0);

        if(cloudPosition <-10) cloudPosition = 350;

        glScalef(10, 6, 1);
        cloud();
    glPopMatrix();
}

void ground()
{
    /// left Front floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
        glPushMatrix();
            glTranslatef(10, 0, -12.5);
            glScalef(20.0, 0.1, 25.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// left Back floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
        glPushMatrix();
            glTranslatef(10, 0, 12.5);
            glScalef(20.0, 0.1, 25.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// right Front floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
        glPushMatrix();
            glTranslatef(-10, 0, -12.5);
            glScalef(20.0, 0.1, 25.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// right Back floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
        glPushMatrix();
            glTranslatef(-10, 0, 12.5);
            glScalef(20.0, 0.1, 25.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}


void ground2()
{
    /// left Front floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[10]);
        glPushMatrix();
            glTranslatef(7, 0, -12.5);
            glScalef(12.0, 0.1, 10.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// left Back floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[10]);
        glPushMatrix();
            glTranslatef(7, 0, 12.5);
            glScalef(12.0, 0.1, 10.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// right Front floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[10]);
        glPushMatrix();
            glTranslatef(-10, 0, -12.5);
            glScalef(10.0, 0.1, 10.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// right Back floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[10]);
        glPushMatrix();
            glTranslatef(-10, 0, 12.5);
            glScalef(10.0, 0.1, 10.0);
            glTranslatef(-0.5, -0.95, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void road()
{
    /// road in z-direction
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[0]);
        glPushMatrix();
            glScalef(15, 1, floor_height);
            glTranslatef(-0.5, -0.9, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// road in x-direction
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[1]);
        glPushMatrix();
            glScalef(floor_width*2.5, 1,11);
            glTranslatef(-0.35, -0.9, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// tow roads intersection
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[2]);
        glPushMatrix();
            glScalef(15, 1, 15);
            glTranslatef(-0.5, -0.8, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}


void tram_road()
{
    /// road in z-direction
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[11]);
        glPushMatrix();
            glScalef(8, 1, 1.9*floor_height);
            glTranslatef(.1, -0.9, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// road in x-direction
    /*glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[11]);
        glPushMatrix();
            glScalef(floor_width, 1,11);
            glTranslatef(-0.5, -0.9, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// tow roads intersection
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[11]);
        glPushMatrix();
            glScalef(15, 1, 15);
            glTranslatef(-0.5, -0.8, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D); */
}


/// Building & Clock
void building()
{
    /// leftFront FrontXCube
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[6]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 20, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[3]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 20, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[3]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 20, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[3]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 20, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[4]);
        glPushMatrix();
            glTranslatef(17.5, 20, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}


void building2()
{
    /// leftFront FrontXCube
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[7]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 16, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[7]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 16, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[7]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 16, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[7]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 16, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[4]);
        glPushMatrix();
            glTranslatef(17.5, 16, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}


void building3()
{
    /// leftFront FrontXCube
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[8]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 20, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[8]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 20, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[8]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 20, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[8]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 20, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[4]);
        glPushMatrix();
            glTranslatef(17.5, 20, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}

void building4()
{
    /// leftFront FrontXCube
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[12]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[12]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[13]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[13]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[4]);
        glPushMatrix();
            glTranslatef(17.5, 40, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}



void building5()
{
    /// leftFront FrontXCube
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[12]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[12]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[13]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[13]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[4]);
        glPushMatrix();
            glTranslatef(17.5, 40, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}



void buildings()
{
    glPushMatrix();
        building2();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 0, 34);
        building2();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 0, -35);
        glRotated(-180, 0, 1, 0);
        building3();
    glPopMatrix();
    glPushMatrix();
        glRotated(-180, 0, 1, 0);
        building3();
    glPopMatrix();
}

///second view
void buildings_set2()
{
    glPushMatrix();
    glTranslatef(-15,0,-10);
    glScalef(2,1,1);
        building4();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-15, 0, 24);
        glScalef(2,1,1);
        building5();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-14, 0, 62);
        glScalef(2,1,1);
        building5();
    glPopMatrix();



    /*glPushMatrix();
        glTranslatef(0, 0, -35);
        glRotated(-180, 0, 1, 0);
        building3();
    glPopMatrix();
    glPushMatrix();
        glRotated(-180, 0, 1, 0);
        //glTranslatef(0,0 ,34);
        building3();
    glPopMatrix();


    glPushMatrix();
        building2();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 0, 34);
        building2();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 0, -35);
        glRotated(-180, 0, 1, 0);
        building3();
    glPopMatrix();
    glPushMatrix();
        glRotated(-180, 0, 1, 0);
        building3();
    glPopMatrix();*/
}

/// Traffic Lights
void lamPost()
{
    GLfloat m_amb[] = {0.275, 0.510, 0.706, 1.0};
    GLfloat m_diff[] = {0.275, 0.510, 0.706, 1.0};
    GLfloat m_spec[] = {0.275, 0.510, 0.706, 1.0};
    GLfloat m_sh[] = {30};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    /// lamp post

    glPushMatrix();
    glScalef(1,0.55,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[30]);
    glPushMatrix();
        glTranslated(9, 0, -22);
        glRotated(90, -1, 0, 0);
        glBegin(GL_POLYGON);
            GLUquadricObj *obj = gluNewQuadric();
            gluCylinder(obj, 0.3, 0.3, 15.0, 300.0, 300.0);
        glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// lamp holder
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[30]);
    glPushMatrix();
        glTranslatef(8.2, 15.0, -22);
        glScalef(2.0, 0.2, 0.5);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.275, 0.510, 0.706, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// lamp
    glPushMatrix();
    //spotLight();
        glTranslatef(7.5, 14.0, -22);
        glScalef(0.2, 1.0, 0.2);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();

    glPopMatrix();
}

void lamPosts()
{
    glPushMatrix();
    spotLight();
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, 10);
        lamPost();
        glTranslated(0, 9, 0);
        spotLight();
        //glScalef(10,10,1);
        //cube(1,1,1,1);
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, 34);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, 44);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, -34);
        glRotated(-180, 0, 1, 0);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, -44);
        glRotated(-180, 0, 1, 0);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, -10);
        glRotated(-180, 0, 1, 0);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glRotated(-180, 0, 1, 0);

        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
}

void lamPosts_2()
{
    glPushMatrix();
        glTranslated(-6, 0, 0);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(-6, 0, 10);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(-6, 0, 34);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(-6, 0, 44);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, -34);
        glRotated(-180, 0, 1, 0);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, -44);
        glRotated(-180, 0, 1, 0);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, 0, -10);
        glRotated(-180, 0, 1, 0);
        lamPost();
        //glTranslated(0, 30, 0);
        //spotLight();
    glPopMatrix();
    glPushMatrix();
        glRotated(-180, 0, 1, 0);
        lamPost();
    glPopMatrix();
}



/// Traffic System
void trafficSystem()
{
    /// traffic light base

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[22]);
        glPushMatrix();
        glScalef(1.5, 1.5, 1.5);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.941, 0.502, 0.502, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);




    /// traffic light root

    GLfloat m_amb[] = {0.941, 0.502, 0.502, 1.0};
    GLfloat m_diff[] = {0.941, 0.502, 0.502, 1.0};
    GLfloat m_spec[] = {0.941, 0.502, 0.502, 1.0};
    GLfloat m_sh[] = {30};
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[23]);
    glPushMatrix();
        glRotated(90, -1, 0, 0);
        glBegin(GL_POLYGON);
            GLUquadricObj *clock_obj = gluNewQuadric();
            gluCylinder(clock_obj, 0.5, 0.5, 25.0, 300.0, 300.0);
        glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// traffic light holder
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[24]);
    glPushMatrix();
        glTranslatef(0, 25.5, 0);
        glScalef(4, 8, 4);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 0.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// traffic lights
    ///front in z
    ///glEnable(GL_TEXTURE_2D);
    ///glBindTexture(GL_TEXTURE_2D,v[22]);
    glPushMatrix();
        glTranslatef(0, 30.0, -2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
            if(!redSignal){
                //glEnable(GL_TEXTURE_2D);
                //glBindTexture(GL_TEXTURE_2D,v[27]);
                cube(1.0, 1.000, 1.000, 1.0);
            //glDisable(GL_TEXTURE_2D);
            }

            else{
                   glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D,v[26]);
                cube(1.0, 1.0, 1.0, 1.0);
            glDisable(GL_TEXTURE_2D);
            }
    glPopMatrix();
    ///glDisable(GL_TEXTURE_2D);

    glPushMatrix();
        glTranslatef(0, 28.0, -2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
            if(!redSignal){
              glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D,v[25]);
                cube(1.0, 1.0, 1.0, 1.0);
                glDisable(GL_TEXTURE_2D);
            }
            else{
                //glEnable(GL_TEXTURE_2D);
                //glBindTexture(GL_TEXTURE_2D,v[28]);
                cube(1.0, 1.0, 1.0, 1.0);
            //glDisable(GL_TEXTURE_2D);
            }
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 26.0, -2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
            if(!redSignal) {
                //glEnable(GL_TEXTURE_2D);
                //glBindTexture(GL_TEXTURE_2D,v[28]);
                cube(1.0, 1.0, 1.0, 1.0);
            //glDisable(GL_TEXTURE_2D);
            }
            else{
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D,v[29]);
                cube(1.0, 1.0, 1.0, 1.0);
            glDisable(GL_TEXTURE_2D);
            }
    glPopMatrix();
    ///back in z
    glPushMatrix();
        glTranslatef(0, 30.0, 2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 28.0, 2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 1.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0, 26.0, 2.0);
        glScalef(1.0, 1.5, 0.1);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 1.000, 0.000, 1.0);
    glPopMatrix();
    ///left in x
    glPushMatrix();
        glTranslatef(2.0, 30.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0, 28.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 1.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0, 26.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 1.000, 0.000, 1.0);
    glPopMatrix();
    ///right in x
    glPushMatrix();
        glTranslatef(-2.0, 30.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0, 28.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.000, 1.000, 0.000, 1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0, 26.0, 0);
        glScalef(0.1, 1.5, 1.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(0.0, 1.000, 0.000, 1.0);
    glPopMatrix();
}

void trafficSystemPositioned()
{
    glPushMatrix();
    glScalef(.8,.5,.8);
        ///glTranslatef(0, 0, 35.0);
        trafficSystem();
    glPopMatrix();
}


/// Curves & Surfaces
const double PI = 3.14159265389;
const int L = 20;
const int nt = 40;				//number of slices along x-direction
const int ntheta = 20;

GLfloat ctrlpoints[L+1][3] =
{
    {-0.0359765, 0.0234949, 0},
    {-0.223935, 0.187959, 0},
    {-0.29442, 0.422907, 0},
    {-0.200441, 0.610866, 0},
    {0.0814978, 0.610866, 0},
    {0.48091, 0.657856, 0},
    {0.739354, 0.657856, 0},
    {1.04479, 0.657856, 0},
    {1.30323, 0.657856, 0},
    {1.46769, 0.446402, 0},
    {1.4207, 0.234949, 0},
    {1.32673, 0.117474, 0}
};

void setNormal(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(-Nx,-Ny,-Nz);
}

long long nCr(int n, int r)
{
    if(r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for(i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

void BezierCurve ( double t,  float xy[2])
{
    double y=0;
    double x=0;
    t=t>1.0?1.0:t;
    for(int i=0; i<=L; i++)
    {
        int ncr=nCr(L,i);
        double oneMinusTpow=pow(1-t,double(L-i));
        double tPow=pow(t,double(i));
        double coef=oneMinusTpow*tPow*ncr;
        x+=coef*ctrlpoints[i][0];
        y+=coef*ctrlpoints[i][1];

    }
    xy[0] = float(x);
    xy[1] = float(y);
}

void chaka()
{
    GLfloat m_amb[] = {0.184, 0.310, 0.310, 1};
    GLfloat m_diff[] = {0.184, 0.310, 0.310, 1};
    GLfloat m_spec[] = {0.184, 0.310, 0.310, 1};
    GLfloat m_sh[] = {30};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    int i, j;
    float x, y, z, r;				//current coordinates
    float x1, y1, z1, r1;			//next coordinates
    float theta;

    const float startx = 0, endx = ctrlpoints[L][0];
    //number of angular slices
    const float dx = (endx - startx) / nt;	//x step size
    const float dtheta = 2*PI / ntheta;		//angular step size

    float t=0;
    float dt=1.0/nt;
    float xy[2];
    BezierCurve( t,  xy);
    x = xy[0];
    r = xy[1];
    //rotate about z-axis
    float p1x,p1y,p1z,p2x,p2y,p2z;
    for ( i = 0; i < nt; ++i )  			//step through x
    {
        theta = 0;
        t+=dt;
        BezierCurve( t,  xy);
        x1 = xy[0];
        r1 = xy[1];

        //draw the surface composed of quadrilaterals by sweeping theta
        glBegin( GL_QUAD_STRIP );
        for ( j = 0; j <= ntheta; ++j )
        {
            theta += dtheta;
            double cosa = cos( theta );
            double sina = sin ( theta );
            y = r * cosa;
            y1 = r1 * cosa;	//current and next y
            z = r * sina;
            z1 = r1 * sina;	//current and next z

            //edge from point at x to point at next x
            glVertex3f (x, y, z);

            if(j>0)
            {
                setNormal(p1x,p1y,p1z,p2x,p2y,p2z,x, y, z);
            }
            else
            {
                p1x=x;
                p1y=y;
                p1z=z;
                p2x=x1;
                p2y=y1;
                p2z=z1;

            }
            glVertex3f (x1, y1, z1);

            //forms quad with next pair of points with incremented theta value
        }
        glEnd();
        x = x1;
        r = r1;
    } //for i
}

void chakaPositioned(){
    /// right chakas
    glPushMatrix();
        glTranslatef(-5, 1.0, -18.0);
        glScalef(1.0, 2.0, 1.5);
        chaka();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5, 1.0, -21.0);
        glScalef(1.0, 2.0, 1.5);
        chaka();
    glPopMatrix();

    /// left chakas
    glPushMatrix();
        glTranslatef(-1.75, 1.0, -18.0);
        glScalef(1.0, 2.0, 1.5);
        chaka();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.75, 1.0, -21.0);
        glScalef(1.0, 2.0, 1.5);
        chaka();
    glPopMatrix();
}





void carBodyRaw()
{
    /// main body
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[8]);
        glPushMatrix();
        glTranslatef(-3.0, 1.5, -19.5);
        glScalef(3.5, 2.0, 5.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);



    /// upper body




    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[7]);
        glPushMatrix();
        glTranslatef(-3.0, 3.5, -20.0);
        glScalef(1.75, 1.0, 2.5);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
}

void car()
{
    /// car 1

    glPushMatrix();

        if(!redSignal){
            glTranslatef(-1.0, 0, car1Initial += carStep);
        }else{
            glTranslatef(-1.0, 0, car1Initial);
        }

        if(car1Initial > 40.0){
            car1Initial = -43.0;
        }

        ///chakaPositioned();


        glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[0]);
        chakaPositioned();
glDisable(GL_TEXTURE_2D);

        glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[9]);
        carBodyRaw();
glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}

void bogi_chakaPositioned(){
    /// right chakas
    /*glPushMatrix();
        glTranslatef(-5, 1.0, -18.0);
        glScalef(1.0, 2.0, 1.5);
        chaka();
    glPopMatrix();*/
    glPushMatrix();
        glTranslatef(-5, 1.0, -21.0);
        glScalef(1.0, 2.0, 1.5);
        chaka();
    glPopMatrix();

    /// left chakas
    /*glPushMatrix();
        glTranslatef(-1.75, 1.0, -18.0);
        glScalef(1.0, 2.0, 1.5);
        chaka();
    glPopMatrix();*/
    glPushMatrix();
        glTranslatef(-1.75, 1.0, -21.0);
        glScalef(1.0, 2.0, 1.5);
        chaka();
    glPopMatrix();
}


void bogiBodyRaw()
{
    /// main body
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[8]);
    /*glPushMatrix();
        glTranslatef(-3.0, 1.5, -19.5);
        glScalef(3.8, 3.5, 5.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
    */
    /// leftFront FrontXCube
    glScalef(.8,.2,.5);
    glTranslatef(-21,0,-24);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[14]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[14]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[16]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[16]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[15]);
        glPushMatrix();
            glTranslatef(17.5, 40, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}

void bogi()
{
    /// car 1
    //glTranslatef(3,0,0);
    glPushMatrix();
    //glTranslatef(.5,0,0);
        if(!tramSignal){
            glTranslatef(-1.0, 0, bogi1Initial += bogiStep);
        }else{
            glTranslatef(-1.0, 0, bogi1Initial);
        }

        if(bogi1Initial > 85.0){
            bogi1Initial = 0.0;
        }

        ///chakaPositioned();


        glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[0]);
        glPushMatrix();
        glScalef(1,.2,1);
        bogi_chakaPositioned();
        glPopMatrix();
glDisable(GL_TEXTURE_2D);

        //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[9]);
        bogiBodyRaw();
//glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}



void car2_BodyRaw()
{
    /// main body
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[8]);
    /*glPushMatrix();
        glTranslatef(-3.0, 1.5, -19.5);
        glScalef(3.8, 3.5, 5.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
    */
    /// leftFront FrontXCube
    glScalef(.8,.2,.5);
    glTranslatef(-21,0,-24);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[18]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[17]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[19]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    /// front_side
    glBindTexture(GL_TEXTURE_2D,v[20]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[21]);
        glPushMatrix();
            glTranslatef(17.5, 40, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}

void car2()
{
    /// car 1
    //glTranslatef(3,0,0);
    glPushMatrix();
    //glTranslatef(.5,0,0);
        if(!redSignal){
            glTranslatef(-1.0, 0, busInitial += busStep);
        }else{
            glTranslatef(-1.0, 0, busInitial);
        }

        if(busInitial > 72.0){
            busInitial = 4.0;
        }

        ///chakaPositioned();


        /*glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[0]);
        glPushMatrix();
        glScalef(1,.2,1);
        bogi_chakaPositioned();
        glPopMatrix();
glDisable(GL_TEXTURE_2D);
*/

        //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[9]);
        car2_BodyRaw();
//glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}



void car3_BodyRaw()
{
    /// main body
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[8]);
    /*glPushMatrix();
        glTranslatef(-3.0, 1.5, -19.5);
        glScalef(3.8, 3.5, 5.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
    */
    /// leftFront FrontXCube
    glScalef(.8,.2,.5);
    glTranslatef(-21,0,-24);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[18]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[17]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[19]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    /// front_side
    glBindTexture(GL_TEXTURE_2D,v[20]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[21]);
        glPushMatrix();
            glTranslatef(17.5, 40, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}

void car3()
{
    /// car 1
    //glTranslatef(3,0,0);
    glPushMatrix();
    //glTranslatef(.5,0,0);
        if(!redSignal){
            glTranslatef(-1.0, 0, busInitial += busStep);
        }else{
            glTranslatef(-1.0, 0, busInitial);
        }

        if(busInitial > 72.0){
            busInitial = 5.0;
        }

        ///chakaPositioned();


        /*glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[0]);
        glPushMatrix();
        glScalef(1,.2,1);
        bogi_chakaPositioned();
        glPopMatrix();
glDisable(GL_TEXTURE_2D);
*/

        //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[9]);
        car2_BodyRaw();
//glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}



void car4_BodyRaw()
{
    /// main body
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[8]);
    /*glPushMatrix();
        glTranslatef(-3.0, 1.5, -19.5);
        glScalef(3.8, 3.5, 5.0);
        glTranslatef(-0.5, 0, -0.5);
        cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
    */
    /// leftFront FrontXCube
    glScalef(.8,.2,.5);
    glTranslatef(-21,0,-24);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[18]);
        glPushMatrix();
            glTranslatef(15.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront BackXCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[17]);
        glPushMatrix();
            glTranslatef(20.0, 0, -17.0);
            glScalef(0.1, 40, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront FrontZCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[19]);
        glPushMatrix();
            glTranslatef(17.5, 0, -22.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    /// front_side
    glBindTexture(GL_TEXTURE_2D,v[20]);
        glPushMatrix();
            glTranslatef(17.5, 0, -12.0);
            glScalef(5, 40, 0.1);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// leftFront RoofCube()
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[21]);
        glPushMatrix();
            glTranslatef(17.5, 40, -17.0);
            glScalef(5, 0.1, 10);
            glTranslatef(-0.5, 0, -0.5);
            cube(1.0, 1.0, 1.0, 1.0);
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}

void car4()
{
    /// car 1
    //glTranslatef(3,0,0);
    glPushMatrix();
    //glTranslatef(.5,0,0);
        if(!redSignal){
            glTranslatef(-1.0, 0, bus2Initial += busStep);
        }else{
            glTranslatef(-1.0, 0, bus2Initial);
        }

        if(bus2Initial > 37.0){
            bus2Initial = -25.0;
        }

        ///chakaPositioned();


        /*glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[0]);
        glPushMatrix();
        glScalef(1,.2,1);
        bogi_chakaPositioned();
        glPopMatrix();
glDisable(GL_TEXTURE_2D);
*/

        //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,v[9]);
        car4_BodyRaw();
//glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}



/// Objects to display
void objects()
{
    //axes();

    //flr();
    glPushMatrix();
    ground();
    road();
    //car3();

    buildings();
    glPopMatrix();
    //Clock();
    //jatriChaonePositioned();

    leftLight();
    rightLight();

        /*glPushMatrix();
        spotLight();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0,0,-30);
        spotLight();
        glPopMatrix();*/




                ///leftLightIndicator();
                ///rightLightIndicator();
                ///spotLightIndicator();

                ///eyeIndicator();
                ///lookAtPointIndicator();

    //sky();
    clouds();
    glPushMatrix();
    glTranslatef(100,10,20);
    clouds();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(200,10,20);
    clouds();
    glPopMatrix();
    //sun();
    //plane();
    glPushMatrix();
    glScalef(1,1.5,1);
    lamPosts();
    glPopMatrix();

    //trees();

    glPushMatrix();
    glTranslatef(0,-100,0);
    glScalef(4,3,1);
    sun();
    glPopMatrix();

    glPushMatrix();
    leftLightIndicator();
    glPopMatrix();

     glPushMatrix();
    rightLightIndicator();
    glPopMatrix();

     glPushMatrix();
    //spotLightIndicator();
    glPopMatrix();



    trafficSystemPositioned();

    ///cout<<"X : "<<lookX<<" "<<"Y : "<<lookY<<" "<<"Z : "<<lookZ<<endl;
}

void objects2()
{
    //axes();

    //flr();
    glPushMatrix();
    //glScaled(1,1,4);
    glTranslatef(0.0,0,-8.2);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glPopMatrix();

    glPushMatrix();
    //glScaled(1,1,4);
    glTranslatef(10.0,0,-8.2);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glTranslatef(0.0,0,10);
    ground2();
    glPopMatrix();

    glPushMatrix();
    //glScaled(1,1,4);
    //glTranslatef(0,0,17);
    ///road();
    glPopMatrix();
    ///road();
    ///car();
    glPushMatrix();
    glTranslatef(-8.0,0,22);
    tram_road();
    glPopMatrix();

    glPushMatrix();
    //glTranslatef(.5,0,0);
    glScalef(1.46,1.2,1);
    glTranslatef(1.5,0,0);
    bogi();
    glTranslatef(0,0,4.5);
    bogi();
    glTranslatef(0,0,4.5);
    bogi();
    glTranslatef(0,0,4.5);
    bogi();
    glTranslatef(0,0,4.5);
    bogi();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-9.0,0,10);
    buildings_set2();
    glPopMatrix();

    //Clock();
    //jatriChaonePositioned();

    ///leftLight();
    ///rightLight();
    ///spotLight();

                ///leftLightIndicator();
                ///rightLightIndicator();
                ///spotLightIndicator();

                ///eyeIndicator();
                ///lookAtPointIndicator();

    //sky();
    //clouds();
    //sun();
    //plane();
    glPushMatrix();
    glScalef(1,1.5,1);
    lamPosts_2();
    glPopMatrix();

    //trees();

    //trafficSystemPositioned();

    ///cout<<"X : "<<lookX<<" "<<"Y : "<<lookY<<" "<<"Z : "<<lookZ<<endl;
}


void objects3()
{
    //axes();

    //flr();
    ground();
    road();
    glPushMatrix();
    glScalef(1.2,1.2,1.4);
    car2();
    glPopMatrix();

    glPushMatrix();
    glScalef(1.2,1.2,1.4);
    glRotatef(180,0,1,0);
    car4();
    glPopMatrix();

    buildings();
    //Clock();
    //jatriChaonePositioned();

    ///leftLight();
    ///rightLight();
    ///spotLight();

                ///leftLightIndicator();
                ///rightLightIndicator();
                ///spotLightIndicator();

                ///eyeIndicator();
                ///lookAtPointIndicator();

    //sky();
    //clouds();
    //sun();
    //plane();

    glPushMatrix();
    glScalef(1,1.5,1);
    lamPosts();
    glPopMatrix();
    //trees();

    trafficSystemPositioned();

    ///cout<<"X : "<<lookX<<" "<<"Y : "<<lookY<<" "<<"Z : "<<lookZ<<endl;
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5, 5, -5, 5, 2, 150.0);
    gluPerspective(0,0,0,0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,  lookX, lookY, lookZ,  ux, uy, uz);
    glRotatef(rotation, 0, 1, 0);
    glTranslatef(0,0,5);
    objects3();
    glTranslatef(0,0,41);
    objects();
    glTranslatef(35,0,0);
    //objects();
    glTranslatef(0,0.05,-40);
    //glScalef(.9,.9,.5);
    objects2();
    glutSwapBuffers();
}


void Yaw_1(){
    GLfloat tx, tz;

    tx = lookX - eyeX;
    tz = lookZ  - eyeZ;

    dx = tx*cs_angle+tz*sn_angle;
    dz = -tx*sn_angle+tz*cs_angle;

    lookX = dx + eyeX;
    lookZ = dz + eyeZ;

}

void Yaw_2(){
    GLfloat tx, tz;

    tx = lookX - eyeX;
    tz = lookZ - eyeZ;

    dx = tx*cs_angle-tz*sn_angle;
    dz = tx*sn_angle+tz*cs_angle;

    lookX = dx + eyeX;
    lookZ = dz + eyeZ;

}

void Pitch_1(){
    GLfloat ty, tz;

    ty = lookY - eyeY;
    tz = lookZ - eyeZ;

    dy = ty*cs_angle-tz*sn_angle;
    dz = ty*sn_angle+tz*cs_angle;

    lookY = dy + eyeY;
    lookZ = dz + eyeZ;
    /*cout<<"Pitch1: "<<endl;

    cout<<"ty: "<<ty<<" cs_angle: "<<cs_angle<<" -tz: "<<-tz<<" sn_angle: "<<sn_angle<<endl;
    cout<<"dy: "<<dy<<endl;

    cout<<"ty: "<<ty<<" sn_angle: "<<sn_angle<<" tz: "<<tz<<" cs_angle: "<<cs_angle<<endl;
    cout<<"dz: "<<dz<<endl;


    cout<<"lookY: "<<lookY<<" -eyeY: "<<-eyeY<<endl;
    cout<<"ty: "<<ty<<endl;

    cout<<"lookZ: "<<lookY<<" -eyeZ: "<<-eyeZ<<endl;
    cout<<"tz: "<<tz<<endl;

    cout<<"dy: "<<dy<<" eyeY: "<<eyeY<<endl;
    cout<<"lookY: "<<lookY<<endl;

    cout<<"dz: "<<dz<<" eyeZ: "<<eyeZ<<endl;
    cout<<"lookZ: "<<lookZ<<endl;
    */

}
void Pitch_2(){
    GLfloat ty, tz;

    ty = lookY - eyeY;
    tz = lookZ - eyeZ;

    dy = ty*cs_angle+tz*sn_angle;
    dz = -ty*sn_angle+tz*cs_angle;

    lookY = dy + eyeY;
    lookZ = dz + eyeZ;
    /*cout<<"Pitch2: "<<endl;

    cout<<"ty: "<<ty<<" cs_angle: "<<cs_angle<<" tz: "<<tz<<" sn_angle: "<<sn_angle<<endl;
    cout<<"dy: "<<dy<<endl;

    cout<<"-ty: "<<-ty<<" sn_angle: "<<sn_angle<<" tz: "<<tz<<" cs_angle: "<<cs_angle<<endl;
    cout<<"dz: "<<dz<<endl;


    cout<<"lookY: "<<lookY<<" -eyeY: "<<-eyeY<<endl;
    cout<<"ty: "<<ty<<endl;

    cout<<"lookZ: "<<lookY<<" -eyeZ: "<<-eyeZ<<endl;
    cout<<"tz: "<<tz<<endl;

    cout<<"dy: "<<dy<<" eyeY: "<<eyeY<<endl;
    cout<<"lookY: "<<lookY<<endl;

    cout<<"dz: "<<dz<<" eyeZ: "<<eyeZ<<endl;
    cout<<"lookZ: "<<lookZ<<endl;
    */

}

void Roll_1(){
    GLfloat tx, ty;

    //tx = lookX - eyeX;
    //ty = lookY - eyeY;

    dx = ux*cs_angle-uy*sn_angle;
    dy = ux*sn_angle+uy*cs_angle;

    ux = dx;
    uy = dy;

    /*cout<<"Roll: "<<endl;
    cout<<"ux: "<<ux<<" cs_angle: "<<cs_angle<<" -uy: "<<-uy<<" sn_angle: "<<sn_angle<<endl;
    cout<<"dx: "<<dx<<endl;

    cout<<"ux: "<<ux<<" sn_angle: "<<sn_angle<<" uy: "<<uy<<" cs_angle: "<<cs_angle<<endl;
    cout<<"dy: "<<dy<<endl;
    */

    //lookX = ux + eyeX;
    //lookY = uy + eyeY;
}

void Roll_2(){
    GLfloat tx, ty;
    //tx = lookX - eyeX;
    //ty = lookY - eyeY;

    dx = ux*cs_angle+uy*sn_angle;
    dy = -ux*sn_angle+uy*cs_angle;

    /*
    cout<<"Roll: "<<endl;
    cout<<"ux: "<<ux<<" cs_angle: "<<cs_angle<<" uy: "<<uy<<" sn_angle: "<<sn_angle<<endl;
    cout<<"dx: "<<dx<<endl;

    cout<<"-ux: "<<-ux<<" sn_angle: "<<sn_angle<<" uy: "<<uy<<" cs_angle: "<<cs_angle<<endl;
    cout<<"dy: "<<dy<<endl;
*/
    ux = dx;
    uy = dy;

    //lookX = ux + eyeX;
    //lookY = uy + eyeY;
}


static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
        exit(0);
        break;
    case ',':
        rotation += 5;
        break;
    case '.':
        rotation -= 5;
        break;

    /// amb, dif & spe control
    case 'a':
        amb_on =! amb_on;
        break;
    case 'd':
        dif_on =! dif_on;
        break;
    case 's':
        spe_on =! spe_on;
        break;

    /// zoom in, zoom out & left-right
    case 'g':
        //if(lookX <22) lookX++;
        //else lookX;
        lookX++;
        break;
    case 'h':
        //if(lookX > -22) lookX--;
        //else lookX;
        lookX--;
        break;

    case 'j':
        eyeX++;
        break;
    case 'k':
        eyeX--;
        break;
    case 'u':
        eyeY++;
        lookY++;
        break;
    case 'i':
        if(lookY > -1){
            lookY--;
            eyeY--;
        }else{
            lookY;
            eyeY;
        }
        //eyeY--;
        //lookY--;
        break;

    case '=':
        if(lookZ < 123){
            lookZ++;
            eyeZ++;
        }else{
            lookZ;
            eyeZ;
        }
        //eyeZ++;
        //lookZ++;
        break;
    case '-':
        if(lookZ > 29){
            lookZ--;
            eyeZ--;
        }else{
            lookZ;
            eyeZ;
        }
        //eyeZ--;
        //lookZ--;
        break;

    ///lighting on off
    case '1':
        leftLight_on =!leftLight_on;
        break;
    case '2':
        rightLight_on =!rightLight_on;
        break;
    case '3':
        spotLight_on =! spotLight_on;
        break;


    case '4':
        Pitch_1();
        break;
    case '5':
        Pitch_2();
        break;
    case '6':
        Yaw_1();
        break;
    case '7':
        Yaw_2();
        break;
    case '8':
        Roll_1();
        break;
    case '9':
        Roll_2();
        break;

    /// moving objects control
    case 't':
        redSignal =!redSignal;
        break;
    case 'y':
        tramSignal =!tramSignal;
        break;
    case 'p':
        planeMovement =!planeMovement;
        break;
    case 'c':
        cloudMovement =!cloudMovement;
        break;
    case 'n':
        renr=0.0;
        reng=0.0;
        renb=0.0;
        break;
    case 'm':
        renr=0.88;
        reng=0.96;
        renb=1.0;
        break;
    }
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

void initRendering(GLfloat renr,GLfloat reng, GLfloat renb)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL); //Enable color
    glClearColor(renr, reng, renb, 1.0f); //Change the background to sky blue
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Project_full - City Road 3D View(1707064)");

    glutReshapeFunc(res);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);

    glEnable(GL_LIGHTING);
    GLfloat globalAmbient[] = {.2, 0.2, 0.2, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    initRendering(renr,reng,renb);
    texture_image();

    printf("\tZoom In -> +\n");
    printf("\tZoom Out -> -\n");
    printf("\tLook Left -> g\n");
    printf("\tLook Right -> h\n");
    printf("\tMove Camera Left -> j\n");
    printf("\tMove Camera Right -> k\n");
    printf("\tUp -> u\n");
    printf("\tDown -> i\n\n");

    printf("\tPoint Light #1 On/Off -> 1\n");
    printf("\tPoint Light #2 On/Off -> 2\n");
    printf("\tSpot Light On/Off -> 3\n\n");


    printf("\tTraffic System control -> t\n");
    printf("\tTraffic System control -> y\n");

    printf("\tTraffic System control -> y\n");
   //cout<<cos(pi/180)<<endl;

    glutMainLoop();

    return EXIT_SUCCESS;
}
