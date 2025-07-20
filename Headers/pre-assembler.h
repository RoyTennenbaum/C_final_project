#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/dynamic-tables.h"
#include "../Headers/static-tables.h"
#include "../Headers/global.h"

enum states
{
    MACRO_SPREAD,
    MACRO_DEFINE,
    NORMAL_LINE
};

int isValidLabel(char *arg, assemblerContext context);
