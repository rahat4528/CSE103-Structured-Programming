/**
 * CSE103-OS: PS/2 Keyboard Driver & Unified Input Stream
 * Scancode Set 1 decoding, Shift/Backspace handling, circular input queue.
 */

#ifndef CSE103_KEYBOARD_H
#define CSE103_KEYBOARD_H

#include "types.h"

#define INPUT_BUFFER_SIZE 256

void keyboard_init(void);
void input_enqueue(char c);
char input_dequeue(void);
bool input_has_char(void);

#endif /* CSE103_KEYBOARD_H */
