#include "sp_render_flow.h"

void sp_render_flow_init(sp_render_flow* flow)
{
    sp_forward_init(&flow->forward);
}   

void sp_render_flow_free(sp_render_flow* flow)
{
    sp_forward_free(&flow->forward);
}

void sp_render_flow_update(sp_render_flow* flow)
{
    sp_forward_update(&flow->forward, flow->update);
}