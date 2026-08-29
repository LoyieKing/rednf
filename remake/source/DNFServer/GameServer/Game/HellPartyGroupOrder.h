#ifndef DNF_GAME_HELL_PARTY_GROUP_ORDER_H_
#define DNF_GAME_HELL_PARTY_GROUP_ORDER_H_

#include <list>
#include <map>
#include <cstring>

struct STHellPartyGroupRate
{
    int m_groupIdx;
    int m_rate;
    char m_difficulty;
    char m_pad9[3];
};

struct STHellPartyGroupOrder
{
    // ORIG C1Ev 0x89e8712：list 构造于 +0xc 后 memset(this,0,0xc)（前 0xc 字节清零）。
    STHellPartyGroupOrder()
    {
        memset(this, 0, 0xc);
    }
    // ORIG C1Ev 0x89e8712 / 拷贝 C1ERKS_ 0x89e9dfc 实证：
    //   memset(this,0,0xc) + list 构造于 +0xc（拷贝为三个 dword 复制 + list），
    //   sizeof == 0x14；add_hell_party_monster 083071ba 读 u16@second+2i、
    //   list@pair+0x10（= 本结构 +0x00/+0x0c）。
    unsigned short m_rateByDifficulty[6];      // +0x00（按地狱派对难度索引）
    std::list<STHellPartyGroupRate> m_rates;   // +0x0c
};

static_assert(sizeof(STHellPartyGroupOrder) == 0x14, "STHellPartyGroupOrder size");

typedef std::map<unsigned short, STHellPartyGroupOrder> HellPartyGroupOrderMap;
typedef std::map<int, HellPartyGroupOrderMap> HellPartyGroupOrderMapPool;

HellPartyGroupOrderMapPool* Get_instance_groupOrder_map_pool();

#endif
