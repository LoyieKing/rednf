// AvatarVariation 域实现 —— ORIG 0x088d43f0..0x088d495b 转写。
// 类型权威定义见 CDataManager.h（namespace AvatarVariation）；声明见 AvatarVariation.h。
// importAvatarColorVariation（ORIG 0x088d444f）定义于 InitScriptProducers_8.cpp。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15AvatarVariation8colorRGBC2Ev`（ORIG 0x88d499c）；首强符号 = colorRGB ctor。
#include <iostream>

#include "CDataManager.h"
#include "AvatarVariation.h"

#include <map>
#include <string>

namespace AvatarVariation
{

// ORIG C1 0x088d43f0：三字节各置 0xff（函数体逐句赋值形态，每句重读 this）。
colorRGB::colorRGB()
{
    m_r = 0xff;
    m_g = 0xff;
    m_b = 0xff;
}

// ORIG 0x088d440a：三字段短路相等比较（movzbl 宽度=unsigned char 实证）。
bool colorRGB::operator==(colorRGB const& rhs)
{
    return m_r == rhs.m_r && m_g == rhs.m_g && m_b == rhs.m_b;
}

// ORIG 0x088d46d3：函数局部 static（guard/atexit 编译器生成），每次调用经
// importAvatarColorVariation 装载；失败返回 0。
AvatarColorInfo* getAvatarColorInfoInst()
{
    static AvatarColorInfo s_avatarColorInfo;
    char ok = importAvatarColorVariation(&s_avatarColorInfo);
    if (ok != 0)
        return &s_avatarColorInfo;
    else
        return 0;
}

// ORIG 0x088d4782：key∈[0,0x1ff] 查 m_colorMap；info 空或未找到 → 默认
// colorRGB()（三字节 0xff）。按值返回（i386 POD 3 字节走 sret）。
colorRGB getAvatarColorValue(int key)
{
    if (key < 0 || key > 0x1ff)
        key = 0;

    std::map<int, colorRGB>* colorMap = &getAvatarColorInfoInst()->m_colorMap;
    if (colorMap == 0)
        return colorRGB();

    std::map<int, colorRGB>::const_iterator it = colorMap->find(key);
    if (it != colorMap->end())
        return it->second;

    return colorRGB();
}

// ORIG 0x088d4856：与 getAvatarColorValue(0)（默认色）相同则不可启用。
bool isEnAbleColorKey(colorRGB& color)
{
    colorRGB defaultColor = getAvatarColorValue(0);
    if (color == defaultColor)
        return false;
    return true;
}

// ORIG 0x088d4896：key∈[0,0x1ff] 查 m_nameMap 返回 c_str；越界/info 空/未找到 → 0。
const char* getAvatarColorName(int key)
{
    if (key < 0 || key > 0x1ff)
        return 0;

    std::map<int, std::string>* nameMap = &getAvatarColorInfoInst()->m_nameMap;
    if (nameMap == 0)
        return 0;

    std::map<int, std::string>::const_iterator it = nameMap->find(key);
    if (it != nameMap->end())
        return it->second.c_str();

    return 0;
}

}  // namespace AvatarVariation
