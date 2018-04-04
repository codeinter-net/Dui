// DUI : Duino's User Interface
// PBA 2018-04-02
	
#include "Dui.h"

// --------------------------------------------------------
// Fonctions internes
// --------------------------------------------------------

byte Dui::readButtons()
// Lecture des boutons : Les boutons forment des ponts diviseurs
// à résistances et sont connectés sur une entrée analogique
{
	word keyInput = analogRead(DUI_BUTTONS_ANALOG_INPUT);  // En commentaire : les valeurs lues lors des tests
	if (keyInput > 950) return DUI_KEY_NONE;   // 1019
	if (keyInput < 50)   return DUI_KEY_RIGHT; // 0
	if (keyInput < 180)  return DUI_KEY_UP;    // 98
	if (keyInput < 330)  return DUI_KEY_DOWN;  // 256
	if (keyInput < 530)  return DUI_KEY_LEFT;  // 411
	if (keyInput < 760)  return DUI_KEY_ESC;   // 644
	return DUI_KEY_OK;
}

void Dui::selectDisplay(byte on)
// Affiche ou efface le curseur de sélection
// type SEL : le texte est encadré par des crochets [...]
// type LIST : le texte est précédé d'une flèche ->
// on : true = affiche ; false = efface
{
/*
Serial.println("selectDisplay");
	char i;
	struct _duiDATA *duiData = currentPage;
	for(i=0; duiData->type; i++)
	{
		byte type=duiData->type&~DUI_ACTIVE;
		if((duiData->text)&&(type==duiData->type&DUI_ACTIVE))
		{
			switch(duiData->type&~DUI_ACTIVE)
			{
				case DUI_SEL : // pour le type SEL : [...]
					lcd->setCursor(duiData->x,duiData->y);
					lcd->write(on?'[':' ');
					lcd->setCursor(duiData->x+(duiData->text?strlen(duiData->text):0)+1, duiData->y);
					lcd->write(on?']':' ');
					break;
				case DUI_LIST :  // pour le type LIST : ->
					lcd->setCursor(duiData->x,duiData->y);
					lcd->write(on?126:' ');
					break;
			}
		}
	}
*/
}

char Dui::findObject(byte search,byte index)
// Recherche un élément faisant partie du même objet (même x,y,type)
// search : ce qu'il faut rechercher
// DUI_FIND_ACTIVE = l'objet actif
// DUI_FIND_PREV = le précédent ; DUI_FIND_NEXT = le suivant
// DUI_FIND_FIRST = le premier ; DUI_FIND_LAST = le dernier
// index : point de départ de la recherche
{
//Serial.println("findObject");
	struct _duiDATA *duiData = currentPage+index;
	byte x=duiData->x;
	byte y=duiData->y;
	byte type=duiData->type&~DUI_ACTIVE;
	char i;
	char find=-1;
	char findActive=-1;
	duiData = currentPage;
	for(i=0; duiData->type; i++)
	{
		if((x==duiData->x)&&(y==duiData->y)&&(duiData->text)&&(type==(duiData->type&~DUI_ACTIVE)))
		{
			switch(search)
			{
				case DUI_FIND_ACTIVE :
					if(duiData->type&DUI_ACTIVE) return i;
					break;
				case DUI_FIND_PREV :
					if(duiData->type&DUI_ACTIVE) return find>=0?find:i;
					break;
				case DUI_FIND_NEXT :
					if(findActive>=0) return i;
					break;
				case DUI_FIND_FIRST :
					return i;
				case DUI_FIND_LAST :
					break;
			}
			find=i;
			if(duiData->type&DUI_ACTIVE) findActive=i;
		}
		duiData++;
	}
	return find;
}

void Dui::changeActive(byte action,byte index)
// Change le texte actif d'un objet
// action : DUI_PREV = aller au texte précédent ; DUI_NEXT = ... suivant
// index : un des éléments composant l'objet (même x,y,type)
{
Serial.println("changeActive");
	char obj=findObject(DUI_FIND_ACTIVE,index);	
	if(obj<0) return; // Pas d'objet actif
	char findObj=-1;
	switch(action)
	{
		case DUI_PREV :
			findObj=findObject(DUI_FIND_PREV,obj);
			if((findObj<0)||(findObj==obj)) findObj=findObject(DUI_FIND_LAST,obj);
			break;
		case DUI_NEXT :
			findObj=findObject(DUI_FIND_NEXT,obj);
			if(findObj==obj) findObj=findObject(DUI_FIND_FIRST,obj);
			break;
	}
	if((findObj>=0)&&(findObj!=obj))
	{
		currentPage[findObj].type|=DUI_ACTIVE;
		currentPage[obj].type&=~DUI_ACTIVE;
	}
}

void Dui::callback(void (*call)(byte action, char* text),byte action, char* text)
{
	call(action, text);
}

void Dui::edit(byte obj, byte action, byte btn)
// Gère le contenu d'un champ éditable
{
Serial.println("edit");
	char sub=findObject(DUI_FIND_NEXT,obj); // infos complémentaires
	byte dataLen=strlen(currentPage[obj].text);
	byte formatLen=strlen(currentPage[sub].text);
	byte size=dataLen;
	if(currentPage[sub].out)
		callback((void (*)(byte action, char* text))currentPage[sub].out, action, currentPage[obj].text);
	switch(action)
	{
		case DUI_EDIT_INIT :
			currentPage[obj].type|=DUI_ACTIVE;
			if(size==formatLen) size--;
			lcd->setCursor(currentPage[obj].x+size,currentPage[obj].y);
			lcd->blink();
			break;
		case DUI_EDIT_BTN :
			break;
		case DUI_EDIT_EXIT :
			currentPage[obj].type&=~DUI_ACTIVE;
			lcd->noBlink();
			break;
	}
}

byte Dui::exec(byte btn)
// Traite l'appui sur un bouton
// btn : Code du bouton appuyé
// -> 2 = effacer et redessiner l'écran ; 1 = juste redessiner ; 0 = ne rien faire
{
Serial.println("exec");
	byte i;
	// Recherche si un champ éditable est en cours d'édition
	struct _duiDATA *duiData = currentPage;
	for(i=0; duiData->type; i++)
	{
		if(duiData->type==(DUI_EDIT|DUI_ACTIVE))
		{
			edit(i,DUI_EDIT_BTN,btn);
			return 0;
		}
		duiData++;
	}

	// Recherche un objet qui est cablé sur ce bouton
	duiData = currentPage;
	for(i=0; duiData->type; i++)
	{
		if((duiData->in&~DUI_ACTIVE)==btn) // Objet trouvé
		{
			if(!(duiData->in&DUI_ACTIVE)||(duiData->type&DUI_ACTIVE))
			{
				switch(duiData->type)
				{
					case DUI_EDIT : edit(i,DUI_EDIT_INIT,0); break;
				}
				switch((int)duiData->out)
				{
					case 0 : break;
					case DUI_PREV : changeActive(DUI_PREV,i); return 1;
					case DUI_NEXT : changeActive(DUI_NEXT,i); return 1;
					default : currentPage=duiData->out; return 2;
				}
			}
		}
		duiData++;
	}
	return 0;
}

void Dui::display(byte clearDisplay)
// Affiche une page de l'interface utilisateur
// clearDisplay : true = tout effacer ;  false = ne pas effacer
{
//Serial.println("display");
  if(clearDisplay) lcd->clear();
  struct _duiDATA *duiData = currentPage;
  byte i,xx,yy,sel;
  xx=99;
  yy=99;
  for(i=0; duiData->type; i++) // Tant qu'il y a quelque chose à afficher
  {
    if(duiData->text&&(duiData->type!=DUI_CHANGE)) // N'affiche pas les objetts CHANGE qui ne sont pas actifs
    {
      lcd->setCursor(duiData->x+(((duiData->type==DUI_SEL)||(duiData->type==DUI_LIST))?1:0), duiData->y); // Décalage pour le curseur
      lcd->print(duiData->text);
    }
    duiData++;
  }
  selectDisplay(true);
}

// --------------------------------------------------------
// Les points d'entrée de DUI
// --------------------------------------------------------

void Dui::begin(LiquidCrystal* _lcd,byte width,byte height) // Initialiseur
{
	lcd=_lcd;
	lcd->begin(width,height);
	lcd->noCursor();
	lcd->noBlink();
	display(true);
}

void Dui::doit() // Gestionnaire d'évènements
{
	byte button=readButtons();
	if(button)
	{
		byte ret=exec(button);
		while(readButtons());
		if(ret) display(ret==2);
	}
}
