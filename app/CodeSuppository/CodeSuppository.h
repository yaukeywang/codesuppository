#ifndef CODE_SUPPOSITORY_H

#define CODE_SUPPOSITORY_H

enum CodeSuppositoryCommand
{
  CSC_BEST_FIT_OBB = 4000,
  CSC_BEST_FIT_CAPSULE,
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
  CSC_SHOW_SKELETON,
  CSC_SHOW_MESH,
  CSC_CLEAR_MESH,
  CSC_SHOW_WIREFRAME,
  CSC_PLAY_ANIMATION,
  CSC_FLIP_WINDING,
  CSC_EXPORT_EZM,
  CSC_EXPORT_OBJ,
  CSC_EXPORT_OGRE,
  CSC_AUTO_GEOMETRY,
  CSC_SHOW_COLLISION,
  CSC_APEX_CLOTH,
  CSC_ANIMATION_SPEED,
  CSC_MERGE_PERCENTAGE,
  CSC_CONCAVITY_PERCENTAGE,
  CSC_FIT_OBB,
  CSC_DEPTH,
  CSC_VOLUME_PERCENTAGE,
  CSC_MAX_VERTICES,
  CSC_SKIN_WIDTH,
  CSC_REMOVE_TJUNCTIONS,
  CSC_INITIAL_ISLAND_GENERATION,
  CSC_ISLAND_GENERATION,
  CSC_SELECT_COLLISION,
  CSC_TEST_REMOVE_TJUNCTIONS,
  CSC_TEST_ISLAND_GENERATION,
  CSC_TEST_MESH_CONSOLIDATION,
  CSC_TEST_VECTOR_FONT,
  CSC_TEST_MARCHING_CUBES,
};

class CodeSuppository
{
public:
  virtual void processCommand(CodeSuppositoryCommand command,bool state=true,const NxF32 *data=0) = 0;
  virtual void render(NxF32 dtime) = 0;
  virtual void importMesh(const char *fname) = 0;
  virtual void process(NxF32 dtime) = 0;
};

CodeSuppository * createCodeSuppository(void);
void              releaseCodeSuppository(CodeSuppository *c);

namespace CLIENT_PHYSICS
{
  class ApexScene;
  class Apex;
};

extern CLIENT_PHYSICS::ApexScene *gApexScene;
extern CLIENT_PHYSICS::Apex *gApex;

extern CodeSuppository *gCodeSuppository;

#endif
