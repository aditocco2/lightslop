/*
This interface specifies the functions for configuring the keypad, executing
one step of the keypad scanning FSM, and getting a character from the driver.

Angelo DiTocco
*/

#ifndef KEYPAD_H
#define KEYPAD_H

void configure_keypad();
void task_keypad_scan();
int keypad_getchar(char *c);

#endif