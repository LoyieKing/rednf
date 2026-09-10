// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BlueMarbleTileScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ BlueMarbleTileScript 专属 TU（从 GameStubs.cpp 拆出）。
// 逐方法对照 ORIG df_game_r 反汇编实现。
#include "BlueMarbleTileScript.h"

#include <string>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanStr/ScanInt

// ORIG 0x88d5068：ctor 仅调 clear()。
#include "BlueMarbleTileScript/BlueMarbleTileScript.inc"
{
    clear();
}

// ORIG 0x88d509c（0x16d 字节）逐分支转写：
//   loadRDARScriptFile("", path) 失败 → 0；否则循环 ScanType：
//   "[type]"→ScanStr 后 toType 写回 m_type；"[enter dungeon]"→ScanInt，
//   非 0 置 m_char4=1。扫描结束返回 1。
#include "BlueMarbleTileScript/importScript.inc"
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
#include "BlueMarbleTileScript/toType.inc"
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
#include "BlueMarbleTileScript/clear.inc"
{
    m_type = 0;
    m_char4 = 0;
    m_value = 0;
}