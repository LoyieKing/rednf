// TowerOfDespairMgr.cpp —— TOD 域首批叶子实现（PartyDepsB2 批次）。
// 逐指令依据：
//   TOD_Layer::TOD_Layer            docs/class_func_reports/TOD_Layer/TOD_Layer.md
//   TowerOfDespairMgr::SendAPCInfo  docs/class_func_reports/TowerOfDespairMgr/SendAPCInfo.md
#include "TowerOfDespairMgr.h"

#include "GameTypes.h"   // cMyTrace(char const*, int, int) / operator()(char const*, ...)

TOD_Layer::TOD_Layer(unsigned short layer)
{
    m_layer = layer;
    if (m_layer > 100)
    {
        cMyTrace trace("TOD_Layer::TOD_Layer(short unsigned int)", 0x10, 5);
        trace("@TOD: invalid layer setting : %d", (unsigned int)layer);
        m_layer = 1;
    }
}

void TowerOfDespairMgr::SendAPCInfo(TOD_Layer const& layer, CUser* user)
{
    // ORIG 0x08644338：尾调用 +0x354 处 APC 管理器（无返回值处理）。
    m_apcMgr->Send_APC_Info(layer, user);
}
