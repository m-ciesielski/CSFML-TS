//--------------------------------------------//

#define FACTION_MAX_UNITS 100
#define MAX_UNITS 300
#define MAX_STRING_LENGTH 250
#define MAX_MENU_BUTTONS 20
#define MAX_MESSAGE_BUTTONS 2

typedef struct ter
{
    sfTexture* texture;
    short int type;
    sfBool occupation;
    sfVector2f position;
    sfVector2i rel_position; //relative to view position

} terrain;

typedef struct
{
    sfTexture* texture;
    sfTexture* missile_texture;
    sfTexture* portrait_texture;
    char name [20];
    short int men; //bazowa ilosc ludzi w jednostce dla typu
    short int morale; //morale jednostki
  //  short int formation; //formacja jednostki
    /* op. przeniesienie ponizszych do unit_type*/
    short int melee_att; //bazowa wartosc ataku wrecz - modyfikowana w funkcji melee combat
    short int ranged_att; //bazowa wartosc ataku z dystansu- modyfikowana w funkcji melee combat
    short int melee_def; //bazowa wartosc obrony w walce wrecz - modyfikowana w funkcji melee combat
    short int ranged_def; //bazowa wartosc obrony w walce z dystansu- modyfikowana w funkcji melee combat
    short int speed; //szybkosc jednostki definiujaca ilosc pol jakie moze przebyc w czasie tury
    short int charge; //bonus szarzy dodawany do pierwszego ataku
    short int command;
    short int command_range;
    short int ammo;
    unsigned short int range;

} Unit_type;

typedef struct
{
   sfRectangleShape* shape;
   sfText* text;
}Unit_infobox;

typedef struct
{
    sfRectangleShape* shape;
    sfText* text;

}Bar;

typedef struct
{
    Unit_type* type;
    sfSprite* sprite ;
    unsigned short int men; //liczba ludzi w jednostce
    sfVertexArray* vertex_array;
    sfVertexArray* missile_vertex_array;
    sfVector2f missile_position;
    sfRenderStates vertex_state;
    sfRenderStates missile_vertex_state;
    sfVector2f position; //pozycja wektorowa
    sfVector2i map_position; // pozycja na tablicy mapy
    //UI dla jednostki
    Bar health_bar; //helath bar jednostki pokazujacy ilosc ludzi w jednostce
    sfVector2f health_bar_size;
    sfVector2f health_bar_position;
    sfVertex line [2];
    short int direction; //kierunek zwrotu
    short int new_direction; //kierunek zwrou po zakonczeniu ruchu
    /*
    Wartosci direction:
    0-polnoc
    1-wschod
    2-poludnie
    3-zachod
    */
    sfBool selected; // zmienna dla select boxa 0-1
    short int move_order; //flaga sprawdzajaca czy wydano rozkaz ruchu 0-1
    short int attack_order; //rozkaz ataku
    /*
    Wartosci attack_order:
    0-brak rozkazu ataku
    1-rozkaz ataku wrecz ->wywolanie funkcji melee_combat po zakonczeniu funkcji movement
    2-rozkaz ataku dystansowego -> wywolanie funkcji ranged_combat
    ...

    */
    sfBool in_combat; //flaga sprawczajaca czy jednostka jest w walce 0-1
    short int combat_focus; // zmienna wskazujaca na indeks jednostki z ktora walczy 0-[rozmiar armii przeciwnika]
    //-----------------
    //zmienne mechaniki walki

    short int morale; //morale jednostki
  //  short int formation; //formacja jednostki
    short int melee_att;
    short int ranged_att;
    short int melee_def;
    short int ranged_def;
    short int ammo;
    short int speed; //szybkosc jednostki definiujaca ilosc pol jakie moze przebyc w czasie tury
    short int charge; //bonus szarzy dodawany do pierwszego ataku
    //short int flanked;
    Unit_infobox infobox;
    sfBool draw_infobox;
    sfBool draw_line;
    //-----------------
    sfVector2f move_destination; //cel ruchu
}Unit;

//UI


typedef struct
{
    sfBool draw;
    sfRectangleShape* background;
    unsigned short int button_count;
    sfText* text [MAX_MENU_BUTTONS];
    sfRectangleShape* button [MAX_MENU_BUTTONS];
}Menu;

typedef struct
{
    sfBool draw;
    sfBool draw_ico;
    sfRectangleShape* background;
    sfVector2f box_size;
    sfRectangleShape* ico;
    sfVector2f camera_destination;
    sfText* text [MAX_MESSAGE_BUTTONS+1];
    sfRectangleShape* button [MAX_MESSAGE_BUTTONS];
}Message;

typedef struct
{
    char* name;
    Unit_type* unit_list [FACTION_MAX_UNITS];
    sfColor color;
}Faction;

typedef struct
{
    Faction* p_faction;
    unsigned short int faction_index;
    //unit unit_list [MAX_UNITS];
    unsigned int army_size;
    //unsigned int selected_units [FACTION_MAX_UNITS];
}Player;

typedef struct lista
{
	char text[MAX_STRING_LENGTH];
   struct lista *next;

}*log_list;

//-------------------------------------------//
