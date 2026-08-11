#ifndef POWERWARTYPES_H_
#define POWERWARTYPES_H_

// from GuildDomain.h —— 轻量 POD 类型，独立成头以避免 PowerManager.h /
// PowerWarCharacInfo.h / Power.h 之间的包含环。
struct STGuildRank
{
    STGuildRank();
    char m_data[8];   // {guildKey@0, point@4}
};

struct STUserRank
{
    STUserRank();
    char m_data[8];   // {charNo@0, point@4}
};

struct STUserPoint
{
    STUserPoint();
    char m_data[8];
};

struct STPowerWarPointInfo
{
    STPowerWarPointInfo();
    char m_data[8];
};

#endif
