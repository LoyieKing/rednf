#ifndef POWERWARTYPES_H_
#define POWERWARTYPES_H_

// from GuildDomain.h —— 轻量 POD 类型，独立成头以避免 PowerManager.h /
// PowerWarCharacInfo.h / Power.h 之间的包含环。
struct STGuildRank
{
    STGuildRank();
    unsigned int m0;  // guildKey@0
    unsigned int m4;  // point@4
};

struct STUserRank
{
    STUserRank();
    unsigned int m0;  // charNo@0
    unsigned int m4;  // point@4
};

struct STUserPoint
{
    STUserPoint();
    unsigned int m0;
    unsigned int m4;
};

struct STPowerWarPointInfo
{
    STPowerWarPointInfo();
    unsigned int m0;
    unsigned int m4;
};

#endif
