// df_game_r Game/ APSystem::CActionPointManager 实现（批3：从 GameStubs.cpp 迁移）。
// 类声明位于 CUser.h（APSystem::CActionPointManager）。本 TU 是这些符号的唯一权威定义。
// ORIG：GM_Set @0x8121b3e（T）、Reset @0x8122af4（W）、GetActionPoint @0x8694bfc（W）。
#include "CUser.h"
#include <string.h>

namespace APSystem
{

// _ActionGroupIndex / CUserProc 权威声明见 CUserProc.h（本 TU 本地镜像已删）。

// ORIG 0x8694bfc（W，内联）：mov 0x8(%ebp),%eax; add $0x8,%eax; ret。
// 返回 this+0x08 数据体首地址。具体元素类型无报告证据（推断标注），
// 以字节指针忠实转写返回形态；mangled 符号不含返回类型，ABI 不变。
unsigned char* CActionPointManager::GetActionPoint() const
{
    return reinterpret_cast<unsigned char*>(const_cast<CActionPointManager*>(this)) + 0x08;
}

// ORIG 0x8121b3e：this[+0x00]=1；*(int*)(+0x0c)=0；*(int*)(+0x04)=0；
// 先把所有已领条目(+0x1b+row*0x0b+slot，行距 0x0b)取负标记；
// 再按行(0..299)/槽(0..4)扫描，在行动点总数(+0x0c)未达目标 value 前逐项恢复、
// 累加并发包通知：SendActionClear(short=+0x15+row*0x0b, group=int@+0x17+row*0x0b,
// true, 0) 与 CheckAndSendToMessageAllUser(prev=+0x04, ap=+0x0c, 当前角色名)。
void CActionPointManager::GM_Set(CUser& user, unsigned int value)
{
    unsigned char* bytes = reinterpret_cast<unsigned char*>(this);
    bytes[0x00] = 1;
    *reinterpret_cast<unsigned int*>(bytes + 0x0c) = 0;
    *reinterpret_cast<unsigned int*>(bytes + 0x04) = 0;
    for (int row = 0; row <= 299; ++row) {
        unsigned char* rec = bytes + 0x15 + row * 0x0b;
        for (int slot = 0; slot <= 4; ++slot) {
            signed char& entry = *reinterpret_cast<signed char*>(rec + 6 + slot);
            if (entry > 0)
                entry = static_cast<signed char>(-entry);
        }
    }
    for (int row = 0;
         row <= 299 && *reinterpret_cast<unsigned int*>(bytes + 0x0c) < value; ++row) {
        unsigned char* rec = bytes + 0x15 + row * 0x0b;
        for (int slot = 0;
             slot <= 4 && *reinterpret_cast<unsigned int*>(bytes + 0x0c) < value; ++slot) {
            signed char& entry = *reinterpret_cast<signed char*>(rec + 6 + slot);
            if (entry < 0) {
                *reinterpret_cast<unsigned int*>(bytes + 0x04) =
                    *reinterpret_cast<unsigned int*>(bytes + 0x0c);
                entry = static_cast<signed char>(-entry);
                *reinterpret_cast<unsigned int*>(bytes + 0x0c) += entry;
                _ActionGroupIndex group;
                group.m_value = *reinterpret_cast<int*>(rec + 2);
                CUserProc::SendActionClear(user,
                    *reinterpret_cast<unsigned short*>(rec), group, true,
                    ENUM_PACKETCLASS_0);
                CUserProc::CheckAndSendToMessageAllUser(
                    *reinterpret_cast<unsigned int*>(bytes + 0x04),
                    *reinterpret_cast<unsigned int*>(bytes + 0x0c),
                    user.getCurCharacName());
            }
        }
    }
}

// ORIG 0x08122af4（W）：this[+0x00]=0；*(int*)(this+0x04)=0；
// memset(this+0x08, 0, 0xcf1)。无负标记扫描循环。
void CActionPointManager::Reset()
{
    unsigned char* bytes = reinterpret_cast<unsigned char*>(this);
    bytes[0x00] = 0;
    *reinterpret_cast<unsigned int*>(bytes + 0x04) = 0;
    memset(bytes + 0x08, 0, 0xcf1);
}

} // namespace APSystem
