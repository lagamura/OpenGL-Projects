#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> //you need this for linux!

#define maxHt 700
#define maxVer 10

void draw_line(int x1, int x2, int y_1, int y2);

int window;
int i;
int numofclicks = 0;
int menuflag = -1;
int Xarray[10];
int Yarray[10];
int EdgesIntersection[10][maxHt];
int num_of_edges = 0;

bool popup_flag;

// Menu items
enum MENU_TYPE
{
	POLYGON,
	FILLING,
	CLEAR,
};

typedef struct edgebucket
{
	int ymax;	   //max y-coordinate of edge
	float xofymin; //x-coordinate of lowest edge point updated only in aet
	float slopeinverse;

} EdgeBucket;

typedef struct edgetabletup
{
	// the array will give the scanline number
	// The edge table (ET) with edges entries sorted
	// in increasing y and x of the lower end

	int countEdgeBucket; //no. of edgebuckets
	EdgeBucket buckets[maxVer];

} EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;

void initEdgeTable()
{
	int i;
	for (i = 0; i < maxHt; i++)
	{
		EdgeTable[i].countEdgeBucket = 0;
	}

	ActiveEdgeTuple.countEdgeBucket = 0;
}
void printTuple(EdgeTableTuple *tup)
{
	int j;

	if (tup->countEdgeBucket)
		printf("\nCount %d-----\n", tup->countEdgeBucket);

	for (j = 0; j < tup->countEdgeBucket; j++)
	{
		printf(" %d+%.2f+%.2f",
			   tup->buckets[j].ymax, tup->buckets[j].xofymin, tup->buckets[j].slopeinverse);
	}
}
void printTable()
{
	int i, j;

	for (i = 0; i < maxHt; i++)
	{
		if (EdgeTable[i].countEdgeBucket)
			printf("\nScanline %d", i);

		printTuple(&EdgeTable[i]);
	}
}

void myInit()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 700, 0, 700);

	for (int i = 0; i < 10; i++)
	{
		Xarray[i] = -1;
		Yarray[i] = -1;
		num_of_edges = 0;
	}
	initEdgeTable();
}

void insertionSort(EdgeTableTuple *ett)
{
	int i, j;
	EdgeBucket temp;

	for (i = 1; i < ett->countEdgeBucket; i++)
	{
		temp.ymax = ett->buckets[i].ymax;
		temp.xofymin = ett->buckets[i].xofymin;
		temp.slopeinverse = ett->buckets[i].slopeinverse;
		j = i - 1;

		while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0))
		{
			ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
			ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
			ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
			j = j - 1;
		}
		ett->buckets[j + 1].ymax = temp.ymax;
		ett->buckets[j + 1].xofymin = temp.xofymin;
		ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
	}
}

void storeEdgeInTuple(EdgeTableTuple *receiver, int ym, int xm, float slopInv)
{
	// both used for edgetable and active edge table..
	// The edge tuple sorted in increasing ymax and x of the lower end.
	(receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
	(receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
	(receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;

	// sort the buckets
	insertionSort(receiver);

	(receiver->countEdgeBucket)++;
}

void storeEdgeInTable(int x1, int y1, int x2, int y2)
{
	float m, minv;
	int ymaxTS, xwithyminTS, scanline; //ts stands for to store

	if (x2 == x1)
	{
		minv = 0.000000;
	}
	else
	{
		m = ((float)(y2 - y1)) / ((float)(x2 - x1));

		// horizontal lines are not stored in edge table
		if (y2 == y1)
			return;

		minv = (float)1.0 / m;
		printf("\nSlope string for %d %d & %d %d: %f", x1, y1, x2, y2, minv);
	}

	if (y1 > y2)
	{
		scanline = y2;
		ymaxTS = y1;
		xwithyminTS = x2;
	}
	else
	{
		scanline = y1;
		ymaxTS = y2;
		xwithyminTS = x1;
	}
	// the assignment part is done..now storage..
	storeEdgeInTuple(&EdgeTable[scanline], ymaxTS, xwithyminTS, minv);
}

void removeEdgeByYmax(EdgeTableTuple *Tup, int yy)
{
	int i, j;
	for (i = 0; i < Tup->countEdgeBucket; i++)
	{
		if (Tup->buckets[i].ymax == yy)
		{
			printf("\nRemoved at %d", yy);

			for (j = i; j < Tup->countEdgeBucket - 1; j++)
			{
				Tup->buckets[j].ymax = Tup->buckets[j + 1].ymax;
				Tup->buckets[j].xofymin = Tup->buckets[j + 1].xofymin;
				Tup->buckets[j].slopeinverse = Tup->buckets[j + 1].slopeinverse;
			}
			Tup->countEdgeBucket--;
			i--;
		}
	}
}

void updatexbyslopeinv(EdgeTableTuple *Tup)
{
	int i;

	for (i = 0; i < Tup->countEdgeBucket; i++)
	{
		(Tup->buckets[i]).xofymin = (Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;
	}
}

void ScanlineFill()
{
	/* Follow the following rules:
	1. Horizontal edges: Do not include in edge table
	2. Horizontal edges: Drawn either on the bottom or on the top.
	3. Vertices: If local max or min, then count twice, else count
		once.
	4. Either vertices at local minima or at local maxima are drawn.*/

	int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
		
	// we will start from scanline 0;
	// Repeat until last scanline:
	for (i = 0; i < maxHt; i++) //4. Increment y by 1 (next scan line)
	{

		// 1. Move from ET bucket y to the
		// AET those edges whose ymin = y (entering edges)
		for (j = 0; j < EdgeTable[i].countEdgeBucket; j++)
		{
			storeEdgeInTuple(&ActiveEdgeTuple, EdgeTable[i].buckets[j].ymax, EdgeTable[i].buckets[j].xofymin,
							 EdgeTable[i].buckets[j].slopeinverse);
			
			//printf("Your edgeStoredinTuple is:",&ActiveEdgeTuple);
			//scanf("Pause to watch storeEdgeTuple\n");
		}

		// 2. Remove from AET those edges for
		// which y=ymax (not involved in next scan line)
		removeEdgeByYmax(&ActiveEdgeTuple, i);

		//sort AET (remember: ET is presorted)
		insertionSort(&ActiveEdgeTuple);

		//3. Fill lines on scan line y by using pairs of x-coords from AET
		j = 0;
		FillFlag = 0;
		coordCount = 0;
		x1 = 0;
		x2 = 0;
		ymax1 = 0;
		ymax2 = 0;
		while (j < ActiveEdgeTuple.countEdgeBucket)
		{
			if (coordCount % 2 == 0)
			{
				x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
				ymax1 = ActiveEdgeTuple.buckets[j].ymax;
				if (x1 == x2)
				{
					/* three cases can arrive-
					1. lines are towards top of the intersection
					2. lines are towards bottom
					3. one line is towards top and other is towards bottom
				*/
					if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
					{
						x2 = x1;
						ymax2 = ymax1;
					}

					else
					{
						coordCount++;
					}
				}

				else
				{
					coordCount++;
				}
			}
			else
			{
				x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
				ymax2 = ActiveEdgeTuple.buckets[j].ymax;

				FillFlag = 0;

				// checking for intersection...
				if (x1 == x2)
				{
					/*three cases can arrive-
					1. lines are towards top of the intersection
					2. lines are towards bottom
					3. one line is towards top and other is towards bottom
				*/
					if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
					{
						x1 = x2;
						ymax1 = ymax2;
					}
					else
					{
						coordCount++;
						FillFlag = 1;
					}
				}
				else
				{
					coordCount++;
					FillFlag = 1;
				}

				if (FillFlag)
				{
					//drawing actual lines...
					glColor3f(0.0f, 0.7f, 0.0f);

					glBegin(GL_LINES);
					glVertex2i(x1, i);
					glVertex2i(x2, i);
					glEnd();
					glFlush();

					// printf("\nLine drawn from %d,%d to %d,%d",x1,i,x2,i);
				}
			}

			j++;
		}

		// 5. For each nonvertical edge remaining in AET, update x for new y
		updatexbyslopeinv(&ActiveEdgeTuple);
	}

	printf("\nScanline filling complete\n");
}

void draw_pixel(int x, int y)
{
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

/* https://www.cpp.edu/~raheja/CS445/MEA.pdf */

void myDisplay()
{
	if (menuflag == POLYGON)
	{
		i = 0;
		while (Xarray[i] != -1)
		{
			draw_pixel(Xarray[i], Yarray[i]);
			i++;
		}
		for (i = 1; i < 10; i++)
		{
			if ( Xarray[i] != -1)
			{
				draw_line(Xarray[i], Xarray[i - 1], Yarray[i], Yarray[i - 1]);
			}
		}
	}
	else if (menuflag == FILLING && !popup_flag)
	{
		for (i = 1; i < 10; i++)
		{

			if ( Xarray[i] != -1)
			{
				storeEdgeInTable(Xarray[i-1], Yarray[i-1], Xarray[i], Yarray[i]);
			}
			else
			{
				Xarray[i] = Xarray[0];
				Yarray[i] = Yarray[0];
				storeEdgeInTable(Xarray[i-1], Yarray[i-1], Xarray[0], Yarray[0]);
				draw_line(Xarray[i-1], Xarray[0], Yarray[i-1], Yarray[0]);
				break;
			}
		}
		ScanlineFill();
		printf("Finished ScanlineFIll");
	}
	else if (menuflag == CLEAR)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean the screen and the depth buffer
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glColor3f(0.5f, 0.5f, 0.3f);
		glFlush();
		initEdgeTable();
		num_of_edges = 0;
		for (i=0; i<10; i++){
			Xarray[i]=-1;
			Yarray[i]=-1;
		}
													
	}

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
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && popup_flag == 0)
		if (num_of_edges < 9)
		{
			if (menuflag == POLYGON)
			{

				printf("numofedges =%d \n", num_of_edges);

				for (int i = 0; i < 10; i++)
				{
					if (Xarray[i] == -1)
					{
						Xarray[i] = x;
						Yarray[i] = (700-y);
						break;
					}
				}
				num_of_edges++;

				glutPostRedisplay();
			}
		}
		else
		{
			printf("Max edges number is 10! \n");
		}
}

void menuhandler(int item)
{

	menuflag = item;

	switch (item)
	{

	case POLYGON:
		printf("I choose line \n");
		num_of_edges = 0;
		break;

	case FILLING:
		printf("I choose Filling the Polygon \n");
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
	glutAddMenuEntry("Polygon Vertexes Input (max 10)", POLYGON);
	glutAddMenuEntry("Draw Inner", FILLING);
	glutAddMenuEntry("Clear", CLEAR);

	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* ---------------------- Scanline algo ---------------------------- */

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