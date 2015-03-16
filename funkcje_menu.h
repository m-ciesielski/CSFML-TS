#include "definicje_typow.h"
unsigned short int map_h;
unsigned short int map_w;
void create_main_menu (menu main_menu, short int screen_res_h, short int screen_res_w)
{
    //sfRectangleShape_setOutlineColor(main_menu.background, sfBlack);
    //sfRectangleShape_setOutlineThickness(main_menu.background, 2);
        sfVector2f main_menu_pos;
main_menu_pos.x=screen_res_w/4;
main_menu_pos.y=screen_res_h/4;
sfVector2f main_menu_size;
main_menu_size.x=screen_res_w/2;
main_menu_size.y=screen_res_h/2;
//sfRectangleShape_setSize(main_menu.background, main_menu_size);
//sfRectangleShape_setPosition(main_menu.background, main_menu_pos);

sfVector2f button_pos;
button_pos.x=main_menu_pos.x+main_menu_size.x/4;
button_pos.y=main_menu_pos.y+main_menu_size.y/10;

sfVector2f button_size;
button_size.x=main_menu_size.x/2;
button_size.y=main_menu_size.y/4;

sfVector2f text_pos;
sfText_setString(main_menu.text[0], "Nowa gra");
sfText_setString(main_menu.text[1], "Wyjscie");

    short int i;
    for(i=0; i<main_menu.button_count; ++i)
    {
        button_pos.y=button_pos.y + i*(button_size.y+50);
        text_pos.x=button_pos.x+button_size.x/4;
        text_pos.y=button_pos.y+button_size.y/10;
        sfRectangleShape_setSize(main_menu.button[i], button_size);
        sfRectangleShape_setFillColor(main_menu.button[i], sfWhite);
        sfRectangleShape_setPosition(main_menu.button[i], button_pos );
        sfText_setPosition(main_menu.text[i], text_pos);
    }

}

void destroy_menu (menu main_menu, sfBool background)
{
    short int i;
    if(background==1)
        sfRectangleShape_destroy(main_menu.background);
    for(i=0; i<main_menu.button_count; ++i)
    {
        if(main_menu.button[i])
        sfRectangleShape_destroy(main_menu.button[i]);
        if(main_menu.text[i])
        sfText_destroy(main_menu.text[i]);
    }
}

void create_select_map_menu (menu select_map_menu, short int screen_res_h, short int screen_res_w)
{
        sfVector2f main_menu_pos;
main_menu_pos.x=screen_res_w/4;
main_menu_pos.y=screen_res_h/4;
sfVector2f main_menu_size;
main_menu_size.x=screen_res_w/2;
main_menu_size.y=screen_res_h/2;
sfRectangleShape_setPosition(select_map_menu.background, main_menu_pos);
sfRectangleShape_setSize(select_map_menu.background, main_menu_size);

sfVector2f button_size;
button_size.x=main_menu_size.x/4;
button_size.y=main_menu_size.y/4;

sfVector2f button_pos;
button_pos.x=main_menu_pos.x;
button_pos.y=main_menu_pos.y+main_menu_size.y/10;


sfVector2f text_pos;
sfText_setString(select_map_menu.text[0], "Enter");
sfText_setString(select_map_menu.text[1], "...");
    short int i;
    for(i=0; i<select_map_menu.button_count; ++i)
    {
        button_pos.x=button_pos.x + (button_size.x+10);
        text_pos.x=button_pos.x+button_size.x/4;
        text_pos.y=button_pos.y+button_size.y/10;
        sfRectangleShape_setSize(select_map_menu.button[i], button_size);
        sfRectangleShape_setFillColor(select_map_menu.button[i], sfWhite);
        sfRectangleShape_setPosition(select_map_menu.button[i], button_pos );
        sfText_setPosition(select_map_menu.text[i], text_pos);
    }
}

void create_battle_menu (menu battle_menu, short int screen_res_h, short int screen_res_w)
{
        sfVector2f main_menu_pos;
main_menu_pos.x=screen_res_w/4;
main_menu_pos.y=screen_res_h-(screen_res_h/6);
sfVector2f main_menu_size;
main_menu_size.x=screen_res_w/2;
main_menu_size.y=screen_res_h/6;

sfVector2f button_size;
button_size.x=main_menu_size.x/4;
button_size.y=main_menu_size.y/4;

sfVector2f button_pos;
button_pos.x=(button_size.x);
button_pos.y=main_menu_pos.y+main_menu_size.y/10;


sfVector2f text_pos;
sfText_setString(battle_menu.text[0], "Dalej");
sfText_setString(battle_menu.text[1], "Wybierz mape");
sfText_setString(battle_menu.text[2], "Generuj mape");
sfText_setString(battle_menu.text[3], "Wstecz");
    short int i;
    for(i=0; i<4; ++i)
    {
        button_pos.x=button_pos.x + (button_size.x+10);
        text_pos.x=button_pos.x+button_size.x/4;
        text_pos.y=button_pos.y+button_size.y/10;
        sfRectangleShape_setSize(battle_menu.button[i], button_size);
        sfRectangleShape_setFillColor(battle_menu.button[i], sfWhite);
        sfRectangleShape_setPosition(battle_menu.button[i], button_pos );
        sfText_setPosition(battle_menu.text[i], text_pos);
    }
    char str [30];
    sprintf(str, "Wysokosc mapy: %d", map_h);
sfText_setString(battle_menu.text[4], str);
sprintf(str, "Szerokosc mapy: %d", map_w);
sfText_setString(battle_menu.text[5], str);
for(i=4;i<6;++i)
{
sfVector2f high_button_position;
high_button_position.x=button_size.x + (i-3)*(button_size.x+10);
high_button_position.y=button_size.y*2;
sfRectangleShape_setSize(battle_menu.button[i], button_size);
sfRectangleShape_setFillColor(battle_menu.button[i], sfWhite);
sfRectangleShape_setPosition(battle_menu.button[i], high_button_position );
sfText_setPosition(battle_menu.text[i], high_button_position);
}

}

void get_text_input (sfEvent event, sfText* text)
{
char txt[100];
int converted=event.text.unicode;
        sprintf(txt,"%s%c", sfText_getString(text),converted);
        sfText_setString(text,txt);
}

void destroy_menu_resources (sfRectangleShape* main_background,
                         sfTexture* main_background_texture, sfTexture* button_background)
{
    sfRectangleShape_destroy(main_background);
    sfTexture_destroy(main_background_texture);
    sfTexture_destroy(button_background);
}

short int button_pressed (sfVector2i mouse_pos, sfRectangleShape* button)
{
    if(mouse_pos.x>=sfRectangleShape_getPosition(button).x && mouse_pos.x<=sfRectangleShape_getPosition(button).x+sfRectangleShape_getSize(button).x
    && mouse_pos.y>=sfRectangleShape_getPosition(button).y && mouse_pos.y<=sfRectangleShape_getPosition(button).y+sfRectangleShape_getSize(button).y)
        return 1;
    else
        return 0;
}

short int button_pressed_2f (sfVector2f mouse_pos, sfRectangleShape* button)
{
    if(mouse_pos.x>=sfRectangleShape_getPosition(button).x && mouse_pos.x<=sfRectangleShape_getPosition(button).x+sfRectangleShape_getSize(button).x
    && mouse_pos.y>=sfRectangleShape_getPosition(button).y && mouse_pos.y<=sfRectangleShape_getPosition(button).y+sfRectangleShape_getSize(button).y)
        return 1;
    else
        return 0;
}
