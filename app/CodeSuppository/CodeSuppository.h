#ifndef CODE_SUPPOSITORY_H

#define CODE_SUPPOSITORY_H

enum CodeSuppositoryCommand
{
  CSC_BEST_FIT_OBB = 4000,
  CSC_BEST_FIT_PLANE,
  CSC_STAN_HULL,
  CSC_INPARSER,
  CSC_CLIPPER,
  CSC_FRUSTUM,
  CSC_PLANE_TRI,
  CSC_VERTEX_LOOKUP,
  CSC_MAP_PAL,
  CSC_MESH_VOLUME,
  CSC_DFRAC,
  CSC_LOOKAT,
  CSC_CONVEX_DECOMPOSITION,
  CSC_WINMSG,
  CSC_WILDCARD,
  CSC_GESTALT,
  CSC_ASC2BIN,
  CSC_FILE_INTERFACE,
  CSC_KEY_VALUE_INI,
  CSC_TINY_XML,
  CSC_KDTREE,
  CSC_MESH_CLEANUP,
  CSC_FAST_ASTAR,
  CSC_SAS,
  CSC_COMPRESSION,
  CSC_SPLIT_MESH,
  CSC_ARROW_HEAD,
  CSC_SEND_MAIL,
  CSC_SEND_AIM,
  CSC_EROSION,
};

class CodeSuppository
{
public:
  virtual void processCommand(CodeSuppositoryCommand command,bool state=true,const float *data=0) = 0;
  virtual void render(float dtime) = 0;
};

CodeSuppository * createCodeSuppository(void);
void              releaseCodeSuppository(CodeSuppository *c);


extern CodeSuppository *gCodeSuppository;

#endif
