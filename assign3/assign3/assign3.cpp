/*
CSCI 480
Assignment 3 Raytracer

Name: <Your name here>
*/

#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <string>

#define MAX_TRIANGLES 2000
#define MAX_SPHERES 10
#define MAX_LIGHTS 10

char *filename=0;
float vector[3];
//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2
int mode=MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
#define WIDTH 320
#define HEIGHT 240

//the field of view of the camera
double fov = 60.0;

unsigned char buffer[HEIGHT][WIDTH][3];

struct Vertex
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double normal[3];
	double shininess;
};

typedef struct _Triangle
{
	struct Vertex v[3];
} Triangle;

typedef struct _Sphere
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double shininess;
	double radius;
} Sphere;

typedef struct _Light
{
	double position[3];
	double color[3];
} Light;

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];
double cameraPos[3] = {0,0,0};
int num_triangles=0;
int num_spheres=0;
int num_lights=0;

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);
double computeB(double *d1, double *d2){
	return 1;
}
double computeC(double *d1,double d2){
	return 1;
}
// dot product
double dot(double *d1, double *d2){
	return d1[0]*d2[0] + d1[1]*d2[1] + d1[2]*d2[2];
}
void normalize(double *d){
	double length = sqrt(abs(dot(d,d)));
	d[0] = d[0]/length;
	d[1] = d[1]/length;
	d[2] = d[2]/length;
}
//MODIFY THIS FUNCTION
void draw_scene()
{
	double x,y;
	double w,h;
	//simple output
	glClearColor(1.0,1.0,1.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	x = 2* ((double)WIDTH/HEIGHT )* tan(fov*3.14/(2*180));
	y = 2 * tan(fov*3.14/(2*180));
	w = (double)x/WIDTH;
	h = (double)y/HEIGHT;
	x = - ((double)WIDTH/HEIGHT) * tan(fov*3.14/(2*180));
	y = tan(fov*3.14/(2*180));
	//gluLookAt(cameraPos[0],cameraPos[1],cameraPos[2],spheres[0].position[0],spheres[0].position[1],spheres[0].position[2],0,1,0);
	for(int i=0; i<WIDTH; i++)
	{
		glPointSize(2.0);  
		glBegin(GL_POINTS);
		for(int j=0;j< HEIGHT;j++)
		{
			double ray[3] = {x+w*i,y-h*j,-1};
			normalize(ray);

			double b = 2 * (ray[0]*(cameraPos[0]-spheres[0].position
				[0])+ray[1]*(cameraPos[1]-spheres[0].position
				[1])+ray[2]*(cameraPos[2]-spheres[0].position
				[2]));
			double c = pow((double)(cameraPos[0]-spheres[0].position
				[0]),2) + pow((double)(cameraPos[1]-spheres[0].position
				[1]),2)+ pow((double)(cameraPos[2]-spheres[0].position
				[2]),2) - pow((double)spheres[0].radius,2);
			if((b*b-4*c)>=0){
				double t0 = (-b+sqrt(b*b-4*c))/2;
				double t1 = (-b-sqrt(b*b-4*c))/2;
				if(t0 >= 0 && t1>=0){
					double t = min(t0,t1);
					double origin[3] = {cameraPos[0] + t* ray[0],cameraPos[1] + t* ray[1],cameraPos[2] + t* ray[2]};
					ray[0] = lights[0].position[0] - cameraPos[0] + t* ray[0];
					ray[1] = lights[0].position[1] - cameraPos[1] + t* ray[1];
					ray[2] = lights[0].position[2] - cameraPos[2] + t* ray[2];
					normalize(ray);
					b =  2 * (ray[0]*(origin[0]-spheres[0].position
						[0])+ray[1]*(origin[1] -spheres[0].position
						[1])+ray[2]*(origin[2] -spheres[0].position
						[2]));
					c = pow((double)(origin[0]-spheres[0].position
						[0]),2) + pow((double)(origin[1]-spheres[0].position
						[1]),2)+ pow((double)(origin[2]-spheres[0].position
						[2]),2) - pow((double)spheres[0].radius,2);
					if((b*b - 4*c)>=0){
						t0 = (-b+sqrt(b*b-4*c))/2;
						t1 = (-b-sqrt(b*b-4*c))/2;
						if(t1 * t0 <=0.001){

							plot_pixel(i,j,255,0,0);
						}

					}

				}
				else if(t0>=0 && t1<=0){
					double t = t0;
					ray[0] = lights[0].position[0] - cameraPos[0] + t* ray[0];
					ray[1] = lights[0].position[1] - cameraPos[0] + t* ray[1];
					ray[2] = lights[0].position[2] - cameraPos[0] + t* ray[2];
					normalize(ray);
					b =  2 * (ray[0]*(lights[0].position[0]-spheres[0].position
						[0])+ray[1]*(lights[0].position[1]-spheres[0].position
						[1])+ray[2]*(lights[0].position[2]-spheres[0].position
						[2]));
					c = pow((double)(lights[0].position[0]-spheres[0].position
						[0]),2) + pow((double)(lights[0].position[1]-spheres[0].position
						[1]),2)+ pow((double)(lights[0].position[2]-spheres[0].position
						[2]),2) - pow((double)spheres[0].radius,2);
					if((int)(b*b - 4*c)==0)
						plot_pixel(i,j,255,0,0);
				}
				else if(t1>=0 && t0<=0){
					double t = t1;
					ray[0] = lights[0].position[0] - cameraPos[0] + t* ray[0];
					ray[1] = lights[0].position[1] - cameraPos[0] + t* ray[1];
					ray[2] = lights[0].position[2] - cameraPos[0] + t* ray[2];
					normalize(ray);
					b =  2 * (ray[0]*(lights[0].position[0]-spheres[0].position
						[0])+ray[1]*(lights[0].position[1]-spheres[0].position
						[1])+ray[2]*(lights[0].position[2]-spheres[0].position
						[2]));
					c = pow((double)(lights[0].position[0]-spheres[0].position
						[0]),2) + pow((double)(lights[0].position[1]-spheres[0].position
						[1]),2)+ pow((double)(lights[0].position[2]-spheres[0].position
						[2]),2) - pow((double)spheres[0].radius,2);
					if((int)(b*b - 4*c)==0)
						plot_pixel(i,j,255,0,0);
				}
			}

			//plot_pixel(x,y,255,255,255);
		}
		glEnd();
		glFlush();
	}
	printf("Done!\n"); fflush(stdout);
}

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
	glColor3f(((double)r)/256.f,((double)g)/256.f,((double)b)/256.f);
	glVertex2i(x,y);
}

void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
	buffer[HEIGHT-y-1][x][0]=r;
	buffer[HEIGHT-y-1][x][1]=g;
	buffer[HEIGHT-y-1][x][2]=b;
}

void plot_pixel(int x,int y,unsigned char r,unsigned char g, unsigned char b)
{
	plot_pixel_display(x,y,r,g,b);
	if(mode == MODE_JPEG)
		plot_pixel_jpeg(x,y,r,g,b);
}

void save_jpg()
{
	Pic *in = NULL;

	in = pic_alloc(640, 480, 3, NULL);
	printf("Saving JPEG file: %s\n", filename);

	memcpy(in->pix,buffer,3*WIDTH*HEIGHT);
	if (jpeg_write(filename, in))
		printf("File saved Successfully\n");
	else
		printf("Error in Saving\n");

	pic_free(in);      

}

void parse_check(char *expected,char *found)
{
	if(stricmp(expected,found))
	{
		char error[100];
		printf("Expected '%s ' found '%s '\n",expected,found);
		printf("Parse error, abnormal abortion\n");
		exit(0);
	}

}

void parse_doubles(FILE*file, char *check, double p[3])
{
	char str[100];
	fscanf(file,"%s",str);
	parse_check(check,str);
	fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
	printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE*file,double *r)
{
	char str[100];
	fscanf(file,"%s",str);
	parse_check("rad:",str);
	fscanf(file,"%lf",r);
	printf("rad: %f\n",*r);
}

void parse_shi(FILE*file,double *shi)
{
	char s[100];
	fscanf(file,"%s",s);
	parse_check("shi:",s);
	fscanf(file,"%lf",shi);
	printf("shi: %f\n",*shi);
}

int loadScene(char *argv)
{
	FILE *file = fopen(argv,"r");
	int number_of_objects;
	char type[50];
	int i;
	Triangle t;
	Sphere s;
	Light l;
	fscanf(file,"%i",&number_of_objects);

	printf("number of objects: %i\n",number_of_objects);
	char str[200];

	parse_doubles(file,"amb:",ambient_light);

	for(i=0;i < number_of_objects;i++)
	{
		fscanf(file,"%s\n",type);
		printf("%s\n",type);
		if(stricmp(type,"triangle")==0)
		{

			printf("found triangle\n");
			int j;

			for(j=0;j < 3;j++)
			{
				parse_doubles(file,"pos:",t.v[j].position);
				parse_doubles(file,"nor:",t.v[j].normal);
				parse_doubles(file,"dif:",t.v[j].color_diffuse);
				parse_doubles(file,"spe:",t.v[j].color_specular);
				parse_shi(file,&t.v[j].shininess);
			}

			if(num_triangles == MAX_TRIANGLES)
			{
				printf("too many triangles, you should increase MAX_TRIANGLES!\n");
				exit(0);
			}
			triangles[num_triangles++] = t;
		}
		else if(stricmp(type,"sphere")==0)
		{
			printf("found sphere\n");

			parse_doubles(file,"pos:",s.position);
			parse_rad(file,&s.radius);
			parse_doubles(file,"dif:",s.color_diffuse);
			parse_doubles(file,"spe:",s.color_specular);
			parse_shi(file,&s.shininess);

			if(num_spheres == MAX_SPHERES)
			{
				printf("too many spheres, you should increase MAX_SPHERES!\n");
				exit(0);
			}
			spheres[num_spheres++] = s;
		}
		else if(stricmp(type,"light")==0)
		{
			printf("found light\n");
			parse_doubles(file,"pos:",l.position);
			parse_doubles(file,"col:",l.color);

			if(num_lights == MAX_LIGHTS)
			{
				printf("too many lights, you should increase MAX_LIGHTS!\n");
				exit(0);
			}
			lights[num_lights++] = l;
		}
		else
		{
			printf("unknown type in scene description:\n%s\n",type);
			exit(0);
		}
	}
	return 0;
}

void display()
{


}

void init()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,WIDTH,0,HEIGHT,1,-1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(1.0,1.0,1.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void idle()
{
	//hack to make it only draw once
	static int once=0;
	if(!once)
	{
		draw_scene();

		if(mode == MODE_JPEG)
			save_jpg();
	}
	once=1;
}

int main (int argc, char ** argv)
{
	if (argc<2 || argc > 3)
	{  
		printf ("usage: %s <scenefile> [jpegname]\n", argv[0]);
		exit(0);
	}
	if(argc == 3)
	{
		mode = MODE_JPEG;
		filename = argv[2];
	}
	else if(argc == 2)
		mode = MODE_DISPLAY;

	glutInit(&argc,argv);
	loadScene(argv[1]);

	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(WIDTH,HEIGHT);
	int window = glutCreateWindow("Ray Tracer");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
}
