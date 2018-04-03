// DUI : Duino's User Interface
// PBA 2018-04-02
	
#ifndef Dui_h
#define Dui_h

#include "Arduino.h"
#include <LiquidCrystal.h>


#define DUI_KEY_NONE   0
#define DUI_KEY_UP     1
#define DUI_KEY_DOWN   2
#define DUI_KEY_LEFT   3
#define DUI_KEY_RIGHT  4
#define DUI_KEY_SELECT 5
#define DUI_KEY_ESC    5 // Nouveau nom pour la touche SELECT
#define DUI_KEY_RST    6
#define DUI_KEY_OK     6 // Nouveau nom pour la touche RESET
// !!! ATTENTION : pour les chiffres, ne pas utiliser 0  pour le '0' sans en tenir compte dans le code

// Objets
#define DUI_TEXT 1    // Texte
#define DUI_SEL 11    // Champ sélectionnable
#define DUI_LIST 12   // Elément de liste déroulante
#define DUI_CHANGE 13 // Champ qui change cycliquement
#define DUI_BTN 14      // Bouton sélectionnable

#define DUI_NUM 31      // Valeur numérique
#define DUI_ALPHA 32    // Valeur alphabétique
#define DUI_ALPHANUM 33 // Valeur alphanumérique

#define DUI_ACTIVE 128 // Champ actif

// Evénements
#define DUI_PREV 1 // Actions possibles
#define DUI_NEXT 2
#define DUI_UP 1
#define DUI_DOWN 2
#define DUI_LEFT 3
#define DUI_RIGHT 4
#define DUI_INIT 5 // Appelé en entrant
#define DUI_EXIT 6 // Appelé en sortant


typedef struct _duiDATA
{
  byte x;
  byte y;
  byte type;
  char *text;
  byte in;
  struct _duiDATA* out;
} duiDATA;

class Dui
{
	public:
		Dui(duiDATA* currentPage);
		void begin(LiquidCrystal* _lcd,byte width,byte height);
		void go();
	private:
		duiDATA* duiCurrentPage;
		LiquidCrystal* lcd;
		byte readButtons();  // Lecture des boutons
		void duiSelectDisplay(byte on); // Affiche ou efface le curseur de sélection
		void duiChangeActive(byte action,byte index); // Change le texte actif d'un bouton
		byte duiExec(byte btn); // Traite l'appui sur un bouton
		void duiDisplay(byte clearDisplay); // Affiche une page de l'interface utilisateur
};

#endif
