// df_game_r Game/ STAssignMonster 专属 TU（从 GameStubs.cpp 拆出）。
// 逐方法对照 ORIG df_game_r 反汇编实现（证据见各函数注释）。
#include "STAssignMonster.h"

// ORIG 0x836c5e0：默认构造 +0x28/+0x40/+0x5c 三个 vector（编译器按成员自动完成），
// 随后调用 Clear() 置初值。
STAssignMonster::STAssignMonster()
{
    Clear();
}

// ORIG 0x8377696：标量逐字段复制 + 三个 vector 拷贝构造。
STAssignMonster::STAssignMonster(const STAssignMonster& other)
    : m_0(other.m_0), m_4(other.m_4), m_8(other.m_8), m_c(other.m_c),
      m_10(other.m_10), m_14(other.m_14), m_18(other.m_18), m_1c(other.m_1c),
      m_20(other.m_20), m_24(other.m_24), m_vec28(other.m_vec28),
      m_34(other.m_34), m_38(other.m_38), m_3c(other.m_3c),
      m_vec40(other.m_vec40), m_4c(other.m_4c), m_50(other.m_50),
      m_54(other.m_54), m_55(other.m_55), m_58(other.m_58),
      m_vec5c(other.m_vec5c)
{
}

// ORIG 0x8377622：三个 vector 按声明逆序自动析构（ORIG 显式调用 ~vector）。
STAssignMonster::~STAssignMonster()
{
}

// ORIG 0x83777f2：标量逐字段赋值 + 三个 vector operator=。
STAssignMonster& STAssignMonster::operator=(const STAssignMonster& other)
{
    if (this != &other)
    {
        m_0 = other.m_0;
        m_4 = other.m_4;
        m_8 = other.m_8;
        m_c = other.m_c;
        m_10 = other.m_10;
        m_14 = other.m_14;
        m_18 = other.m_18;
        m_1c = other.m_1c;
        m_20 = other.m_20;
        m_24 = other.m_24;
        m_vec28 = other.m_vec28;
        m_34 = other.m_34;
        m_38 = other.m_38;
        m_3c = other.m_3c;
        m_vec40 = other.m_vec40;
        m_4c = other.m_4c;
        m_50 = other.m_50;
        m_54 = other.m_54;
        m_55 = other.m_55;
        m_58 = other.m_58;
        m_vec5c = other.m_vec5c;
    }
    return *this;
}

// ORIG 0x836c4fa：字段复位初值；仅 m_vec28/m_vec5c 清空（m_vec40 不动）。
void STAssignMonster::Clear()
{
    m_0 = -1;
    m_4 = 1;
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
    m_14 = 0;
    m_1c = 1;
    m_18 = m_1c;
    m_20 = 0;
    m_24 = 0;
    m_vec28.clear();
    m_50 = 0;
    m_4c = 100;
    m_54 = 0;
    m_55 = 0;
    m_58 = -1;
    m_38 = 0;
    m_3c = 0;
    m_34 = 0;
    m_vec5c.clear();
}