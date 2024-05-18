#include <stdlib.h>
#include <string.h>
#include "Utility.h"
#include <ctype.h>   

int parseInt(char test[])
{
    if(isdigit(test[0])){
        int intRes;
        intRes = atol(test);
        return intRes;
    }
    else{
    }
}
