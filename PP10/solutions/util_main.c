#include <stdio.h>
#include <stdlib.h>
#include "libutil.h"

int main(){
int cl = clamp(15, 0, 10);

printf("clamp(15, 0, 10)\n"); 
printf("result: %d \n",cl);

return 0;

}
