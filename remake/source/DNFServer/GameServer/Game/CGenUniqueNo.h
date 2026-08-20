#ifndef GAME_CGENUNIQUENO_H_
#define GAME_CGENUNIQUENO_H_

namespace WongWork
{
class CGenUniqueNo
{
public:
    CGenUniqueNo();
    char* GenUniqueNo(char* out);
    enum ENUM_IPGNO_TYPE
    {
        ENUM_IPGNO_TYPE_0,
        ENUM_IPGNO_TYPE_1,
        ENUM_IPGNO_TYPE_2,
        ENUM_IPGNO_TYPE_3,
        ENUM_IPGNO_TYPE_4,
        ENUM_IPGNO_TYPE_5,
        ENUM_IPGNO_TYPE_6
    };
    static char* genIPGNo(ENUM_IPGNO_TYPE type, unsigned int no, char* out);

private:
    static void _updateTimeCount();
    static unsigned short m_nTimeCount;
};
}

#endif
