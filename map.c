#include "lib.h"

void load_map_from_file (sfText* text)
{
	char temp=getchar();
	char str [100];
	while(temp!='\n')
	{
		temp=getchar();
		sprintf(str, "%c", temp);
	}
	sfText_setString(text, str);
}


void generate_map (unsigned int map_w, unsigned int map_h, terrain g_map [max_map_h][max_map_w])
{
	srand( time( NULL ) );
	short int random;
	short int i,j;
	for(i=0; i<map_h; ++i)
	{
		for(j=0; j<map_w; ++j)
		{
			random = (rand() % terrain_types);
			g_map[i][j].type=random;
		}
	}
}

void map_interpreter (FILE* map_def_file,char* map_def_localisation, terrain g_map [max_map_h][max_map_w])
{
	map_def_file=fopen(map_def_localisation, "r");
	char temp_char;
	short int i=0,j=0;
	while(fscanf(map_def_file, "%c", &temp_char)!=EOF)
	{
		if(temp_char=='\n')
		{
			++j;
			i=0;
		}
		else
		{
			if(temp_char=='g')
			{
				g_map[j][i].type=0;
				++i;
			}
			else if (temp_char=='d')
			{
				g_map[j][i].type=1;
				++i;
			}
		}
	}
	fclose(map_def_file);
}

short int create_map (terrain g_map [max_map_h][max_map_w], sfVertexArray* vertex_map, sfVertexArray* map_grid, sfVertexArray* flora_array)
{
	int i, j=0; //random;
//sfColor color_variation;
//color_variation.a=0;
//srand(time(NULL));
	generate_flora(g_map,flora_array);
	sfVertexArray_clear(vertex_map);
	sfVertexArray_clear(map_grid);

	sfVertex map_vertex;

	for(i=0; i<map_h; ++i)
	{
		for(j=0; j<map_w; ++j)
		{
			g_map[i][j].occupation=0;
			g_map[i][j].position.x=(j*map_block_w);
			g_map[i][j].position.y=(i*map_block_h);
		}
	}
	for(i=0; i<map_h; ++i)
	{
		for(j=0; j<map_w; ++j)
		{
			map_vertex.position.x=g_map[i][j].position.x;
			map_vertex.position.y=g_map[i][j].position.y;
			map_vertex.texCoords.x=(g_map[i][j].type*map_texture_size);
			map_vertex.texCoords.y=0;
			//random=(rand()%50);color_variation.r=random;
			//random=(rand()%50);color_variation.g=random;
			//random=(rand()%50);color_variation.b=random;
			map_vertex.color=sfWhite;
			sfVertexArray_append(vertex_map,map_vertex);
			map_vertex.color=sfBlack;
			sfVertexArray_append(map_grid,map_vertex);

			map_vertex.position.x=g_map[i][j].position.x+map_block_w;
			map_vertex.position.y=g_map[i][j].position.y;
			map_vertex.texCoords.x=((g_map[i][j].type*map_texture_size)+map_texture_size);
			map_vertex.texCoords.y=0;
			map_vertex.color=sfWhite;
			sfVertexArray_append(vertex_map,map_vertex);
			map_vertex.color=sfBlack;
			sfVertexArray_append(map_grid,map_vertex);

			map_vertex.position.x=g_map[i][j].position.x+map_block_w;
			map_vertex.position.y=g_map[i][j].position.y+map_block_h;
			map_vertex.texCoords.x=((g_map[i][j].type*map_texture_size)+map_texture_size);
			map_vertex.texCoords.y=map_texture_size;
			map_vertex.color=sfWhite;
			sfVertexArray_append(vertex_map,map_vertex);
			map_vertex.color=sfBlack;
			sfVertexArray_append(map_grid,map_vertex);

			map_vertex.position.x=g_map[i][j].position.x;
			map_vertex.position.y=g_map[i][j].position.y+map_block_h;
			map_vertex.texCoords.x=(g_map[i][j].type*map_texture_size);
			map_vertex.texCoords.y=map_texture_size;
			map_vertex.color=sfWhite;
			sfVertexArray_append(vertex_map,map_vertex);
		}

	}
	return 1;
}

void map_highlight (sfVector2f unit_pos, short int range, sfVertexArray* grid, sfBool* draw_grid_highlight, sfColor highlight)
{
	if(highlight.r>0)
		highlight.r=100;
	if(highlight.g>0)
		highlight.g=100;
	if(highlight.b>0)
		highlight.b=100;
	highlight.a=50;
	sfVertex highlight_vertex [4];

	highlight_vertex[0].position.x=unit_pos.x-(range*map_block_w);
	if(highlight_vertex[0].position.x<0)
		highlight_vertex[0].position.x=0;
	highlight_vertex[0].position.y=unit_pos.y-(range*map_block_h);
	if(highlight_vertex[0].position.y<0)
		highlight_vertex[0].position.y=0;
	highlight_vertex[0].color=highlight;
	sfVertexArray_append(grid, highlight_vertex[0]);
	highlight_vertex[1].position.x=unit_pos.x+(range*map_block_w)+map_block_w;
	if(highlight_vertex[1].position.x>map_w*map_block_w)
		highlight_vertex[1].position.x=map_w*map_block_w;
	highlight_vertex[1].position.y=unit_pos.y-(range*map_block_h);
	if(highlight_vertex[1].position.y<0)
		highlight_vertex[1].position.y=0;
	highlight_vertex[1].color=highlight;
	sfVertexArray_append(grid, highlight_vertex[1]);
	highlight_vertex[2].position.x=unit_pos.x+(range*map_block_w)+map_block_w;
	if(highlight_vertex[2].position.x>map_w*map_block_w)
		highlight_vertex[2].position.x=map_w*map_block_w;
	highlight_vertex[2].position.y=unit_pos.y+(range*map_block_h)+map_block_h;
	if(highlight_vertex[2].position.y>map_h*map_block_h)
		highlight_vertex[2].position.y=map_h*map_block_h;
	highlight_vertex[2].color=highlight;
	sfVertexArray_append(grid, highlight_vertex[2]);
	highlight_vertex[3].position.x=unit_pos.x-(range*map_block_w);
	if(highlight_vertex[3].position.x<0)
		highlight_vertex[3].position.x=0;
	highlight_vertex[3].position.y=unit_pos.y+(range*map_block_h)+map_block_h;
	if(highlight_vertex[3].position.y>map_h*map_block_h)
		highlight_vertex[3].position.y=map_h*map_block_h;
	highlight_vertex[3].color=highlight;
	sfVertexArray_append(grid, highlight_vertex[3]);

	*draw_grid_highlight=1;
}


void map_remove_highlight(sfVertexArray* grid, sfBool* draw_grid_highlight)
{
	sfVertexArray_clear(grid);
	*draw_grid_highlight=0;

}
