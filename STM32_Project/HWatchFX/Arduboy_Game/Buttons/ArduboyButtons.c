#include "ArduboyButtons.h"
#include "stdio.h"

char text[] = "Press Buttons!";
byte x;
byte y;

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

#define NUM_CHARS (sizeof(text) - 1)
#define X_MAX (WIDTH - (NUM_CHARS * CHAR_WIDTH) + 1)

#define Y_MAX (HEIGHT - CHAR_HEIGHT)

void arduboy_btn_init()
{
  arm_arduboy.begin();
  arm_arduboy.setFrameRate(30);

  x = (arm_arduboy.width() / 2) - (NUM_CHARS * CHAR_WIDTH / 2);
  y = (arm_arduboy.height() / 2) - (CHAR_HEIGHT / 2);
}
void arduboy_btn_loop()
{
  if (!(arm_arduboy.nextFrame()))
    return;
  if(arm_arduboy.pressed(RIGHT_BUTTON) && (x < X_MAX)) {
    x++;
  }
  if(arm_arduboy.pressed(LEFT_BUTTON) && (x > 0)) {
    x--;
  }
  if((arm_arduboy.pressed(UP_BUTTON) || arm_arduboy.pressed(B_BUTTON)) && (y > 0)) {
    y--;
  }
  if((arm_arduboy.pressed(DOWN_BUTTON) || arm_arduboy.pressed(A_BUTTON)) && (y < Y_MAX)) {
    y++;
  }
	if(arm_arduboy.pressed(A_BUTTON)){
		printf("A button pressed\n");
	}
	if(arm_arduboy.pressed(B_BUTTON)){
		printf("B button pressed\n");
	}
  arm_arduboy.clear();
  arm_arduboy.setCursor(x, y);
  arm_arduboy.print(text, sizeof(text));
  arm_arduboy.display();
}