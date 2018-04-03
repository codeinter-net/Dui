// DUI : Duino's User Interface
// PBA 2018-04-02
	
#include "Dui.h"

// --------------------------------------------------------
// Fonctions internes
// --------------------------------------------------------

byte Dui::readButtons()  // Lecture des boutons
{
  word keyInput = analogRead(0);  // En commentaire : les valeurs lues lors des tests
  if (keyInput > 950) return DUI_KEY_NONE;   // 1019
  if (keyInput < 50)   return DUI_KEY_RIGHT; // 0
  if (keyInput < 180)  return DUI_KEY_UP;    // 98
  if (keyInput < 330)  return DUI_KEY_DOWN;  // 256
  if (keyInput < 530)  return DUI_KEY_LEFT;  // 411
  if (keyInput < 760)  return DUI_KEY_ESC;   // 644
  return DUI_KEY_OK;
}

void Dui::duiSelectDisplay(byte on) // Affiche ou efface le curseur de sélection
{
  Serial.println("duiSelectDisplay");
/*
  if(duiActiveItem==99) return;
  struct _duiDATA duiItem = duiCurrentPage[duiActiveItem];
  lcd.setCursor(duiItem.x, duiItem.y);
  switch(duiCurrentPage[duiActiveItem].type)
  {
    case DUI_SEL : // pour le type SEL : [...]
      lcd.write(on?'[':' ');
      lcd.setCursor(duiItem.x+(duiItem.text?strlen(duiItem.text):0)+1, duiItem.y);
      lcd.write(on?']':' ');
      break;
    case DUI_LIST :  // pour le type LIST : ->
      lcd.write(on?126:' ');
      break;
  }
*/
}

void Dui::duiChangeActive(byte action,byte index) // Change le texte actif d'un bouton
{
  Serial.println("duiChangeActive");
  struct _duiDATA *duiData = duiCurrentPage+index;
  byte x=duiData->x;
  byte y=duiData->y;
  byte type=duiData->type;

  char i;
  duiData = duiCurrentPage;
  for(i=0; duiData->type; i++) // Cherche l'objet actif
  {
    if((x==duiData->x)&&(y==duiData->y)&&((type|DUI_ACTIVE)==duiData->type))
      break;
    duiData++;
  }
  if(!duiData->type) return; // Pas d'objet actif

  struct _duiDATA *duiFindData=duiData;
  switch(action)
  {
    case DUI_PREV:
      for(; --i>=0;) // Cherche l'objet précédent
      {
        duiFindData--;
        if((x==duiFindData->x)&&(y==duiFindData->y)&&(type==duiFindData->type)&&duiFindData->text)
        {
          duiFindData->type|=DUI_ACTIVE;
          duiData->type&=~DUI_ACTIVE;
          return;
        }
      }
      break;
    case DUI_NEXT:
      for(i++; (++duiFindData)->type; i++) // Cherche l'objet suivant
      {
        if((x==duiFindData->x)&&(y==duiFindData->y)&&(type==duiFindData->type)&&duiFindData->text)
        {
          duiFindData->type|=DUI_ACTIVE;
          duiData->type&=~DUI_ACTIVE;
          return;
        }
      }
      break;
  }
}

byte Dui::duiExec(byte btn) // Traite l'appui sur un bouton
// -> 2 = effacer et redessiner l'écran ; 1 = juste redessiner ; 0 = ne rien faire
{
  Serial.println("duiExec");
  struct _duiDATA *duiData = duiCurrentPage;
  byte i;
  for(i=0; duiData->type; i++)
  {
    if((duiData->in&~DUI_ACTIVE)==btn)
    {
      if(!(duiData->in&DUI_ACTIVE)||(duiData->type&DUI_ACTIVE))
      {
        switch((int)duiData->out)
        {
          case 0 : break;
          case DUI_PREV : duiChangeActive(DUI_PREV,i); return 1;
          case DUI_NEXT : duiChangeActive(DUI_NEXT,i); return 1;
          default : duiCurrentPage=duiData->out; return 2;
        }
      }
    }
    duiData++;
  }
  return 0;
}

void Dui::duiDisplay(byte clearDisplay) // Affiche une page de l'interface utilisateur
{
  Serial.println("duiDisplay");
  if(clearDisplay) lcd->clear();
  struct _duiDATA *duiData = duiCurrentPage;
  byte i,xx,yy,sel;
  xx=99;
  yy=99;
//  duiActiveItem=99;
  for(i=0; duiData->type; i++) // Tant qu'il y a quelque chose à afficher
  {
    if(duiData->text&&(duiData->type!=DUI_CHANGE))
    {
      lcd->setCursor(duiData->x+(((duiData->type==DUI_SEL)||(duiData->type==DUI_LIST))?1:0), duiData->y); // Décalage pour le curseur
      lcd->print(duiData->text);
//      if(((duiData->type==DUI_SEL)||(duiData->type==DUI_LIST))&&(duiActiveItem==99))
//        duiActiveItem=i;
    }
    duiData++;
  }
  duiSelectDisplay(true);
}

// --------------------------------------------------------
// Les points d'entrée de DUI
// --------------------------------------------------------

Dui::Dui(duiDATA* currentPage) // Constructeur
{
	duiCurrentPage=currentPage;
}

void Dui::begin(LiquidCrystal* _lcd,byte width,byte height) // Initialiseur
{
	lcd=_lcd;
  lcd->begin(width,height);
  lcd->noCursor();
  lcd->noBlink();
  duiDisplay(true);
}

void Dui::go() // Gestionnaire d'évènements
{
  byte button=readButtons();
  if(button)
  {
    byte ret=duiExec(button);
    while(readButtons());
    if(ret) duiDisplay(ret==2);
  }
}
