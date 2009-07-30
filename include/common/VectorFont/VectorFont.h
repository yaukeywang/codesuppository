#ifndef VECTOR_FONT_H

#define VECTOR_FONT_H

class VectorFont
{
public:
  virtual void vprintf(const char *fmt,...) = 0;
  virtual void vputc(char c) = 0;
};


VectorFont * createVectorFont(void);
void         releaseVectorFont(VectorFont *vf);

#endif
