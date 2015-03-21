#include "lib.h"
FILE* log_file;



log_list add_log_string (char add_text[MAX_STRING_LENGTH], log_list game_log)
{
	log_list aux;
	aux=(struct lista*)(malloc(sizeof(struct lista)));

	if(game_log==NULL)
        aux->next=NULL;

	strcpy(aux->text, add_text);

	if(game_log!=NULL)
        aux->next=game_log;

	  return aux;
}

log_list reverse_log (log_list game_log)
{
    log_list aux=NULL;
    char text[MAX_STRING_LENGTH];
    while(game_log!=NULL)
    {
        strcpy (text, game_log->text);
        game_log=game_log->next;
        aux=add_log_string(text, aux);
    }
    return aux;
}


void update_log_file (FILE* log_file,char* log_file_localisation, log_list game_log)
{
    log_file=fopen(log_file_localisation, "w");
    while(game_log!=NULL)
    {
            fprintf(log_file, game_log->text );
            fprintf(log_file, "\n");
            game_log=game_log->next;
    }
    fclose(log_file);
}

void clear_log (log_list game_log)
{
    log_list aux=game_log;

    while(game_log!=NULL)
    {
        aux=game_log->next;
        free(game_log);
        game_log=aux;
    }
}
