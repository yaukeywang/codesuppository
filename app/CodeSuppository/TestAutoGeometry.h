#ifndef TEST_AUTO_GEOMETRY_H

#define TEST_AUTO_GEOMETRY_H

class MeshSystemHelper;

class TestAutoGeometry
{
public:
  virtual bool pump(void) = 0;  // returns true if still processing, false it can be released.
};


TestAutoGeometry * createTestAutoGeometry(MeshSystemHelper *h);
void               releaseTestAutoGeometry(TestAutoGeometry *t);


#endif
