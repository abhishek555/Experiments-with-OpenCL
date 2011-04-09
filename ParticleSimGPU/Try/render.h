#include <stdio.h>
#include <GL/glew.h>
#include "cll.h"
#include "util.h"

#include <vector>


#define GLUT_DISABLE_ATEXIT_HACK
#define GLEW_STATIC

#include <windows.h>
//OpenGL stuff
#if defined __APPLE__ || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <vector>
#include "cll.h"
#include "util.h"
#include <string>
#include <string.h>
