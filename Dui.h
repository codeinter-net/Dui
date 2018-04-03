// DUI : Duino's User Interface
// PBA 2018-04-02
	
#ifndef Dui_h
#define Dui_h

#define MAX_EDIT_SIZE 16 // Taille maximum d'un champ éditable

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
#define DUI_KEY_0 '0'
#define DUI_KEY_1 '1'
#define DUI_KEY_2 '2'
#define DUI_KEY_3 '3'
#define DUI_KEY_4 '4'
#define DUI_KEY_5 '5'
#define DUI_KEY_6 '6'
#define DUI_KEY_7 '7'
#define DUI_KEY_8 '8'
#define DUI_KEY_9 '9'
#define DUI_KEY_A 'A'
#define DUI_KEY_B 'B'
#define DUI_KEY_C 'C'
#define DUI_KEY_D 'D'
#define DUI_KEY_STAR '*'	// Ou définie comme ESC
#define DUI_KEY_HASH '#'	// Ou définie comme OK

// Objets
#define DUI_TEXT 1    // Texte
#define DUI_SEL 11    // Champ sélectionnable
#define DUI_LIST 12   // Elément de liste déroulante
#define DUI_CHANGE 13 // Champ qui change cycliquement
#define DUI_BTN 14      // Bouton sélectionnable

#define DUI_EDIT 30     // Champ éditable
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
#define DUI_OK 7 // Appelé en cas de validation

// Critères de recherche
#define DUI_FIND_ACTIVE 0
#define DUI_FIND_PREV 1
#define DUI_FIND_NEXT 2
#define DUI_FIND_FIRST 3
#define DUI_FIND_LAST 4

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
		Dui(duiDATA* _currentPage) : currentPage(_currentPage){};
		void begin(LiquidCrystal* _lcd,byte width,byte height);
		void doit();
	private:
		duiDATA* currentPage;
		LiquidCrystal* lcd;
		char editData[MAX_EDIT_SIZE+1]; // Buffer pour l'édition et l'affichage des champs éditables
		byte readButtons();  // Lecture des boutons
		void selectDisplay(byte on); // Affiche ou efface le curseur de sélection
		void changeActive(byte action,byte index); // Change le texte actif d'un bouton
		byte exec(byte btn); // Traite l'appui sur un bouton
		void display(byte clearDisplay); // Affiche une page de l'interface utilisateur
		void edit(byte btn);
		char findObject(byte type,byte index);
};

#endif
