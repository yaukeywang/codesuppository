#ifndef HERSHEY_H

#define HERSHEY_H

class HersheyCallback
{
public:
  virtual void line(float x1,float y1,float x2,float y2) = 0;
};

void hersheyChar(char c,HersheyCallback *callback);

#endif
