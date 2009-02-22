#ifndef APEX_RENDER_INTERFACE_H

#define APEX_RENDER_INTERFACE_H


namespace CLIENT_PHYSICS
{
class ApexRenderInterface;
};

CLIENT_PHYSICS::ApexRenderInterface * createApexRenderInterface(void);

extern CLIENT_PHYSICS::ApexRenderInterface *gApexRenderInterface;

#endif
