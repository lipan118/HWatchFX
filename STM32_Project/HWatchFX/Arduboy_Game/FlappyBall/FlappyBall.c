#include "FlappyBall.h"
#include "bitmaps.h"
#include "stdio.h"

// Things that make the game work the way it does
#define FRAMES_PER_SECOND 30   // The update and refresh speed
#define FRAC_BITS 6            // The number of bits in the fraction part of a fixed point int

// The following values define how fast the ball will accelerate and how high it will jump.
// They are given as fixed point integers so the true value is multiplied by (1 << FRAC_BITS)
// to give the value used. The resulting values must be integers.
#define BALL_ACCELERATION 16      // (0.25) the ball acceleration in pixels per frame squared
#define BALL_JUMP_VELOCITY (-144) // (-2.25) The inital velocity of a ball jump in pixels per frame
// ---------------------------

// This value is an offset to make it easier to work with negative numbers.
// It must be greater than the maximum number of pixels that Floaty will jump above
// the start height (based on the acceleration and initial velocity values),
// but must be low enough not to cause an overflow when added to the maximum
// screen height as an integer.
#define NEG_OFFSET 64

// Pipe
#define PIPE_ARRAY_SIZE 4  // At current settings only 3 sets of pipes can be onscreen at once
#define PIPE_MOVE_DISTANCE 2   // How far each pipe moves per frame
#define PIPE_GAP_MAX 30        // Maximum pipe gap
#define PIPE_GAP_MIN 18        // Minimum pipe gap
#define PIPE_GAP_REDUCE 5      // Number of points scored to reduce gap size
#define PIPE_WIDTH 12
#define PIPE_CAP_WIDTH 2
#define PIPE_CAP_HEIGHT 3      // Caps push back into the pipe, it's not added length
#define PIPE_MIN_HEIGHT 6      // Higher values center the gaps more
#define PIPE_GEN_FRAMES 32     // How many frames until a new pipe is generated

// Ball
#define BALL_RADIUS 4
#define BALL_Y_START ((HEIGHT / 2) - 1) // The height Floaty begins at
#define BALL_X 32              // Floaty's X Axis

// Storage Vars
static byte gameState = 0;
unsigned int gameScore = 0;
unsigned int gameHighScore = 0;
char pipes[2][PIPE_ARRAY_SIZE]; // Row 0 for x values, row 1 for gap location
byte pipeGap = PIPE_GAP_MAX;    // Height of gap between pipes to fly through
byte pipeReduceCount = PIPE_GAP_REDUCE; // Score tracker for pipe gap reduction
char ballY = BALL_Y_START;      // Floaty's height
char ballYprev = BALL_Y_START;  // Previous height
char ballYi = BALL_Y_START;     // Floaty's initial height for the current arc
int ballV = 0;                  // For height calculations (Vi + ((a * t) / 2))
byte ballFrame = 0;             // Frame count for the current arc
char ballFlapper = BALL_RADIUS; // Floaty's wing length
char gameScoreX = 0;
char gameScoreY = 0;
byte gameScoreRiser = 0;

char msg[32];

static void copy_str(const char* str)
{
	memset(msg, 0x00, sizeof(msg));
	sprintf(msg, "%s", str);
}

static void copy_number(unsigned int number)
{
	memset(msg, 0x00, sizeof(msg));
	sprintf(msg, "%d", number);
}

void drawInfo() {
  byte ulStart;     // Start of underline to indicate sound status
  byte ulLength;    // Length of underline

  arm_arduboy.setCursor(6, 3);
	copy_str("A,B,Up,Down: Jump");
  arm_arduboy.print(msg, strlen(msg));
  arm_arduboy.setCursor(6, 51);
	copy_str("Left: Sound On/Off");
  arm_arduboy.print(msg, strlen(msg));

	ulStart = 16 * 6;
	ulLength = 3 * 6 - 1;
  arm_arduboy.drawFastHLine(ulStart, 51 + 8, ulLength, WHITE); // Underline the current sound mode
  arm_arduboy.drawFastHLine(ulStart, 51 + 9, ulLength, WHITE);
}

void drawFloor() {
  arm_arduboy.drawFastHLine(0, HEIGHT-1, WIDTH, WHITE);
}

void drawFloaty() {
  ballFlapper--;
  if (ballFlapper < 0) { ballFlapper = BALL_RADIUS; }  // Flapper starts at the top of the ball
  arm_arduboy.drawCircle(BALL_X, ballY, BALL_RADIUS, BLACK);  // Black out behind the ball
  arm_arduboy.drawCircle(BALL_X, ballY, BALL_RADIUS, WHITE);  // Draw outline
  arm_arduboy.drawLine(BALL_X, ballY, BALL_X - (BALL_RADIUS+1), ballY - ballFlapper, WHITE);  // Draw wing
  arm_arduboy.drawPixel(BALL_X - (BALL_RADIUS+1), ballY - ballFlapper + 1, WHITE);  // Dot the wing
  arm_arduboy.drawPixel(BALL_X + 1, ballY - 2, WHITE);  // Eye
}

void drawPipes() {
  for (byte x = 0; x < PIPE_ARRAY_SIZE; x++){
    if (pipes[0][x] != 0) {    // Value set to 0 if array element is inactive,
                               // otherwise it is the xvalue of the pipe's left edge
      // Pipes
      arm_arduboy.drawRect(pipes[0][x], -1, PIPE_WIDTH, pipes[1][x], WHITE);
      arm_arduboy.drawRect(pipes[0][x], pipes[1][x] + pipeGap, PIPE_WIDTH, HEIGHT - pipes[1][x] - pipeGap, WHITE);
      // Caps
      arm_arduboy.drawRect(pipes[0][x] - PIPE_CAP_WIDTH, pipes[1][x] - PIPE_CAP_HEIGHT, PIPE_WIDTH + (PIPE_CAP_WIDTH*2), PIPE_CAP_HEIGHT, WHITE);
      arm_arduboy.drawRect(pipes[0][x] - PIPE_CAP_WIDTH, pipes[1][x] + pipeGap, PIPE_WIDTH + (PIPE_CAP_WIDTH*2), PIPE_CAP_HEIGHT, WHITE);
      // Detail lines
      arm_arduboy.drawLine(pipes[0][x]+2, 0, pipes[0][x]+2, pipes[1][x]-5, WHITE);
      arm_arduboy.drawLine(pipes[0][x]+2, pipes[1][x] + pipeGap + 5, pipes[0][x]+2, HEIGHT - 3,WHITE);
    }
  }
}

void generatePipe() {
  for (byte x = 0; x < PIPE_ARRAY_SIZE; x++) {
    if (pipes[0][x] == 0) { // If the element is inactive
      pipes[0][x] = WIDTH;  // Then create it starting right of the screen
      pipes[1][x] = random(PIPE_MIN_HEIGHT, HEIGHT - PIPE_MIN_HEIGHT - pipeGap);
      return;
    }
  }
}

boolean checkPipe(byte x) {  // Collision detection, x is pipe to check
  byte AxA = BALL_X - (BALL_RADIUS-1);  // Hit box for floaty is a square
  byte AxB = BALL_X + (BALL_RADIUS-1);  // If the ball radius increases too much, corners
  byte AyA = ballY - (BALL_RADIUS-1);  // of the hitbox will go outside of floaty's
  byte AyB = ballY + (BALL_RADIUS-1);  // drawing
  byte BxA, BxB, ByA, ByB;

  // check top cylinder
  BxA = pipes[0][x];
  BxB = pipes[0][x] + PIPE_WIDTH;
  ByA = 0;
  ByB = pipes[1][x];
  if (AxA < BxB && AxB > BxA && AyA < ByB && AyB > ByA) { return true; } // Collided with top pipe

  // check top cap
  BxA = pipes[0][x] - PIPE_CAP_WIDTH;
  BxB = BxA + PIPE_WIDTH + (PIPE_CAP_WIDTH*2);
  ByA = pipes[1][x] - PIPE_CAP_HEIGHT;
  if (AxA < BxB && AxB > BxA && AyA < ByB && AyB > ByA) { return true; } // Collided with top cap

  // check bottom cylinder
  BxA = pipes[0][x];
  BxB = pipes[0][x] + PIPE_WIDTH;
  ByA = pipes[1][x] + pipeGap;
  ByB = HEIGHT-1;
  if (AxA < BxB && AxB > BxA && AyA < ByB && AyB > ByA) { return true; } // Collided with bottom pipe

  // check bottom cap
  BxA = pipes[0][x] - PIPE_CAP_WIDTH;
  BxB = BxA + PIPE_WIDTH + (PIPE_CAP_WIDTH*2);
  ByB = ByA + PIPE_CAP_HEIGHT;
  if (AxA < BxB && AxB > BxA && AyA < ByB && AyB > ByA) { return true; } // Collided with bottom pipe

  return false; // Nothing hits
}

boolean jumpPressed() { // Return "true" if a jump button is pressed
  return (buttonsState() & (UP_BUTTON | DOWN_BUTTON | A_BUTTON | B_BUTTON)) != 0;
}

void beginJump() {
  ballV = BALL_JUMP_VELOCITY;
  ballFrame = 0;
  ballYi = ballY;
}

void startFalling() {   // Start falling from current height
  ballFrame = 0;        // Start a new fall
  ballYi = ballY;       // Set initial arc position
  ballV = 0;            // Set velocity to 0
}

void moveFloaty() {
  ballYprev = ballY;                   // Save the previous height
  ballFrame++;                         // Next frame
  ballV += (BALL_ACCELERATION / 2);    // Increase the velocity
                                     // Calculate Floaty's new height:
  ballY = ((((ballV * ballFrame)       // Complete "distance from initial height" calculation
           + (NEG_OFFSET << FRAC_BITS) // Add an offset to make sure the value is positive
           + (1 << (FRAC_BITS - 1)))   // Add 0.5 to round up
            >> FRAC_BITS)              // shift down to remove the fraction part
             - NEG_OFFSET)             // Remove previously added offset
              + ballYi;                // Add the result to the start height to get the new height
}

void debounceButtons() { // prevent "noisy" buttons from appearing as multiple presses
  delay(100);
  while (buttonsState()) { }  // Wait for all keys released
  delay(100);
}

byte getOffset(unsigned int s) {
  if (s > 9999) { return 24; }
  if (s > 999) { return 18; }
  if (s > 99) { return 12; }
  if (s > 9) { return 6; }
  return 0;
}


void flappyball_init()
{
  arm_arduboy.begin();
  arm_arduboy.setFrameRate(FRAMES_PER_SECOND);
  arm_arduboy.clear();
  arm_arduboy.drawSlowXYBitmap(0,0,floatyball,128,64,1);
  arm_arduboy.display();

  delay(500);
  arm_arduboy.setCursor(18,55);
	copy_str("Press Any Button");
  arm_arduboy.print(msg, strlen(msg));
  arm_arduboy.display();

  while (!buttonsState()) { } // Wait for any key press
		debounceButtons();

  arm_arduboy.initRandomSeed();
  for (byte x = 0; x < PIPE_ARRAY_SIZE; x++) { pipes[0][x] = 0; }  // Set all pipes offscreen
}
void flappyball_loop()
{
  if (!arm_arduboy.nextFrame())
    return;

  if (arm_arduboy.pressed(LEFT_BUTTON)) { // If the button for sound toggle is pressed
    debounceButtons();                // Wait for button release
  }

  arm_arduboy.clear();

  // ===== State: Wait to begin =====
  if (gameState == 0) {     // If waiting to begin
    drawFloor();
    drawFloaty();
    drawInfo();             // Display usage info

    if (jumpPressed()) {    // Wait for a jump button press
      gameState = 1;        // Then start the game
      beginJump();          // And make Floaty jump
    }
  }

  // ===== State: Playing =====
  if (gameState == 1) {     // If the game is playing
    // If the ball isn't already rising, check for jump
    if ((ballYprev <= ballY) && jumpPressed()) {
      beginJump();          // Jump
    }

    moveFloaty();

    if (ballY < BALL_RADIUS) {  // If Floaty has moved above the top of the screen
      ballY = BALL_RADIUS;      // Set position to top
      startFalling();           // Start falling
    }

    if (arm_arduboy.everyXFrames(PIPE_GEN_FRAMES)) { // Every PIPE_GEN_FRAMES worth of frames
      generatePipe();                  // Generate a pipe
    }

    for (byte x = 0; x < PIPE_ARRAY_SIZE; x++) {  // For each pipe array element
      if (pipes[0][x] != 0) {           // If the pipe is active
        pipes[0][x] = pipes[0][x] - PIPE_MOVE_DISTANCE;  // Then move it left
        if (pipes[0][x] + PIPE_WIDTH < 0) {  // If the pipe's right edge is off screen
          pipes[0][x] = 0;              // Then set it inactive
        }
        if (pipes[0][x] + PIPE_WIDTH == (BALL_X - BALL_RADIUS)) {  // If the pipe passed Floaty
          gameScore++;                  // Increment the score
          pipeReduceCount--;            // Decrement the gap reduction counter
          gameScoreX = BALL_X;                  // Load up the floating text with
          gameScoreY = ballY - BALL_RADIUS - 8; //  current ball x/y values
          gameScoreRiser = 15;          // And set it for 15 frames
        }
      }
    }

    if (gameScoreRiser > 0) {  // If we have floating text
      gameScoreY--;
      if (gameScoreY >= 0) { // If the score will still be on the screen
        arm_arduboy.setCursor(gameScoreX, gameScoreY);
				copy_number(gameScore);
        arm_arduboy.print(msg, strlen(msg));
        gameScoreX = gameScoreX - 2;
        gameScoreRiser--;
      } else {
        gameScoreRiser = 0;
      }
    }

    if (ballY + BALL_RADIUS > (HEIGHT-1)) {  // If the ball has fallen below the screen
      ballY = (HEIGHT-1) - BALL_RADIUS;      // Don't let the ball go under :O
      gameState = 2;                        // Game over. State is 2.
    }
    // Collision checking
    for (byte x = 0; x < PIPE_ARRAY_SIZE; x++) { // For each pipe array element
      if (pipes[0][x] != 0) {                 // If the pipe is active (not 0)
        if (checkPipe(x)) { gameState = 2; }  // If the check is true, game over
      }
    }

    drawPipes();
    drawFloor();
    drawFloaty();

    // Reduce pipe gaps as the game progresses
    if ((pipeGap > PIPE_GAP_MIN) && (pipeReduceCount <= 0)) {
      pipeGap--;
      pipeReduceCount = PIPE_GAP_REDUCE;  // Restart the countdown
    }
  }

  // ===== State: Game Over =====
  if (gameState == 2) {  // If the gameState is 2 then we draw a Game Over screen w/ score
    if (gameScore > gameHighScore) { gameHighScore = gameScore; }
    arm_arduboy.display();              // Make sure final frame is drawn
    startFalling();                 // Start falling from current position
    while (ballY + BALL_RADIUS < (HEIGHT-1)) {  // While floaty is still airborne
      moveFloaty();
      arm_arduboy.clear();
      drawPipes();
      drawFloor();
      drawFloaty();
      arm_arduboy.display();
      while (!arm_arduboy.nextFrame()) { }  // Wait for next frame
    }
    arm_arduboy.drawRect(16,8,96,48, WHITE); // Box border
    arm_arduboy.fillRect(17,9,94,46, BLACK); // Black out the inside
    arm_arduboy.drawSlowXYBitmap(30,12,gameover,72,14,1);
    arm_arduboy.setCursor(50 - getOffset(gameScore),30);
		copy_number(gameScore);
		arm_arduboy.print(msg, strlen(msg));
    arm_arduboy.setCursor(62,30);
		copy_str("Score");
    arm_arduboy.print(msg, strlen(msg));

    arm_arduboy.setCursor(50 - getOffset(gameHighScore),42);
		copy_number(gameHighScore);
		arm_arduboy.print(msg, strlen(msg));
    arm_arduboy.setCursor(62,42);
		copy_str("High");
    arm_arduboy.print(msg, strlen(msg));

    arm_arduboy.display();
    delay(1000);         // Give some time to stop pressing buttons

    while (!jumpPressed()) { } // Wait for a jump button to be pressed
    debounceButtons();

    gameState = 0;       // Then start the game paused
    gameScore = 0;       // Reset score to 0
    gameScoreRiser = 0;  // Clear the floating score
    for (byte x = 0; x < PIPE_ARRAY_SIZE; x++) { pipes[0][x] = 0; }  // set all pipes inactive
    ballY = BALL_Y_START;   // Reset to initial ball height
    pipeGap = PIPE_GAP_MAX; // Reset the pipe gap height
    pipeReduceCount = PIPE_GAP_REDUCE; // Init the pipe gap reduction counter
  }

  arm_arduboy.display();  // Finally draw this thang
}
