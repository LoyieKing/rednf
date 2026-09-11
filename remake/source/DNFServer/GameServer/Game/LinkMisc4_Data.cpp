// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkMisc4_Data/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstdarg>
#include <cstdio>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"     // stMonsterIndependentDrop_t
#include "STMonsterScript.h"  // stEnamyDropAvatarInfo_t（sizeof 0x3c）
#include "Inven_Item.h"
#include "PacketHeader.h"

// ---------------------------------------------------------------------------
// 名字表（ORIG .rodata 字符串指针，dump 自 df_game_r 0x08c7fcf6..0x08c7fd83）
// ---------------------------------------------------------------------------
char* CHARACTER_JOB_NAMES[] = {
    (char*)"swordman",    // +0x00 0x08c7fcf6
    (char*)"fighter",     // +0x04 0x08c7fcff
    (char*)"gunner",      // +0x08 0x08c7fd07
    (char*)"mage",        // +0x0c 0x08c7fd0e
    (char*)"priest",      // +0x10 0x08c7fd13
    (char*)"at gunner",   // +0x14 0x08c7fd1a
    (char*)"thief",       // +0x18 0x08c7fd24
    (char*)"at fighter",  // +0x1c 0x08c7fd2a
    0,                    // +0x20（ORIG NULL）
    0,                    // +0x24（ORIG NULL）
    (char*)"basic",       // +0x28 0x08c7fd35
};

char* BIND_NAMES[] = {
    (char*)"gold",  // 0x08c7fd3b
    (char*)"c_c",   // 0x08c7fd40
};

char* RECIPE_NAMES[] = {
    (char*)"uc_c",     // 0x08c7fd44
    (char*)"uc_uc",    // 0x08c7fd49
    (char*)"headgear", // 0x08c7fd4f
};

char* EQUIP_NAMES[] = {
    (char*)"headgear",  // 0x08c7fd4f
    (char*)"hair",      // 0x08c7fd58
    (char*)"face",      // 0x08c7fd5d
    (char*)"jacket",    // 0x08c7fd62
    (char*)"pants",     // 0x08c7fd69
    (char*)"shoes",     // 0x08c7fd6f
    (char*)"breast",    // 0x08c7fd75
    (char*)"waist",     // 0x08c7fd7c
};

// ---------------------------------------------------------------------------
// Packet_Monitor_Event_Start（Monitor/DNFPacketTranslater.h 权威布局的最小
// 复刻，与 GmCmds_F.cpp 本地对接版同构：packed、继承 PacketHeader、
// sizeof 0x12；本 TU 不构造对象，仅为 ORIG W ctor 提供唯一定义点）。
// ORIG ctor 0x084b4184：PacketHeader(this, 0x44f, 0x12) 后直接返回。
// ---------------------------------------------------------------------------
class Packet_Monitor_Event_Start : public PacketHeader
{
public:
    Packet_Monitor_Event_Start();
    unsigned int m_field1;    // +0xa eventType/eventCode
    unsigned short m_field2;  // +0xe param1
    unsigned short m_field3;  // +0x10 param2
} __attribute__((packed));

#include "LinkMisc4_Data/Packet_Monitor_Event_Start.inc"
    : PacketHeader(0x44f, 0x12)
{
}

// ---------------------------------------------------------------------------
// CAICharacter 完整布局局部镜像（ORIG ctor 0x08373a9a 初始化序实证）：
//   +0x00 idx / +0x04 level / +0x08 field8 / +0x0c appearancePoint
//   +0x10 vector<pair<int,int>> dropItems / +0x1c/+0x20 PvP 胜点上下界
//   +0x24 vector<stMonsterIndependentDrop_t> independentDrops
//   +0x30 stEnamyDropAvatarInfo_t dropAvatarInfo（0x30..0x6b）
//   +0x6c std::string name
// 仅读访问（不构造对象），与各 TU 现有 0x30 截断镜像在前 0x30 字节一致。
// ---------------------------------------------------------------------------
class CAICharacter
{
public:
    // ORIG W 0x084b4138 _ZNK12CAICharacter7GetNameEv：return m_name.c_str()
    const char* GetName() const;

    // ORIG W 0x084b410e _ZNK12CAICharacter19getIndepentDropItemEv：
    // sret 拷贝 m_independentDrops 后返回 this。
    std::vector<stMonsterIndependentDrop_t> getIndepentDropItem() const;

private:
    int m_idx;                                      // +0x00
    int m_level;                                    // +0x04
    int m_field8;                                   // +0x08
    int m_appearancePoint;                          // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;  // +0x10
    int m_pvpWinLow;                                // +0x1c
    int m_pvpWinHigh;                               // +0x20
    std::vector<stMonsterIndependentDrop_t>
        m_independentDrops;                         // +0x24
    stEnamyDropAvatarInfo_t m_dropAvatarInfo;       // +0x30（0x3c）
    std::string m_name;                             // +0x6c
};

#include "LinkMisc4_Data/GetName.inc"
{
    return m_name.c_str();
}

std::vector<stMonsterIndependentDrop_t>
#include "LinkMisc4_Data/getIndepentDropItem.inc"
{
    return m_independentDrops;
}

// ---------------------------------------------------------------------------
// OS_API::WriteLog — ORIG T 0x0858c78a（报告 OS_API/WriteLog.md）：
// fopen(name,"ab") 成功则 vsprintf 到 1024 栈缓冲 → fputs → fflush → fclose。
// 声明点：GmCmds_G.cpp:63 asm 标签直连（本定义自然 mangled 一致）。
// ---------------------------------------------------------------------------
namespace OS_API
{

#include "LinkMisc4_Data/WriteLog.inc"


}  // namespace OS_API
