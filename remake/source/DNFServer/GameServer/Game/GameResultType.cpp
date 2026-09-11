// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameResultType/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN14GameResultTypeC2Ev`（ORIG 0x84b764d）。
#include <iostream>

#include <cstring>

// ---- DungeonUserShotCount（8 字节：ushort + int，ctor 084b766a）----
class DungeonUserShotCount
{
public:
    DungeonUserShotCount();
    void reset();

    unsigned short m_count;  // +0x00
    int m_field4;            // +0x04
};

// ---- GameResultType（0x50） ----
class GameResultType
{
public:
    GameResultType();
    ~GameResultType();
    int Set(const GameResultType& other);
    void Clear();
    int UpdateHighScore(const GameResultType& other);
    int GetUniqueKey();

    unsigned char m_field0;   // +0x00
    int m_field4;             // +0x04
    unsigned char m_field8;   // +0x08
    unsigned char m_field9;   // +0x09
    unsigned char m_fielda;   // +0x0a
    int m_fieldc;             // +0x0c
    unsigned char m_field10;  // +0x10
    DungeonUserShotCount m_shot[4];  // +0x14（0x20）
    unsigned short m_field34; // +0x34
    unsigned short m_field36; // +0x36
    unsigned short m_field38; // +0x38
    unsigned short m_field3a; // +0x3a
    unsigned short m_field3c; // +0x3c
    unsigned short m_field3e; // +0x3e
    unsigned short m_field40; // +0x40
    unsigned short m_field42; // +0x42
    unsigned int m_field44;   // +0x44（b28d-54：ORIG 0x084b7596 无符号比较 jae +
                              //   0x084b758b 单段 `mov 0x44(%eax),%edx` ⇒ 字段与 RHS
                              //   同型化（P211），int 恒多一条 eax→edx 中转）
    unsigned short m_field48; // +0x48
    unsigned short m_field4a; // +0x4a
    int m_field4c;            // +0x4c
};

#include "GameResultType/GameResultType.inc"


#include "GameResultType/dtor_GameResultType.inc"


#include "GameResultType/DungeonUserShotCount.inc"


#include "GameResultType/reset.inc"


#include "GameResultType/Set.inc"


#include "GameResultType/Clear.inc"


#include "GameResultType/UpdateHighScore.inc"


#include "GameResultType/GetUniqueKey.inc"
{
    return m_field4 * 10 + m_field8;
}

// ---- GameResultSet（ORIG T 0x84b75d2 / 0x84b75e6 / 0x84b75fa，与 GameResultType
//      各符号地址邻接 = 原同一翻译单元；P210：基类 ctor/dtor 定义前置使派生
//      D1/C1 帧 0x18→0x4 与 ORIG 一致）。类声明权威见 GameResultSet.h。----
class GameResultSet : public GameResultType
{
public:
    GameResultSet();
    ~GameResultSet();
    void Clear();
};

#include "GameResultType/GameResultSet.inc"


#include "GameResultType/dtor_GameResultSet.inc"


#include "GameResultType/Clear_1.inc"

