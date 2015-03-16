
#include "lib.h"

void movement_order (terrain g_map [max_map_h][max_map_w], unit* unit1, sfVector2f mouse_pos)
{
    if((unit1->move_destination.x!=mouse_pos.x ||unit1->move_destination.y!=mouse_pos.y)&& g_map[(int)((mouse_pos.y)/(map_block_h))][(int)((mouse_pos.x)/(map_block_w))].occupation==0)
    {
         g_map[unit1->map_position.y][unit1->map_position.x].occupation=0;
        unit1->move_destination.x= ((int)((mouse_pos.x)/(map_block_w)))*map_block_w;
                        unit1->move_destination.y= ((int)((mouse_pos.y)/(map_block_h)))*map_block_h;
                        create_ord_line(unit1, unit1->move_destination, sfGreen);
                        unit1->draw_line=1;
                        unit1->move_order=1;
                        if(unit1->in_combat==1)
                        {
                          unit1->in_combat=0;

                        }

    }

}

short int create_movement_list (short int army_size,unit army_list [MAX_UNITS], int movement_list [army_size], int dest_x, int dest_y)
{
int i,j=0;

int temp;
short int units_to_move=0;
for(i=0; i<army_size;++i)
{
    if(army_list[i].selected==1 && army_list[i].men>0)
    {
        movement_list[j]=i;
        units_to_move=units_to_move+1;
        ++j;
    }

}
int n=units_to_move;
if(units_to_move>1)
{
        while(n>1)
    {
        for(i=0;i<(units_to_move-1);++i)
        {
            if(distance(army_list[movement_list[i]].map_position, dest_x, dest_y)>distance(army_list[movement_list[i+1]].map_position, dest_x, dest_y) )
            {
                temp=movement_list[i];
                movement_list[i]=movement_list[i+1];
                movement_list[i+1]=temp;
            }
        }
        n=n-1;
    }
}
    return units_to_move;
}

int movement (unit unit1)
{
                sfVector2f move_vector;

                unit1.health_bar_position.x=sfSprite_getPosition(unit1.sprite).x;
unit1.health_bar_position.y=sfSprite_getPosition(unit1.sprite).y-25;
sfRectangleShape_setPosition(unit1.health_bar.shape, unit1.health_bar_position);
unit1.health_bar_position.y=unit1.health_bar_position.y-20;
sfText_setPosition(unit1.health_bar.text, unit1.health_bar_position);
                /*Ustalanie wektora ruchu */
                /*
                if(unit1.move_destination.x<map_block_w)
                {
                    move_vector.x=-1;
                }
                */
                if(unit1.move_destination.x==sfSprite_getPosition(unit1.sprite).x)
                    move_vector.x=0;
                else
                {
                        if(unit1.move_destination.x>sfSprite_getPosition(unit1.sprite).x)
                        move_vector.x=4;
                        else
                        move_vector.x=-4;

                }
                if(unit1.move_destination.y==sfSprite_getPosition(unit1.sprite).y)
                    move_vector.y=0;
                else
                {
                        if(unit1.move_destination.y>sfSprite_getPosition(unit1.sprite).y)
                        move_vector.y=4;
                        else
                        move_vector.y=-4;
                }
         if ((sfSprite_getPosition(unit1.sprite).x==unit1.move_destination.x && sfSprite_getPosition(unit1.sprite).y==unit1.move_destination.y))
       {
           unit1.move_order=0;
           return 1;
       }

        else
        {
           // sfClock* movement_clock=sfClock_create();
           // sfTime dt_time=sfClock_getElapsedTime(movement_clock);
          // float dt_float=(float)dt_time.microseconds;
         //  move_vector.x=move_vector.x*dt_float;
         //  move_vector.y=move_vector.y*dt_float;
        sfSprite_move(unit1.sprite, move_vector);
        update_vertex_array(4*unit1.men,unit1.vertex_array, sfSprite_getPosition(unit1.sprite), unit1.direction);
       // sfClock_destroy(movement_clock);
       return 0;
        }
}

void create_movement_orders (short int army_size, unit unit_list[MAX_UNITS], sfVector2f mouse_pos, terrain g_map[max_map_h][max_map_w])//, short int mode)
{
                    short int w=0;
                    short int i=0;
                short int mov_size;
                int movement_list[army_size];

                mov_size=create_movement_list(army_size,unit_list, movement_list,(int)((mouse_pos.x)/(map_block_w)),(int)((mouse_pos.y)/(map_block_h)));
                            for(i=0;i<mov_size;++i)
                        {
                            if(unit_list[movement_list[i]].selected==1 && unit_list[movement_list[i]].men>0)
                            {
                                sfVector2f dest;
                                if(w==1)
                                {
                                    dest=relative_position_vector(unit_list[movement_list[0]].position, unit_list[movement_list[i]].position);
                                        dest.x=dest.x+mouse_pos.x;
                                        dest.y=dest.y+mouse_pos.y;
                                    if(dest.x>=0
                                       && dest.x<=map_w*map_block_w
                                       &&dest.y>=0
                                        &&dest.y<=map_h*map_block_h)
                                    {
                                        if( distance(unit_list[movement_list[i]].map_position,(int)((dest.x)/(map_block_w)) ,(int)((dest.y)/(map_block_h)))<=unit_list[movement_list[i]].speed
                                           && g_map[(int)((dest.y)/(map_block_h))][(int)((dest.x)/(map_block_w))].occupation==0
                                           && check_movement_destination(army_size, unit_list, ((int)((dest.y)/(map_block_h)))*map_block_h, ((int)((dest.x)/(map_block_w)))*map_block_w, i)==0)
                                        {
                                            g_map[(int)unit_list[movement_list[i]].move_destination.y/map_block_h][(int)unit_list[movement_list[i]].move_destination.x/map_block_w].occupation=0;
                                            movement_order(g_map,&unit_list[movement_list[i]], dest);
                                            //g_map[(int)((dest.y)/(map_block_h))][(int)((dest.x)/(map_block_w))].occupation=1;
                                        }

                                    }

                                }
                            else
                            {
                                if( distance(unit_list[movement_list[i]].map_position,(int)((mouse_pos.x)/(map_block_w)) ,(int)((mouse_pos.y)/(map_block_h)))<=unit_list[movement_list[i]].speed
                                   && g_map[(int)((mouse_pos.y)/(map_block_h))][(int)((mouse_pos.x)/(map_block_w))].occupation==0
                                   && check_movement_destination(army_size, unit_list, ((int)((mouse_pos.y)/(map_block_h)))*map_block_h, ((int)((mouse_pos.x)/(map_block_w)))*map_block_w, i)==0)
                                {
                                    g_map[(int)unit_list[movement_list[i]].move_destination.y/map_block_h][(int)unit_list[movement_list[i]].move_destination.x/map_block_w].occupation=0;
                                    movement_order(g_map, &unit_list[movement_list[i]], mouse_pos);
                                    //g_map[(int)((mouse_pos.y)/(map_block_h))][(int)((mouse_pos.x)/(map_block_w))].occupation=1;
                                }
                            w=1;
                            }
                            }
                        }
}

sfBool check_movement_destination (short int army_size, unit unit_list[MAX_UNITS], int dest_y, int dest_x, int index)
{
    short int i;

    for(i=0;i<army_size;++i)
    {
        if(unit_list[i].men>0 && unit_list[i].move_destination.x==dest_x && unit_list[i].move_destination.y==dest_y && i!=index)
        {
            return 1;
        }
    }
    return 0;
}
