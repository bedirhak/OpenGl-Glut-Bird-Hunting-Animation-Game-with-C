/*********
   CTIS164 - Homework 2: Bird hunting game
----------
STUDENT : Bedirhan KARA
SECTION : 3
HOMEWORK: 2
----------
PROBLEMS: Nothing
----------
ADDITIONAL FEATURES:You can see your score and you have time decreasing. Game will start with 3 live, 15 second and 5 bullet.When you hit one bird you will get 1.5 second extra and 1 more bullet.
					If your time is up or yor bullets over you will see a choice, you can use your live with pressing 'c' or 'C'. If you use your live, you will get 15 second and 5 bullet more.
					Also you can restart your game when stop. If your time or bullets are finish and you dont have live, game is over. When game over you can restart again. The stars sembolize
					your lives in the upper left side of the screen. Additionally, if your time is less than 5 second, you will see your score and time with red color.

*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 790

#define TIMER_PERIOD  10 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

int xB = 0, yB = 0;// to make draw functions move

int currentPosOfX, currentPosOfY; // for pos of mouse

int live = 3; // this is for displaying live stars

float timeRemain = 15; // counting time 

int bulletFired = 0, // bullets that you fire
score = 0,		 // birds that you hunt
numOfBullet = 5; // your ammunition

bool t = true,
restart = false,
decrease = false,
reset = true,
cont = true;

typedef struct {
	float gunX = -640, gunY = 0;
	float bulletX = -500, bulletY = 25;
	bool fire;
}gun_t;


gun_t gun;

typedef struct {
	float posX, posY;
	float r, g, b;
	bool active = false;
}targets_t;

targets_t birds[7];


bool doesHit(int cx, int cy, int px, int py, int radius) {
	float distance = sqrt((px - cx)*(px - cx) + (py - cy) * (py - cy)); // formul of distance between 2 point

	if (distance < radius) // if distance between bullet and center of the target is less than radius that means bullet hit
		return true;
	else
		return false;
}


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void throwPart(int xBB, int yBB)
{
	glBegin(GL_QUADS);
	glColor3f(0.976, 0.984, 0.015);
	glVertex2f(51 + xBB, -24 + yBB);
	glVertex2f(47 + xBB, -22 + yBB);
	glVertex2f(47 + xBB, -28 + yBB);
	glVertex2f(51 + xBB, -26 + yBB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.976, 0.984, 0.015);
	glVertex2f(43 + xBB, -22 + yBB);
	glVertex2f(47 + xBB, -22 + yBB);
	glVertex2f(47 + xBB, -28 + yBB);
	glVertex2f(43 + xBB, -28 + yBB);
	glEnd();

}

void drawBullet(int xB, int yB)
{
	glBegin(GL_QUADS);
	glColor3f(0.93, 0.4038, 0.0279);
	glVertex2f(2 + xB, -21 + yB);
	glVertex2f(35 + xB, -21 + yB);
	glVertex2f(35 + xB, -29 + yB);
	glVertex2f(2 + xB, -29 + yB);
	glEnd();

	glLineWidth(2.2);
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(7 + xB, -21 + yB);
	glVertex2f(7 + xB, -22 + yB);
	glEnd();

	glLineWidth(2.2);
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(7 + xB, -28 + yB);
	glVertex2f(7 + xB, -29 + yB);
	glEnd();

	glLineWidth(1);
	glColor3f(0.392, 0.231, 0.125);
	glBegin(GL_LINES);
	glVertex2f(7 + xB, -22 + yB);
	glVertex2f(7 + xB, -28 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.93, 0.4038, 0.0279);
	glVertex2f(43 + xB, -22 + yB);
	glVertex2f(35 + xB, -21 + yB);
	glVertex2f(35 + xB, -29 + yB);
	glVertex2f(43 + xB, -28 + yB);
	glEnd();


	glColor3f(0.976, 0.984, 0.015);
	circle(34 + xB, -24 + yB, 1);

	glColor3f(0.976, 0.984, 0.015);
	circle(15 + xB, -24 + yB, 1);

	glBegin(GL_QUADS);
	glColor3f(0.976, 0.984, 0.015);
	glVertex2f(33 + xB, -23 + yB);
	glVertex2f(16 + xB, -23 + yB);
	glVertex2f(16 + xB, -25 + yB);
	glVertex2f(33 + xB, -25 + yB);
	glEnd();

	// throwing part of bullet

	glBegin(GL_QUADS);
	glColor3f(0.976, 0.984, 0.015);
	glVertex2f(51 + xB, -24 + yB);
	glVertex2f(47 + xB, -22 + yB);
	glVertex2f(47 + xB, -28 + yB);
	glVertex2f(51 + xB, -26 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.976, 0.984, 0.015);
	glVertex2f(43 + xB, -22 + yB);
	glVertex2f(47 + xB, -22 + yB);
	glVertex2f(47 + xB, -28 + yB);
	glVertex2f(43 + xB, -28 + yB);
	glEnd();

	glLineWidth(1);
	glColor3f(0.392, 0.231, 0.125);
	glBegin(GL_LINES);
	glVertex2f(44 + xB, -22 + yB);
	glVertex2f(44 + xB, -28 + yB);
	glEnd();

}

void displayGun(int xB, int yB)
{

	//Main body of gun

	// back part of main
	glColor3f(0.266, 0.219, 0.050);
	circle(-40 + xB, 0 + yB, 10);

	glColor3f(0.266, 0.219, 0.050);
	circle(-40 + xB, -10 + yB, 10);

	glBegin(GL_QUADS);
	glColor3f(0.266, 0.219, 0.050);
	glVertex2f(-40 + xB, 0 + yB);
	glVertex2f(-50 + xB, 0 + yB);
	glVertex2f(-50 + xB, -10 + yB);
	glVertex2f(-40 + xB, -10 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.266, 0.219, 0.050);
	glVertex2f(-40 + xB, 10 + yB);
	glVertex2f(-20 + xB, 10 + yB);
	glVertex2f(-20 + xB, -20 + yB);
	glVertex2f(-40 + xB, -20 + yB);
	glEnd();

	//top of big part

	glBegin(GL_QUADS);
	glColor3f(0.129, 0.129, 0.129);
	glVertex2f(50 + xB, 20 + yB);
	glVertex2f(-19 + xB, 20 + yB);
	glVertex2f(-19 + xB, 35 + yB);
	glVertex2f(50 + xB, 35 + yB);
	glEnd();

	// big part of main 
	glBegin(GL_QUADS);
	glColor3f(0.329, 0.309, 0.309);
	glVertex2f(80 + xB, 30 + yB);
	glVertex2f(0 + xB, 30 + yB);
	glVertex2f(0 + xB, -30 + yB);
	glVertex2f(80 + xB, -30 + yB);
	glEnd();

	glColor3f(0.329, 0.309, 0.309);
	circle(0 + xB, 0 + yB, 30);

	glBegin(GL_QUADS);
	glColor3f(0.329, 0.309, 0.309);
	glVertex2f(-30 + xB, 0 + yB);
	glVertex2f(0 + xB, 0 + yB);
	glVertex2f(0 + xB, -30 + yB);
	glVertex2f(-30 + xB, -30 + yB);
	glEnd();

	//magazine (clips) part of gun

	glBegin(GL_QUADS);
	glColor3f(0.627, 0.619, 0.611);
	glVertex2f(-10 + xB, -10 + yB);
	glVertex2f(60 + xB, -10 + yB);
	glVertex2f(60 + xB, -35 + yB);
	glVertex2f(-10 + xB, -35 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	glVertex2f(0 + xB, -15 + yB);
	glVertex2f(54 + xB, -15 + yB);
	glVertex2f(54 + xB, -30 + yB);
	glVertex2f(0 + xB, -30 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.87, 0.7539, 0.0957);
	glVertex2f(10 + xB, -95 + yB);
	glVertex2f(40 + xB, -95 + yB);
	glVertex2f(40 + xB, -18 + yB);
	glVertex2f(10 + xB, -18 + yB);
	glEnd();



	// handle of the gun

	glBegin(GL_QUADS);
	glColor3f(0.823, 0.811, 0.776);
	glVertex2f(65 + xB, 30 + yB);
	glVertex2f(75 + xB, 30 + yB);
	glVertex2f(75 + xB, 70 + yB);
	glVertex2f(65 + xB, 70 + yB);
	glEnd();

	glColor3f(0.823, 0.811, 0.776);
	circle(70 + xB, 70 + yB, 5);

	glBegin(GL_QUADS);
	glColor3f(0.823, 0.811, 0.776);
	glVertex2f(40 + xB, 75 + yB);
	glVertex2f(70 + xB, 75 + yB);
	glVertex2f(70 + xB, 65 + yB);
	glVertex2f(40 + xB, 65 + yB);
	glEnd();

	glColor3f(0.266, 0.219, 0.050);
	circle(40 + xB, 70 + yB, 10);

	glBegin(GL_QUADS);
	glColor3f(0.266, 0.219, 0.050);
	glVertex2f(40 + xB, 60 + yB);
	glVertex2f(20 + xB, 60 + yB);
	glVertex2f(20 + xB, 80 + yB);
	glVertex2f(40 + xB, 80 + yB);
	glEnd();

	glColor3f(0.450, 0.360, 0.047);
	circle(20 + xB, 70 + yB, 10);

	// gun barrels

	glBegin(GL_QUADS);
	glColor3f(0.823, 0.811, 0.776);
	glVertex2f(185 + xB, 15 + yB);
	glVertex2f(80 + xB, 15 + yB);
	glVertex2f(80 + xB, 25 + yB);
	glVertex2f(185 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.823, 0.811, 0.776);
	glVertex2f(185 + xB, 5 + yB);
	glVertex2f(80 + xB, 5 + yB);
	glVertex2f(80 + xB, -5 + yB);
	glVertex2f(185 + xB, -5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.823, 0.811, 0.776);
	glVertex2f(185 + xB, -15 + yB);
	glVertex2f(80 + xB, -15 + yB);
	glVertex2f(80 + xB, -25 + yB);
	glVertex2f(185 + xB, -25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.411, 0.407, 0.388);
	glVertex2f(100 + xB, -30 + yB);
	glVertex2f(110 + xB, -30 + yB);
	glVertex2f(110 + xB, 30 + yB);
	glVertex2f(100 + xB, 30 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.411, 0.407, 0.388);
	glVertex2f(150 + xB, -30 + yB);
	glVertex2f(160 + xB, -30 + yB);
	glVertex2f(160 + xB, 30 + yB);
	glVertex2f(150 + xB, 30 + yB);
	glEnd();

	// Bullets with repeated function

	drawBullet(0 + xB, 1 + yB);
	drawBullet(0 + xB, -11 + yB);
	drawBullet(0 + xB, -23 + yB);
	drawBullet(0 + xB, -35 + yB);
	drawBullet(0 + xB, -47 + yB);
	drawBullet(0 + xB, -59 + yB);
	drawBullet(0 + xB, -71 + yB);
	//	drawBullet(0 + xB, -82 + yB);

}

void drawLives(int xB, int yB) // really small star
{
	for (int k = 1; k <= live; k++)
	{
		glColor3f(0.941, 0.960, 0.196);
		glBegin(GL_TRIANGLES);
		glVertex2f(-643 + xB + k * 50, 346 + yB);
		glVertex2f(-638 + xB + k * 50, 350 + yB);
		glVertex2f(-638 + xB + k * 50, 359 + yB);
		glEnd();

		glBegin(GL_TRIANGLES);
		glVertex2f(-646 + xB + k * 50, 353 + yB);
		glVertex2f(-638 + xB + k * 50, 350 + yB);
		glVertex2f(-630 + xB + k * 50, 353 + yB);
		glEnd();

		glBegin(GL_TRIANGLES);
		glVertex2f(-633 + xB + k * 50, 346 + yB);
		glVertex2f(-638 + xB + k * 50, 350 + yB);
		glVertex2f(-638 + xB + k * 50, 359 + yB);
		glEnd();
	}
}

void drawTarget(int xB, int yB, int r, int g, int b)
{
	glColor3ub(r, g, b);

	glBegin(GL_QUADS);
	glVertex2f(-5 + xB, -5 + yB);
	glVertex2f(5 + xB, -5 + yB);
	glVertex2f(5 + xB, 10 + yB);
	glVertex2f(-5 + xB, 10 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, 5 + yB);
	glVertex2f(5 + xB, 5 + yB);
	glVertex2f(5 + xB, 0 + yB);
	glVertex2f(10 + xB, 0 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(0 + xB, -5 + yB);
	glVertex2f(-10 + xB, -5 + yB);
	glVertex2f(-10 + xB, -20 + yB);
	glVertex2f(0 + xB, -20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(0 + xB, -10 + yB);
	glVertex2f(5 + xB, -10 + yB);
	glVertex2f(5 + xB, -20 + yB);
	glVertex2f(0 + xB, -20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, -15 + yB);
	glVertex2f(5 + xB, -15 + yB);
	glVertex2f(5 + xB, -20 + yB);
	glVertex2f(10 + xB, -20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, -15 + yB);
	glVertex2f(-15 + xB, -15 + yB);
	glVertex2f(-15 + xB, -10 + yB);
	glVertex2f(-10 + xB, -10 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(0 + xB, 5 + yB);
	glVertex2f(-10 + xB, 5 + yB);
	glVertex2f(-10 + xB, 30 + yB);
	glVertex2f(0 + xB, 30 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(0 + xB, 25 + yB);
	glVertex2f(5 + xB, 25 + yB);
	glVertex2f(5 + xB, 30 + yB);
	glVertex2f(0 + xB, 30 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, 25 + yB);
	glVertex2f(-15 + xB, 25 + yB);
	glVertex2f(-15 + xB, 15 + yB);
	glVertex2f(-10 + xB, 15 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-20 + xB, 25 + yB);
	glVertex2f(-15 + xB, 25 + yB);
	glVertex2f(-15 + xB, 20 + yB);
	glVertex2f(-20 + xB, 20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, 5 + yB);
	glVertex2f(-15 + xB, 5 + yB);
	glVertex2f(-15 + xB, -5 + yB);
	glVertex2f(-10 + xB, -5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-35 + xB, 15 + yB);
	glVertex2f(-15 + xB, 15 + yB);
	glVertex2f(-15 + xB, -5 + yB);
	glVertex2f(-35 + xB, -5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-30 + xB, -10 + yB);
	glVertex2f(-15 + xB, -10 + yB);
	glVertex2f(-15 + xB, -5 + yB);
	glVertex2f(-30 + xB, -5 + yB);
	glEnd();
	//end of the body part

	//sides
	//glColor3f(0.803, 0.054, 0.654);
	glColor3ub(1, 1, 1);

	glBegin(GL_QUADS);
	glVertex2f(0 + xB, -10 + yB);
	glVertex2f(5 + xB, -10 + yB);
	glVertex2f(5 + xB, -5 + yB);
	glVertex2f(0 + xB, -5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, 0 + yB);
	glVertex2f(5 + xB, 0 + yB);
	glVertex2f(5 + xB, -5 + yB);
	glVertex2f(10 + xB, -5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, -15 + yB);
	glVertex2f(5 + xB, -15 + yB);
	glVertex2f(5 + xB, -10 + yB);
	glVertex2f(10 + xB, -10 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(40 + xB, -5 + yB);
	glVertex2f(35 + xB, -5 + yB);
	glVertex2f(35 + xB, 0 + yB);
	glVertex2f(40 + xB, 0 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, -25 + yB);
	glVertex2f(-10 + xB, -25 + yB);
	glVertex2f(-10 + xB, -20 + yB);
	glVertex2f(10 + xB, -20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, -15 + yB);
	glVertex2f(35 + xB, -15 + yB);
	glVertex2f(35 + xB, -20 + yB);
	glVertex2f(10 + xB, -20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(30 + xB, -15 + yB);
	glVertex2f(35 + xB, -15 + yB);
	glVertex2f(35 + xB, -5 + yB);
	glVertex2f(30 + xB, -5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(30 + xB, -10 + yB);
	glVertex2f(10 + xB, -10 + yB);
	glVertex2f(10 + xB, -5 + yB);
	glVertex2f(30 + xB, -5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(35 + xB, 0 + yB);
	glVertex2f(10 + xB, 0 + yB);
	glVertex2f(10 + xB, 5 + yB);
	glVertex2f(35 + xB, 5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, 30 + yB);
	glVertex2f(5 + xB, 30 + yB);
	glVertex2f(5 + xB, 25 + yB);
	glVertex2f(10 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, 10 + yB);
	glVertex2f(5 + xB, 10 + yB);
	glVertex2f(5 + xB, 5 + yB);
	glVertex2f(10 + xB, 5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(20 + xB, 30 + yB);
	glVertex2f(15 + xB, 30 + yB);
	glVertex2f(15 + xB, 25 + yB);
	glVertex2f(20 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(20 + xB, 20 + yB);
	glVertex2f(25 + xB, 20 + yB);
	glVertex2f(25 + xB, 25 + yB);
	glVertex2f(20 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(0 + xB, 10 + yB);
	glVertex2f(5 + xB, 10 + yB);
	glVertex2f(5 + xB, 25 + yB);
	glVertex2f(0 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(20 + xB, 10 + yB);
	glVertex2f(15 + xB, 10 + yB);
	glVertex2f(15 + xB, 20 + yB);
	glVertex2f(20 + xB, 20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(30 + xB, 5 + yB);
	glVertex2f(25 + xB, 5 + yB);
	glVertex2f(25 + xB, 20 + yB);
	glVertex2f(30 + xB, 20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, 35 + yB);
	glVertex2f(15 + xB, 35 + yB);
	glVertex2f(15 + xB, 30 + yB);
	glVertex2f(-10 + xB, 30 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, -5 + yB);
	glVertex2f(-5 + xB, -5 + yB);
	glVertex2f(-5 + xB, 5 + yB);
	glVertex2f(-10 + xB, 5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, -5 + yB);
	glVertex2f(-15 + xB, -5 + yB);
	glVertex2f(-15 + xB, -10 + yB);
	glVertex2f(-10 + xB, -10 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, 5 + yB);
	glVertex2f(-15 + xB, 5 + yB);
	glVertex2f(-15 + xB, 15 + yB);
	glVertex2f(-10 + xB, 15 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, -15 + yB);
	glVertex2f(-15 + xB, -15 + yB);
	glVertex2f(-15 + xB, -20 + yB);
	glVertex2f(-10 + xB, -20 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-30 + xB, -15 + yB);
	glVertex2f(-15 + xB, -15 + yB);
	glVertex2f(-15 + xB, -10 + yB);
	glVertex2f(-30 + xB, -10 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-30 + xB, -5 + yB);
	glVertex2f(-35 + xB, -5 + yB);
	glVertex2f(-35 + xB, -10 + yB);
	glVertex2f(-30 + xB, -10 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-40 + xB, -5 + yB);
	glVertex2f(-35 + xB, -5 + yB);
	glVertex2f(-35 + xB, 15 + yB);
	glVertex2f(-40 + xB, 15 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-15 + xB, 20 + yB);
	glVertex2f(-35 + xB, 20 + yB);
	glVertex2f(-35 + xB, 15 + yB);
	glVertex2f(-15 + xB, 15 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-20 + xB, 20 + yB);
	glVertex2f(-25 + xB, 20 + yB);
	glVertex2f(-25 + xB, 25 + yB);
	glVertex2f(-20 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-20 + xB, 20 + yB);
	glVertex2f(-25 + xB, 20 + yB);
	glVertex2f(-25 + xB, 25 + yB);
	glVertex2f(-20 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(-10 + xB, 30 + yB);
	glVertex2f(-20 + xB, 30 + yB);
	glVertex2f(-20 + xB, 25 + yB);
	glVertex2f(-10 + xB, 25 + yB);
	glEnd();
	// end of sides

	// mouth
	glColor3f(0.960, 0.513, 0.180);

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, 0 + yB);
	glVertex2f(35 + xB, 0 + yB);
	glVertex2f(35 + xB, -5 + yB);
	glVertex2f(10 + xB, -5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, -10 + yB);
	glVertex2f(5 + xB, -10 + yB);
	glVertex2f(5 + xB, -5 + yB);
	glVertex2f(10 + xB, -5 + yB);
	glEnd();


	glBegin(GL_QUADS);
	glVertex2f(10 + xB, -10 + yB);
	glVertex2f(30 + xB, -10 + yB);
	glVertex2f(30 + xB, -15 + yB);
	glVertex2f(10 + xB, -15 + yB);
	glEnd();
	// end of mouth 

	// eye
	glColor3f(0.054, 0.094, 0.803);

	glBegin(GL_QUADS);
	glVertex2f(15 + xB, 10 + yB);
	glVertex2f(5 + xB, 10 + yB);
	glVertex2f(5 + xB, 25 + yB);
	glVertex2f(15 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, 30 + yB);
	glVertex2f(15 + xB, 30 + yB);
	glVertex2f(15 + xB, 25 + yB);
	glVertex2f(10 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(15 + xB, 20 + yB);
	glVertex2f(20 + xB, 20 + yB);
	glVertex2f(20 + xB, 25 + yB);
	glVertex2f(15 + xB, 25 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(10 + xB, 10 + yB);
	glVertex2f(25 + xB, 10 + yB);
	glVertex2f(25 + xB, 5 + yB);
	glVertex2f(10 + xB, 5 + yB);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(20 + xB, 10 + yB);
	glVertex2f(25 + xB, 10 + yB);
	glVertex2f(25 + xB, 20 + yB);
	glVertex2f(20 + xB, 20 + yB);
	glEnd();
}

void drawBackground()
{
	for (int i = 0; i < 1500; i += 100) //YATAY
	{
		
		for (int k = -100; k < 800; k += 100)
		{
			glBegin(GL_QUADS);
			glColor4f(0.501, 0.435, 0.435, 1);
			glVertex2f(-701 + i, -34 + k);
			glColor4f(0.760, 0.752, 0.658, 1);
			glVertex2f(-740 + i, -86 + k);
			glColor4f(0.074, 0.003, 0.003, 1);
			glVertex2f(-701 + i, -134 + k);
			glColor4f(0.266, 0.152, 0.152, 1);
			glVertex2f(-662 + i, -86 + k);
			glEnd();

			glBegin(GL_QUADS);
			glColor4f(0.498, 0.447, 0.019, 1);
			glVertex2f(-651 + i, -214 + k);
			glColor4f(0.278, 0.043, 0.152, 1);
			glVertex2f(-701 + i, -134 + k);
			glColor4f(0.117, 0.278, 0.043, 1);
			glVertex2f(-651 + i, -64 + k);
			glColor4f(0.043, 0.254, 0.278, 1);
			glVertex2f(-601 + i, -134 + k);
			glEnd();
		}
	}

	for (int i = 0; i < 1400; i += 4) //ALT TARAF
	{
		glBegin(GL_QUADS);
		glColor4f(0.458, 0.262, 0.325, 1);
		glVertex2f(-700 + i, -400);
		glColor4f(0.043, 0.031, 0.047, 1);
		glVertex2f(-700 + i, -325);
		glColor4f(0.168, 0.749, 0.749, 1);
		glVertex2f(-633 + i, -342);
		glColor4f(0.094, 0.945, 0.949, 1);
		glVertex2f(-652 + i, -318);
		glEnd();

		glBegin(GL_QUADS);
		glColor4f(0.043, 0.031, 0.047, 0.8);
		glVertex2f(-633 + i, -342);
		glColor4f(1, 0, 0, 0.8);
		glVertex2f(-652 + i, -318);
		glColor4f(0.18, 0.49, 0.949, 0.8);
		glVertex2f(-600 + i, -292);
		glColor4f(1, 0.697, 0.439, 0.8);
		glVertex2f(-452 + i, -218);
		glEnd();
	}

	for (int i = 0; i < 800; i += 4)
	{
		glBegin(GL_QUADS);
		glColor4f(0.458, 0.262, 0.325, 0.8);
		glVertex2f(-700, -400 + i);
		glColor4f(0.043, 0.031, 0.047, 0.8);
		glVertex2f(-700, -325 + i);
		glColor4f(0.168, 0.749, 0.749, 0.8);
		glVertex2f(-633, -342 + i);
		glColor4f(1, 0, 0, 0.8);
		glVertex2f(-652, -318 + i);
		glEnd();

		glBegin(GL_QUADS);
		glColor4f(0.043, 0.031, 0.047, 0.8);
		glVertex2f(-633, -342 + i);
		glColor4f(1, 0, 0, 0.8);
		glVertex2f(-652, -318 + i);
		glColor4f(0.18, 0.49, 0.949, 0.8);
		glVertex2f(-600, -292 + i);
		glColor4f(1, 0.697, 0.439, 0.8);
		glVertex2f(-452, -218 + i);
		glEnd();
	}
}

void startThrowBirds(int i) // for initialize all array elements of the targets when they inactive (fired or pass the upper border)
{
	birds[i].active = true;
	birds[i].posX = rand() % ((670) - 300) + 300; // down side of the screen 
	birds[i].posY = -400 - 90 * i; // for send birds different at different times
	birds[i].r = rand() % 254;  	//for random colors
	birds[i].g = rand() % 254;	//for random colors
	birds[i].b = rand() % 254;	//for random colors
}
//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	
	

	if (timeRemain > 0 && numOfBullet >= 0)
	{
		reset = false;
		cont = false;
		drawBackground();

		if (gun.fire)
		{
			throwPart(gun.bulletX, gun.bulletY);
		}

		if (gun.bulletX >= 700)
		{
			gun.fire = false;
			gun.bulletX = -500;
			gun.bulletY = gun.gunY + 25;
		}

		for (int k = 0; k < live; k++)
		{
			drawLives(-500 + k * 50, 350);
		}

		for (int k = 0; k < 7; k++)
		{
			if (birds[k].active)
			{
				drawTarget(birds[k].posX, birds[k].posY, birds[k].r,
					birds[k].g, birds[k].b);
			}

			if (doesHit(birds[k].posX, birds[k].posY + 15, gun.bulletX, gun.bulletY, 35))
			{
				birds[k].active = false;
				score++;
				numOfBullet++;
				timeRemain += 1.5;
			}
		}

		glBegin(GL_QUADS);
		glColor3f(0.725, 0.844, 0.362);
		glVertex2f(-35, 390);
		glColor4f(0.160, 0.839, 0.141, 1);
		glVertex2f(110, 390);
		glColor4f(0.192, 0.447, 0.447, 1);
		glVertex2f(110, 320);
		glColor4f(0.541, 0.066, 0.815, 1);
		glVertex2f(-35, 320);
		glEnd();

		vprint(-30, 365, GLUT_BITMAP_HELVETICA_18, "SCORE & TIME");
		
		if (timeRemain < 5)
			glColor3f(1, 0, 0);
		vprint(-1, 338, GLUT_BITMAP_TIMES_ROMAN_24, "%d       %0.1f", score, timeRemain);

		
		displayGun(gun.gunX, gun.gunY);
		glColor3f(1, 1, 1);
		vprint(gun.gunX + 15, gun.gunY, GLUT_BITMAP_TIMES_ROMAN_24, "%d", numOfBullet);
	}

	else if (live > 0)
	{
		reset = true;
		cont = true;
		glBegin(GL_QUADS);
		glColor4f(0, 0, 0, 0.5);
		glVertex2f(300, 150);
		glVertex2f(-100, 150);
		glVertex2f(-100, -20);
		glVertex2f(300, -20);
		glEnd();
		glColor4f(0.192, 0.447, 0.447, 1);
		if (timeRemain > 0)
			vprint(25, 115, GLUT_BITMAP_TIMES_ROMAN_24, "BULLETS OVER");
		else
			vprint(25, 115, GLUT_BITMAP_TIMES_ROMAN_24, "TIME IS UP");
		vprint(-60, 85, GLUT_BITMAP_HELVETICA_18, "PRESS 'R' TO RESTART THE GAME ");
		vprint(-45, 55, GLUT_BITMAP_HELVETICA_18, "YOU HAVE %d MORE CHANCE ", live);
		vprint(-32, 25, GLUT_BITMAP_HELVETICA_18, "PRESS 'C' TO CONTINUE");
		vprint(-42, -5, GLUT_BITMAP_HELVETICA_18, "SCORE: %d  WITH %d BULLETS", score, bulletFired);
	}

	else
	{
		reset = true;
		glBegin(GL_QUADS);
		glColor4f(0, 0, 0, 0.5);
		glVertex2f(270, 150);
		glVertex2f(-92, 150);
		glVertex2f(-92, -20);
		glVertex2f(270, -20);
		glEnd();
		glColor3f(1, 0, 0);
		vprint(30, 115, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
		vprint(-48, 58, GLUT_BITMAP_TIMES_ROMAN_24, "PRESS 'R' TO RESTART");
		vprint(-59, 10, GLUT_BITMAP_TIMES_ROMAN_24, "SCORE %d WITH %d BULLETS", score, bulletFired);
	}
	drawLives(10, 0);

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	if (key == 32)
	{
		gun.fire = true;
		reset = false;
		cont = false;

		if (gun.bulletX < -499 && timeRemain > 0)
		{
			numOfBullet--;
			bulletFired++;
		}
	}

	if (key == 82 && reset) //R
	{
		timeRemain = 15;
		score = 0;
		bulletFired = 0;
		restart = true;
		gun.gunX = -640;
		gun.gunY = 0;
		gun.bulletX = -500;
		gun.bulletY = 25;
		numOfBullet = 5;
		live = 3;
	}

	if (key == 114 && reset) //r
	{
		timeRemain = 15;
		score = 0;
		bulletFired = 0;
		restart = true;
		gun.gunX = -640;
		gun.gunY = 0;
		gun.bulletX = -500;
		gun.bulletY = 25;
		numOfBullet = 5;
		live = 3;
	}

	if (key == 67 && cont) // C
	{
		timeRemain += 15;
		numOfBullet += 6;
		live--;
	}
	if (key == 99 && cont) // c
	{
		timeRemain += 15;
		numOfBullet += 6;
		live--;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: if (gun.gunY <= 310)
	{
		// if you dont write this if check, bullet will move while going when you press up or down button
		if (!gun.fire)
		{
			gun.bulletY += 10;
		}
		gun.gunY += 10;

	} break;
	case GLUT_KEY_DOWN: if (gun.gunY >= -310)
	{
		// if you dont write this if check, bullet will move while going when you press up or down button
		if (!gun.fire)
		{
			gun.bulletY -= 10;
		}
		gun.gunY -= 10;

	} break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.


	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}
// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.

	currentPosOfX = x - winWidth / 2;
	currentPosOfY = winHeight / 2 - y;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (numOfBullet > 0 && timeRemain > -0.01)//to stop while asking restart or continue
		timeRemain -= 0.015;

	if (gun.fire) // to move bullet
	{
		gun.bulletX += 15;
	}

	for (int i = 0; i < 7; i++) // 
	{
		if (birds[i].active)
		{
			birds[i].posY += 2; // move birds up
			if (birds[i].posY >= 400) // when birds pass the max screen send them again with random x values
			{
				startThrowBirds(i);

			}

		}
		else//for start and send birds when they got hunted again
		{
			birds[i].active = true;
			startThrowBirds(i);
		}
	}

	if (restart)
	{
		for (int i = 0; i < 7; i++)
			startThrowBirds(i);
		restart = false;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()
}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Bird hunting game (by Bedirhan KARA)");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}