#ifndef DNF_GAME_CTRACE_MOB_DIE_HACK_H_
#define DNF_GAME_CTRACE_MOB_DIE_HACK_H_

#include <map>
#include <vector>

class CUser;

struct stMonsterDamageLog_t
{
    stMonsterDamageLog_t();
    unsigned short m_monsterId;
    unsigned short m_damage;
    int m_count;
    int m_monsterIndex;
};

struct stHackLog_t
{
    bool operator==(const stHackLog_t& other) const;
    unsigned int m_accId;
    unsigned int m_characNo;
    unsigned short m_hackType;
    int m_value;
    int m_count;
};

class CTraceMobDieHack
{
public:
    CTraceMobDieHack();
    ~CTraceMobDieHack();
    void reset();
    void traceBegin(long t, bool b);


    long m_traceBeginTime;
    std::map<unsigned short, stMonsterDamageLog_t> m_map1;
    std::map<unsigned short, stMonsterDamageLog_t> m_map2;
    std::vector<int> m_mobKillers;
    std::vector<stHackLog_t> m_hackLogs;
    void setMobKiller(int killer);
    void setDungeonDiff(int diff);
    int addMonsterDamage(int nMonsterIndex, unsigned short monsterId, int damage);
    void reportHackInfo();
    void traceEnd(CUser* killer, CUser** users);
    void _collect(CUser** users, int param_2);
    bool m_bTracing;
    char m_pad4d[3];
    struct stTraceRecord_t
    {
        CUser* m_user;
        int m_damage;
        int m_count;
    };
    stTraceRecord_t m_records[4];
    bool m_bInspectUser;
    char m_pad81[3];
    int m_dungeonDiff;
    // [b28d-solo-CParty2] 尾部 `char m_pad88[0x0c]` 是错误建模，已删：
    // ORIG setDungeonDiff 0x085bf646 `mov %edx,0x84(%eax)` ⇒ m_dungeonDiff@+0x84、
    // **sizeof==0x88**；其后 +0x298 起为 CParty 直属字段（m_memberLevelGap 等）。
};

#endif
