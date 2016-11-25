#define _USE_MATH_DEFINES

#include <cassert>
#include <cmath>
#include <cstdio>

#include <fstream>
#include <iterator>
#include <string>

#include <GL/glew.h>
#include <GLUT/glut.h>

bool g_useShader = false;
int windowWidth = 1024, windowHeight = 576;
GLuint program = 0;
GLint uniformCenter = -1;

//http://blog.chinaunix.net/uid-20384806-id-1954362.html
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

void SpecialKeys(int key, int x, int y);
void reshape( GLsizei w, GLsizei h );
void SetupRC();

void display()
{
    //清空颜色缓冲区，填充的颜色由 glClearColor( 0, 0.0, 0.0, 1 ); 指定为黑色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Save matrix state and do the rotation

    glPushMatrix();
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    //绘制太阳
    glColor3f( 1.0f, 1.0f, 0.0f );//黄色

    glDisable( GL_LIGHTING );
    glutSolidSphere( 50.f, 15, 15 );

    //启动0号光源（模拟太阳的光芒）
    glEnable( GL_LIGHTING );
    GLfloat lightPos[] = { 0, 0, 0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPos );


    //绘制地球
    static GLfloat earthAngle = 0;
    const GLfloat RADIUS = 100.f;
    glRotatef( earthAngle, 0, 0, 1 );
    glTranslatef( RADIUS, RADIUS, 0 );
    glColor3f( 0.0f, 1.0f, 0.0f );//绿色

    glutSolidSphere( 20.f, 15, 15 );

    //启动1号光源,模拟地球的散射光
    glLightfv( GL_LIGHT1, GL_POSITION, lightPos );
    GLfloat whiteDiffuseLight[] = {0.1, 0.1, 0.1}; //set the diffuse light to white

    glLightfv( GL_LIGHT1, GL_DIFFUSE, whiteDiffuseLight );

    // 绘制黄色的“月亮”
    glColor3f(1.0f, 1.0f, 0.0f);
    glRotatef(earthAngle/30.0*360.0 - earthAngle/360.0*360.0, 0.0f, 0.0f, -1.0f);
    glTranslatef(RADIUS/3, 0.0f, 0.0f);
    glutSolidSphere(7, 20, 20);


    //启动2号光源,模拟月球的反射光

    glLightfv( GL_LIGHT2, GL_POSITION, lightPos );
    GLfloat graySpecularLight[] = {0.2, 0.2, 0.2}; //set the Specular light to gray white

    glLightfv( GL_LIGHT2, GL_DIFFUSE, graySpecularLight );

    earthAngle += 0.1;
    if ( earthAngle >= 360 )
    {
        earthAngle -= 360;
    }

    printf( "Angle:%f\n", earthAngle );

    //Restore transformations

    glPopMatrix();
    glutSwapBuffers();
}

GLuint loadShaderSource(const char *path, GLenum type)
{
    GLuint shader = glCreateShader(type);

    std::ifstream file(path);
    std::string sourceString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    const char *sourcePointers[1] = { sourceString.c_str() };

    glShaderSource(shader, 1, sourcePointers, nullptr);
    glCompileShader(shader);

    char msg[1024] = "";
    glGetShaderInfoLog(shader, sizeof(msg), nullptr, msg);
    std::fprintf(stderr, "%s\n", msg);

    return shader;
}

void init()
{
    if (!g_useShader)
        return;
    GLuint vShader = loadShaderSource("transform.vert", GL_VERTEX_SHADER);
    GLuint fShader = loadShaderSource("transform.frag", GL_FRAGMENT_SHADER);

    program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    char msg[1024] = "";
    glGetProgramInfoLog(program, sizeof(msg), nullptr, msg);
    std::fprintf(stderr, "%s\n", msg);

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    uniformCenter = glGetUniformLocation(program, "center");
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Solar System");

//    glewInit();
    init();

    glutDisplayFunc(display);
    glutIdleFunc (display); //glutIdleFunc 表示在CPU空闲的时间调用某一函数

    glutReshapeFunc(reshape);
    glutSpecialFunc( SpecialKeys );
    SetupRC();
    glutMainLoop();

    return 1;

}



void SetupRC()
{
    glClearColor( 0, 0.0, 0.0, 1 );
    glShadeModel( GL_SMOOTH );
    glEnable(GL_DEPTH_TEST);//被遮住的部分不绘制

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
}


void reshape( GLsizei w, GLsizei h )
{
    GLfloat nRange = 200.0f;

    // Prevent a divide by zero

    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions

    glViewport(0, 0, w, h);

    // Reset projection matrix stack

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Establish clipping volume (left, right, bottom, top, near, far)

    if (w <= h)
        glOrtho (-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange, nRange);
    else
        glOrtho (-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange, nRange);

    //45度视野，近、远平面分别为1和200

    //gluPerspective( 45.f, (GLfloat)w/(GLfloat)h, 1, 200 );


    // Reset Model view matrix stack

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        xRot -= 5.0f;

    if(key == GLUT_KEY_DOWN)
        xRot += 5.0f;

    if(key == GLUT_KEY_LEFT)
        yRot -= 5.0f;

    if(key == GLUT_KEY_RIGHT)
        yRot += 5.0f;

    if(xRot > 356.0f)
        xRot = 0.0f;

    if(xRot < -1.0f)
        xRot = 355.0f;

    if(yRot > 356.0f)
        yRot = 0.0f;

    if(yRot < -1.0f)
        yRot = 355.0f;

    // Refresh the Window

    glutPostRedisplay();// this will refresh the window, so, it works the same to call RenderScene() directly

}






void checkGL()
{
    glFinish();
    GLenum error = glGetError();
    assert(error == GL_NO_ERROR);
}

void drawCube()
{
    glBegin(GL_QUADS);
    for (int side = 0; side < 6; ++side)
    {
        glColor3fv(cubeSideColors[side]);
        for (int vertex = 0; vertex < 4; ++vertex)
            glVertex3fv(cubeVertices[cubeVertexIds[side][vertex]]);
    }
    glEnd();
}



void display()
{
    static double theta = 0;

    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double cameraX = std::cos(theta) * 20;
    double cameraY = std::sin(theta) * 20;
    theta += M_PI / 360.0;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, 20, 0, 0, 0, 0, 0, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, double(windowWidth) / double(windowHeight), 0.1, 1000.0);
    if (g_useShader)
        glUseProgram(program);
    else
        glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    for (int y = -20; y <= 20; y += 4)
    {
        for (int x = -20; x <= 20; x += 4)
        {
            if (g_useShader)
                glUniform3f(uniformCenter, float(x), float(y), 0.0f);
            else
            {
                glLoadIdentity();
                gluLookAt(cameraX, cameraY, 20, 0, 0, 0, 0, 0, 1);
                glTranslatef(float(x), float(y), 0.0f);
            }
            drawCube();
        }
    }

    glDisable(GL_DEPTH_TEST);
    if (g_useShader)
        glUseProgram(0);

    checkGL();

    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}
