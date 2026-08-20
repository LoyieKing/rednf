#ifndef DNF_GAME_HELL_PARTY_GROUP_ORDER_H_
#define DNF_GAME_HELL_PARTY_GROUP_ORDER_H_

#include <list>
#include <map>

struct STHellPartyGroupRate
{
    int m_groupIdx;
    int m_rate;
    char m_difficulty;
    char m_pad9[3];
};

struct STHellPartyGroupOrder
{
    unsigned short m_groupIdx;
    unsigned short m_pad2;
    unsigned short m_rateByDifficulty[6];
    std::list<STHellPartyGroupRate> m_rates;
};

typedef std::map<unsigned short, STHellPartyGroupOrder> HellPartyGroupOrderMap;
typedef std::map<int, HellPartyGroupOrderMap> HellPartyGroupOrderMapPool;

HellPartyGroupOrderMapPool* Get_instance_groupOrder_map_pool();

#endif
