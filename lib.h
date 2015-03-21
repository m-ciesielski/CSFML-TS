#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/System.h>
#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define players_count 2
#define map_texture_size 200
#define unit_texture_height 600
#define unit_texture_width 300
#define map_block_h 800
#define map_block_w 800
#define vertex_tile 80
#define terrain_types 3
#define faction_count 2

#include "type_definitions.h"
extern unsigned  int map_h;
extern unsigned  int map_w;
extern unsigned  int max_map_h;
extern unsigned  int max_map_w;


extern log_list game_log;
extern FILE* log_file;

//--------------------------------------------//
///#######################################################################################################///
///MENU FUNCTIONS
void create_main_menu (Menu main_menu, short int screen_res_h, short int screen_res_w);
void destroy_menu (Menu main_menu, sfBool background);
void create_select_map_menu (Menu select_map_menu, short int screen_res_h, short int screen_res_w);
void create_battle_menu (Menu battle_menu, short int screen_res_h, short int screen_res_w);
void create_army_select_menu(Menu army_select_menu,short int screen_res_h,short int screen_res_w);
void get_text_input (sfEvent event, sfText* text);
void destroy_menu_resources (sfRectangleShape* main_background,
                         sfTexture* main_background_texture, sfTexture* button_background);
short int button_pressed (sfVector2i mouse_pos, sfRectangleShape* button);
short int button_pressed_2f (sfVector2f mouse_pos, sfRectangleShape* button);
void create_end_game_menu (Menu end_game_menu,short int screen_res_h, short int screen_res_w, char* message_text );
///#######################################################################################################///

///#######################################################################################################///
///AUXILARY FUNCTIONS
float precise_distance (sfVector2f object1_pos, sfVector2f object2_pos);
sfVector2f relative_position_vector (sfVector2f object1_pos, sfVector2f object2_pos);
short int distance (sfVector2i object, int destination_x, int destination_y);
void set_army_direction (int army_size, Unit unit_list[MAX_UNITS], short int direction);
///#######################################################################################################///

///#######################################################################################################///
///MOVEMENT FUNCTIONS
void movement_order (terrain g_map [max_map_h][max_map_w], Unit* unit1, sfVector2f mouse_pos);
short int create_movement_list (short int army_size,Unit army_list [MAX_UNITS], int movement_list [army_size], int dest_x, int dest_y);
int movement (Unit unit1);
void create_movement_orders (short int army_size, Unit unit_list[MAX_UNITS], sfVector2f mouse_pos, terrain g_map[max_map_h][max_map_w]);
sfBool check_movement_destination (short int army_size, Unit unit_list[MAX_UNITS], int dest_y, int dest_x, int index);
///#######################################################################################################///

///#######################################################################################################///
///MAP FUNCTIONS
void load_map_from_file (sfText* text);
void generate_map (unsigned int map_w, unsigned int map_h, terrain g_map [max_map_h][max_map_w]);
void map_interpreter (FILE* map_def_file,char* map_def_localisation, terrain g_map [max_map_h][max_map_w]);
short int create_map (terrain g_map [max_map_h][max_map_w], sfVertexArray* vertex_map, sfVertexArray* map_grid, sfVertexArray* flora_array);
void map_highlight (sfVector2f unit_pos, short int range, sfVertexArray* grid, sfBool* draw_grid_highlight, sfColor highlight);
///#######################################################################################################///

///#######################################################################################################///
///FLORA
void generate_flora (terrain g_map [max_map_h][max_map_w], sfVertexArray* flora_array);

///#######################################################################################################///

///#######################################################################################################///
///UI
void men_and_morale_counter_update (short int army_size, Unit army_list[MAX_UNITS], sfText* text);
short int rotation_direction (sfVector2f mouse_pos, sfVector2f unit_pos);
void change_unit_rotation (Unit* unit1, sfVector2f mouse_pos);
void deselect_unit (Unit* unit1);
int select_check (Unit* unit_1, sfRectangleShape* select_box);
void create_infobox (Unit unit_1, sfVector2f infobox_pos);
void map_remove_highlight(sfVertexArray* grid, sfBool* draw_grid_highlight);
void hp_bar_update (Unit unit1, sfColor hp_bar_color, sfVector2f hp_bar_dynamic);
void create_order_line (Unit* unit1, sfVector2f dest_pos, sfColor color);
void select_all (short int army_size, Unit unit_list[MAX_UNITS], sfVertexArray* map_grid_highlight ,
                  sfBool* draw_unit_ui, sfBool* draw_grid_highlight);
///#######################################################################################################///

///#######################################################################################################///
///COMBAT
short int attack_direction (sfVector2f mouse_pos, terrain terr);
void create_attack_orders(short int army_size, Unit unit_list[MAX_UNITS], short int op_army_size,
                          Unit op_unit_list[MAX_UNITS] ,sfVector2f mouse_pos, terrain g_map[max_map_h][max_map_w], short int direction,
                          short int attack_mode);
void melee_combat(Unit* att_unit, Unit* def_unit);
void ranged_combat (Unit* att_unit, Unit* def_unit);
int ranged_combat_animation (Unit* unit1, sfVector2f destination);
void apply_command_morale_bonus (int army_size, Unit unit_list[army_size]);
///#######################################################################################################///

///#######################################################################################################///
///GAME CORE FUNCTIONS
void update_vertex_array (int v_size ,sfVertexArray* v_array,
                                    sfVector2f position, short int direction);
void create_army (short int army_size, Unit unit_list[MAX_UNITS], sfVector2i deployment_pos, terrain g_map [max_map_h][max_map_w]
                  , sfTexture* health_bar_tex, sfFont* ui_font);
void destroy_army (short int army_size, Unit unit_list[MAX_UNITS]);
short int game_mech (short int army_size, Unit unit_list[MAX_UNITS], short int op_army_size,Unit op_unit_list[op_army_size],
                 short int* execute_orders, short int* active_player, terrain g_map[max_map_h][max_map_w]);
void assign_selected_units (Unit unit_list[MAX_UNITS], int army_size, Faction* army_faction);
void load_unit_stats(FILE* unit_def_file,char* unit_def_localisation, Unit_type* type, char* unit_name);

///#######################################################################################################///

///#######################################################################################################///
///LOG
log_list add_log_string (char add_text[MAX_STRING_LENGTH], log_list game_log);
log_list reverse_log (log_list game_log);
void update_log_file (FILE* log_file,char* log_file_localisation, log_list game_log);
void clear_log (log_list game_log);
///#######################################################################################################///

///#######################################################################################################///
///MESSAGES
void destroy_message (Message msg);


///#######################################################################################################///
