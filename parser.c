#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:
Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         translate: create a translation matrix,
                    then multiply the transform matrix by the translation matrix -
                    takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing
See the file script for an example of the file format
IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * edges,
				  struct matrix * polygons,
                  screen s) {

  FILE *f;
  char line[256];
  clear_screen(s);
  color c;
  c.red=255;
  c.green=0;
  c.blue=0;
  struct matrix *t_m;

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    int steps_3d = 30;
    int steps_2d = 100;
    char *newline = strchr(line, '\n');
	if (newline != NULL){
		*(newline) = 0;
	}
	newline = strchr(line, 13);//CR character case only occurs when reading from files?
	if (newline != NULL){
		*(newline) = 0;
	}
	//char ch=line[0];
	//int i=0;
	//while(!(ch==0)){
		//printf(":%d:\n",ch);
		//i++;
		//ch=line[i];
//	}
	//printf("%d",strcmp(line, "line"));
	//printf("%s\n",line);
	if (!strcmp(line, "line")){
		//printf("l\n");
		fgets(line, 255, f);
		double x0, y0, z0, x1, y1, z1;
		sscanf(line, "%lf %lf %lf %lf %lf %lf",&x0,&y0,&z0,&x1,&y1,&z1);
		add_edge(edges,x0,y0,z0,x1,y1,z1);
	}
	else if(!strcmp(line, "box")){
		fgets(line,255,f);
		double x,y,z,w,h,d;
		sscanf(line, "%lf %lf %lf %lf %lf %lf",&x,&y,&z,&w,&h,&d);
		add_box(polygons, x,y,z,w,h,d);
	}
	else if(!strcmp(line, "clear")){
		edges->lastcol=0;
		polygons->lastcol=0;
	}
	else if(!strcmp(line, "sphere")){
		fgets(line,255,f);
		double cx,cy,cz,r;
		sscanf(line, "%lf %lf %lf %lf",&cx,&cy,&cz,&r);
		add_sphere(polygons, cx,cy,cz,r, steps_3d);
	}
	else if(!strcmp(line, "torus")){
		fgets(line,255,f);
		double cx,cy,cz,r1,r2;
		sscanf(line, "%lf %lf %lf %lf %lf",&cx,&cy,&cz,&r1,&r2);
		add_torus(polygons, cx,cy,cz,r1,r2, steps_3d);
	}
	else if (!strcmp(line, "circle")){
				fgets(line, 255, f);
		double cx, cy,cz, r;
		sscanf(line, "%lf %lf %lf %lf",&cx,&cy,&cz,&r);
		add_circle(edges,cx,cy,cz,r,0.01);
	}
	
	else if (!strcmp(line, "bezier")){
		fgets(line, 255, f);
		double x0, x1,x2, x3, y0, y1, y2, y3;
		sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",&x0,&y0,&x1,&y1,&x2,&y2,&x3,&y3);
		add_curve(edges,x0,y0,x1,y1,x2,y2,x3,y3,steps_2d, 1);
	}
	
		else if (!strcmp(line, "hermite")){
		fgets(line, 255, f);
		double x0, x1,x2, x3, y0, y1, y2, y3;
		sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",&x0,&y0,&x1,&y1,&x2,&y2,&x3,&y3);
		add_curve(edges,x0,y0,x1,y1,x2,y2,x3,y3,steps_2d, 0);
	}
		
	
	else if (!strcmp(line, "ident")){
		ident(transform);
	}
	else if (!strcmp(line, "scale")){
		fgets(line, 255, f);
		double sx, sy, sz;
		sscanf(line, "%lf %lf %lf",&sx,&sy,&sz);
		t_m=make_scale(sx,sy,sz);
		matrix_mult(t_m,transform);
	}
	else if (!strcmp(line, "move")){
		fgets(line, 255, f);
		double tx, ty, tz;
		sscanf(line, "%lf %lf %lf",&tx,&ty,&tz);
		t_m=make_translate(tx,ty,tz);
		matrix_mult(t_m,transform);
	}
	else if (!strcmp(line, "rotate")){
		fgets(line, 255, f);
		char axis;
		double theta;
		sscanf(line, "%c %lf",&axis, &theta);
		//printf("%c %lf\n",axis, theta);
		theta=theta*M_PI/180;
		if (axis=='x' || axis=='X'){
			t_m=make_rotX(theta);
		}
		if (axis=='y' || axis=='Y'){
			t_m=make_rotY(theta);
		}
		if (axis=='z' || axis=='Z'){
			t_m=make_rotZ(theta);
		}
		matrix_mult(t_m,transform);
	}
	else if(!strcmp(line,"apply")){
		matrix_mult(transform,edges);
		matrix_mult(transform,polygons);
	}
	else if(!strcmp(line,"display")){
		clear_screen(s);
		if(edges->lastcol>0)
			draw_lines(edges,s,c);
		if(polygons->lastcol>0)
			draw_polygons(polygons,s,c);
		display(s);
	}
	else if (!strcmp(line,"save")){
		fgets(line, 255, f);
		line[strlen(line)-1]='\0';
		clear_screen(s);
		draw_lines(edges,s,c);
		save_extension(s, line);
		
	}
	else{
		//printf("%s\n",line);
	}
  }
}
  