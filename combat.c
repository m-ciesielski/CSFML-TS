
#include "lib.h"
void create_attack_orders(short int army_size, unit unit_list[MAX_UNITS], short int op_army_size,
                          unit op_unit_list[MAX_UNITS] ,sfVector2f mouse_pos, terrain g_map[max_map_h][max_map_w], short int direction,
                          short int attack_mode)
{
    short int legitAttack=0;
    short int i,j , dir_x, dir_y;
    if(direction==0)
    {
        dir_x=0;
        dir_y=-1;
    }
    else if(direction==1)
    {
        dir_x=1;
        dir_y=0;
    }
    else if(direction==2)
    {
        dir_x=0;
        dir_y=1;
    }
    else if(direction==3)
    {
        dir_x=-1;
        dir_y=0;
    }

    sfVector2f move_vector;
    move_vector.x=mouse_pos.x-(map_block_w*(-(dir_x)));
    move_vector.y=mouse_pos.y-(map_block_h*(-(dir_y)));
if(g_map[(int)((move_vector.y)/map_block_h)][(int)((move_vector.x)/map_block_w)].occupation==0 && attack_mode==0)
create_movement_orders(army_size,unit_list,move_vector, g_map);
    for(i=0;i<army_size;++i)
    {
        if(unit_list[i].in_combat==0 && unit_list[i].selected==1 && unit_list[i].men>0 &&g_map[(int)(unit_list[i].move_destination.y/map_block_h)][(int)(unit_list[i].move_destination.x/map_block_w)].occupation==0)
        {

            for(j=0;j<op_army_size;++j)
            {
                if(op_unit_list[j].map_position.y==((((int)unit_list[i].move_destination.y)/(map_block_h))-dir_y)
                    && op_unit_list[j].map_position.x==((((int)unit_list[i].move_destination.x)/(map_block_w))-dir_x))
                {
                    if(attack_mode==0)
                    {
                        unit_list[i].attack_order=1;
                        op_unit_list[j].combat_focus=i;
                        legitAttack=1;
                   // else if(op_unit_list[j].in_combat==0)
                       // op_unit_list[j].flanked=0;
                    }

                    else if(attack_mode==1 &&  unit_list[i].ammo>0 && distance(op_unit_list[j].map_position, unit_list[i].map_position.x,  unit_list[i].map_position.y)<= unit_list[i].type->range)
                    {
                        unit_list[i].missile_position.x= unit_list[i].position.x;
                        unit_list[i].missile_position.y= unit_list[i].position.y;
                        unit_list[i].move_order=0;
                        unit_list[i].attack_order=2;
                        legitAttack=1;
                    }
                    if(legitAttack==1)
                    {
                        unit_list[i].combat_focus=j;
                    sfVector2f line_dest;
                    line_dest.x=op_unit_list[j].position.x-(-dir_x*map_block_w/2);
                    line_dest.y=op_unit_list[j].position.y-(-dir_y*map_block_h/2);
                    create_ord_line(&unit_list[i], line_dest, sfRed);
                    unit_list[i].new_direction=(direction+2)%4;
                    }

                }
            }
        }
    }

}

short int attack_direction (sfVector2f mouse_pos, terrain terr)
{
short int direction;
sfVector2f dir_points [4];
//zachod
dir_points[3].x=terr.position.x;
dir_points[3].y=terr.position.y+map_block_h/2;
//polnoc
dir_points[0].x=terr.position.x+map_block_w/2;
dir_points[0].y=terr.position.y;
//poludnie
dir_points[2].x=terr.position.x+map_block_w/2;
dir_points[2].y=terr.position.y+map_block_h;
//wschod
dir_points[1].x=terr.position.x+map_block_w;
dir_points[1].y=terr.position.y+map_block_h/2;

float distance_to_points [4];
short int i;
float min_distance=0;
for(i=0;i<4;++i)
{
    distance_to_points[i]=precise_distance(mouse_pos, dir_points[i]);
    if(distance_to_points[i]<=min_distance ||min_distance==0)
    {
        min_distance=distance_to_points[i];
        direction=i;
    }

}
return direction;
}

void melee_combat(unit* att_unit, unit* def_unit)
{
    //WARTOSCI OBRAZEN
    float att_damage=0, def_damage=0;
    sfBool flanked=0;
    sfBool charge=0;
    //WYLICZENIE OBRAZEN ZADANYCH PRZEZ ATAKUJACA JEDNOSTKE
    att_damage=( ((att_unit->melee_att/10.0)*(att_unit->morale/50.0) )
                -(( (def_unit->men/10.0)*(def_unit->melee_def/100.0)*(def_unit->morale/100.0) )/2) );
    if(att_unit->in_combat==0)
    {
        att_damage=att_damage*(1+(att_unit->charge/100.0));
        charge=1;
    }

    if(def_unit->direction!=(att_unit->direction+2)%4)
    {
        att_damage=att_damage*1.25;
        flanked=1;
        sfText_setString(def_unit->health_bar.text,"|ATAKOWANA Z FLANKI|");
    }
    else if(def_unit->direction==att_unit->direction)
    {
        att_damage=att_damage*1.5;
        flanked=2;
        sfText_setString(def_unit->health_bar.text,"|ATAKOWANA Z TYLU|");
    }

    //WYLICZENIE OBRAZEN ZADANYCH PRZEZ BRONIACA JEDNOSTKE
    def_damage=(((def_unit->melee_att/10.0)*(def_unit->morale/50.0) )
                -(( (att_unit->men/10.0)*(att_unit->melee_def/100.0)*(att_unit->morale/100.0) )/2) );

    //AKTUALIZACJA STANU JEDNOSTEK
    if(att_damage<0) att_damage=0;
    if(def_damage<0) def_damage=0;
    if(att_damage>def_unit->men) att_damage=def_unit->men;
    if(def_damage>att_unit->men) def_damage=att_unit->men;
    att_unit->in_combat=1;
    def_unit->in_combat=1;
    att_unit->attack_order=0;
    def_unit->attack_order=0;
    att_unit->men=(att_unit->men)-(int)def_damage;
    //morale dmg
    att_unit->morale=(att_unit->morale)-(int)(def_damage/10);

    def_unit->men=(def_unit->men)-(int)att_damage;
    //morale dmg
    def_unit->morale=(def_unit->morale)-(int)(att_damage/10);
    //zapisanie walki do loga
    char aux_txt[MAX_STRING_LENGTH];
    sprintf(aux_txt, "Jednostka %s, zaatakowala jednostke %s, eliminujac %d ludzi i trac¹c %d ludzi. Atak z flanki: %d, szarza: %d."
    ,att_unit->type->name,def_unit->type->name, (int)att_damage, (int)def_damage, flanked, charge);
    game_log=add_log_string(aux_txt, game_log);
    update_log_file(log_file, "logs/log.txt", game_log);

     update_vertex_array(4*att_unit->men,att_unit->vertex_array, sfSprite_getPosition(att_unit->sprite), att_unit->direction);
     update_vertex_array(4*def_unit->men,def_unit->vertex_array, sfSprite_getPosition(def_unit->sprite),def_unit->direction);
}
//TODO: ustawianie flagi ataku rozkazu na 0 etc
void ranged_combat (unit* att_unit, unit* def_unit)
{
    float att_damage=0;
    att_damage=(att_unit->men*att_unit->ranged_att)/200;
    def_unit->men=(def_unit->men)-(int)att_damage;
    //morale dmg
    def_unit->morale=(def_unit->morale)-(int)(att_damage/10);
    att_unit->attack_order=0;
    //zapisanie do loga
    char aux_txt[MAX_STRING_LENGTH];
                                 sprintf(aux_txt, "Jednostka %s, ostrzelala jednostke %s, zadaj¹c %d obra¿eñ.",att_unit->type->name,def_unit->type->name, (int)att_damage);
                                 game_log=add_log_string(aux_txt, game_log);
                                 update_log_file(log_file, "logs/log.txt", game_log);
    update_vertex_array(4*def_unit->men,def_unit->vertex_array, sfSprite_getPosition(def_unit->sprite), def_unit->direction);
}

int ranged_combat_animation (unit* unit1, sfVector2f destination)
{
    sfVector2f move_vector;

    if(destination.x>=unit1->missile_position.x)
                        move_vector.x=10;
    else
                        move_vector.x=-10;

    if(destination.y>=unit1->missile_position.y)
                        move_vector.y=10;
    else
                        move_vector.y=-10;


    if(unit1->missile_position.x!=destination.x || unit1->missile_position.y!=destination.y)
    {
        unit1->missile_position.x=unit1->missile_position.x+move_vector.x;
        unit1->missile_position.y=unit1->missile_position.y+move_vector.y;
        update_vertex_array(unit1->men*4, unit1->missile_vertex_array, unit1->missile_position, unit1->direction);
        return 1;
    }
    else
        return 0;
}


void command_morale_bonus (int army_size, unit unit_list[army_size])
{
    int i, j;
    for(i=0;i<army_size;++i)
    {
        if(unit_list[i].type->command==1)
        {
            for(j=0;j<army_size;++j)
            {
                if(unit_list[j].men>0 &&
                   distance(unit_list[i].map_position, unit_list[j].map_position.x, unit_list[j].map_position.y)<unit_list[i].type->command_range
                   && unit_list[j].morale<(unit_list[j].type->morale+10))
                    unit_list[j].morale=unit_list[j].morale+1;
            }
        }
    }
}
/*
int ranged_combat_animation_loop (short int army_size, unit army_list[army_size], short int op_army_size, unit op_army_list[op_army_size])
{
    short int i;
    short int licznik_jednostek=0;
    short int licznik_animacji=0;

        for(i=0;i<army_size;++i)
    {
        if(army_list[i].attack_order==2)
        {
            if(army_list[i].missile_position.x==0 && army_list[i].missile_position.y==0 &&
               army_list[i].position.x!=0 && army_list[i].position.y!=0)
            {
                army_list[i].missile_position.x=army_list[i].pos
            }
        }
    }

    for(i=0;i<army_size;++i)
    {
        if(army_list[i].attack_order==2)
        {
            if(army_list[i].missile_position.x==0 && army_list[i].missile_position.y==0 &&
               army_list[i].position.x!=0 && army_list[i].position.y!=0)
            {
                army_list[i].missile_position.x=army_list[i].pos
            }
            licznik_jednostek=licznik_jednostek+1;
            if(ranged_combat_animation(army_list[i],op_army_list[army_list[i].combat_focus])==0)
                licznik_animacji=licznik_animacji+1;
        }
    }
    if(licznik_jednostek==licznik_animacji)
        return 0;
    else return 1;
}
*/
