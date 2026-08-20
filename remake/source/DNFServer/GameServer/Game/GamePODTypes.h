#ifndef DNF_GAME_POD_TYPES_H_
#define DNF_GAME_POD_TYPES_H_

#pragma pack(push, 1)
struct stAvatarEmblemInfo_t
{
    void init();
    struct
    {
        unsigned short emblemSocketType_;
        int emblemItemIdx_;
    } stEmblemSocket[5];
};
#pragma pack(pop)

struct stAvatarExpansionInfo_t
{
    void init();
    short m_color[2];
};

struct AvatarDetailInfo
{
    int m_key;
    int m_expireDate;
    int m_registTime;
    char m_ipgAgencyNo[25];
    stAvatarEmblemInfo_t m_jewelSocket;
    stAvatarExpansionInfo_t m_expansionInfo;
};

#endif
