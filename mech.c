#include "lib.h"
/**
*Updates Vertex Array position and texture coordinates.
* v_size - number of vertices in vertex array(4*unit size for units)
* v_array - vertex array to update
* position - new position of vertex array
* direction - new direction of vertex array
**/
// TODO : cache optimization?
void update_vertex_array (int v_size ,sfVertexArray* v_array,
                                    sfVector2f position, short int direction)
{
    //clearing vertex array
    sfVertexArray_clear(v_array);
    short int i;
    unsigned int quad_count;
    if(v_size>4)
        quad_count=v_size/4;
    else
        quad_count=v_size;

    sfVertex temp_vertex;
    for(i=0;i<(quad_count);++i)
        {
            temp_vertex.position.x=position.x+(i%10)*vertex_tile;
            temp_vertex.position.y=position.y+(i/10)*vertex_tile;
            temp_vertex.color=sfWhite;
            temp_vertex.texCoords.x=direction*unit_texture_width;
            temp_vertex.texCoords.y=0;
            sfVertexArray_append(v_array, temp_vertex);

            temp_vertex.position.x=position.x+(i%10)*vertex_tile+vertex_tile;
            temp_vertex.position.y=position.y+(i/10)*vertex_tile;
            temp_vertex.color=sfWhite;
            temp_vertex.texCoords.x=direction*unit_texture_width+unit_texture_width;
            temp_vertex.texCoords.y=0;
            sfVertexArray_append(v_array, temp_vertex);

            temp_vertex.position.x=position.x+(i%10)*vertex_tile+vertex_tile;
            temp_vertex.position.y=position.y+(i/10)*vertex_tile+vertex_tile;
            temp_vertex.color=sfWhite;
            temp_vertex.texCoords.x=direction*unit_texture_width+unit_texture_width;
            temp_vertex.texCoords.y=unit_texture_height;
            sfVertexArray_append(v_array, temp_vertex);

            temp_vertex.position.x=position.x+(i%10)*vertex_tile;
            temp_vertex.position.y=position.y+(i/10)*vertex_tile+vertex_tile;
            temp_vertex.color=sfWhite;
            temp_vertex.texCoords.x=direction*unit_texture_width;
            temp_vertex.texCoords.y= unit_texture_height;
            sfVertexArray_append(v_array, temp_vertex);
        }
}

/**
*Assign units selected in menu to units array. Had to be reworked.
**/
void assign_selected_units (Unit unit_list[MAX_UNITS], int army_size, Faction* army_faction)
{
int i;


    for(i=0;i<army_size;++i)
    {
        if(i==0)
            unit_list[i].type=army_faction->unit_list[1];
        else if(i%10==0 && strcmp(unit_list[0].type->name, "Pedites Singulares")==0)
            unit_list[i].type=army_faction->unit_list[3];
        else if(i%5==0)
            unit_list[i].type=army_faction->unit_list[2];
        else
        unit_list[i].type=army_faction->unit_list[0];
    }
}

/**
*Initializes and deploy all units in army and creates some UI elements for each unit (currently health bar and info box).
* army_size - number of units in army
* unit_list - list of army units
* deployment_pos - deployment starting position (upper left corner)
* g_map - game map
* health_bar_tex - health bar texture
* ui_font - UI default font
**/
void create_army (short int army_size, Unit unit_list[MAX_UNITS], sfVector2i deployment_pos, terrain g_map [max_map_h][max_map_w]
                  , sfTexture* health_bar_tex, sfFont* ui_font)
{
    int i;
    sfVector2f infobox_size;
    infobox_size.x=map_block_w/2;
    infobox_size.y=map_block_h/2;
    sfColor box_transp;
    box_transp.a=100;
    box_transp.r=0;box_transp.g=0;box_transp.b=0;

    for(i=0;i<army_size;++i)
    {
        unit_list[i].speed=unit_list[i].type->speed;
        unit_list[i].melee_att=unit_list[i].type->melee_att;
        unit_list[i].ranged_att=unit_list[i].type->ranged_att;
        unit_list[i].melee_def=unit_list[i].type->melee_def;
        unit_list[i].morale=unit_list[i].type->morale;
        unit_list[i].men=unit_list[i].type->men;
        unit_list[i].ammo=unit_list[i].type->ammo;
        unit_list[i].sprite=sfSprite_create();
        unit_list[i].move_order=0;

        //sfSprite_setTexture(unit_list[i].sprite, unit_list[i].typetexture, sfTrue);
        unit_list[i].position.x=deployment_pos.x+map_block_w*(i%(map_w-1));
        unit_list[i].position.y=deployment_pos.y+map_block_h*(i/(map_h-1));
        unit_list[i].move_destination=unit_list[i].position;
        unit_list[i].map_position.x=unit_list[i].position.x/map_block_w;
        unit_list[i].map_position.y=unit_list[i].position.y/map_block_h;
        g_map[unit_list[i].map_position.y][unit_list[i].map_position.x].occupation=1;
        sfSprite_setPosition(unit_list[i].sprite, unit_list[i].position );
        unit_list[i].in_combat=0;
        //vertex
        unit_list[i].vertex_array= sfVertexArray_create();
        sfVertexArray_setPrimitiveType(unit_list[i].vertex_array, sfQuads);

        unit_list[i].vertex_state.blendMode= sfBlendAlpha;
        unit_list[i].vertex_state.shader=NULL;
        unit_list[i].vertex_state.texture=unit_list[i].type->texture;
        sfTransform vertex_transform;

        vertex_transform.matrix[0]=1;vertex_transform.matrix[1]=0;vertex_transform.matrix[2]=0;
        vertex_transform.matrix[3]=0;vertex_transform.matrix[4]=1;vertex_transform.matrix[5]=0;
        vertex_transform.matrix[6]=0;vertex_transform.matrix[7]=0;vertex_transform.matrix[8]=1;

        unit_list[i].vertex_state.transform=vertex_transform;

        update_vertex_array(4*unit_list[i].men, unit_list[i].vertex_array, unit_list[i].position, unit_list[i].direction);
//creating missile vertex array for ranged units
    if(unit_list[i].ranged_att>0)
    {
        unit_list[i].missile_vertex_array= sfVertexArray_create();
        sfVertexArray_setPrimitiveType(unit_list[i].missile_vertex_array, sfQuads);

        unit_list[i].missile_vertex_state.blendMode= sfBlendAlpha;
        unit_list[i].missile_vertex_state.shader=NULL;
        unit_list[i].missile_vertex_state.texture=unit_list[i].type->missile_texture;
        unit_list[i].missile_vertex_state.transform=vertex_transform;
    }
    //hp bar
    unit_list[i].health_bar.shape=sfRectangleShape_create();
    unit_list[i].health_bar_position.x=unit_list[i].position.x;
    unit_list[i].health_bar_position.y=unit_list[i].position.y+50;
    sfRectangleShape_setPosition(unit_list[i].health_bar.shape, unit_list[i].health_bar_position);
    unit_list[i].health_bar_size.x=(float)unit_list[i].men*2;
    unit_list[i].health_bar_size.y=40;
    sfRectangleShape_setSize(unit_list[i].health_bar.shape, unit_list[i].health_bar_size);
    sfRectangleShape_setTexture(unit_list[i].health_bar.shape, health_bar_tex, sfFalse);
    sfRectangleShape_setFillColor(unit_list[i].health_bar.shape, sfGreen);
    unit_list[i].health_bar.text=sfText_create();
    sfText_setCharacterSize(unit_list[i].health_bar.text, 15);
    sfText_setColor(unit_list[i].health_bar.text, sfRed);
    sfText_setFont(unit_list[i].health_bar.text, ui_font);
    //infobox
    unit_list[i].infobox.shape=sfRectangleShape_create();
    sfRectangleShape_setSize(unit_list[i].infobox.shape, infobox_size);
    sfRectangleShape_setFillColor(unit_list[i].infobox.shape, box_transp);
    unit_list[i].infobox.text=sfText_create();
    sfText_setCharacterSize(unit_list[i].infobox.text, 15);
    sfText_setColor(unit_list[i].infobox.text, sfWhite);
    sfText_setFont(unit_list[i].infobox.text, ui_font);
    unit_list[i].draw_infobox=0;

    }

}
/**
*Destroys all CSFML elements associated with army.
* army_size - number of units in army
* unit_list - list of army units
**/
void destroy_army (short int army_size, Unit unit_list[MAX_UNITS])
{
    int i=0;
    for(i=0;i<army_size;++i)
    {
        sfSprite_destroy(unit_list[i].sprite);
        sfRectangleShape_destroy(unit_list[i].health_bar.shape);
        sfText_destroy(unit_list[i].infobox.text);
        sfText_destroy(unit_list[i].health_bar.text);
        sfRectangleShape_destroy(unit_list[i].infobox.shape);
        sfVertexArray_destroy(unit_list[i].vertex_array);
        if(unit_list[i].ranged_att>0)
        sfVertexArray_destroy(unit_list[i].missile_vertex_array);
    }
}
/**
*Main function of game logic.
* army_size - number of units in army of current player
* unit_list - list of army units of current player
* op_army_size - number of units in army of current player's opponent
* op_unit_list - list of army units of current player's opponent
*
**/

short int game_mech (short int army_size, Unit unit_list[MAX_UNITS], short int op_army_size,Unit op_unit_list[MAX_UNITS],
                 short int* execute_orders, short int* active_player, terrain g_map[max_map_h][max_map_w])
{
    short int inactive_units_counter=0;
    short int move_order_counter=0;
    int i;
    //RUCH JEDNOSTEK
        //poczatek petli
    for(i=0;i<army_size;++i)
    {

        if(unit_list[i].men>0)
        {

            if (unit_list[i].attack_order==2 &&
                ranged_combat_animation(&unit_list[i], op_unit_list[unit_list[i].combat_focus].position)==0))
            {
                ranged_combat(&unit_list[i], &op_unit_list[unit_list[i].combat_focus]);
                unit_list[i].draw_line=0;
                unit_list[i].ammo=unit_list[i].ammo-1;
                move_order_counter=move_order_counter+1;
                g_map[unit_list[i].map_position.y][unit_list[i].map_position.x].occupation=1;
            }

            if(unit_list[i].move_order==1)//|| precise_distance(unit_list[i].position, unit_list[i].move_destination)==0) //wykonanie ruchu
            {
                if( movement(unit_list[i])==1) //zakonczenie ruchu i sprawdzenie czy wydany zostal rozkaz ataku
                {

                    unit_list[i].move_order=0;
                    deselect_unit(&unit_list[i]);
                    unit_list[i].draw_line=0;

                                 //zmiana kierunku po wykonaniu ruchu
                    if(unit_list[i].attack_order==1)
                    {
                        unit_list[i].direction=unit_list[i].new_direction;
                        update_vertex_array(4*unit_list[i].men, unit_list[i].vertex_array,
                                            sfSprite_getPosition( unit_list[i].sprite),  unit_list[i].direction);
                    }
                                //aktualizacja pozycji
                    unit_list[i].map_position.x=(int)unit_list[i].move_destination.x/(map_block_w);
                    unit_list[i].map_position.y=(int)unit_list[i].move_destination.y/(map_block_h);
                    unit_list[i].position.x=unit_list[i].move_destination.x;
                    unit_list[i].position.y=unit_list[i].move_destination.y;
                    g_map[unit_list[i].map_position.y][unit_list[i].map_position.x].occupation=1;
                    move_order_counter=move_order_counter+1;
                                //zapisanie ruchu do loga
                    char aux_txt[MAX_STRING_LENGTH];
                    sprintf(aux_txt, "Jednostka %s gracza %d , [id]:%d przemiescila sie na pole: %d, %d.",unit_list[i].type->name,*active_player, i, unit_list[i].map_position.x, unit_list[i].map_position.y);
                    game_log=add_log_string(aux_txt, game_log);
                    update_log_file(log_file, "logs/log.txt", game_log);
                                //wywolanie melee_combat
                    //if(unit_list[i].attack_order==1)
                        //melee_combat(&unit_list[i], &op_unit_list[unit_list[i].combat_focus]);
                        }

                }
                else if(unit_list[i].attack_order!=2)
                {

                    move_order_counter=move_order_counter+1;

                }

                }

                else
                {
                    inactive_units_counter=inactive_units_counter+1;
                    g_map[unit_list[i].map_position.y][unit_list[i].map_position.x].occupation=0;
                }

        }
        if(move_order_counter+inactive_units_counter==army_size)
        {
            for(i=0;i<army_size;++i)
                {

                    if(unit_list[i].men>0  && unit_list[i].in_combat==1)
                    {
                        if(op_unit_list[unit_list[i].combat_focus].men>0 && op_unit_list[unit_list[i].combat_focus].in_combat==1)
                            melee_combat(&unit_list[i], &op_unit_list[unit_list[i].combat_focus]);
                        else if (op_unit_list[unit_list[i].combat_focus].men<=0)
                        {
                           unit_list[i].in_combat=0;
                           g_map[op_unit_list[unit_list[i].combat_focus].map_position.y][op_unit_list[unit_list[i].combat_focus].map_position.x].occupation=0;
                        }

                    }
                }
            *execute_orders=0;
            move_order_counter=0;
            if(*active_player==1)
                *active_player=2;
            else
                *active_player=1;
        }

            if(inactive_units_counter==army_size)
            {
              return 1;
            }
            else
            {
                return 0;
            }

}

void load_unit_stats(FILE* unit_def_file,char* unit_def_localisation, Unit_type* type, char* unit_name)
{
    unit_def_file=fopen(unit_def_localisation, "r");
    char string_bufor [20];
    int stats_bufor;
    //fscanf(unit_def_file, "%s", string_bufor);

    while(strcmp(string_bufor, unit_name)!=0 && strcmp(string_bufor, "END_OF_STATS")!=0)
        fscanf(unit_def_file, "%s", string_bufor);

    strcpy(type->name, string_bufor);
    fscanf(unit_def_file, "%s", string_bufor);
    fscanf(unit_def_file, "%2d", &stats_bufor);
    while(strcmp(string_bufor, "END")!=0)
    {
        if(strcmp(string_bufor, "men")==0)
            type->men=stats_bufor;
        else if(strcmp(string_bufor, "melee_att")==0)
            type->melee_att=stats_bufor;
        else if(strcmp(string_bufor, "melee_def")==0)
            type->melee_def=stats_bufor;
        else if(strcmp(string_bufor, "speed")==0)
            type->speed=stats_bufor;
        else if(strcmp(string_bufor, "morale")==0)
            type->morale=stats_bufor;
        else if(strcmp(string_bufor, "ammo")==0)
            type->ammo=stats_bufor;
        else if(strcmp(string_bufor, "ranged_att")==0)
            type->ranged_att=stats_bufor;
        else if(strcmp(string_bufor, "command")==0)
            type->command=stats_bufor;
        else if(strcmp(string_bufor, "command_range")==0)
            type->command_range=stats_bufor;
        else if(strcmp(string_bufor, "range")==0)
            type->range=stats_bufor;
        fscanf(unit_def_file, "%s", string_bufor);
        fscanf(unit_def_file, "%2d", &stats_bufor);

        //fscanf(unit_def_file, "%c", aux);
    }
    fclose(unit_def_file);
}

