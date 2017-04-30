/**************************************************
 *
 *                 Shaders.h
 *
 *  Utility functions that make constructing shaders
 *  a bit easier.
 *
 ***************************************************/

int buildShader(int type, const char *filename);
int buildProgram(int first, ...);
void dumpProgram(int program, char *description);
