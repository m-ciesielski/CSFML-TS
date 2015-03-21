
#include "lib.h"

void set_army_direction (int army_size, Unit unit_list[MAX_UNITS], short int direction)
{
    int i;
    for(i=0;i<army_size;++i)
    {
        unit_list[i].direction=direction;
    }
}
//FUNKCJE POMOCNICZE
//+++++++++++++++++++++++++++++++++++++//
//wyliczanie rzeczywistej odleglosci
float precise_distance (sfVector2f object1_pos, sfVector2f object2_pos)
{
    float distance=sqrt((pow((double)(object1_pos.x-object2_pos.x),2))+(pow((double)(object1_pos.y-object2_pos.y),2)));
    return distance;
}

sfVector2f relative_position_vector (sfVector2f object1_pos, sfVector2f object2_pos)
{
    sfVector2f rel;
    rel.x=object2_pos.x-object1_pos.x;
    rel.y=object2_pos.y-object1_pos.y;
    return rel;
}

//wyliczanie dystansu dla ruchu po mapie
short int distance (sfVector2i object, int destination_x, int destination_y)
{
    short int distance=sqrt((pow((double)(object.x-destination_x),2))+(pow((double)(object.y-destination_y),2)));
    return distance;
}

//+++++++++++++++++++++++++++++++++++++//


