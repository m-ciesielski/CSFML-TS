#include "lib.h"


void show_end_turn_message (message msg,char* text, int old_army_size[players_count], int current_army_size[players_count])
{
    sprintf(text, "Player I: \t Player II: \n Old Manpower: %d \t Old Manpower: %d \n -%d \t -%d \n New Manpower: %d  \t New Manpower: %d"
            , old_army_size[0], old_army_size[1], old_army_size[0]-current_army_size[0], old_army_size[1]-current_army_size[1], current_army_size[0], current_army_size[1] );
    strcpy(msg.text[0], text);
}

void destroy_message (message msg)
{
    short int i;

    sfRectangleShape_destroy(msg.background);
    sfRectangleShape_destroy(msg.ico);
    for(i=0;i<MAX_MESSAGE_BUTTONS;++i)
    {
        if(msg.text[i])
        sfText_destroy(msg.text[i]);
        if(msg.button[i])
        sfRectangleShape_destroy(msg.button[i]);
    }
}
