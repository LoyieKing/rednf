// arad::SigLoadRewardEventItem —— ORIG W 生命周期/方法声明头（仅声明，无实现）。
// ORIG：C1/C2 W 0x08186686、init() 0x0818669a、set(unsigned int) 0x081866b6。
// 布局（来自 W8_AradSig.cpp set() 实证）：m_field0 / m_count /
// m_characNos[0x28] / m_eventCodes[0x28]；总长 0x195 由
// Arad_ServerStateManager::put_binary(&sig, 0x195) 佐证——完整权威布局以
// 未来收敛 TU 为准，此处仅锁定方法集与尺寸。
#ifndef DNF_GAME_SIGLOADREWARDEVENTITEM_H_
#define DNF_GAME_SIGLOADREWARDEVENTITEM_H_

namespace arad
{

class SigLoadRewardEventItem
{
public:
    SigLoadRewardEventItem();
    void init();
    void set(unsigned int v);

    unsigned int m_field0;             // +0x00【推断】事件/字段 0
    unsigned int m_count;              // +0x04【推断】条目数
    unsigned char m_characNos[0x28];   // +0x08【推断】
    unsigned char m_eventCodes[0x28];  // +0x30【推断】（合计 0x195 布局待实证）
};

}  // namespace arad

#endif  // DNF_GAME_SIGLOADREWARDEVENTITEM_H_