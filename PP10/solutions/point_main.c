#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "point.h"

int main(){

Point p={3.0 , 4.0};
double distance;

distance = sqrt(p.x * p.x + p.y * p.y);
printf("Point p: (%.2f, %.2f)\n", p.x, p.y);
printf("The distance is: %.2f\n", distance);

return 0;
}

