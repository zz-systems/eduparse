#ifndef CONSOLE_H
#define CONSOLE_H

enum color 
{
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_GREEN
};

// sets console color
void console_set_color(enum color color);
// resets console color to default
void console_reset_color();

#endif