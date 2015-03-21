#include "lib.h"

void create_order_line (Unit* unit1, sfVector2f dest_pos, sfColor color)
{
   unit1->line[1].position.x=unit1->position.x+map_block_w/2;
   unit1->line[1].position.y=unit1->position.y+map_block_h/2;
   unit1->line[1].color=color;
   unit1->line[0].color=color;
   unit1->line[0].position.x=dest_pos.x+map_block_w/2;
   unit1->line[0].position.y=dest_pos.y+map_block_h/2;
    unit1->draw_line=1;

}

void men_and_morale_counter_update (short int army_size, Unit army_list[army_size], sfText* text)
{
    short int i, total_men=0, avg_morale=0, active_units=0;
    for (i=0;i<army_size;++i)
    {
        total_men=total_men+army_list[i].men;
        if(army_list[i].men>0)
        {
            avg_morale=avg_morale+army_list[i].morale;
            ++active_units;
        }

    }
    if(active_units==0)
        avg_morale=0;
    else
        avg_morale=avg_morale/active_units;

    char str_text [30];
    sprintf(str_text, "Total men: %d Average Morale: %d", total_men, avg_morale);
    sfText_setString(text, str_text);
}

void deselect_unit (Unit* unit1)
{
    unit1->selected=0;
    unit1->draw_infobox=0;
}
short int rotation_direction (sfVector2f mouse_pos, sfVector2f unit_pos)
{
    short int direction;
    sfVector2f dir_points [4];
    //zachod
    dir_points[3].x=unit_pos.x;
    dir_points[3].y=unit_pos.y+map_block_h/2;
    //polnoc
    dir_points[0].x=unit_pos.x+map_block_w/2;
    dir_points[0].y=unit_pos.y;
    //poludnie
    dir_points[1].x=unit_pos.x+map_block_w/2;
    dir_points[1].y=unit_pos.y+map_block_h;
    //wschod
    dir_points[2].x=unit_pos.x+map_block_w;
    dir_points[2].y=unit_pos.y+map_block_h/2;

    float distance_to_points [4];
    short int i;
    float min_distance=0;
    for(i=0;i<4;++i)
    {
        distance_to_points[i]=precise_distance(mouse_pos, dir_points[i]);
        if(distance_to_points[i]<min_distance ||min_distance==0)
        {
            min_distance=distance_to_points[i];
            direction=i;
        }

    }
    return direction;
}

void change_unit_rotation (Unit* unit1, sfVector2f mouse_pos)
{
   short int direction=rotation_direction(mouse_pos, unit1->position);
   unit1->direction=direction;
   update_vertex_array(4*unit1->men,unit1->vertex_array, sfSprite_getPosition(unit1->sprite), unit1->direction);

}

int select_check (Unit* unit_1, sfRectangleShape* select_box)
{
    int selected=0;

    if(unit_1->men>0
        &&sfRectangleShape_getPosition(select_box).x<=sfSprite_getPosition(unit_1->sprite).x
        && sfSprite_getPosition(unit_1->sprite).x<=(sfRectangleShape_getPosition(select_box).x+sfRectangleShape_getSize(select_box).x)
        && sfRectangleShape_getPosition(select_box).y<=sfSprite_getPosition(unit_1->sprite).y
        && sfSprite_getPosition(unit_1->sprite).y<=(sfRectangleShape_getPosition(select_box).y+sfRectangleShape_getSize(select_box).y)
        )
    {
            unit_1->selected=1;
            sfSprite_setColor(unit_1->sprite, sfYellow);
            selected=1;
    }
    return selected;
}

void select_all (short int army_size, Unit unit_list[MAX_UNITS], sfVertexArray* map_grid_highlight ,
                  sfBool* draw_unit_ui, sfBool* draw_grid_highlight)
{
    short int i;
    for (i=0;i<army_size;++i)
    {
        if(unit_list[i].men>0 && unit_list[i].selected==0)
        {
            *draw_unit_ui=1;
            map_highlight(unit_list[i].position,unit_list[i].speed, map_grid_highlight, draw_grid_highlight, sfYellow);
            create_infobox(unit_list[i], unit_list[i].position);
            unit_list[i].draw_infobox=1;
            unit_list[i].selected=1;
        }
    }
}
void create_infobox (Unit unit_1, sfVector2f infobox_pos)
{
    sfRectangleShape_setPosition(unit_1.infobox.shape, infobox_pos);
    char text[200];
    //sprintf
    sprintf(text, "%s \n Men: %d \n Morale: %d \n Melee attack: %d \n Melee defence: %d \n Speed: %d \n \n Direction: %d \n Ranged_att: %d",
             unit_1.type->name, unit_1.men, unit_1.morale, unit_1.melee_att, unit_1.melee_def, unit_1.speed,unit_1.direction, unit_1.ranged_att);
    sfText_setString(unit_1.infobox.text, text);
    sfText_setPosition(unit_1.infobox.text, infobox_pos);
}

void hp_bar_update (Unit unit1, sfColor hp_bar_color, sfVector2f hp_bar_dynamic)
{
	hp_bar_dynamic.x=unit1.men;
        sfRectangleShape_setSize(unit1.health_bar.shape,hp_bar_dynamic);
        if(unit1.selected==0)
        {
            hp_bar_color.a=50;
        }
        else
        {
            hp_bar_color.a=250;
        }

        if(unit1.men<=(unit1.type->men/2))
        {
            hp_bar_color.g=0;
            hp_bar_color.r=250;
        }
        else
        {
            hp_bar_color.g=250;
            hp_bar_color.r=0;
        }
    sfRectangleShape_setFillColor(unit1.health_bar.shape, hp_bar_color);
    //if(unit1.flanked==1)
    //sfText_setString(unit1.health_bar.text,"|FLANKED|");
    if(unit1.type->command==1)
    sfText_setString(unit1.health_bar.text, "|COMMAND|");
}

void add_units_to_group (sfBool group [],int army_size, Unit unit_list[MAX_UNITS])
{
int i;
    for(i=0;i<army_size;++i)
    {
    if(unit_list[i].selected==1)
    group[i]=1;
    }
}
/*
void show_army_summary (short int army_size, unit unit_list[MAX_UNITS])
{
    short int i;
    sfVector2f infobox_pos;
    infobox_pos.x=0;
    infobox_pos.y=0;
    for(i=0;i<army_size;++i)
    {
        create_infobox(unit_list[i], infobox_pos);
        infobox_pos.x=(i%10)*(map_block_w/2);
        infobox_pos.y=(i/10)*(map_block_h/2);
    }
}
*/
