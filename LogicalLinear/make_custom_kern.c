/*
 * make_custom_kern.c
 *
 *  Created on: Jan. 4, 2019
 *      Author: morteza
 */



#include "conv.h"
#include "basis.h"

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

	//printf("1\n");

	int edge_or_line = atoi(argv[1]);
	int i = atoi(argv[2]);
	//printf("2\n");
	int ndirs = atoi(argv[3]);;
	float sx   = 2.5;
	int   xn   = 4;
	float deg  = 2.0;
	float stab = 0.2;
	float sy   = 1.4;
	float sep  = 0.8;
	//printf("3\n");
	//printf("argc = %d\n",argc);
	if(argc > 4)
	{
		sx   = atof(argv[4]);
		xn   = atoi(argv[5]);
		deg  = atof(argv[6]);
		stab = atof(argv[7]);
		sy   = atof(argv[8]);
		sep  = atof(argv[9]);
	}
	//printf("4\n");


	cxkernel *the_kernel;
	//printf("5\n");

	char szFileName[255] = {0};

	if(edge_or_line == 1)
	{
		sprintf(szFileName, "edge_%d_%d.kern",i,ndirs);
		the_kernel =  make_edge2_cxkern(i * M_PI/ndirs, sx, xn, deg, stab, sy, sep);
	}
	else
	{
		sprintf(szFileName, "line_%d_%d.kern",i,ndirs);
		the_kernel = make_line2_cxkern(i * M_PI/ndirs, sx, xn, deg, stab, sy, sep);
	}

	//printf("6\n");
	FILE *f;


	f = fopen (szFileName, "w");
	write_kern (f, the_kernel);
	fclose (f);

	return 0;
}

