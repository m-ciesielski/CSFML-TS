#include "lib.h"
#define flora_vertex_tile_size 160
#define flora_texture_width 116
#define flora_texture_height 143
void generate_flora (terrain g_map [max_map_h][max_map_w], sfVertexArray* flora_array)
{
srand( time( NULL ) );
     short int random;
     short int i,j, k, l;
     sfVertex flora_vertex;
     for(i=0;i<map_h;++i)
     {
         for(j=0;j<map_w;++j)
         {
             if(g_map[i][j].type==2)
             {
                 for(k=0;k<(map_block_h/100);++k)
                 {
                     for(l=0;l<(map_block_w/100);++l)
                     {
                         random=(rand() % 100);

                         if(random>90)
                         {
                             flora_vertex.position.x=map_block_w*j+k*flora_vertex_tile_size;
                             flora_vertex.position.y=map_block_h*i+l*flora_vertex_tile_size;
                             flora_vertex.texCoords.x=0;
                             flora_vertex.texCoords.y=0;
                             flora_vertex.color=sfWhite;
                             sfVertexArray_append(flora_array, flora_vertex);

                             flora_vertex.position.x=map_block_w*j+k*flora_vertex_tile_size+flora_vertex_tile_size;
                             flora_vertex.position.y=map_block_h*i+l*flora_vertex_tile_size;
                             flora_vertex.texCoords.x=flora_texture_width;
                             flora_vertex.texCoords.y=0;
                             flora_vertex.color=sfWhite;
                             sfVertexArray_append(flora_array, flora_vertex);

                             flora_vertex.position.x=map_block_w*j+k*flora_vertex_tile_size+flora_vertex_tile_size;
                             flora_vertex.position.y=map_block_h*i+l*flora_vertex_tile_size+flora_vertex_tile_size;
                             flora_vertex.texCoords.x=flora_texture_width;
                             flora_vertex.texCoords.y=flora_texture_height;
                             flora_vertex.color=sfWhite;
                             sfVertexArray_append(flora_array, flora_vertex);

                             flora_vertex.position.x=map_block_w*j+k*flora_vertex_tile_size;
                             flora_vertex.position.y=map_block_h*i+l*flora_vertex_tile_size+flora_vertex_tile_size;
                             flora_vertex.texCoords.x=0;
                             flora_vertex.texCoords.y=flora_texture_height;
                             flora_vertex.color=sfWhite;
                             sfVertexArray_append(flora_array, flora_vertex);
                         }
                     }
                 }
             }
             /*
             else if(g_map[i][j].type==2)
             {

             }
             else
             {

             }
             */
         }
     }

}
