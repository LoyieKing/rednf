#ifndef DNF_GAME_TOD_LAYER_H_
#define DNF_GAME_TOD_LAYER_H_

// ============================================================================
// TOD_Layer —— 死亡之塔层数（2 字节 ushort）
// 自 TowerOfDespairMgr.h 抽出（SigTypes_D.h 的 SIG_REQUEST_TOD_USER_STATE
// m_todayLayer 成员构造需完整类型，且 TowerOfDespairMgr.h 依赖 PacketBuf.h
// 存在包含环）。方法定义：ctor/increaseLayer/operator+ 见 PartyDepsA2.cpp，
// GetLayer（ORIG W 0x0822ec40）同 TU。
// 拷贝构造 ORIG W 0x085ff0fa：调 GetLayer() 后 mov %ax 直存（C1ERKS/C2ERKS 同址）。
// ============================================================================

class TOD_Layer
{
public:
    TOD_Layer(unsigned short layer);       // ORIG T 0x085fe7b4（C1/C2 同址）
    TOD_Layer(const TOD_Layer& other)      // ORIG W 0x085ff0fa（C1ERKS/C2ERKS 同址）
        : m_layer(other.GetLayer())
    {
    }
    void increaseLayer();                 // ORIG T 0x085fe824：m_layer+1（TOD_UserState::ClearLayer 用）

    unsigned short GetLayer() const;       // ORIG W 0x0822ec40（定义 PartyDepsA2.cpp）
    void resetLayer() { m_layer = 1; }     // ORIG 0x085fe852
    TOD_Layer operator+(int n) const;      // ORIG T 0x086439c8（定义 PartyDepsA2.cpp）

    bool operator<(TOD_Layer const& rhs) const   // ORIG W 0x085ff114（map key 序）
    {
        return m_layer < rhs.m_layer;
    }

    unsigned short m_layer;                // +0x00
};

#endif  // DNF_GAME_TOD_LAYER_H_
