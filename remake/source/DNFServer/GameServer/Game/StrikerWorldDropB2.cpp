// ============================================================================
// StrikerWorldDropB2.cpp — Striker/WorldDrop/StageMap 依赖恢复（独占 TU，
// 任务 StrikerWorldDropB2）
//
// 覆盖符号（全部经 ORIG df_game_r nm 核实；报告存在者已完整阅读 ##汇编 +
// ##反编译 C 后转写，无函数报告两处 objdump 逐指令转写并标注）：
//   StrikerScript::importSkillDataUpVector()                        T 0x08a9e4d6
//   StrikerScript::importSkillAttackPenalty()                       T 0x08a9e5d0
//   StrikerScript::importSkillCoolTimePenalty()                     T 0x08a9e5fc
//   StrikerScript::importParentLevelPenlaty()                       T 0x08a9e654
//   StrikerScript::readSkillPenalty(std::vector<TagSkillPenalty>&)  T 0x08a9e6cc
//       （importSkillAttackPenalty/importSkillCoolTimePenalty 的直接依赖；
//        此前全仓无定义，随本批闭环）
//   StrikerSkill::StrikerSkill()                                    W 0x08a9ed1e
//       （无独立函数报告【标记】：先读同类宿主报告 StrikerScript/
//        StrikerScript.md 与 readSkillPenalty.md，再 objdump 转写：
//        仅构造 +0xc std::string，int 字段不初始化）
//   WongWork::CWorldDrop::CWorldDrop(int)                           W 0x0853a826
//       （无独立函数报告【标记】：先读同类报告 WongWork__CWorldDrop/
//        loadScript.md、generateRandomItem.md 域注释与 CItemGenerator.h
//        布局证据，再 objdump 转写：调基类 C1 → 写 vptr 0x08c9a718 →
//        于 +0x18 / +0xfcc 各循环 WorldDropInfoC1Ev ×0xc8）
//   WongWork::CWorldDrop::loadScript(char const*)                   W 0x0853a81c
//       （无独立函数报告【标记】：objdump 显示纯 return 0 —— ORIG 即如此，
//        虚槽 +0x04 覆写占位；真实装载走非虚双参重载）
//   WongWork::CWorldDrop::loadScript(char const*, char const*)      T 0x0853855c
//   StageMapList::makeStageMapUniqueId(int, int, int)               T 0x08364818
//   AvatarRoulette::swap(AvatarRoulette*)                           T 0x08911e4e
//
// 报告路径（##汇编 + ##反编译 C 全文已读）：
//   docs/class_func_reports/StrikerScript/{importSkillDataUpVector,
//       importSkillAttackPenalty,importSkillCoolTimePenalty,
//       importParentLevelPenlaty,readSkillPenalty,StrikerScript}.md
//   docs/class_func_reports/AvatarRoulette/swap.md
//   docs/class_func_reports/WongWork__CWorldDrop/loadScript.md
//   docs/class_func_reports/StageMapList/makeStageMapUniqueId.md
//   docs/class_func_reports/TagSkillPenalty/TagSkillPenalty.md（元素类型同类）
//
// 不含（他人已落盘，查重后跳过）：
//   StrikerScript::{ctor,dtor,getInsance,clear,create,destory} CtorProducers_2.cpp
//   StrikerScript::importScript                                 ScriptLeafA2.cpp
//   AvatarRoulette::{C1,D1,clear}                          InitScriptProducers_6.cpp
//   STCharacterScript C1/D1（ScriptDepsA 已负责落盘；头内 inline 构造 + Clear）
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

// ============================================================================
// StrikerScript 脚本导入叶子（报告全文转写）
// ============================================================================

// ORIG T 0x08a9e4d6：clear this+0x40；顶测循环两段条件各自独立判退：
// ScanType(token,true)!=true → 退出；token!="[level]" → 退出；
// 循环体 { StrikerSkillDataUp 局部(0x14)；+0=ScanInt、+4=ScanInt；
// importSkillDataUp(&局部+8)；push_back }。for(;;)+break 形态与 ORIG 的
// 「条件在顶部、无前置 jmp、两个 break 各自跳出」布局同构。
void StrikerScript::importSkillDataUpVector()
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
void StrikerScript::importSkillAttackPenalty()
{
    m_attackPenalty.clear();
    readSkillPenalty(m_attackPenalty);
}

// ORIG T 0x08a9e5fc：clear this+0x58 后直呼 readSkillPenalty(this+0x58)。
void StrikerScript::importSkillCoolTimePenalty()
{
    m_coolTimePenalty.clear();
    readSkillPenalty(m_coolTimePenalty);
}

// ORIG T 0x08a9e654：clear this+0x6c；ok=true 置一次；每轮首值 ScanInt(&ok)
// 失败即停；随后 ScanInt / ScanFloat×2 凑成 ParentLevelPenalty(2×int+2×float)
// push_back。ORIG 栈对象无 ctor 调用（POD 直接逐字段写入）。
void StrikerScript::importParentLevelPenlaty()
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
void StrikerScript::readSkillPenalty(std::vector<TagSkillPenalty>& out)
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
StrikerSkill::StrikerSkill() : m_name() {}

// ============================================================================
// WongWork::CWorldDrop（无独立函数报告两处【标记】，objdump 转写；
// 双参 loadScript 为报告全文转写）
// ============================================================================

// ORIG W 0x0853a826：call CItemGeneratorC1Ei(count) → movl $0x8c9a718,(this)
// → 循环 WorldDropInfoC1Ev ×0xc8 @this+0x18 → 同 ×0xc8 @this+0xfcc。
// 成员数组的默认元素构造与 ORIG 循环等价。
WongWork::CWorldDrop::CWorldDrop(int count) : CItemGenerator(count) {}

// ORIG W 0x0853a81c（虚槽 +0x04 覆写）：mov $0x0,%eax; leave; ret —— ORIG
// 即空实现恒返回 0（真实装载入口是非虚双参重载），忠实转写，非占位桩。
int WongWork::CWorldDrop::loadScript(char const* path)
{
    (void)path;
    return 0;
}

// ORIG T 0x0853855c（报告转写）：ImportWorldDrop(this+0x18, path) 失败返回
// 0x7b9；ImportWorldDrop(this+0xfcc, warArea) 失败返回 0x7bb；成功返回 0。
// 注：DataManagerInitDepends.h 既存声明第二参为 char*（ORIG mangled 实为
// PKc），属 InitScriptProducers_1 域既存 ABI 偏差，此处 const_cast 桥接。
int WongWork::CWorldDrop::loadScript(char const* path, char const* warArea)
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
int StageMapList::makeStageMapUniqueId(int areaId, int districtId, int stageIdx)
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
int AvatarRoulette::swap(AvatarRoulette* other)
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
