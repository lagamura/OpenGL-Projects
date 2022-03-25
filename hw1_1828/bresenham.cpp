#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> //you need this for linux!

int window;
int x1, y_1, x2, y2, xc, yc, a, b;
int numofclicks = 0;
int menuflag = -1;
bool popup_flag;

// Menu items
enum MENU_TYPE
{
	LINE,
	ELLIPSE,
	CLEAR,
};

void myInit()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 700, 700, 0);
}

void draw_pixel(int x, int y)
{
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}


void PlotEllipse(int xc, int yc, int x, int y)
{
	draw_pixel(xc + x, yc + y);
	draw_pixel(xc + x, yc - y);
	draw_pixel(xc - x, yc + y);
	draw_pixel(xc - x, yc - y);
}

/* https://www.cpp.edu/~raheja/CS445/MEA.pdf */


void draw_ellipse(int xC,int yC,int rx,int ry)
{
    //int xC = 250;
    //int yC = 300;
    //int rx = 200;
    //int ry = 200;

    //plotting for 1st region of 1st quardant and the slope will be < -1
     //----------------------Region-1------------------------//
    float x = 0;
    float y = ry;//(0,ry) ---
    float p1 = ry * ry - (rx * rx)* ry + (rx * rx) * (0.25) ;
    //slope
    float dx = 2 * (ry * ry) * x;
    float dy = 2 * (rx * rx) * y;
    while(dx < dy)
    {
        //plot (x,y)
         draw_pixel(xC + x  ,  yC+y);
         draw_pixel( xC - x,   yC + y);
         draw_pixel( xC + x  , yC - y );
         draw_pixel( xC - x  , yC - y);
        if(p1 < 0)
        {
            x = x + 1;
            dx = 2 * (ry * ry) * x;
            p1 = p1 + dx + (ry * ry);
        }
        else
        {
            x = x + 1;
            y = y - 1;
            dx =  2 * (ry * ry) * x;
            dy =  2 * (rx * rx) * y;
             p1 = p1 + dx - dy +(ry * ry);
        }
    }
    //ploting for 2nd region of 1st quardant and the slope will be > -1
     //----------------------Region-2------------------------//
    float p2 = (ry * ry )* ( x +  0.5) * ( x +  0.5) +  ( rx * rx) * ( y - 1) * ( y - 1) - (rx * rx )* (ry * ry);


    while(y > 0)
    {
        //plot (x,y)
         draw_pixel(xC + x  ,  yC+y);
         draw_pixel( xC - x,   yC + y);
         draw_pixel( xC + x  , yC - y );
         draw_pixel( xC - x  , yC - y);     //glEnd();
        if(p2 > 0)
        {
            x = x;
            y = y - 1;
            dy = 2 * (rx * rx) * y;
            //dy = 2 * rx * rx *y;
            p2 = p2 - dy + (rx * rx);
        }
        else
        {
            x = x + 1;
            y = y - 1;
            dy = dy - 2 * (rx * rx) ;
            dx = dx + 2 * (ry * ry) ;
            p2 = p2 + dx -
            dy + (rx * rx);
        }
    }

}

void draw_line(int x1, int x2, int y_1, int y2)
{
	printf("Time to draw a line with vertices of: (%d - %d, %d - %d)\n", x1, y_1, x2, y2);
	float dx, dy, i, e;
	float incx, incy, inc1, inc2;
	float x, y;

	dx = x2 - x1;
	dy = y2 - y_1;

	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;
	incx = 1;
	if (x2 < x1)
		incx = -1;
	incy = 1;
	if (y2 < y_1)
		incy = -1;
	x = x1;
	y = y_1;
	if (dx > dy)
	{
		draw_pixel(x, y);
		e = 2 * dy - dx;
		inc1 = 2 * (dy - dx);
		inc2 = 2 * dy;
		for (i = 0; i < dx; i++)
		{
			if (e >= 0)
			{
				y += incy;
				e += inc1;
			}
			else
				e += inc2;
			x += incx;
			draw_pixel(x, y);
			//usleep(100000);
			//glFlush();
		}
	}
	else
	{
		draw_pixel(x, y);
		e = 2 * dx - dy;
		inc1 = 2 * (dx - dy);
		inc2 = 2 * dx;
		for (i = 0; i < dy; i++)
		{
			if (e >= 0)
			{
				x += incx;
				e += inc1;
			}
			else
				e += inc2;
			y += incy;
			draw_pixel(x, y);
			//usleep(100000);
			//glFlush();
		}
	}
}

void myDisplay()
{
	if (menuflag == LINE)
	{
		//if(x1 > 0 && x2 > 0 )
		draw_line(x1, x2, y_1, y2);
		printf("Finished Drawing Line \n");
	}
	else if (menuflag == ELLIPSE)
	{
		if (a > 0 && b > 0)
		{
			printf("draw ellipse with centra: %d,%d and  a=%d, b=%d \n", xc, yc, a, b);
			draw_ellipse(xc, yc, a, b);
		}
	}
	else if (menuflag == CLEAR)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean the screen and the depth buffer
		//glLoadIdentity();									// Reset The Projection Matrix
	}

	//draw_ellipse(100,100,200,100);
	glFlush();
}

void changeSize(int w, int h)
{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = 1.0 * w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 1, 1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void mouseButton(int button, int state, int x, int y)
{
	if (!popup_flag)
	{

		if (menuflag == ELLIPSE)
		{
			if (button == GLUT_LEFT_BUTTON)
			{

				if (state == GLUT_UP)
				{
					numofclicks++;
					printf("numofclicks =%d \n", numofclicks);

					// when the button is released
					if (numofclicks % 3 == 1)
					{
						xc = x;
						yc = y;
						printf("I clicked in the cordinates %d %d \n", x, y);
						printf("new xc=%d new yc=%d \n", xc, yc);
					}
					else if (numofclicks % 3 == 2)
					{
						a = abs(x - xc);

						printf("I clicked in the cordinates %d %d \n", x, y);
						printf("new a = %d \n", a);
					}
					else if (numofclicks % 3 == 0)
					{

						b = abs(y - yc);
						printf("I clicked in the cordinates %d %d \n", x, y);
						printf("new b=%d \n", b);
						printf("Redisplay triggered \n");
						glutPostRedisplay();
					}
				}
			}
		}
		else if (menuflag == LINE)
		{
			if (button == GLUT_LEFT_BUTTON)
			{

				if (state == GLUT_UP)
				{
					numofclicks++;
					printf("numofclicks =%d \n", numofclicks);

					// when the button is released
					if (numofclicks % 2 == 1)
					{
						x1 = x;
						y_1 = y;
						printf("I clicked in the cordinates %d %d \n", x, y);
						printf("new x1=%d new y_1=%d \n", x1, y_1);
					}
					else
					{
						x2 = x;
						y2 = y;
						printf("I clicked in the cordinates %d %d \n", x, y);
						printf("new x2=%d new y2=%d \n", x2, y2);
						printf("Redisplay triggered \n");
						glutPostRedisplay();
					}
				}
			}
		}
	}
}

void menuhandler(int item)
{

	menuflag = item;

	switch (item)
	{

	case LINE:
		printf("I choose line \n");
		break;

	case ELLIPSE:
		printf("I choose ellipse \n");
		break;

	case CLEAR:
		printf("Clear the window \n");
	}

	glutPostRedisplay();

	return;
}
void statusfunc(int status, int x, int y)
{
	if (status == GLUT_MENU_IN_USE)
		popup_flag = 1;
	else
		popup_flag = 0;
	printf("status callback:%d\n", popup_flag);
}

void createGLUTMenus()
{

	int menu;

	// create the menu and
	// tell glut that "processMenuEvents" will
	// handle the events
	menu = glutCreateMenu(menuhandler);

	//add entries to our menu
	glutAddMenuEntry("Line", LINE);
	glutAddMenuEntry("Ellipse", ELLIPSE);
	glutAddMenuEntry("Clear", CLEAR);

	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	window = glutCreateWindow("Bresenham's Drawing");
	myInit();
	createGLUTMenus();
	glutMenuStatusFunc(statusfunc);
	//glutReshapeFunc(changeSize);
	glutMouseFunc(mouseButton); // mouse func
	glutDisplayFunc(myDisplay);
	glutMainLoop();
	return (1);
}