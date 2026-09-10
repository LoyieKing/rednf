// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StrikerWorldDropB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"             // StageMapList 权威声明
#include "DataManagerInitDepends.h"   // AvatarRoulette 权威布局（须在 CDataManager.h 之后）
#include "StrikerScript.h"            // StrikerScript / StrikerSkill / 子类型
#include "EquipmentParameterInfo.h"   // importSkillDataUp + EquipmentSkillDataUp
#include "DNFLexWrapper.h"            // ScanType / ScanInt / ScanFloat / ScanStr
#include "CItemGenerator.h"           // WongWork::CWorldDrop

// [GI 调序 T1a] TagSkillPenalty ctor 外联定义（ORIG T 0x08a9d768：仅 call clear，体 0x13）。
// 置于本 TU 首个强符号位 ⇒ GI 键名 = `_GLOBAL__I__ZN15TagSkillPenaltyC2Ev`（ORIG 0x08a9eced，
// ORIG CU = RDARScriptStriker.cpp，即本 TU 对应物）。判据 P327：仅外联强符号参与键名。
#include "StrikerWorldDropB2/TagSkillPenalty.inc"
{
    clear();
}

// ============================================================================
// StrikerScript 脚本导入叶子（报告全文转写）
// ============================================================================

// ORIG T 0x08a9e4d6：clear this+0x40；顶测循环两段条件各自独立判退：
// ScanType(token,true)!=true → 退出；token!="[level]" → 退出；
// 循环体 { StrikerSkillDataUp 局部(0x14)；+0=ScanInt、+4=ScanInt；
// importSkillDataUp(&局部+8)；push_back }。for(;;)+break 形态与 ORIG 的
// 「条件在顶部、无前置 jmp、两个 break 各自跳出」布局同构。
#include "StrikerWorldDropB2/importSkillDataUpVector.inc"
{
    std::string token;
    m_skillDataUp.clear();
    for (;;)
    {
        if (!ScanType(token, true))          // ORIG xor $1/test/jne 反相形
            break;
        if (token == "[level]")              // ORIG test/je 直测形
        {
            StrikerSkillDataUp up;               // ORIG C1 W 0x08a9ed4a
            up.m_level = ScanInt((bool*)0);      // 局部 -0x20
            up.m_field04 = ScanInt((bool*)0);    // 局部 -0x1c
            importSkillDataUp(up.m_dataUps);     // 参数=&局部+8
            m_skillDataUp.push_back(up);         // vector@this+0x40
        }
        else
        {
            break;
        }
    }
}

// ORIG T 0x08a9e5d0：clear this+0x4c 后直呼 readSkillPenalty(this+0x4c)。
#include "StrikerWorldDropB2/importSkillAttackPenalty.inc"
{
    m_attackPenalty.clear();
    readSkillPenalty(m_attackPenalty);
}

// ORIG T 0x08a9e5fc：clear this+0x58 后直呼 readSkillPenalty(this+0x58)。
#include "StrikerWorldDropB2/importSkillCoolTimePenalty.inc"
{
    m_coolTimePenalty.clear();
    readSkillPenalty(m_coolTimePenalty);
}

// ORIG T 0x08a9e654：clear this+0x6c；ok=true 置一次；每轮首值 ScanInt(&ok)
// 失败即停；随后 ScanInt / ScanFloat×2 凑成 ParentLevelPenalty(2×int+2×float)
// push_back。ORIG 栈对象无 ctor 调用（POD 直接逐字段写入）。
#include "StrikerWorldDropB2/importParentLevelPenlaty.inc"
{
    m_parentLevelPenalty.clear();
    bool ok = true;
    while (true)
    {
        ParentLevelPenalty entry;                 // 局部 -0x1c 起
        entry.m_field00 = ScanInt(&ok);           // -0x1c
        if (!ok)
            break;
        entry.m_field04 = ScanInt((bool*)0);      // -0x18
        entry.m_rate08 = ScanFloat((bool*)0);     // -0x14 (fstps)
        entry.m_rate0c = ScanFloat((bool*)0);     // -0x10 (fstps)
        m_parentLevelPenalty.push_back(entry);
    }
}

// ORIG T 0x08a9e6cc：死循环 { ok=true（循环体首指令）；TagSkillPenalty 局部
// C1（=clear，报告 TagSkillPenalty.md）；+0=ScanInt(&ok) 失败即停；
// +4=ScanInt；ScanStr 后与 "%" 比较 → +8 bool；+0xc=ScanInt；push_back }。
#include "StrikerWorldDropB2/readSkillPenalty.inc"
{
    while (true)
    {
        bool ok = true;
        TagSkillPenalty penalty;                       // ORIG C1 T 0x08a9d768
        penalty.m_skillIdx = ScanInt(&ok);             // +0x00（-0x20）
        if (!ok)
            break;
        penalty.m_value = ScanInt((bool*)0);           // +0x04（-0x18）
        std::string tok;
        ScanStr(&tok);
        if (tok == "%")                            // ORIG test/je + movb $1/$0
            penalty.m_isPercent = true;            // 条件赋值形态，保持指令同构
        else
            penalty.m_isPercent = false;           // +0x08（-0x14）
        penalty.m_pad = ScanInt((bool*)0);             // +0x0c（-0x10，真实第 4 数据字段）
        out.push_back(penalty);
    }
}

// ============================================================================
// StrikerSkill 生命周期（无独立函数报告【标记】，objdump 逐指令转写）
// ============================================================================

// ORIG W 0x08a9ed1e：仅 call _ZNSsC1Ev(this+0xc)；三个 int 字段保持未初始化。
#include "StrikerWorldDropB2/StrikerSkill.inc"

// ============================================================================
// WongWork::CWorldDrop（无独立函数报告两处【标记】，objdump 转写；
// 双参 loadScript 为报告全文转写）
// ============================================================================

// ORIG W 0x0853a826：call CItemGeneratorC1Ei(count) → movl $0x8c9a718,(this)
// → 循环 WorldDropInfoC1Ev ×0xc8 @this+0x18 → 同 ×0xc8 @this+0xfcc。
// 成员数组的默认元素构造与 ORIG 循环等价。
#include "StrikerWorldDropB2/CWorldDrop.inc"

// ORIG W 0x0853a81c（虚槽 +0x04 覆写）：mov $0x0,%eax; leave; ret —— ORIG
// 即空实现恒返回 0（真实装载入口是非虚双参重载），忠实转写，非占位桩。
#include "StrikerWorldDropB2/loadScript.inc"
{
    (void)path;
    return 0;
}

// ORIG T 0x0853855c（报告转写）：ImportWorldDrop(this+0x18, path) 失败返回
// 0x7b9；ImportWorldDrop(this+0xfcc, warArea) 失败返回 0x7bb；成功返回 0。
// 注：DataManagerInitDepends.h 既存声明第二参为 char*（ORIG mangled 实为
// PKc），属 InitScriptProducers_1 域既存 ABI 偏差，此处 const_cast 桥接。
#include "StrikerWorldDropB2/loadScript_1.inc"
{
    if (!ImportWorldDrop(m_worldDrop, const_cast<char*>(path)))
        return 0x7b9;
    if (!ImportWorldDrop(m_warAreaDrop, const_cast<char*>(warArea)))
        return 0x7bb;
    return 0;
}

// ============================================================================
// ORIG T 0x08364818：三参在栈上分别 +100/+1000/+1000（addl 直改栈槽）后
// imul $0x5f5e100 / imul $0x2710 / add 线性组合。保持同构写法以对齐指令形态。
#include "StrikerWorldDropB2/makeStageMapUniqueId.inc"
{
    areaId += 100;
    districtId += 1000;
    stageIdx += 1000;
    return areaId * 100000000 + districtId * 10000 + stageIdx;
}
// ============================================================================
// AvatarRoulette::swap（报告全文转写；生命周期其余部分在 InitScriptProducers_6）
// ============================================================================

// ORIG T 0x08911e4e：前 5 个 dword 自 other 单向拷贝（+0x00..+0x10），
// 随后 i=0..10 对 m_avatars[i]（步长 0xc @+0x14）调 vector::swap（双向），
// 恒返回 1。stiker 数组不参与。
#include "StrikerWorldDropB2/swap.inc"
{
    m_avatarCoinInfo = other->m_avatarCoinInfo;
    m_rareCloneAvatarRate = other->m_rareCloneAvatarRate;
    m_rareAvatarRate = other->m_rareAvatarRate;
    m_upperAvatarRate = other->m_upperAvatarRate;
    m_normalAvatarRate = other->m_normalAvatarRate;
    for (int i = 0; i <= 10; ++i)
    {
        m_avatars[i].swap(other->m_avatars[i]);
    }
    return 1;
}
