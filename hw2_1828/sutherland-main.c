/* Program example for Sutherland cropping algorithm
by Lagaras Stelios hw2 Graphics 2021-2022 */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <math.h>
#include <unistd.h> //you need this for linux!
#include <stdbool.h>

// #define SCREEN_WIDTH 700
// #define SCREEN_HEIGHT 700

#define MAX_ITEMS 10000 // Size of the array of lines.

void draw_polygon();
void draw_line(int x1, int y1, int x2, int y2);
void printTables();
void swap(int *xp, int *yp);

int useSuthHodgClip();

int window;
int itemCt = 0; // Number of items in the array.
int x_ct = 0;
int mode;
int space_mode;
int mode_polygon;
bool dragging = false; // Set to true while the user is drawing.
bool Clearflag = false;
int width;  // Current width of window; value is maintained by reshape().
int height; // Current height of window; value is maintained by reshape().

int dragModifiers; // While the user is drawing, this holds the
                   // state of the modifier keys at the START of
                   // the drawing operation.  See the documentation
                   // for glutGetModifiers().

struct ItemData
{
    int x1, y1; // Coordinates of one endpoint of the line.
};

struct ItemData Spots[1000];
struct ItemData Xspot;
struct ItemData Yspot;
struct ItemData starting_point;

// Menu items
enum MENU_TYPE
{
    POLYGON,
    SELECTOR,
    HIDE_BOX,
    REPAINT,
    CLEAR,
    DRAW_POLYGON // be carefull , may need a boolean
};

void myInit()
{
    width = 900;
    height = 700;
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, width, 0, height);
    glEnable(GL_BLEND);                                // Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending function.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void clearInit()
{
    struct ItemData Xspot = {0};
    struct ItemData Yspot = {0};
    struct ItemData starting_point = {0};
    struct ItemData Spots[1000] = {0};

    itemCt = 0; // Number of items in the array.
    x_ct = 0;
    mode_polygon = 4;
    space_mode = 0;
    mode = POLYGON;
}

void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Fill the window with the color.

    if (mode == POLYGON)
    {
        glPointSize(5);

        glBegin(GL_POINTS);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // white

        for (int i = 0; i < itemCt; i++)
        {
            glVertex2i(Spots[i].x1, Spots[i].y1);
            // printf("drawing a spot in (%d,%d) \n", Spots[i].x1, Spots[i].y1);
        }
        glEnd();
        glFlush();

        // printf("itemCt= %d \n", itemCt);

        if (mode_polygon == DRAW_POLYGON)
        {
            // printf("i am in polygon display \n");
            draw_polygon();
        }
    }

    else if ((mode == SELECTOR && dragging) || (space_mode == REPAINT))
    {
        // printf("drawing dragging lines (%d,%d) \n", Xspot.x1, Xspot.y1);

        glColor4f(1, 0.859, 0.345, 1.0f); // white

        // draw_line(starting_point.x1, starting_point.y1, Xspot.x1, Xspot.y1);
        // draw_line(starting_point.x1, starting_point.y1, Yspot.x1, Yspot.y1);
        // draw_line(Yspot.x1, Yspot.y1, Xspot.x1, Yspot.y1);
        // draw_line(Xspot.x1, Xspot.y1, Xspot.x1, Yspot.y1);

        glRecti(starting_point.x1, starting_point.y1, Xspot.x1, Yspot.y1);

        draw_polygon();
    }
    else if (space_mode == HIDE_BOX)
    {
        // printf("spacebar triggered \n");
        glColor3i(0, 0, 0);

        glRecti(starting_point.x1, starting_point.y1, Xspot.x1, Yspot.y1);

        glColor4f(1.0f, 1.0f, 1.0f, 0.0f); // white

        draw_polygon();
    }
}

void draw_line(int x1, int y1, int x2, int y2)
{

    // glColor3f(0.0f, 0.4f, 0.2f);
    glPointSize(3.0);

    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();
    glFlush();
    // printf("Successfully drawn a line \n");
}

void draw_square(struct ItemData points[])
{
}

void draw_polygon()
{
    glColor4f(1.0f, 1.0f, 1.0f, 0.7f); // white
 


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // printf("Draw polygon rendering \n");

    glBegin(GL_POLYGON);

    for (int i = 0; i < itemCt; i++)
    {
        glVertex2i(Spots[i].x1, Spots[i].y1);
    }

    glEnd();
    glFlush();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void handleStartDraw(int x, int y, int modifiers)
{
    // Called by the mouse() function when the user clicks the drawing
    // area with the left mouse button.  (x,y) is the point where the
    // user clicked, in the coordinate system used on the drawing area.
    // modifiers encodes the modifier keys that the user is holding down.

    dragging = true; // Set dragging to true to indicate drawing in progress.
    dragModifiers = modifiers;

    starting_point.x1 = x; // The data for the line that is being
    starting_point.y1 = y; //    drawn.  The second point will be

    printf("Starting point is: (%d ,%d) \n", starting_point.x1, starting_point.y1);
}

void handleContinueDraw(int x, int y)
{
    // Called by the motion() function when the user drags the
    //    mouse.  (x,y) is the mouse position, in drawing coords.

    if (!dragging)
        return;
    int current = itemCt - 1; // This is the position in the array
                              //   of the line that is being drawn.

    Xspot.x1 = x; // Modify the terminal point of the line.
    Xspot.y1 = starting_point.y1;

    Yspot.x1 = starting_point.x1; // Modify the terminal point of the line.
    Yspot.y1 = height - y;

    // printf("Xspots is: (%d,%d) \n", Xspots[x_ct].x1, Xspots[x_ct].y1);

    x_ct++;
    // printf("HandleContinueDraw x: %d y: %d \n", x, y);
    // printf("dragging will increase x_ct: %d \n", x_ct);

    glutPostRedisplay(); // Window must be redrawn to show modified line.
}

void handleFinishDraw(int x, int y)
{
    // Called by the mouse() function when the user releases the
    //    mouse button.  (x,y) is the mouse position in drawing coords.
    if (!dragging)
        return;
    dragging = false; // End the draw operation.

    // printTables();

    glutPostRedisplay(); // Redraw, just in case.
}

void printTables()
{
}

void motion(int x, int y)
{
    if (dragging)
    {
        handleContinueDraw(x, y);
    }
}

void mouseButton(int button, int state, int x, int y)
{
    y = height - y;

    if (Clearflag == 0)
    {

        if (space_mode == HIDE_BOX)
        {
            printf("Warning you are clicking while box is in hidden mode! To see your box press Space again \n");
        }

        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && mode == POLYGON)
        {

            Spots[itemCt].x1 = x;
            Spots[itemCt].y1 = y;
            // printf("allocated a spot (%d,%d) \n", x, y);
            itemCt++; // Number of items in the array.
        }
        else if (mode == SELECTOR)
        {
            if (state == GLUT_DOWN)
                handleStartDraw(x, y, glutGetModifiers());
            else
                handleFinishDraw(x, y);
        }
    }
    else
    {
        Clearflag = 0;
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 32) // spacebar = 32
    {
        printf("spacebar works \n");
        if (space_mode == HIDE_BOX)
        {
            space_mode = REPAINT;
        }
        else
        {
            space_mode = HIDE_BOX;
        }
    }
    if (key == ('D') || key == ('d'))
    {
        printf("D is captured \n");
        mode_polygon = DRAW_POLYGON;
    }
    if (key == ('C') || key == ('c'))
    {
        useSuthHodgClip();
        mode = SELECTOR;
        space_mode = REPAINT;
        printf("finished SuthHodhClip \n");
        // printf("%d \n", mode);
        // printf("%d \n", space_mode);
    }

    if (key == 27)
    {
        exit(0);
    }
    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    if (key == GLUT_KEY_F1)
    { // This is the escape key.
        if (mode == SELECTOR)
        {
            mode = POLYGON;
            printf("Mode is on drawing a polygon \n");
        }
        else
        {
            mode = SELECTOR;
            printf("Mode is on drawing a Selector \n");
        }
    }
}

void menuhandler(int item)
{

    switch (item)
    {
    case CLEAR:
        printf("Clear the window \n");
        Clearflag = 1;
        clearInit();
    }

    glutPostRedisplay();

    return;
}

void createGLUTMenus()
{

    int menu;

    // create the menu and
    // tell glut that "processMenuEvents" will
    // handle the events
    menu = glutCreateMenu(menuhandler);

    glutAddMenuEntry("Clear", CLEAR);

    // attach the menu to the right button
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int w, int h)
{
    width = w;
    height = h;
    // Compute aspect ratio of the new window
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset the projection matrix
    gluOrtho2D(0, width, 0, height);
    glutPostRedisplay();
}

// C++ program for implementing Sutherland–Hodgman
// algorithm for polygon clipping

const int MAX_POINTS = 20;

// Returns x-value of point of intersection of two
// lines
int x_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1 * y2 - y1 * x2) * (x3 - x4) -
              (x1 - x2) * (x3 * y4 - y3 * x4);
    int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    return num / den;
}

// Returns y-value of point of intersection of
// two lines
int y_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1 * y2 - y1 * x2) * (y3 - y4) -
              (y1 - y2) * (x3 * y4 - y3 * x4);
    int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    return num / den;
}

// This functions clips all the edges w.r.t one clip
// edge of clipping area
void clip(int x1, int y1, int x2, int y2)
{
    int new_points[MAX_POINTS][2], new_poly_size = 0;

    // (ix,iy),(kx,ky) are the co-ordinate values of
    // the points
    for (int i = 0; i < itemCt; i++)
    {
        // i and k form a line in polygon
        int k = (i + 1) % itemCt;
        int ix = Spots[i].x1, iy = Spots[i].y1;
        int kx = Spots[k].x1, ky = Spots[k].y1;

        // Calculating position of first point
        // w.r.t. clipper line
        int i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

        // Calculating position of second point
        // w.r.t. clipper line
        int k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

        // Case 1 : When both points are inside
        if (i_pos < 0 && k_pos < 0)
        {
            // Only second point is added
            new_points[new_poly_size][0] = kx;
            new_points[new_poly_size][1] = ky;
            new_poly_size++;
        }

        // Case 2: When only first point is outside
        else if (i_pos >= 0 && k_pos < 0)
        {
            // Point of intersection with edge
            // and the second point is added
            new_points[new_poly_size][0] = x_intersect(x1,
                                                       y1, x2, y2, ix, iy, kx, ky);
            new_points[new_poly_size][1] = y_intersect(x1,
                                                       y1, x2, y2, ix, iy, kx, ky);
            new_poly_size++;

            new_points[new_poly_size][0] = kx;
            new_points[new_poly_size][1] = ky;
            new_poly_size++;
        }

        // Case 3: When only second point is outside
        else if (i_pos < 0 && k_pos >= 0)
        {
            // Only point of intersection with edge is added
            new_points[new_poly_size][0] = x_intersect(x1,
                                                       y1, x2, y2, ix, iy, kx, ky);
            new_points[new_poly_size][1] = y_intersect(x1,
                                                       y1, x2, y2, ix, iy, kx, ky);
            new_poly_size++;
        }

        // Case 4: When both points are outside
        else
        {
            // No points are added
        }
    }

    // Copying new points into original array
    // and changing the no. of vertices
    itemCt = new_poly_size;
    for (int i = 0; i < itemCt; i++)
    {
        Spots[i].x1 = new_points[i][0];
        Spots[i].y1 = new_points[i][1];
    }
}

// Implements Sutherland–Hodgman algorithm
void suthHodgClip(int clipper_points[][2], int clipper_size)
{
    // i and k are two consecutive indexes
    for (int i = 0; i < clipper_size; i++)
    {
        int k = (i + 1) % clipper_size;

        // We pass the current array of vertices, it's size
        // and the end points of the selected clipper line
        clip(clipper_points[i][0],
             clipper_points[i][1], clipper_points[k][0],
             clipper_points[k][1]);
    }

    // Printing vertices of clipped polygon
    for (int i = 0; i < itemCt; i++)
    {
        printf("Vertices of clipped polygon \n");
        printf("(%d ,", Spots[i].x1);
        printf("%d) \n", Spots[i].y1);
    }
}

// Driver code
int useSuthHodgClip()
{
    // Defining polygon vertices in clockwise order

    // 2nd Example with triangle clipper

    // printf("I entered the Suth algorithm \n");

    int clipper_points[][2] = {{starting_point.x1, starting_point.y1}, {Yspot.x1, Yspot.y1}, {Xspot.x1, Yspot.y1}, {Xspot.x1, Xspot.y1}};

    int min[2] = {width, height};
    int temp_x, temp_y;

    int i, j;

    for (i = 0; i < 3; i++)
    {
        // Last i elements are already in place
        for (j = 0; j < 3; j++)
        {
            if (clipper_points[j][0] > clipper_points[j + 1][0])
            {
                swap(&clipper_points[j][0], &clipper_points[j + 1][0]);
                swap(&clipper_points[j][1], &clipper_points[j + 1][1]);
            }
        }
    }

    if (clipper_points[0][1] > clipper_points[1][1])
    {
        swap(&clipper_points[0][0], &clipper_points[1][0]);
        swap(&clipper_points[0][1], &clipper_points[1][1]);
    }

    if (clipper_points[2][1] < clipper_points[3][1])
    {
        swap(&clipper_points[2][0], &clipper_points[3][0]);
        swap(&clipper_points[2][1], &clipper_points[3][1]);
    }

    for (int i = 0; i < 4; i++)
    {
        printf("Clipping point %d, is: (%d,%d) \n", i, clipper_points[i][0], clipper_points[i][1]);
    }

    // Calling the clipping function
    suthHodgClip(clipper_points,
                 4);

    return 0;
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

int main(int argc, char **argv)
{
    printf("Welcome to the command line feedback: Please press the type \n'd' when you have finished with the polygon vertices \n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900, 700);
    glutInitWindowPosition(50, 50);
    window = glutCreateWindow("Sutherland Clipping Algorithm - \
    Press D for rendering the Polygon Vertexes - F1 for Selector \
    - Space for Hiding the selector - Right Click to Clear");

    createGLUTMenus();

    glutMouseFunc(mouseButton); // mouse func
    glutMotionFunc(motion);     // dragging
    glutKeyboardFunc(keyboard); // Uncomment to enable ASCII key handling.
    glutSpecialFunc(special);   // Uncomment to enable special key handling.

    glutDisplayFunc(myDisplay);
    glutReshapeFunc(reshape);
    myInit();

    glutMainLoop();
    return 1;
}