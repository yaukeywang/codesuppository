
#ifndef __VSOFONT_H
#define __VSOFONT_H

#define VSOFONT_STATUS_ORIGINAL   0
#define VSOFONT_STATUS_CLONE      1

#define VSOFONT_TYPE_FIXED        0
#define VSOFONT_TYPE_PROPORTIONAL 1

struct vsocharacter {
  unsigned short indicesn;
  unsigned char *indices;
  float width;
  float left_margin;
};

struct vsofont {
  unsigned char status;
  unsigned char type;
  unsigned int indices_type;
  unsigned int grid_dx;
  unsigned int grid_dy;
  float scale_x;
  float scale_y;
  float spacing;
  float *vertices;
  float r, g, b, a;
  struct vsocharacter **characters;
};

float vsofont_get_string_width(struct vsofont *font, unsigned char *s);
int vsofont_write_string_billboard_centered(struct vsofont *font, char *s, float x, float y, float z);
int vsofont_write_string_billboard(struct vsofont *font, char *s, float x, float y, float z);
int vsofont_write_string_2D_centered(struct vsofont *font, char *s, float x, float y);
int vsofont_write_string_2D(struct vsofont *font, char *s, float x, float y);
int vsofont_recreate_vertices(struct vsofont *font, float scale_x, float scale_y);
int vsofont_free(struct vsofont *font);
int vsofont_create(struct vsofont **font, int x, int y, float sx, float sy, float r, float g, float b, float a, float spacing);
int vsofont_character_create(struct vsocharacter **c, int i);
int vsofont_read_font_from_file(char *name, struct vsofont **font);
int vsofont_read_font_from_memory(unsigned char *mem, struct vsofont **font);
int vsofont_set_color(struct vsofont *font, float r, float g, float b, float a);
int vsofont_set_spacing(struct vsofont *font, float spacing);
int vsofont_set_type(struct vsofont *font, int type);
int vsofont_clone(struct vsofont *font_in, struct vsofont **font_out);

#endif
