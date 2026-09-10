// df_game_r Game/ BlueMarbleTileScript 专属 TU（从 GameStubs.cpp 拆出）。
// 逐方法对照 ORIG df_game_r 反汇编实现。
#include "BlueMarbleTileScript.h"

#include <string>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanStr/ScanInt

// ORIG 0x88d5068：ctor 仅调 clear()。
BlueMarbleTileScript::BlueMarbleTileScript()
{
    clear();
}

// ORIG 0x88d509c（0x16d 字节）逐分支转写：
//   loadRDARScriptFile("", path) 失败 → 0；否则循环 ScanType：
//   "[type]"→ScanStr 后 toType 写回 m_type；"[enter dungeon]"→ScanInt，
//   非 0 置 m_char4=1。扫描结束返回 1。
bool BlueMarbleTileScript::importScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    std::string token;
    std::string value;
    int enterDungeon = 0;
    // [b28d-47] ORIG 0x088d50f9 循环头 `call ScanType; xor $1,%eax; test %al,%al;
    // jne <退出>`（P168a bool callee 裸取反形）⇒ 写 `if (!ScanType) break;`；
    // `while (ScanType(...))` 4.4 直产 `test %al; je`（少 xor）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[type]")
        {
            ScanStr(&value);
            m_type = toType(value);
        }
        else if (token == "[enter dungeon]")
        {
            ScanInt(&enterDungeon);
            if (enterDungeon != 0)
            {
                m_char4 = 1;
            }
        }
    }
    return 1;
}

// ORIG 0x88d520a（0x1cb 字节）：字符串链式比较映射类型枚举。
int BlueMarbleTileScript::toType(std::string type)
{
    if (type == "empty") return 0;
    if (type == "start") return 1;
    if (type == "end") return 2;
    if (type == "gold") return 3;
    if (type == "exp") return 4;
    if (type == "drop") return 5;
    if (type == "item") return 6;
    if (type == "unique item") return 7;
    if (type == "boss unique") return 8;
    if (type == "move") return 9;
    if (type == "jump") return 0xa;
    if (type == "gold goblin") return 0xb;
    if (type == "rope") return 0xc;
    if (type == "kick") return 0xd;
    return 0;
}

// ORIG 0x88d507c：m_type=0、m_char4=0、m_value=0。
void BlueMarbleTileScript::clear()
{
    m_type = 0;
    m_char4 = 0;
    m_value = 0;
}