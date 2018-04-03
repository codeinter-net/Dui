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

void Dui::selectDisplay(byte on) // Affiche ou efface le curseur de sélection
{
  Serial.println("selectDisplay");
/*
  if(duiActiveItem==99) return;
  struct _duiDATA duiItem = currentPage[duiActiveItem];
  lcd.setCursor(duiItem.x, duiItem.y);
  switch(currentPage[duiActiveItem].type)
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

char Dui::findObject(byte search,byte index) // Recherche un objet similaire (x,y,type)
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

void Dui::changeActive(byte action,byte index) // Change le texte actif d'un bouton
{
Serial.println("changeActive");
	char obj=findObject(DUI_FIND_ACTIVE,index);	
	if(obj<0) return; // Pas d'objet actif
	char findObj=-1;
	switch(action)
	{
		case DUI_PREV :
			findObj=findObject(DUI_FIND_PREV,obj);
Serial.println((byte)obj);
Serial.println((byte)findObj);
			if((findObj<0)||(findObj==obj)) findObj=findObject(DUI_FIND_LAST,obj);
Serial.println((byte)findObj);
			break;
		case DUI_NEXT :
			findObj=findObject(DUI_FIND_NEXT,obj);
Serial.println((byte)obj);
Serial.println((byte)findObj);
			if(findObj==obj) findObj=findObject(DUI_FIND_FIRST,obj);
Serial.println((byte)findObj);
			break;
	}
	if((findObj>=0)&&(findObj!=obj))
	{
		currentPage[findObj].type|=DUI_ACTIVE;
		currentPage[obj].type&=~DUI_ACTIVE;
	}
}

/*
void Dui::changeActive(byte action,byte index) // Change le texte actif d'un bouton
{
	Serial.println("changeActive");
	struct _duiDATA *duiData = currentPage+index;
	byte x=duiData->x;
	byte y=duiData->y;
	byte type=duiData->type;
	
	char i;
	duiData = currentPage;
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
*/

void Dui::edit(byte btn)
{
}

byte Dui::exec(byte btn) // Traite l'appui sur un bouton
// -> 2 = effacer et redessiner l'écran ; 1 = juste redessiner ; 0 = ne rien faire
{
	Serial.println("exec");
	struct _duiDATA *duiData = currentPage;
	byte i;
	// Recherche un champ éditable en cours d'édition
	for(i=0; duiData->type; i++)
	{
		if(duiData->type==(DUI_EDIT|DUI_ACTIVE))
		{
			edit(btn);
			return 0;
		}
		duiData++;
	}

	// Recherche un bouton actif
	duiData = currentPage;
	for(i=0; duiData->type; i++)
	{
		if((duiData->in&~DUI_ACTIVE)==btn)
		{
			if(!(duiData->in&DUI_ACTIVE)||(duiData->type&DUI_ACTIVE))
			{
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

void Dui::display(byte clearDisplay) // Affiche une page de l'interface utilisateur
{
//  Serial.println("display");
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
