#ifndef GAME_CGENUNIQUENO_H_
#define GAME_CGENUNIQUENO_H_

namespace WongWork
{
class CGenUniqueNo
{
public:
    // static 实证：ORIG 0x08177d35..0x08177d3e（ProcessBuyRequestEx 内）调用点只
    // push 一个实参（`lea -0x1a1(%ebp),%eax; mov %eax,(%esp); call 084b76d8`），
    // 无 this 槽；且实现（CGenUniqueNo.cpp）只读 static m_nTimeCount 与
    // G_CEnvironment()/GlobalData::s_systemTime_，不触碰 this。
    // C++98 下既有 obj.GenUniqueNo(x) 调用点对 static 成员同样合法。
    static char* GenUniqueNo(char* out);
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
