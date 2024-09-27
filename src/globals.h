#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>

#define RENDER_WIDTH 240 * 1
#define RENDER_HEIGHT 160 * 1
#define FRAME_LIMITING true
#define TARGET_FPS 120
#define TARGET_FRAME_TIME (1000 / TARGET_FPS)

extern int WIDTH;
extern int HEIGHT;

#endif // GLOBALS_H
