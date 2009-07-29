
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "defines.h"
#include "vsofont.h"

#define UNSIGNED_BYTE 0
#define UNSIGNED_SHORT 1

#pragma warning(disable:4100 4244 4018)

/* reset the modelview to no rotations and no scaling */
static int _vsofont_billboard_spherical_cheat_apply(float x, float y, float z)
{

  float m[4*4];
  int i;


//  glPushMatrix();
  /* go to the new position */
//  glTranslatef(x, y, z);
//  glGetFloatv(GL_MODELVIEW_MATRIX, m);

  /* undo rotations and scaling */
  i = 0;

  m[i++] = 1.0f;
  m[i++] = 0.0f;
  m[i++] = 0.0f;
  i++;
  m[i++] = 0.0f;
  m[i++] = 1.0f;
  m[i++] = 0.0f;
  i++;
  m[i++] = 0.0f;
  m[i++] = 0.0f;
  m[i] = 1.0f;

  /* set the modelview with no rotations and scaling */
//  glLoadMatrixf(m);

  return SUCCEEDED;
}

static void drawLines(float x,const float *vertices,int icount,const unsigned char *indices)
{
}

static void drawLines(float x,const float *vertices,int icount,const unsigned short *indices)
{
}

static int _vsofont_write_string(struct vsofont *font, unsigned char *u)
{

  struct vsocharacter *c;
  unsigned int i, k;
  float p, fx, fg;


  const float * vertices = font->vertices;


  fx = font->scale_x + font->spacing;
  fg = 1.0f/((float)(font->grid_dx-1))*font->scale_x;

  float cpos = 0;

  /* write the letters one by one */
  if (font->type == VSOFONT_TYPE_FIXED)
  {
    i = 0;
    while (u[i] != 0)
	{
      k = u[i++];
      c = font->characters[k];
      if (c != NULL)
	  {
        if ( font->indices_type == UNSIGNED_BYTE )
  		  drawLines(cpos, vertices, c->indicesn, (const unsigned char *) c->indices);
        else
		  drawLines(cpos, vertices, c->indicesn, (const unsigned short *) c->indices);
      }
      cpos+=fx;
    }
  }
  else
  {
    i = 0;
    p = 0.0f;
    while (u[i] != 0)
	{
      k = u[i++];
      c = font->characters[k];
      if (c != NULL)
	  {
//		glTranslatef(-(c->left_margin*fg) + p, 0, 0);
//		glDrawElements(GL_LINES, c->indicesn, font->indices_type, c->indices);
		p = c->width + c->left_margin;
		p = p*fg + font->spacing;
      }
      else
	  {
//		glTranslatef(fx, 0, 0);
	  }
    }
  }

  return SUCCEEDED;
}


int vsofont_recreate_vertices(struct vsofont *font, float scale_x, float scale_y) {

  float px, py, ax, ay;
  int x, y, i;


  if (font == NULL)
    return FAILED;

  ax = 1.0f/(font->grid_dx - 1.0f)*scale_x;
  ay = 1.0f/(font->grid_dy - 1.0f)*scale_y;
  py = scale_y;

  for (i = 0, y = 0; y < font->grid_dy; y++) {
    px = 0.0f;
    for (x = 0; x < font->grid_dx; x++) {
      font->vertices[i++] = px;
      font->vertices[i++] = py;
      px += ax;
    }
    py -= ay;
  }

  font->scale_x = scale_x;
  font->scale_y = scale_y;

  return SUCCEEDED;
}


int vsofont_write_string_billboard_centered(struct vsofont *font, char *s, float x, float y, float z) 
{

  float p;


  if (font == NULL || s == NULL)
    return 0;

  p = vsofont_get_string_width(font, (unsigned char *)s);
  p = -p/2;

  /* fix the modelview */
  _vsofont_billboard_spherical_cheat_apply(x, y, z);
//  glTranslatef(p, -font->scale_y/2.0f, 0);

  /* write the string */
  _vsofont_write_string(font, (unsigned char *)s);

  /* return the modelview */
//  glPopMatrix();

  return SUCCEEDED;
}


int vsofont_write_string_billboard(struct vsofont *font, char *s, float x, float y, float z) {

  if (font == NULL || s == NULL)
    return 0;

  /* fix the modelview */
  _vsofont_billboard_spherical_cheat_apply(x, y, z);

  /* write the string */
  _vsofont_write_string(font, (unsigned char *)s);

  /* return the modelview */
//  glPopMatrix();

  return SUCCEEDED;
}


int vsofont_write_string_2D_centered(struct vsofont *font, char *s, float x, float y) {

  float p;


  if (font == NULL || s == NULL)
    return 0;

  p = vsofont_get_string_width(font, (unsigned char *)s);
  p = -p/2;

  /* go to the starting position */
//  glPushMatrix();
//  glTranslatef(p + x, y - font->scale_y/2.0f, 0);

  /* write the string */
  _vsofont_write_string(font, (unsigned char *)s);

//  glPopMatrix();

  return SUCCEEDED;
}


int vsofont_write_string_2D(struct vsofont *font, char *s, float x, float y)
{

  if (font == NULL || s == NULL)
    return 0;

  /* go to the starting position */
//  glPushMatrix();
//  glTranslatef(x, y, 0);

  /* write the string */
  _vsofont_write_string(font, (unsigned char *)s);

//  glPopMatrix();

  return SUCCEEDED;
}


float vsofont_get_string_width(struct vsofont *font, unsigned char *s) {

  struct vsocharacter *vc;
  float f, fx, fg, g;
  int n;


  if (font == NULL || s == NULL)
    return 0;

  f = -font->spacing;
  n = 0;
  fx = font->scale_x + font->spacing;

  if (font->type == VSOFONT_TYPE_FIXED) {
    while (s[n] != 0) {
      f += fx;
      n++;
    }
  }
  else {
    fg = 1.0f/(font->grid_dx-1)*font->scale_x;
    g = 0.0f;
    while (s[n] != 0) {
      vc = font->characters[(int)s[n++]];
      if (vc != NULL) {
	g += vc->width;
	f += font->spacing;
      }
      else
	f += fx;
    }
    f += g*fg;
  }

  return f;
}


int vsofont_set_color(struct vsofont *font, float r, float g, float b, float a) {

  if (font == NULL)
    return FAILED;

  font->r = r;
  font->g = g;
  font->b = b;
  font->a = a;

  return SUCCEEDED;
}


int vsofont_set_spacing(struct vsofont *font, float spacing) {

  if (font == NULL)
    return FAILED;

  font->spacing = spacing;

  return SUCCEEDED;
}


int vsofont_set_type(struct vsofont *font, int type) {

  if (font == NULL)
    return FAILED;

  if (!(type == VSOFONT_TYPE_FIXED || type == VSOFONT_TYPE_PROPORTIONAL))
    return FAILED;

  font->type = type;

  return SUCCEEDED;
}


int vsofont_free(struct vsofont *font) {

  int i;


  if (font == NULL)
    return FAILED;

  /* only original fonts hold the characters */
  if (font->status == VSOFONT_STATUS_ORIGINAL) {
    for (i = 0; i < 256; i++) {
      if (font->characters[i] != NULL) {
	if (font->characters[i]->indices != NULL)
	  free(font->characters[i]->indices);
	free(font->characters[i]);
      }
    }
  }

  free(font->vertices);
  free(font);

  return SUCCEEDED;
}


int vsofont_character_create(struct vsocharacter **c, int i) {

  if (c == NULL || i <= 0)
    return FAILED;

  *c = (vsocharacter *)malloc(sizeof(struct vsocharacter));
  if (*c == NULL) {
    fprintf(stderr, "VSOFONT_CHARACTER_CREATE: Out of memory error.\n");
    return FAILED;
  }

  if (i < 256)
    (*c)->indices = (unsigned char *)malloc(i);
  else
    (*c)->indices = (unsigned char *)malloc(i*sizeof(unsigned short));
  if ((*c)->indices == NULL) {
    free(*c);
    *c = NULL;
    fprintf(stderr, "VSOFONT_CHARACTER_CREATE: Out of memory error.\n");
    return FAILED;
  }

  (*c)->indicesn = i;
  (*c)->width = 0.0f;
  (*c)->left_margin = 0.0f;

  return SUCCEEDED;
}


int vsofont_create(struct vsofont **font, int x, int y, float sx, float sy, float r, float g, float b, float a, float spacing) {

  struct vsofont *f;
  int i;


  if (font == NULL)
    return FAILED;

  *font = NULL;

  f = (vsofont *)malloc(sizeof(struct vsofont));
  if (f == NULL) {
    fprintf(stderr, "VSOFONT_CREATE: Out of memory error.\n");
    return FAILED;
  }

  /* create list of grid's vertices */
  f->vertices = (float *)malloc(sizeof(float) * x*y * 2);
  if (f->vertices == NULL) {
    free(f);
    fprintf(stderr, "VSOFONT_CREATE: Out of memory error.\n");
    return FAILED;
  }

  /* allocate room for the characters */
  f->characters = (vsocharacter **)malloc(sizeof(struct vsocharacter *) * 256);
  if (f->characters == NULL) {
    free(f->vertices);
    free(f);
    fprintf(stderr, "VSOFONT_CREATE: Out of memory error.\n");
    return FAILED;
  }

  f->grid_dx = x;
  f->grid_dy = y;
  f->scale_x = sx;
  f->scale_y = sy;
  f->spacing = spacing;
  f->r = r;
  f->g = g;
  f->b = b;
  f->a = a;
  f->status = VSOFONT_STATUS_ORIGINAL;
  f->type = VSOFONT_TYPE_FIXED;

  if (x*y < 255)
    f->indices_type = UNSIGNED_BYTE;
  else
    f->indices_type = UNSIGNED_SHORT;

  /* clear the character array */
  for (i = 0; i < 256; i++)
    f->characters[i] = NULL;

  vsofont_recreate_vertices(f, sx, sy);

  *font = f;

  return SUCCEEDED;
}


int vsofont_clone(struct vsofont *font_in, struct vsofont **font_out) {

  struct vsofont *f;
  int i;


  if (font_in == NULL || font_out == NULL)
    return FAILED;

  *font_out = NULL;

  f = (vsofont *)malloc(sizeof(struct vsofont));
  if (f == NULL) {
    fprintf(stderr, "VSOFONT_CLONE: Out of memory error.\n");
    return FAILED;
  }

  /* create list of grid's vertices */
  f->vertices = (float *)malloc(sizeof(float) * font_in->grid_dx*font_in->grid_dy * 2);
  if (f->vertices == NULL) {
    free(f);
    fprintf(stderr, "VSOFONT_CLONE: Out of memory error.\n");
    return FAILED;
  }

  /* clone the vertices */
  for (i = 0; i < font_in->grid_dx*font_in->grid_dy*2; i++)
    f->vertices[i] = font_in->vertices[i];

  /* clone the data */
  f->characters = font_in->characters;
  f->grid_dx = font_in->grid_dx;
  f->grid_dy = font_in->grid_dy;
  f->scale_x = font_in->scale_x;
  f->scale_y = font_in->scale_y;
  f->spacing = font_in->spacing;
  f->r = font_in->r;
  f->g = font_in->g;
  f->b = font_in->b;
  f->a = font_in->a;
  f->type = font_in->type;
  f->indices_type = font_in->indices_type;
  f->status = VSOFONT_STATUS_CLONE;

  *font_out = f;

  return SUCCEEDED;
}


static void _vsofont_read_string(unsigned char *m, unsigned char *o, int *l, int *n) {

  int i;


  *l = 0;
  i = *n;

  while (m[i] == ' ' || m[i] == 0xA || m[i] == 0xD)
    i++;

  /* read the string */
  while (m[i] != 0) {
    if (m[i] == ' ' || m[i] == 0xA || m[i] == 0xD)
      break;
    *(o++) = m[i++];
    (*l)++;
  }

  *o = 0;
  *n = i;
}


static void _vsofont_read_int(unsigned char *m, int *r, int *n) {

  int i, v, s;


  i = *n;
  v = 0;

  while (m[i] == ' ' || m[i] == 0xA || m[i] == 0xD)
    i++;

  /* determine sign */
  if (m[i] == '-') {
    i++;
    s = -1;
  }
  else
    s = 1;

  /* read the integer */
  while (m[i] != 0) {
    if (m[i] < '0' || m[i] > '9')
      break;
    v = v*10 + m[i++]-'0';
  }

  if (s == 1)
    *r = v;
  else
    *r = -v;

  *n = i;
}


static void _vsofont_read_float(unsigned char *m, float *r, int *n) {

  float v, a;
  int i, s;


  i = *n;
  v = 0.0f;

  while (m[i] == ' ' || m[i] == 0xA || m[i] == 0xD)
    i++;

  /* determine sign */
  if (m[i] == '-') {
    i++;
    s = -1;
  }
  else
    s = 1;

  /* read the integer part */
  while (m[i] != 0) {
    if (m[i] < '0' || m[i] > '9')
      break;
    v = v*10 + m[i++]-'0';
  }

  if (m[i] == '.') {
    /* read the rest */
    i++;
    a = 0.1f;
    while (m[i] != 0) {
      if (m[i] < '0' || m[i] > '9')
	break;
      v += a*(m[i++]-'0');
      a /= 10.0f;
    }
  }

  if (s == 1)
    *r = v;
  else
    *r = -v;

  *n = i;
}


int vsofont_read_font_from_memory(unsigned char *mem, struct vsofont **font) {

  struct vsocharacter *vc;
  float sp, sx, sy, r, g, b, a;
  int d, pos, i, k, l, ch, m, o, x;
  unsigned char n, tmp[256];


  if (mem == NULL || font == NULL)
    return FAILED;

  x = 0;

  /* seek to "JUMP!" */
  static bool state = true;
  while ( state ) 
  {
    _vsofont_read_string(mem, tmp, &o, &x);
    if (o == 0)
      break;
    if (strcmp((char *)tmp, "JUMP!") == 0)
      break;
  }

  /* read the grid's size */
  _vsofont_read_string(mem, tmp, &o, &x);
  _vsofont_read_int(mem, &d, &x);
  _vsofont_read_string(mem, tmp, &o, &x);
  _vsofont_read_int(mem, &i, &x);

  if (d*i >= 65536) {
    fprintf(stderr, "VSOFONT_READ_FONT_FROM_MEMORY: Grid %dx%d has more than 65535 vertices. Cannot create such a big grid.\n", d, i);
    return FAILED;
  }

  /* read the spacing */
  _vsofont_read_string(mem, tmp, &o, &x);
  _vsofont_read_float(mem, &sp, &x);

  /* read the scaling */
  _vsofont_read_string(mem, tmp, &o, &x);
  _vsofont_read_float(mem, &sx, &x);
  _vsofont_read_string(mem, tmp, &o, &x);
  _vsofont_read_float(mem, &sy, &x);

  /* read the color */
  _vsofont_read_string(mem, tmp, &o, &x);
  _vsofont_read_float(mem, &r, &x);
  _vsofont_read_float(mem, &g, &x);
  _vsofont_read_float(mem, &b, &x);
  _vsofont_read_float(mem, &a, &x);

  /* create a font structure */
  if (vsofont_create(font, d, i, sx, sy, r, g, b, a, sp) == FAILED)
    return FAILED;

  /* read the characters */
  d = 0;
  while (mem[x] != 0) {
    n = mem[x++];

    if (n == 0xA || n == 0xD || n == ' ')
      continue;

    if (n == '\\') {
      /* read the value */
      _vsofont_read_int(mem, &ch, &x);
    }
    else
      ch = n;

    if (ch < 1 || ch > 255) {
      fprintf(stderr, "VSOFONT_READ_FONT_FROM_MEMORY: %d is out of range [1, 255].\n", ch);
      vsofont_free(*font);
      return FAILED;
    }

    /* count the indices */
    pos = x;

    i = 0;
    while (mem[x] != 0) {
      _vsofont_read_int(mem, &k, &x);
      if (k == -1)
	break;
      i++;
    }

    if ((i & 1) != 0) {
      fprintf(stderr, "VSOFONT_READ_FONT_FROM_MEMORY: Odd amount of indices in character '%c'.\n", n);
      vsofont_free(*font);
      return FAILED;
    }

    if ((*font)->characters[ch] != NULL) {
      fprintf(stderr, "VSOFONT_READ_FONT_FROM_MEMORY: Character '%c' has already been defined.\n", n);
      continue;
    }

    /* create a new character */
    if (vsofont_character_create(&(*font)->characters[ch], i) == FAILED) {
      vsofont_free(*font);
      return FAILED;
    }

    vc = (*font)->characters[ch];

    /* input the indices */
    x = pos;

    for (l = 0; l < i; l++) {
      _vsofont_read_int(mem, &pos, &x);
      vc->indices[l] = pos;
    }

    /* skip -1 */
    _vsofont_read_string(mem, tmp, &o, &x);

    /* compute character width and left margin */
    k = 0;
    m = (*font)->grid_dx;
    for (l = 0; l < vc->indicesn; l++) {
      d = vc->indices[l] % (*font)->grid_dx;
      if (d > k)
	k = d;
      if (d < m)
	m = d;
    }

    vc->width = k-m;
    vc->left_margin = m;

    d++;
  }

  return SUCCEEDED;
}


int vsofont_read_font_from_file(char *name, struct vsofont **font) {

  unsigned char *c;
  FILE *f;
  int i;


  if (name == NULL || font == NULL)
    return FAILED;

  f = fopen(name, "rb");
  if (f == NULL) {
    fprintf(stderr, "VSOFONT_READ_FONT_FROM_FILE: Could not open file \"%s\".\n", name);
    return FAILED;
  }

  fseek(f, 0, SEEK_END);
  i = ftell(f);
  fseek(f, 0, SEEK_SET);

  c = (unsigned char *)malloc(i+1);
  if (c == NULL) {
    fprintf(stderr, "VSOFONT_READ_FONT_FROM_FILE: Out of memory error.\n");
    fclose(f);
    return FAILED;
  }

  fread(c, 1, i, f);
  fclose(f);

  /* terminate the buffer */
  c[i] = 0;

  i = vsofont_read_font_from_memory(c, font);
  free(c);

  return i;
}

