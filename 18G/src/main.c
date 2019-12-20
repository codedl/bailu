#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
	unsigned char strarr[6] = {'1', '2', '3', '4', '5'}; 
	strarr[5] = '\0';
	double temp = atof(strarr);
	printf("%s\n", strarr);
	printf("%f\n",temp);

}
