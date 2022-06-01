#include "SnakePort.h"
#include "Snake.h"
#include "stdio.h"
#include <string>
using namespace std;

Snake snake;
static uint8_t gameState;             // Etat du jeu à afficher 
char scoreText[4];             // Score à afficher
uint8_t highScore = 0;         // Meilleur score
char highScoreText[4];         // Meilleur score à afficher 
uint8_t blinkingTextState = 0; // Permet de gérer un texte clignotant

void snake_init()
{
	arm_arduboy.begin();
	
  // Le menu principal est affiché en premier
  gameState = GAMESTATE_MAINMENU;
  snake.init(&arm_arduboy, GAMESTATE_MAINMENU);
}

void snake_loop()
{
	string msg = "";
  // Si le serpent est mort, un écran « GAME OVER » est affiché
  if (snake.isDead) {
    
    // Ralentissement de la vitesse de rafraîchissement de l'écran pour 
    // pour qu'elle soit conforme au clignotement du texte
    arm_arduboy.setFrameRate(1);

    // Si le meilleur score est battu
    if (snake.foodEaten > highScore) {

      // Sauvegarde du meilleur score sur l'EEPROM
      highScore = snake.foodEaten;
      //EEPROM.update(EEPROM_ADDRESS, snake.foodEaten);
    }
  
    // Si le joueur appui sur le bouton B
    if (arm_arduboy.pressed(B_BUTTON)) {

      // Affichage du menu principal
      gameState = GAMESTATE_MAINMENU;
      snake.init(&arm_arduboy, GAMESTATE_MAINMENU);
  
      // Ajout d'un délai pour éviter que la pression de la touche B se répercute 
      // sur l'écran suivant
      delay(200);

      return;
    }

    // Maintient de la bonne fréquence d'affichage
    if (!arm_arduboy.nextFrame()) return;
    
    arm_arduboy.clear();

    // Affichage du titre "GAME OVER"
    arm_arduboy.setTextSize(2);
    arm_arduboy.setCursor(10, 2);
		msg = "GAME OVER";
    arm_arduboy.print(msg.c_str(), msg.length());

    // Affichage du score
    arm_arduboy.setTextSize(1);
    arm_arduboy.setCursor(19, 25);
		msg = "Your score: ";
    arm_arduboy.print(msg.c_str(), msg.length());
    arm_arduboy.setCursor(91, 25);;
    sprintf(scoreText, "%03u", snake.foodEaten);
    arm_arduboy.print(scoreText, sizeof(scoreText));

    // Affichage du meilleur score
    arm_arduboy.setCursor(19, 40);
		msg = "High score:";
		arm_arduboy.print(msg.c_str(), msg.length());
    arm_arduboy.setCursor(91, 40);
    sprintf(highScoreText, "%03u", highScore);
    arm_arduboy.print(highScoreText, sizeof(highScoreText));
  
    // Affichage d'un message "Press B to continue" clignotant
    blinkingTextState = (blinkingTextState + 1) % 6;
    if (blinkingTextState < 3) {
  
      arm_arduboy.setCursor(7, 55);
			msg = "Press B to continue";
      arm_arduboy.print(msg.c_str(), msg.length());
    }
       
    arm_arduboy.display();
  }
  
  // Si le serpent est en vie, la partie continue
  else if (gameState == GAMESTATE_GAME) {

    // Lecture des touches
    if (arm_arduboy.pressed(UP_BUTTON)) {
  
      snake.direction = SNAKEDIR_UP;
    }
    else if (arm_arduboy.pressed(DOWN_BUTTON)) {
      
      snake.direction = SNAKEDIR_DOWN;    
    }
    else if (arm_arduboy.pressed(LEFT_BUTTON)) {
      
      snake.direction = SNAKEDIR_LEFT;    
    }
    else if (arm_arduboy.pressed(RIGHT_BUTTON)) {
      
      snake.direction = SNAKEDIR_RIGHT;    
    }
    else if (arm_arduboy.pressed(A_BUTTON)) {
  
      // Activation ou désactivation du son
      arm_arduboy.setTextSize(1);
      if (snake.canMakeSound) {
  
        snake.canMakeSound = false;
        arm_arduboy.setCursor(28, 0);
				msg = "            ";
        arm_arduboy.print(msg.c_str(), msg.length());
        arm_arduboy.setCursor(34, 0);
				msg = "Sound: OFF";
        arm_arduboy.print(msg.c_str(), msg.length());
      }
      else {
  
        snake.canMakeSound = true;
        arm_arduboy.setCursor(31, 0);
				msg = "           ";
        arm_arduboy.print(msg.c_str(), msg.length());
        arm_arduboy.setCursor(37, 0);
				msg = "Sound: ON";
        arm_arduboy.print(msg.c_str(), msg.length());
//        arduboy.tunes.tone(1000, 200);
      }
      arm_arduboy.display();
      delay(1000);
    }
    else if (arm_arduboy.pressed(B_BUTTON)) {
      
      gameState = GAMESTATE_PAUSED;
      
      // Ajout d'un délai pour éviter que la pression de la touche B se répercute 
      // sur l'écran suivant
      delay(200);    
    }
    
    // Maintient de la bonne fréquence d'affichage
    if (!arm_arduboy.nextFrame()) return;
    
    snake.move();
    if (snake.isDead) return;

    arm_arduboy.clear();

    // Affichage d'une bordure
    arm_arduboy.drawRect(2, 2, 124, 60, 1);

    // Affichage du score
    arm_arduboy.setTextSize(1);
    arm_arduboy.setCursor(49, 0);
		msg = "     ";
    arm_arduboy.print(msg.c_str(), msg.length());
    arm_arduboy.setCursor(55, 0);
    sprintf(scoreText, "%03u", snake.foodEaten);
    arm_arduboy.print(scoreText, sizeof(scoreText));
   
    snake.draw();
    snake.food.draw();
  
    arm_arduboy.display();
  }

  // Si le jeu est en pause, un message "PAUSED" est affiché
  else if (gameState == GAMESTATE_PAUSED) {

    // Si le joueur appui sur le bouton B la partie reprend
    if (arm_arduboy.pressed(B_BUTTON)) {
    
      gameState = GAMESTATE_GAME;
  
      // Ajout d'un délai pour éviter que la pression de la touche B se répercute 
      // sur l'écran suivant
      delay(200);
    }

    // Maintient de la bonne fréquence d'affichage
    if (!arm_arduboy.nextFrame()) return;
    
    // Affichage du titre "PAUSED"
    arm_arduboy.fillRect(22, 18, 84, 28, 0);
    arm_arduboy.drawRect(23, 19, 82, 26, 1);
    arm_arduboy.setTextSize(2);
    arm_arduboy.setCursor(29, 25);
		msg = "PAUSED";
    arm_arduboy.print(msg.c_str(), msg.length());
  
    arm_arduboy.display();
  }

  // Par défaut le menu principal est affiché
  else {

    // Modification de la vitesse de rafraîchissement de l'écran pour pour qu'elle 
    // soit conforme au clignotement du texte et au déplacement du serpent
    arm_arduboy.setFrameRate(20);

    // Si le joueur appui sur le bouton B
    if (arm_arduboy.pressed(B_BUTTON)) {

      // Lancement de la partie
      gameState = GAMESTATE_GAME;
      snake.init(&arm_arduboy, GAMESTATE_GAME);

      // Ajout d'un délai pour éviter que la pression de la touche B se répercute 
      // sur l'écran suivant
      delay(200);

      return;
    }
    
    // Maintient de la bonne fréquence d'affichage
    if (!arm_arduboy.nextFrame()) return;

    snake.moveOnMainMenu();
  
    arm_arduboy.clear();
    
    // Affichage du titre
    arm_arduboy.setTextSize(2);
    arm_arduboy.setCursor(10, 11);
		msg = "ARDUSNAKE";
    arm_arduboy.print(msg.c_str(), msg.length());
    arm_arduboy.setTextSize(1);
    
    // Affichage d'un message "Press B to start" clignotant
    blinkingTextState = (blinkingTextState + 1) % 30;
    if (blinkingTextState < 15) {
    
      arm_arduboy.setCursor(16, 44);
			msg = "Press B to start";
      arm_arduboy.print(msg.c_str(), msg.length());
    }

    snake.draw();
  
    arm_arduboy.display();
  }
}

