#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/System.h>
#include <stdio.h>
#include <strings.h>

//#include "definicje_typow.h"
#include "lib.h"
///Known bugs:
// TODO repair bug: unit with melee attack order doesn't finish it's movement when preceeding units are performing ranged attack.
// this bug can be caused by wrong counting of inactive units in GAME LOOP or by similar problem in game_mech function or by ranged attack function

//TODO repair bug: melee combat is sometimes executed twice in one turn

//TODO repair bug: morale damage doesn't seems to work

//TODO repair bug: program terminates sometimes with non-zero status, it is propably caused by some error in destruction of elements
///Map definition file, currently not used
FILE* map_def_file;
///Log
FILE* log_file;
///Units definition file
FILE* unit_def_file;

///Default map size
unsigned int map_h=15;
unsigned int map_w=15;
///Maximum map size
unsigned int max_map_h=200;
unsigned int max_map_w=200;

log_list game_log=NULL;

int main()
{
    ///Variables initialization
    ///+++++++++++++++++++++++++//
//TODO screen resolution detection
short int screen_res_w=1366, screen_res_h=768;
sfVector2f mouse_pos;
//FLAGS
//TODO change short ints flags to boolean or enum types

//draw grid on map
sfBool draw_grid=1;
 //draw highlight on map
sfBool draw_grid_highlight=0;
//draw minimap (currently only in menu)
sfBool draw_minimap=1;
//scrolling map with mouse (when cursor is close to the window's border)
sfBool camera_scroll=1;
//0 - if no units are selected, 1 - at least one unit is selected
short int units_selected=0;
//this flag is raised when player ends turn
short int end_turn=0;
//indicates number of active player
short int active_player=1;
//this flag is raised when player rotates units
short int change_rotation=0;
/**This flag indicates current game phase. Current phases and flag's values:
* 0 - main menu
* 1 - battle configuration menu
* 2 - battle
**/
short int game_phase=0;
//Game sub phase flag used in menus
short int game_sub_phase=0;
//1 - draw UI of selected units, 0 - contrary case
sfBool draw_unit_ui=0;
/**
*Attack mode flag. Current modes and flag's values:
* 0 - melee
* 1 - ranged
**/
short int attack_mode=0;


/**
* Available factions initialization.
**/
//TODO reading faction's definition from config file
unsigned short int number_of_factions=2;
faction faction_list[number_of_factions];
faction_list[0].name="SPQR";
faction_list[0].color=sfRed;
faction_list[1].name="Arverni";
faction_list[1].color=sfGreen;



//
unsigned short int number_of_players=2;

player player_list[number_of_players];
player_list[0].p_faction=&faction_list[0];
player_list[0].faction_index=0;
//player1_army_size=0;
player_list[1].p_faction=&faction_list[1];
player_list[1].faction_index=1;
    ///++++++++++++++++++++++++++++//

///+++++++++++++++++++++++++++++++++++++++++++++//
//Loading terrain texture
sfTexture* terr_texture;
terr_texture = sfTexture_createFromFile("gfx/terrain.png", NULL);
if (!terr_texture)
{
    game_log=add_log_string("ERROR: Brak tekstury terenu(gfx/terrain.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
    return 1;
}

///----------------------------------------//
///Map initialization
terrain g_map [max_map_h][max_map_w];
sfVertexArray* vertex_map=sfVertexArray_create();
sfVertexArray_setPrimitiveType(vertex_map, sfQuads);
sfRenderStates map_vertex_states;
map_vertex_states.blendMode= sfBlendNone;
map_vertex_states.shader=NULL;
map_vertex_states.texture=terr_texture;
sfTransform vertex_transform;

vertex_transform.matrix[0]=1; vertex_transform.matrix[1]=0; vertex_transform.matrix[2]=0;
vertex_transform.matrix[3]=0; vertex_transform.matrix[4]=1; vertex_transform.matrix[5]=0;
vertex_transform.matrix[6]=0; vertex_transform.matrix[7]=0; vertex_transform.matrix[8]=1;

map_vertex_states.transform=vertex_transform;

sfVertexArray* map_grid=sfVertexArray_create();
sfVertexArray_setPrimitiveType(map_grid, sfLinesStrip);

sfVertexArray* map_grid_highlight=sfVertexArray_create();
    sfVertexArray_setPrimitiveType(map_grid_highlight, sfQuads);

///Flora initialization
sfTexture* pine_tree;
pine_tree=sfTexture_createFromFile("gfx/tree2.png", NULL);
if (!pine_tree)
{
    game_log=add_log_string("ERROR: Brak tekstury pine_tree(gfx/tree2.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}

sfVertexArray* flora_array=sfVertexArray_create();
sfVertexArray_setPrimitiveType(flora_array, sfQuads);
sfRenderStates flora_array_states;
flora_array_states.blendMode=sfBlendAlpha;
flora_array_states.shader=NULL;
flora_array_states.texture=pine_tree;
flora_array_states.transform=vertex_transform;
//map_interpreter(map_def_file, "maps/map.txt", g_map);
generate_map(map_w, map_h, g_map);
create_map(g_map, vertex_map, map_grid, flora_array);

int i;
///----------------------//==============================//
///Loading UI and units' textures
// TODO loading in loop using faction info

    sfTexture* health_bar_tex;

health_bar_tex=sfTexture_createFromFile("gfx/health_bar.png", NULL);
if(!health_bar_tex)
{
        game_log=add_log_string("ERROR: Brak tekstury health_bar_tex(gfx/health_bar.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
sfFont* ui_font=sfFont_createFromFile("OptimusPrinceps.ttf");
if(!ui_font)
{
        game_log=add_log_string("ERROR: Brak czcionki Optimus Princeps(OptimusPrinceps.ttf).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
sfTexture* pilum_texture=sfTexture_createFromFile("gfx/pilum_2.png", NULL);;
if(!pilum_texture)
{
        game_log=add_log_string("ERROR: Brak tekstury pilum_texture(gfx/pilum_2.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
unit_type cingetos;
load_unit_stats(unit_def_file, "stats.txt", &cingetos, "Cingetos");

cingetos.texture=sfTexture_createFromFile("gfx/cingetos_atlas.png", NULL);
if(!cingetos.texture)
{
        game_log=add_log_string("ERROR: Brak tekstury Cingetos(gfx/cingetos_atlas.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
cingetos.missile_texture=pilum_texture;

faction_list[1].unit_list[0]=&cingetos;

unit_type solduros;
load_unit_stats(unit_def_file, "stats.txt", &solduros, "Solduros");
solduros.texture=sfTexture_createFromFile("gfx/cingetos_atlas.png", NULL);
if(!solduros.texture)
{
        game_log=add_log_string("ERROR: Brak tekstury Solduros(gfx/cingetos_atlas.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
solduros.missile_texture=pilum_texture;
faction_list[1].unit_list[1]=&solduros;

unit_type sotaroas;
load_unit_stats(unit_def_file, "stats.txt", &sotaroas, "Sotaroas");
sotaroas.texture=sfTexture_createFromFile("gfx/sotaroas_atlas_v1.png", NULL);
if(!sotaroas.texture)
{
        game_log=add_log_string("ERROR: Brak tekstury Sotaroas(gfx/sotaroas_atlas_v1.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
sotaroas.missile_texture=pilum_texture;

faction_list[1].unit_list[2]=&sotaroas;

unit_type evocati;
load_unit_stats(unit_def_file, "stats.txt", &evocati, "Evocati");
evocati.texture=sfTexture_createFromFile("gfx/evocati_atlas.png", NULL);
if(!evocati.texture)
{
        game_log=add_log_string("ERROR: Brak tekstury Evocati(gfx/evocati_atlas.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
evocati.missile_texture=pilum_texture;
evocati.portrait_texture=sfTexture_createFromFile("gfx/evocati_portrait.png", NULL);

faction_list[0].unit_list[0]=&evocati;

unit_type pedites_singulares;
load_unit_stats(unit_def_file, "stats.txt", &pedites_singulares, "Pedites_Singulares");
pedites_singulares.texture=sfTexture_createFromFile("gfx/evocati_atlas.png", NULL);
if(!pedites_singulares.texture)
{
        game_log=add_log_string("ERROR: Brak tekstury Pedites_Singulares(gfx/evocati_atlas.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
pedites_singulares.missile_texture=pilum_texture;
//pedites_singulares.portrait_texture=sfTexture_createFromFile("gfx/evocati_portrait.png", NULL);
strcpy(pedites_singulares.name, "Pedites Singulares");

faction_list[0].unit_list[1]=&pedites_singulares;

unit_type sagittari_auxilari;
load_unit_stats(unit_def_file, "stats.txt", &sagittari_auxilari, "Sagittari_Auxilari");
sagittari_auxilari.texture=sfTexture_createFromFile("gfx/sotaroas_atlas_v1.png", NULL);
if(!sagittari_auxilari.texture)
{
        game_log=add_log_string("ERROR: Brak tekstury Sagittari_Auxilari (gfx/sotaroas_atlas_v1.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
sagittari_auxilari.missile_texture=pilum_texture;
faction_list[0].unit_list[2]=&sagittari_auxilari;

unit_type catapulta;
load_unit_stats(unit_def_file, "stats.txt", &catapulta, "Catapulta");
catapulta.texture=sfTexture_createFromFile("gfx/catapult_atlas.png", NULL);
if(!catapulta.texture)
{
        game_log=add_log_string("ERROR: Brak tekstury Catapulta (gfx/catapult_atlas.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
catapulta.missile_texture=pilum_texture;
faction_list[0].unit_list[3]=&catapulta;


//Health bar size vector
sfVector2f hp_bar_dynamic;
hp_bar_dynamic.y=15;
sfColor hp_bar_color;
hp_bar_color.g=250;hp_bar_color.b=0;hp_bar_color.r=0;hp_bar_color.a=250;




short int player1_army_size=0;
unit player1_unit_list [MAX_UNITS];

short int player2_army_size=0;
unit player2_unit_list [MAX_UNITS];
///---------------------//=====================================//

///Main CSFML structures initialization

sfVideoMode mode = {screen_res_w, screen_res_h, 32};

sfRenderWindow* window;

sfView* view ;

sfEvent event;

sfClock* clock;
clock=sfClock_create();

///Window creation
window = sfRenderWindow_create(mode, "CSFML_TS", sfResize | sfClose, NULL);
if (!window)
{
  game_log=add_log_string("ERROR: Nie mo¿na utworzyc okna.", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
  return 1;
}

sfRenderWindow_setFramerateLimit(window, 60);

//-----------------------------//
//UI

///Main menu - game_phase: 0
///==================================================================//

sfRectangleShape* main_background;
main_background=sfRectangleShape_create();
sfVector2f main_background_size;
main_background_size.x=screen_res_w;
main_background_size.y=screen_res_h;
sfRectangleShape_setSize(main_background, main_background_size);
sfTexture* main_background_texture;
main_background_texture=sfTexture_createFromFile("gfx/main_background_darker.png", NULL);
if(!main_background_texture)
{
      game_log=add_log_string("ERROR: Brak tekstury tla menu (gfx/main_background_darker.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
sfRectangleShape_setTexture(main_background, main_background_texture, sfNone);

menu main_menu;
main_menu.button_count=2;
 sfTexture* button_background;
 button_background=sfTexture_createFromFile("gfx/button_background.png", NULL);
 if(!button_background)
{
      game_log=add_log_string("ERROR: Brak tekstury button background (gfx/button_background.png).", game_log);
    update_log_file(log_file, "logs/log.txt", game_log);
}
  for(i=0; i<main_menu.button_count; ++i)
  {
      main_menu.button[i]=sfRectangleShape_create();
      main_menu.text[i]=sfText_create();
      sfText_setFont(main_menu.text[i], ui_font);
      sfRectangleShape_setTexture(main_menu.button[i], button_background, sfNone);
  }

create_main_menu(main_menu, screen_res_h, screen_res_w);

///Battle menu - game_phase: 1
///==================================================================//

menu battle_menu;
battle_menu.button_count=6;
//battle_menu.background=sfRectangleShape_create();
  for(i=0; i<battle_menu.button_count; ++i)
  {
      battle_menu.button[i]=sfRectangleShape_create();
      battle_menu.text[i]=sfText_create();
      sfText_setFont(battle_menu.text[i], ui_font);
      sfText_setCharacterSize(battle_menu.text[i], 15);
      sfRectangleShape_setTexture(battle_menu.button[i], button_background, sfNone);
  }

menu select_map_menu;

select_map_menu.draw=0;
select_map_menu.button_count=2;
select_map_menu.background=sfRectangleShape_create();
sfRectangleShape_setTexture(select_map_menu.background, button_background, sfNone);
  for(i=0; i<select_map_menu.button_count; ++i)
  {
      select_map_menu.button[i]=sfRectangleShape_create();
      select_map_menu.text[i]=sfText_create();
      sfText_setFont(select_map_menu.text[i], ui_font);
      sfRectangleShape_setTexture(select_map_menu.button[i], button_background, sfNone);
  }

menu army_select_menu;
army_select_menu.button_count=17;
army_select_menu.draw=0;
  for(i=0; i<army_select_menu.button_count; ++i)
  {
      army_select_menu.button[i]=sfRectangleShape_create();
      army_select_menu.text[i]=sfText_create();
      sfText_setFont(army_select_menu.text[i], ui_font);
      sfText_setCharacterSize(army_select_menu.text[i], 15);
      if(i!=10)
      sfRectangleShape_setTexture(army_select_menu.button[i], button_background, sfNone);
  }
      sfRectangleShape_setTexture(army_select_menu.button[10], evocati.portrait_texture, sfNone);

///Messages
///Currently only a placeholder
///==================================================================//
sfTexture* end_turn_msg_ico=sfTexture_createFromFile("gfx/test.png", NULL);
message end_turn_message;
end_turn_message.draw=0;
end_turn_message.draw_ico=0;
//ico
end_turn_message.ico=sfRectangleShape_create();
sfVector2f ico_size; ico_size.x=50; ico_size.y=75;
sfRectangleShape_setSize(end_turn_message.ico, ico_size);
sfRectangleShape_setTexture(end_turn_message.ico, end_turn_msg_ico, sfNone);
sfVector2f ico_position; ico_position.x=0; ico_position.y=screen_res_h/2;
sfRectangleShape_setPosition(end_turn_message.ico,ico_position );
//box
end_turn_message.background=sfRectangleShape_create();
end_turn_message.box_size.x=300;
end_turn_message.box_size.y=150;
sfRectangleShape_setSize(end_turn_message.background, end_turn_message.box_size);

sfVector2f msg_box_pos;
msg_box_pos.x=screen_res_w/2-end_turn_message.box_size.x;
msg_box_pos.y=screen_res_h/2-end_turn_message.box_size.y;

end_turn_message.background=sfRectangleShape_create();

sfRectangleShape_setPosition(end_turn_message.background, msg_box_pos);

sfRectangleShape_setFillColor(end_turn_message.background, sfYellow);

for(i=0;i<MAX_MESSAGE_BUTTONS;++i)
{
    end_turn_message.text[i]=sfText_create();
    sfText_setFont(end_turn_message.text[i], ui_font);
    sfText_setCharacterSize(end_turn_message.text[i], 15);
    end_turn_message.button[i]=sfRectangleShape_create();
    sfRectangleShape_setTexture(end_turn_message.text[i], button_background, sfNone);
}


///Initialization of game_phase: 2 UI elements
///==================================================================//
///Top Bar
bar top_bar;
top_bar.shape=sfRectangleShape_create();
sfVector2f top_bar_size;
top_bar_size.x=screen_res_w;
top_bar_size.y=screen_res_h/20;
sfTexture* top_bar_texture;
top_bar_texture=sfTexture_createFromFile("gfx/top_bar.png", NULL);

sfRectangleShape_setSize(top_bar.shape, top_bar_size);
sfRectangleShape_setFillColor(top_bar.shape, sfRed);
sfRectangleShape_setTexture(top_bar.shape, top_bar_texture, sfNone);

top_bar.text=sfText_create();
sfText_setCharacterSize(top_bar.text, 12);
sfText_setFont(top_bar.text, ui_font);
sfText_setColor(top_bar.text, sfWhite);


///-----------------------------//
///ROT_CIRCLE
sfCircleShape* rot_circle;
rot_circle=sfCircleShape_create();

///-----------------------------//
///VIEW BOX
sfFloatRect view_box;
view_box.left=0;
view_box.top=0;
view_box.width=screen_res_w;
view_box.height=screen_res_h;
///-----------------------------//
///SELECT BOX
sfRectangleShape* select_box;
select_box=sfRectangleShape_create();
sfRectangleShape_setOutlineThickness(select_box, 1);
              sfRectangleShape_setOutlineColor(select_box, sfWhite);
sfColor box_transp;
box_transp.a=50;
box_transp.r=0;box_transp.g=0;box_transp.b=0;
sfRectangleShape_setFillColor(select_box, box_transp );

sfVector2f select_vect_origin;
sfVector2f select_vect;


///-----------------------------//
///attack mode button
sfRectangleShape* attack_mode_button;
attack_mode_button=sfRectangleShape_create();
sfTexture* attack_mode_button_texture;
sfIntRect attack_mode_button_text_area;
attack_mode_button_text_area.top=0; attack_mode_button_text_area.left=0; attack_mode_button_text_area.height=200; attack_mode_button_text_area.width=200;
attack_mode_button_texture=sfTexture_createFromFile("gfx/attack_mode_button.png", NULL);
sfRectangleShape_setTexture(attack_mode_button, attack_mode_button_texture, sfNone);
sfRectangleShape_setTextureRect(attack_mode_button, attack_mode_button_text_area);
sfVector2f attack_mode_button_position; attack_mode_button_position.x=(screen_res_w/2)-200; attack_mode_button_position.y=screen_res_h-175;
sfVector2f attack_mode_button_size;
attack_mode_button_size.x=75;
attack_mode_button_size.y=75;
sfRectangleShape_setSize(attack_mode_button, attack_mode_button_size);
sfRectangleShape_setPosition(attack_mode_button, attack_mode_button_position);
///-----------------------------//
///Menu button
sfRectangleShape* menu_button;
menu_button=sfRectangleShape_create();

sfVector2f menu_button_position; menu_button_position.x=screen_res_w-100; menu_button_position.y=10;
sfVector2f menu_button_size;
menu_button_size.x=75;
menu_button_size.y=25;
sfRectangleShape_setSize(menu_button, menu_button_size);
sfRectangleShape_setPosition(menu_button, menu_button_position);
sfRectangleShape_setTexture(menu_button, button_background, sfNone);
/*
//-----------------------------//
//army_summ_button
sfRectangleShape* summary_button=sfRectangleShape_create();
sfRectangleShape_setSize(summary_button, attack_mode_button_size);
sfVector2f summary_button_pos; summary_button_pos.x=screen_res_w-100; summary_button_pos.y=200;
sfRectangleShape_setPosition(summary_button, summary_button_pos );
sfRectangleShape_setFillColor(summary_button, sfYellow);
*/
///-----------------------------//
///End game menu
menu end_game_menu;
end_game_menu.button_count=2;
end_game_menu.draw=0;
end_game_menu.background=sfRectangleShape_create();
sfRectangleShape_setTexture(end_game_menu.background, button_background, sfNone);
  for(i=0; i<end_game_menu.button_count; ++i)
  {
      end_game_menu.button[i]=sfRectangleShape_create();
      sfRectangleShape_setTexture(end_game_menu.button[i], button_background, sfNone);
      end_game_menu.text[i]=sfText_create();
      sfText_setFont(end_game_menu.text[i], ui_font);
  }


///-----------------------------//


///-----------------------------//
///CSFML views


///Main view
view=sfView_createFromRect(view_box);
sfVector2f camera_vect;
sfVector2f view_size;
//view_size.x=1;
//view_size.y=2;
//sfView_setSize(view, view_size );
//sfView_setRotation(view, 26.5);
short int zoom_level=5;

///UI view
sfView* ui_view;
ui_view=sfView_createFromRect(view_box);

///Writing diag info to log
char diag_info [250];

sprintf(diag_info, "SCREEN HEIGHT: %d, SCREEN WIDTH: %d \n MAX MAP HEIGHT: %d, MAX MAP WIDTH: %d \n SIZE OF UNIT STRUCT: %d, SIZE OF UNIT TYPE STRUCT: %d, SIZE OF TERRAIN STRUCT: %d \n POPRAWNA INICJALIZACJA",
         screen_res_h, screen_res_w, max_map_h, max_map_w, sizeof(unit), sizeof(unit_type), sizeof(terrain));

game_log=add_log_string(diag_info, game_log);
update_log_file(log_file, "logs/log.txt", game_log);


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!////
/// GAME LOOP
while (sfRenderWindow_isOpen(window))
{
    ///Handling camera input
///%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
        if(sfKeyboard_isKeyPressed(sfKeyLeft) && sfView_getCenter(view).x-sfView_getSize(view).x/2+map_block_w>0)
        {
            camera_vect.x=-map_block_w/2;
            sfView_move(view,camera_vect);
        }
        else if(sfKeyboard_isKeyPressed(sfKeyRight)&& sfView_getCenter(view).x<(map_w*map_block_w)-sfView_getSize(view).x/2+map_block_w)
        {
            camera_vect.x=map_block_w/2;
            sfView_move(view,camera_vect);
        }
        else if(sfKeyboard_isKeyPressed(sfKeyUp)&& sfView_getCenter(view).y-sfView_getSize(view).y/2+map_block_h>0)
        {
            camera_vect.y=-map_block_h/2;
            sfView_move(view,camera_vect);
        }
        else if(sfKeyboard_isKeyPressed(sfKeyDown)&& sfView_getCenter(view).y<((map_h*map_block_h)-sfView_getSize(view).y/2)+map_block_h)
        {
            camera_vect.y=map_block_h/2;
            sfView_move(view,camera_vect);
        }
        if(sfKeyboard_isKeyPressed(sfKeyTab))
        {
            sfView_rotate(view, 5);
        }
        if(sfKeyboard_isKeyPressed(sfKeyAdd))// &&zoom_level<=8)
        {
            sfView_zoom(view, 0.9);
            ++zoom_level;
        }
         if(sfKeyboard_isKeyPressed(sfKeySubtract))// && zoom_level>=2)
        {
            sfView_zoom(view, 1.1);
            --zoom_level;
        }

        if(camera_scroll==1)
        {

            if(sfMouse_getPosition(window).x>=0 &&sfMouse_getPosition(window).x<5 && sfView_getCenter(view).x-sfView_getSize(view).x/2+map_block_w>0)
            {
                camera_vect.x=-map_block_w/2;
                    sfView_move(view,camera_vect);
            }
            else if(sfMouse_getPosition(window).x>sfWindow_getSize(window).x-5 && sfView_getCenter(view).x<(map_w*map_block_w)-sfView_getSize(view).x/2+map_block_w )
            {
                camera_vect.x=map_block_w/2;
                    sfView_move(view,camera_vect);
            }
            else if (sfMouse_getPosition(window).y>=0 && sfMouse_getPosition(window).y<5+top_bar_size.y && sfView_getCenter(view).y-sfView_getSize(view).y/2+map_block_h>0)
            {
                camera_vect.y=-map_block_h/2;
                sfView_move(view,camera_vect);
            }
            else if (sfMouse_getPosition(window).y>sfWindow_getSize(window).y && sfView_getCenter(view).y<((map_h*map_block_h)-sfView_getSize(view).y/2)+map_block_h)
            {
                camera_vect.y=map_block_h/2;
                sfView_move(view,camera_vect);
            }
        }

        camera_vect.x=0;
        camera_vect.y=0;
///%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
    ///Event handling
   while (sfRenderWindow_pollEvent(window, &event))
    {
        if (event.type == sfEvtClosed)
            sfRenderWindow_close(window);
        if(sfKeyboard_isKeyPressed(sfKeyEscape))
            sfRenderWindow_close(window);
        ///Update view when window is resized
          if (event.type == sfEvtResized)
            {
                sfVector2f resize_vector;
                resize_vector.y= event.size.height;
                resize_vector.x=event.size.width;
                sfView_setSize(view, resize_vector);
            }
            ///Game phase: 0 event handling
            if(game_phase==0)
            {
                if(event.type==sfEvtMouseButtonPressed && event.mouseButton.button==sfMouseLeft)
                {
                        if((button_pressed(sfMouse_getPosition(window), main_menu.button[0]))==1)
                        {
                            create_select_map_menu(select_map_menu, screen_res_h,screen_res_w);
                            create_battle_menu(battle_menu, screen_res_h, screen_res_w);
                            create_army_select_menu(army_select_menu, screen_res_h,screen_res_w);
                            battle_menu.draw=1;
                          game_phase=1;
                        }
                        else if((button_pressed(sfMouse_getPosition(window), main_menu.button[1]))==1)
                        {
                             destroy_menu_resources(main_background, main_background_texture, button_background);
                          destroy_menu(main_menu,0);
                          destroy_menu(select_map_menu,1);
                          sfRenderWindow_close(window);
                        }

                }
            }
            ///Game phase: 1 event handling
            else if(game_phase==1)
            {
                unsigned short int new_map_h;
               // unsigned short int new_map_w;
                short int change_army_size;
                short int change_map_dim;
                if(change_map_dim==1)
                {
                    if(event.text.unicode==58|| (event.type==sfEvtMouseButtonPressed && event.mouseButton.button==sfMouseLeft))
                    {
                        char conv [5];
                        strncpy(conv,sfText_getString(battle_menu.text[4]), 4);
                        sscanf(conv, "%hu", &new_map_h);
                        sfText_setString(battle_menu.text[5]," ");
                        change_map_dim=3;
                    }
                    if ((unsigned)event.text.unicode<58 && (unsigned)event.text.unicode>=48)
                            get_text_input(event, battle_menu.text[4]);
                }
                /*
                else if(change_map_dim==2)
                {
                    if(zdarzenie.text.unicode==58)
                    {
                        char conv [4];
                        strncpy(conv,sfText_getString(battle_menu.text[5]), 3);
                        sscanf(conv, "%d", &new_map_w);
                        change_map_dim=3;
                    }
                    if ((unsigned)zdarzenie.text.unicode<58 && (unsigned)zdarzenie.text.unicode>=48)
                            get_text_input(zdarzenie, battle_menu.text[5]);
                }
                */
                else if(change_map_dim==3)
                {
                    if(new_map_h>max_map_h || new_map_h==0)
                    new_map_h=100;
                     map_h=new_map_h;
                     map_w=new_map_h;
                    draw_minimap=0;
                    sfVertexArray_destroy(vertex_map);
                    sfVertexArray_destroy(map_grid);
                    vertex_map=sfVertexArray_create();
                    sfVertexArray_setPrimitiveType(vertex_map, sfQuads);
                    map_grid=sfVertexArray_create();
                    sfVertexArray_setPrimitiveType(map_grid, sfLinesStrip);
                     generate_map(map_w, map_h, g_map);
                     create_map(g_map,vertex_map,map_grid,flora_array);
                     draw_minimap=1;
                     change_map_dim=0;
                }
                            if(change_army_size==1)
                            {
                                    if(event.text.unicode==58||(event.type==sfEvtMouseButtonPressed && event.mouseButton.button==sfMouseLeft))
                                    {
                                        char conv [5];
                                        strncpy(conv,sfText_getString(army_select_menu.text[12]), 4);
                                        sscanf(conv, "%hu", &player1_army_size);
                                        change_army_size=0;

                                    }
                               if ((unsigned)event.text.unicode<58 && (unsigned)event.text.unicode>=48)
                                   get_text_input(event, army_select_menu.text[12]);

                            }
                            else if(change_army_size==2)
                            {
                                    if(event.text.unicode==58||(event.type==sfEvtMouseButtonPressed && event.mouseButton.button==sfMouseLeft))
                                    {
                                        char conv [5];
                                        strncpy(conv,sfText_getString(army_select_menu.text[15]), 4);
                                        sscanf(conv, "%hu", &player2_army_size);
                                        change_army_size=0;
                                    }
                               if ((unsigned)event.text.unicode<58 && (unsigned)event.text.unicode>=48)
                            get_text_input(event, army_select_menu.text[15]);
                            }
                /*
                if(select_map_menu.draw==1)
                        {
                            if ((unsigned)zdarzenie.text.unicode<128 && (unsigned)zdarzenie.text.unicode>=48)
                            get_text_input(zdarzenie, select_map_menu.text[1]);
                        }
                */
                if(event.type==sfEvtMouseButtonPressed && event.mouseButton.button==sfMouseLeft)
                {
                        if((button_pressed(sfMouse_getPosition(window), battle_menu.button[0]))==1)
                        {
                          army_select_menu.draw=1;
                          game_sub_phase=1;
                          battle_menu.draw=0;
                        }
                        else if((button_pressed(sfMouse_getPosition(window), battle_menu.button[3]))==1)
                        {
                               if(select_map_menu.draw==1)
                                select_map_menu.draw=0;
                                else
                            game_phase=0;
                        }
                        else if ((button_pressed(sfMouse_getPosition(window), battle_menu.button[2]))==1)
                        {
                            sfText_setString(battle_menu.text[4]," ");
                            change_map_dim=1;
                        }
                        /*
                        else if ((button_pressed(sfMouse_getPosition(window), battle_menu.button[1]))==1)
                        {
                            select_map_menu.draw=1;
                            //load_map_from_file(select_map_menu.text[0]);
                        }

                        else if((button_pressed(sfMouse_getPosition(window), battle_menu.button[4]))==1)
                        {
                             //map_h=(int)sfText_getString(battle_menu.text[4]);
                        }
                        */
                        else if(army_select_menu.draw==1)
                        {
                                if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[8]))==1)
                            {
                              //  destroy_menu(main_menu, 0);
                              // destroy_menu(select_map_menu,1);
                              // destroy_menu(battle_menu,0);
                             //  destroy_menu(army_select_menu,0);
                             // destroy_menu_resources(main_background, main_background_texture, button_background);
                              assign_selected_units(player1_unit_list, player1_army_size, player_list[0].p_faction);
                              assign_selected_units(player2_unit_list, player2_army_size, player_list[1].p_faction);
                              sfVector2i player_1_deploy;
                                player_1_deploy.x=map_block_w; player_1_deploy.y=(player1_army_size/10)*map_block_h;
                                sfVector2i player_2_deploy;
                                player_2_deploy.x=map_block_w;player_2_deploy.y=(map_h-1)*map_block_h-(player2_army_size/10)*map_block_h;
                                set_army_direction(player1_army_size, player1_unit_list, 2);
                              create_army(player1_army_size, player1_unit_list, player_1_deploy, g_map, health_bar_tex, ui_font );
                               create_army(player2_army_size, player2_unit_list, player_2_deploy, g_map, health_bar_tex, ui_font );
                              game_phase=2;
                            }
                               else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[9]))==1)
                            {
                                game_sub_phase=0;
                                battle_menu.draw=1;
                                army_select_menu.draw=0;
                            }
                            else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[0]))==1)
                            {
                                if(player_list[0].faction_index>0)
                                {
                                    --player_list[0].faction_index;
                                    player_list[0].p_faction=&faction_list[player_list[0].faction_index];
                                    sfRectangleShape_setFillColor(army_select_menu.button[4], player_list[0].p_faction->color);
                                }
                            }
                            else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[1]))==1)
                            {
                                if(player_list[0].faction_index<(faction_count-1))
                                {
                                    ++player_list[0].faction_index;
                                    player_list[0].p_faction=&faction_list[player_list[0].faction_index];
                                    sfRectangleShape_setFillColor(army_select_menu.button[4], player_list[0].p_faction->color);
                                }
                            }
                           else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[2]))==1)
                            {
                                if(player_list[1].faction_index>0)
                                {
                                    --player_list[1].faction_index;
                                    player_list[1].p_faction=&faction_list[player_list[1].faction_index];
                                    sfRectangleShape_setFillColor(army_select_menu.button[5], player_list[1].p_faction->color);
                                }
                            }
                           else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[3]))==1)
                            {
                                if(player_list[1].faction_index<(faction_count-1))
                                {
                                    ++player_list[1].faction_index;
                                    player_list[1].p_faction=&faction_list[player_list[1].faction_index];
                                    sfRectangleShape_setFillColor(army_select_menu.button[5], player_list[0].p_faction->color);
                                }
                            }
                           else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[11]))==1)
                            {
                                if(player1_army_size>0)
                                {
                                    --player1_army_size;
                                }

                              char buff[30];
                              sprintf(buff, "%d", player1_army_size);
                              sfText_setString(army_select_menu.text[12], buff );
                            }
                           else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[12]))==1)
                            {
                                 sfText_setString(army_select_menu.text[12]," ");
                                change_army_size=1;
                            }
                           else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[13]))==1)
                            {
                                if(player1_army_size<MAX_UNITS-1)
                              ++player1_army_size;
                              char buff[30];
                              sprintf(buff, "%d", player1_army_size);
                              sfText_setString(army_select_menu.text[12], buff );
                            }
                           else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[14]))==1)
                            {
                                if(player2_army_size>0)
                                {
                                    --player2_army_size;
                                }
                              char buff[30];
                              sprintf(buff, "%d", player2_army_size);
                              sfText_setString(army_select_menu.text[15], buff );
                            }
                           else if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[15]))==1)
                            {
                                sfText_setString(army_select_menu.text[15]," ");
                                change_army_size=2;
                            }
                           else  if((button_pressed(sfMouse_getPosition(window), army_select_menu.button[16]))==1)
                            {
                                if(player2_army_size<MAX_UNITS-1)
                              ++player2_army_size;
                              char buff[30];
                              sprintf(buff, "%d", player2_army_size);
                              sfText_setString(army_select_menu.text[15], buff );
                            }


                        }




                }
            }



            ///Game phase: 2 event handling
            else if(game_phase==2)
            {
              ///Show grid on map
                if(event.type==sfEvtKeyPressed && event.key.code==sfKeyG)
                {

                    sfTime cooldown;
                    cooldown.microseconds = 100000;
                    if(draw_grid==0 && sfClock_getElapsedTime(clock).microseconds>cooldown.microseconds)
                    {
                       draw_grid=1;
                       sfClock_restart(clock);
                    }

                    else if (sfClock_getElapsedTime(clock).microseconds>cooldown.microseconds)
                    {
                        draw_grid=0;
                        sfClock_restart(clock);
                    }

                }
        ///Unit rotation
                else if(event.type==sfEvtKeyPressed && event.key.code==sfKeyR)
                {
                    if(active_player==1)
                        {
                                for (i=0;i<player1_army_size;++i)
                                {
                                    if(player1_unit_list[i].selected==1)
                                      change_unit_rotation(&player1_unit_list[i], mouse_pos);
                                }
                        }
                        else if (active_player==2)
                        {
                                for (i=0;i<player2_army_size;++i)
                                {
                                     if(player2_unit_list[i].selected==1)
                                      change_unit_rotation(&player2_unit_list[i], mouse_pos);
                                }
                        }
                }
        ///Select all units
                else if(sfKeyboard_isKeyPressed(sfKeyA) && sfKeyboard_isKeyPressed(sfKeyLControl))
                {
                    if(active_player==1)
                    {
                     select_all(player1_army_size,player1_unit_list, map_grid_highlight,&draw_unit_ui, &draw_grid_highlight);

                    }
                    else if (active_player==2)
                    {
                       select_all(player2_army_size, player2_unit_list, map_grid_highlight, &draw_unit_ui, &draw_grid_highlight);
                    }

                }


///Mapping mouse position to main view coordinates
mouse_pos=sfRenderWindow_mapPixelToCoords(window, sfMouse_getPosition(window), view);

        ///Left click events
        if(event.type==sfEvtMouseButtonPressed && event.mouseButton.button==sfMouseLeft)
        {
                    ///return to main menu when battle ends
                    if(end_game_menu.draw==1 && button_pressed(sfMouse_getPosition(window), end_game_menu.button[1]))
                    {
                        destroy_army(player1_army_size, player1_unit_list);
                        destroy_army(player2_army_size, player2_unit_list);
                        clear_log(game_log);
                        player1_army_size=0; player2_army_size=0;
                        select_map_menu.draw=0;
                        end_game_menu.draw=0;
                        battle_menu.draw=0;
                        active_player=1;
                        game_sub_phase=0;
                        game_phase=0;
                    }
            ///Attack mode button
            if((button_pressed_2f(sfRenderWindow_mapPixelToCoords(window, sfMouse_getPosition(window), ui_view), attack_mode_button))==1)
            {
                if(attack_mode==0)
                {
                    attack_mode=1;
                    attack_mode_button_text_area.left=200;
                    sfRectangleShape_setTextureRect(attack_mode_button, attack_mode_button_text_area);
                }

                else if(attack_mode==1)
                {
                    attack_mode=0;
                    attack_mode_button_text_area.left=0;
                    sfRectangleShape_setTextureRect(attack_mode_button, attack_mode_button_text_area);
                }

            }
            ///unselecting units
            else
            {
             select_vect_origin.x=mouse_pos.x;
                select_vect_origin.y=mouse_pos.y;

                if(active_player==1)
                {
                        for (i=0;i<player1_army_size;++i)
                        {
                            draw_unit_ui=0;
                                deselect_unit(&player1_unit_list[i]);
                        }
                    map_remove_highlight(map_grid_highlight, &draw_grid_highlight);
                }
                else if (active_player==2)
                {
                        for (i=0;i<player2_army_size;++i)
                        {
                            draw_unit_ui=0;
                               deselect_unit(&player2_unit_list[i]);
                               sfSprite_setColor(player2_unit_list[i].sprite, sfBlue);
                        }
                        map_remove_highlight(map_grid_highlight,&draw_grid_highlight);
                }

                units_selected=1;
            }

        }

                if(event.type==sfEvtMouseButtonReleased && event.mouseButton.button==sfMouseLeft && units_selected==1)
        {
                units_selected=0;

        }
        ///Selecting units
        if(sfMouse_isButtonPressed(sfMouseLeft) &&units_selected==1)
        {
                sfRectangleShape_setPosition(select_box, select_vect_origin);
                select_vect.x=(mouse_pos.x-select_vect_origin.x);
                select_vect.y=(mouse_pos.y-select_vect_origin.y);
                sfRectangleShape_setSize(select_box, select_vect);

                //petla sprawdzajaca zaznaczone jednostki
                if(active_player==1)
                {
                        for (i=0;i<player1_army_size;++i)
                        {
                           if(player1_unit_list[i].men>0 && player1_unit_list[i].selected==0)
                           {
                               if(select_check(&player1_unit_list[i], select_box)==1)
                               {
                                draw_unit_ui=1;
                                map_highlight(player1_unit_list[i].position,player1_unit_list[i].speed, map_grid_highlight, &draw_grid_highlight, sfYellow);
                                if(player1_unit_list[i].type->command==1)
                                map_highlight(player1_unit_list[i].position,player1_unit_list[i].type->command_range, map_grid_highlight, &draw_grid_highlight, sfBlue);
                                create_infobox(player1_unit_list[i], player1_unit_list[i].position);
                                player1_unit_list[i].draw_infobox=1;
                               }

                           }

                        }
                }
                else if(active_player==2)
                {
                        for (i=0;i<player2_army_size;++i)
                        {
                           if(player2_unit_list[i].men>0 &&player2_unit_list[i].selected==0)
                           {
                               if(select_check(&player2_unit_list[i], select_box)==1)
                               {
                                    draw_unit_ui=1;
                               map_highlight(player2_unit_list[i].position,player2_unit_list[i].speed, map_grid_highlight, &draw_grid_highlight, sfYellow);
                               if(player2_unit_list[i].type->command==1)
                                map_highlight(player2_unit_list[i].position,player2_unit_list[i].type->command_range, map_grid_highlight, &draw_grid_highlight, sfBlue);
                               create_infobox(player2_unit_list[i], player2_unit_list[i].position);
                                player2_unit_list[i].draw_infobox=1;
                               }

                           }
                        }
                }
            }
            ///============================================================================================//
        ///Movement and attack events handling
        if (sfMouse_isButtonPressed(sfMouseRight) && (0<mouse_pos.x && mouse_pos.x<(map_w*map_block_w))
            &&(0<mouse_pos.y && mouse_pos.y<(map_h*map_block_h)))
        {
            ///Movement
            if(g_map[(int)((mouse_pos.y)/map_block_h)][(int)((mouse_pos.x)/map_block_w)].occupation==0)
            {
                 if(active_player==1)
            {
                create_movement_orders(player1_army_size, player1_unit_list, mouse_pos, g_map);
            }

                    else if(active_player==2)
                    {
                    create_movement_orders(player2_army_size, player2_unit_list, mouse_pos, g_map);
                    }
            }
            ///Attack

           else if(g_map[(int)((mouse_pos.y)/(map_block_h))][(int)((mouse_pos.x)/(map_block_w))].occupation==1)
            {

                 if(active_player==1)
                 {
                     create_attack_orders(player1_army_size, player1_unit_list, player2_army_size, player2_unit_list, mouse_pos, g_map,
                                          attack_direction(mouse_pos, g_map[(int)((mouse_pos.y)/(map_block_h))][(int)((mouse_pos.x)/(map_block_w))]),
                                          attack_mode);
                 }
                 else if(active_player==2)
                 {
                     create_attack_orders(player2_army_size, player2_unit_list, player1_army_size, player1_unit_list, mouse_pos, g_map,
                                          attack_direction(mouse_pos, g_map[(int)((mouse_pos.y)/(map_block_h))][(int)((mouse_pos.x)/(map_block_w))]),
                                          attack_mode);
                 }
            }



        }
            ///End turn event
            if(event.type==sfEvtKeyPressed && event.key.code==sfKeySpace)
            {
                ///applying morale bonus from commander
                if(active_player==1)
                    command_morale_bonus(player1_army_size, player1_unit_list);
                if(active_player==2)
                    command_morale_bonus(player2_army_size, player2_unit_list);
                end_turn=1;
                end_turn_message.draw_ico=1;
            }


        }


    }

/**
            RENDERING SECTION
**/

sfRenderWindow_clear(window, sfBlack);

if(game_phase==0)
{
    sfRenderWindow_drawRectangleShape(window, main_background, NULL);
    //sfRenderWindow_drawRectangleShape(window, main_menu.background, NULL);
    short int i;
    for (i=0; i<main_menu.button_count; ++i)
    {
        sfRenderWindow_drawRectangleShape(window, main_menu.button[i], NULL);
        sfRenderWindow_drawText(window, main_menu.text[i], NULL);
    }

}

else if(game_phase==1)
{
    sfRenderWindow_setView(window, ui_view);
    sfRenderWindow_drawRectangleShape(window, main_background, NULL);
   // sfRenderWindow_drawRectangleShape(window, battle_menu.background, NULL);
    short int i;
    if(battle_menu.draw==1)
    {
        for (i=0; i<battle_menu.button_count; ++i)
        {
            sfRenderWindow_drawRectangleShape(window, battle_menu.button[i], NULL);
            sfRenderWindow_drawText(window, battle_menu.text[i], NULL);
        }

    }

    if(game_sub_phase==0)
    {
        ///minimap #
        ///###############################///
        sfFloatRect mini_map_viewport;
        mini_map_viewport.height=0.5; mini_map_viewport.width=0.5; mini_map_viewport.top=0.25; mini_map_viewport.left=0.25;
        sfVector2f mini_map_center;
        mini_map_center.y=(map_h*map_block_h)/2;
        mini_map_center.x=(map_w*map_block_w)/2;
        sfView* mini_map;
        mini_map=sfView_createFromRect(view_box);
        sfView_setViewport(mini_map,mini_map_viewport);
        sfView_setCenter(mini_map, mini_map_center);
        //sfView_setSize(mini_map, mini_map_center);
        sfView_zoom(mini_map, 10+(map_w/10)*10);
        ///minimap draw
        if(draw_minimap==1)
        {
            sfRenderWindow_setView(window, mini_map);
            sfRenderWindow_drawVertexArray(window, vertex_map, &map_vertex_states);
            sfRenderWindow_drawVertexArray(window, map_grid, NULL);
        }
        ///###############################///
        sfRenderWindow_setView(window, ui_view);
        if(select_map_menu.draw==1)
        {
                        sfRenderWindow_drawRectangleShape(window,select_map_menu.background, NULL);
                            for (i=0; i<select_map_menu.button_count; ++i)
                {
                    sfRenderWindow_drawRectangleShape(window, select_map_menu.button[i], NULL);
                    sfRenderWindow_drawText(window, select_map_menu.text[i], NULL);
                }

        }
    }
    else if(game_sub_phase==1)
    {
          for (i=0; i<army_select_menu.button_count; ++i)
                {
                    sfRenderWindow_drawRectangleShape(window, army_select_menu.button[i], NULL);
                    sfRenderWindow_drawText(window, army_select_menu.text[i], NULL);
                }
    }

}


///Game phase: 2 Rendering and game logic
else if(game_phase==2)
{
  sfRenderWindow_setView(window,view);
if(active_player==1)
    sfRectangleShape_setFillColor(top_bar.shape, player_list[0].p_faction->color);
else
    sfRectangleShape_setFillColor(top_bar.shape, player_list[1].p_faction->color);
///Executing orders
if(end_turn==1)
{
    if(active_player==1)
    {

       if(game_mech(player1_army_size, player1_unit_list,player2_army_size, player2_unit_list, &end_turn, &active_player
                 ,g_map)==1)
                  {
                      create_end_game_menu(end_game_menu, screen_res_h,screen_res_w, "Wygral drugi gracz.");
                      end_game_menu.draw=1;
                  }
                men_and_morale_counter_update(player2_army_size, player2_unit_list, top_bar.text);

    }
    else if(active_player==2)
    {
        if(game_mech(player2_army_size, player2_unit_list,player1_army_size ,player1_unit_list, &end_turn, &active_player
                  ,g_map)==1)
                  {
                      create_end_game_menu(end_game_menu, screen_res_h,screen_res_w, "Wygral pierwszy gracz.");
                      end_game_menu.draw=1;
                  }


                  men_and_morale_counter_update(player1_army_size, player1_unit_list, top_bar.text);
    }
}

    ///Map Rendering
    sfRenderWindow_drawVertexArray(window, vertex_map, &map_vertex_states);

     if(draw_grid==1)
     {
         sfRenderWindow_drawVertexArray(window, map_grid, NULL);
         if(draw_grid_highlight==1)
         sfRenderWindow_drawVertexArray(window, map_grid_highlight,NULL);
     }
///Select box rendering
if(units_selected==1)
sfRenderWindow_drawRectangleShape(window, select_box, NULL);
///--------------------------------------------------------------------------
///Units rendering

///player 1
for(i=0;i<player1_army_size;++i)
{
   // if((sfView_getCenter(view).x-(view_box.width/2))<=player1_unit_list[i].position.x && player1_unit_list[i].position.x<=view_box.width
   // &&((sfView_getCenter(view).y-(view_box.height/2))<=player1_unit_list[i].position.y && player1_unit_list[i].position.y<=view_box.height))

    if(player1_unit_list[i].men>0)
    {
        //sfRenderWindow_drawSprite(window, player1_unit_list[i].sprite, NULL);
        sfRenderWindow_drawVertexArray(window, player1_unit_list[i].vertex_array,  &player1_unit_list[i].vertex_state);
        hp_bar_update(player1_unit_list[i], hp_bar_color, hp_bar_dynamic);
            if(player1_unit_list[i].draw_line==1)
            sfRenderWindow_drawPrimitives(window, player1_unit_list[i].line, 2, sfLines, NULL);

        sfRenderWindow_drawRectangleShape(window, player1_unit_list[i].health_bar.shape, NULL);
        sfRenderWindow_drawText(window, player1_unit_list[i].health_bar.text, NULL);
                if(player1_unit_list[i].draw_infobox==1)
                {
                    sfRenderWindow_drawRectangleShape(window, player1_unit_list[i].infobox.shape, NULL);
                    sfRenderWindow_drawText(window, player1_unit_list[i].infobox.text, NULL);
                }
                if(player1_unit_list[i].attack_order==2)
                sfRenderWindow_drawVertexArray(window,player1_unit_list[i].missile_vertex_array,  &player1_unit_list[i].missile_vertex_state );
    }

}
///player 2
for(i=0;i<player2_army_size;++i)
{
  //  if((sfView_getCenter(view).x-(view_box.width/2))<=player2_unit_list[i].position.x && player2_unit_list[i].position.x<=view_box.width
  //         &&((sfView_getCenter(view).y-(view_box.height/2))<=player2_unit_list[i].position.y && player2_unit_list[i].position.y<=view_box.height))
  if(player2_unit_list[i].men>0)
  {
                        //sfRenderWindow_drawSprite(window, player2_unit_list[i].sprite, NULL);
                        sfRenderWindow_drawVertexArray(window, player2_unit_list[i].vertex_array,  &player2_unit_list[i].vertex_state);
                        if(player2_unit_list[i].draw_line==1)
                        sfRenderWindow_drawPrimitives(window, player2_unit_list[i].line, 2, sfLines, NULL);
                        //---------------------//
                        //obsluga dynamiki hp_bara
                        hp_bar_update(player2_unit_list[i], hp_bar_color, hp_bar_dynamic);
                        sfRenderWindow_drawRectangleShape(window, player2_unit_list[i].health_bar.shape, NULL);
                        sfRenderWindow_drawText(window, player2_unit_list[i].health_bar.text, NULL);
                        //------------------------//
                            if(player2_unit_list[i].draw_infobox==1)
                        {
                            sfRenderWindow_drawRectangleShape(window, player2_unit_list[i].infobox.shape, NULL);
                            sfRenderWindow_drawText(window, player2_unit_list[i].infobox.text, NULL);
                        }
  }

}
///Flora rendering
sfRenderWindow_drawVertexArray(window, flora_array, &flora_array_states);
    ///UI rendering
    if(change_rotation==1)
        sfRenderWindow_drawCircleShape(window, rot_circle, NULL);
    sfRenderWindow_setView(window,ui_view);
    sfRenderWindow_drawRectangleShape(window,top_bar.shape, NULL);
    sfRenderWindow_drawText(window, top_bar.text, NULL);
    if(draw_unit_ui==1)
    sfRenderWindow_drawRectangleShape(window,attack_mode_button, NULL);
    if(end_turn_message.draw_ico==1)
        sfRenderWindow_drawRectangleShape(window, end_turn_message.ico, NULL);
    if(end_turn_message.draw==1)
    {
        sfRenderWindow_drawRectangleShape(window, end_turn_message.background, NULL);
        for(i=0;i<MAX_MESSAGE_BUTTONS;++i)
        {
           sfRenderWindow_drawRectangleShape(window, end_turn_message.button[i], NULL);
           sfRenderWindow_drawText(window, end_turn_message.text[i], NULL);
        }
    }


    if(end_game_menu.draw==1)
    {
        sfRenderWindow_drawRectangleShape(window, end_game_menu.background, NULL);
                  for (i=0; i<end_game_menu.button_count; ++i)
                    {
                        sfRenderWindow_drawRectangleShape(window, end_game_menu.button[i], NULL);
                        sfRenderWindow_drawText(window, end_game_menu.text[i], NULL);
                    }
    }
sfRenderWindow_drawRectangleShape(window, menu_button, NULL);

}


//---------------------------------------------------------
sfRenderWindow_display(window);
}

///Destruction
clear_log(game_log);
destroy_army(player1_army_size, player1_unit_list);
destroy_army(player2_army_size, player2_unit_list);
destroy_menu(end_game_menu, 1);
destroy_menu(main_menu, 0);
destroy_menu(select_map_menu,1);
destroy_menu(battle_menu,0);
destroy_menu(army_select_menu,0);
destroy_menu_resources(main_background, main_background_texture, button_background);
destroy_message (end_turn_message);
sfVertexArray_destroy(vertex_map);
sfVertexArray_destroy(map_grid);
sfVertexArray_destroy(map_grid_highlight);
sfVertexArray_destroy(flora_array);
  sfTexture_destroy(terr_texture);
  sfTexture_destroy(evocati.texture);
  sfTexture_destroy(cingetos.texture);
  sfTexture_destroy(solduros.texture);
  sfTexture_destroy(sotaroas.texture);
  sfTexture_destroy(sagittari_auxilari.texture);
  sfTexture_destroy(pedites_singulares.texture);
  sfTexture_destroy(catapulta.texture);
  sfTexture_destroy(pilum_texture);
  sfTexture_destroy(evocati.portrait_texture);
  sfTexture_destroy(health_bar_tex);
  sfTexture_destroy(top_bar_texture);
  sfTexture_destroy(attack_mode_button_texture);
  sfTexture_destroy(button_background);
  sfTexture_destroy(pine_tree);
  sfTexture_destroy(end_turn_msg_ico);
  sfFont_destroy(ui_font);
  sfRectangleShape_destroy(select_box);
  sfRectangleShape_destroy(top_bar.shape);
  sfText_destroy(top_bar.text);
  sfRectangleShape_destroy(attack_mode_button);
  sfRectangleShape_destroy(menu_button);
 // sfRectangleShape_destroy(summary_button);
  sfCircleShape_destroy(rot_circle);
  sfClock_destroy(clock);
  sfView_destroy(view);
  sfView_destroy(ui_view);
  sfRenderWindow_destroy(window);

  return 0;
}
