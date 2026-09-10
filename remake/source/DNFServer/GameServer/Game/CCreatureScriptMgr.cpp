// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCreatureScriptMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ user_creature::CCreatureScriptMgr 实现。
#include "CCreatureScriptMgr.h"
#include "GameTypes.h"
#include "CreatureScript.h"
#include "LogManager.h"
#include "sync_script.h"   // CSyncScript::truncate/insert/IsEnable
#include <stdio.h>
#include <string.h>

// [T2b-GI · P338/P327] 补 <iostream> 引入 file-scope 动态初始化 __ioinit，恢复本 TU
// `__static_initialization_and_destruction_0`/`_GLOBAL__I_` 发射；键名目标
// `_ZN13user_creature28GetInstanceCreatureScriptMgrEv`（ORIG 0x833df33）。零语义改动。
#include <iostream>

namespace user_creature
{

// [T2b-GI · P327] 定义前移：本函数是 ORIG 该 TU 首个强外部符号 ⇒ GI 键名来源
// （`_GLOBAL__I__ZN13user_creature28GetInstanceCreatureScriptMgrEv`，ORIG 0x833df33）。
// 只移函数定义、不动带动态初始化的对象 ⇒ 构造序零变化。ORIG 原址 0x8336bd0。
CCreatureScriptMgr* GetInstanceCreatureScriptMgr()
{
    static CCreatureScriptMgr instance;
    return &instance;
}

// ORIG CCreatureScriptMgr::CCreatureScriptMgr @ 0x833cb60：两个 map 默认构造。
#include "CCreatureScriptMgr/CCreatureScriptMgr.inc"
    : m_creatureIdToItemId(), m_scripts()
{
}

#include "CCreatureScriptMgr/dtor_CCreatureScriptMgr.inc"
{
    // map<CCreatureScript*> 默认析构仅释放节点，不释放指针指向对象
    // （ORIG 显式释放见 Destroy()）。
}

// ORIG Destroy @ 0x833d3bc：遍历 +0x18 map，删除（D1 + operator delete）每个
// CCreatureScript*，最后 clear()。
// [b28d-30] 复核 DIFF：我方少 8 条（析构/clear 路径形差，N82/P12 族），ORIG 0x0833dxxx 见 report。
#include "CCreatureScriptMgr/Destroy.inc"
{
    // round1 R19：ORIG 0x0833d3c3 段 `call size; test %eax,%eax; sete %al;
    // test %al,%al; jne`（P197④：`size() == 0` 才出 sete 物化，`empty()` 无）；
    // cond 段每次重算 `end()`（无具名 end 槽，临时 -0x14）；增量段调 `ppEi`
    // （P28：ORIG 后置 ⇒ 源写 `it++`）；循环体 p 有预清 `movl $0x0,-0xc` 与
    // delete 后 `p = 0;`（P197③ 两句式）。
    if (m_scripts.size() == 0)
        return;
    for (std::map<int, CCreatureScript*>::iterator it = m_scripts.begin();
         it != m_scripts.end(); it++)
    {
        CCreatureScript* p = 0;
        p = it->second;
        if (p != 0)
        {
            delete p;
            p = 0;
        }
    }
    m_scripts.clear();
}

//   3) sync IsEnable 时按 native.m_attackInfo[0..1] 拼 Attack 路径并
//      ImportAttackInfoScript，序列化 "%d,%d,%f,%d"（元素集追加 ",%d"，
//      空集追加 ",4"；失败则 "%d,%d"）入 buf270/buf370；
//   4) 构造 CCreatureScript，Set(native)，SetItemId(注册表 itemId)，
//      insert_creature_info_to_db，查重后注册进 m_scripts。
// [b28d-30] 复核 DIFF：大件残差（N82/N89 族），ORIG 见 class_func_reports/user_creature__CCreatureScriptMgr/LoadCreatureScriptFile.md。
#include "CCreatureScriptMgr/LoadCreatureScriptFile.inc"
{
    // [N5Free211to230.N5D] ORIG 0x0833cbb2 首条 `movl $0x0,-0x60(%ebp)`：script 指针
    // 具名预清（P198① 两句式）⇒ 声明提到函数头（native 之前）。
    CCreatureScript* script = 0;
    CreatureScript native;
    // [N5D] ORIG 0x0833cbc7（native 构造之后、buf rep stos 之前）`movl $0x0,-0x20(%ebp)`
    // = path 具名预清 ⇒ 同样两句式：头部声明 + 循环内赋值。
    const char* path = 0;
    char buf170[0x100] = {0};   // ORIG rep stos 0x100 清零
    char buf270[0x100] = {0};
    char buf370[0x100] = {0};
    AttackInfo attackInfo;

    if (!sync_script::GetInstanceSyncScript()->truncate_creature_info_table())
    {
        puts("failed to truncate creature info table");
        return false;
    }

    // round1 R19（p167 差集定位）：ORIG 走**非 const** `_Rb_tree_iterator`
    // （`ptEv`/`ne` 均为其 const 成员方法），我方 const_iterator 多发 2 条
    // `const_iterator::C1ERK iterator` 转换构造（P96/P270）⇒ 改非 const。
    for (std::map<int, std::string>::iterator it = g_creatureScriptFileList.m_fileMap.begin();
         it != g_creatureScriptFileList.m_fileMap.end(); it++)
    {
        path = it->second.c_str();
        if (path == 0)
        {
            // [N5D] ORIG 0x0833d352 = 尾 nop 岛（其后 0x0833d353 mov $1,%ebx）：
            // 4.4 的 `goto 尾标签` 形精确复现（探针 /tmp/r5/n5/p_ret.cpp q2）。
            goto L_true;
        }

        if (!importCreatureScript(&native, path))
        {
            LogManager::logFormat(1, "Creature.cpp",
                __PRETTY_FUNCTION__,
                0xf6b, "importCreatureScript fail file - %s", path);
            return false;
        }

        if (sync_script::GetInstanceSyncScript()->IsEnable())
        {
            // P197④：ORIG 用 `size()`（无 empty 调用）；P175：ORIG 走 `at()` 索引
            // （`vector<string>::at` ×4）；`[0][0]` 的 string 下标在 ORIG 是
            // `c_str()[0]`（p167：MISSING c_str ×2 / EXTRA string::operator[] ×2）。
            // [N5D] P225 推广（P226 条件向）：ORIG 0x0833cd09/0x0833cd3c 每个 && 操作数
            // 各自 `setne %al; test %al,%al; je` 物化，4.4 对 && 链恒折直跳 ⇒ 拆成两条
            // 相邻 if（无内层花括号，缩进零变化）复现（探针 p_creature.cpp f2 vs f1）。
            if (native.m_attackInfo.size() != 0)
            if (native.m_attackInfo.at(0).c_str()[0] != 0)
            {
                sprintf(buf170, "%s%s", native.m_directory.c_str(),
                        native.m_attackInfo.at(0).c_str());
                if (ImportAttackInfoScript(&attackInfo, buf170, 0))
                {
                    sprintf(buf270, "%d,%d,%f,%d",
                            native.m_skillType, native.m_skillMP,
                            (double)attackInfo.m_damage, attackInfo.m_attackType);
                    if (attackInfo.m_elements.size() == 0)
                    {
                        sprintf(buf270 + strlen(buf270), ",%d", 4);
                    }
                    else
                    {
                        for (std::set<ENUM_ELEMENT>::const_iterator e =
                                 attackInfo.m_elements.begin();
                             e != attackInfo.m_elements.end(); ++e)
                        {
                            sprintf(buf270 + strlen(buf270), ",%d", *e);
                        }
                    }
                }
                else
                {
                    sprintf(buf270, "%d,%d", native.m_skillType, native.m_skillMP);
                }
            }
            // [N5D] 同上：ORIG 0x0833cf32 `cmp $0x1,%eax; seta %al; test; je`。
            if (native.m_attackInfo.size() > 1)
            if (native.m_attackInfo.at(1).c_str()[0] != 0)
            {
                sprintf(buf170, "%s%s", native.m_directory.c_str(),
                        native.m_attackInfo.at(1).c_str());
                if (ImportAttackInfoScript(&attackInfo, buf170, 0))
                {
                    // ORIG 0x0833d018：第二块（attackInfo[1]）用 over 组字段
                    // (-0x64c=+0x6c m_overSkillType, -0x654=+0x64 m_overSkillMP)
                    sprintf(buf370, "%d,%d,%f,%d",
                            native.m_overSkillType, native.m_overSkillMP,
                            (double)attackInfo.m_damage, attackInfo.m_attackType);
                    if (attackInfo.m_elements.size() == 0)
                    {
                        sprintf(buf370 + strlen(buf370), ",%d", 4);
                    }
                    else
                    {
                        for (std::set<ENUM_ELEMENT>::const_iterator e =
                                 attackInfo.m_elements.begin();
                             e != attackInfo.m_elements.end(); ++e)
                        {
                            sprintf(buf370 + strlen(buf370), ",%d", *e);
                        }
                    }
                }
                else
                {
                    // ORIG 0x0833d11b：同为 over 组字段
                    sprintf(buf370, "%d,%d", native.m_overSkillType,
                            native.m_overSkillMP);
                }
            }
        }

        script = new CCreatureScript();
        script->Set(&native);
        int itemId = m_creatureIdToItemId[native.m_id];
        script->SetItemId(itemId);

        if (!sync_script::GetInstanceSyncScript()->insert_creature_info_to_db(
                itemId, &native, buf270, buf370))
        {
            printf("failed to insert id number < %d > creature info to db\n",
                   native.m_id);
            return false;
        }

        // [N5D] P293：内联表达式 `find(k) != end()` 在 4.4 恒「先 end 后 find」，
        // ORIG 0x0833d1f6 find → 0x0833d22a end ⇒ 改具名 iterator（探针
        // /tmp/r5/n5/p_creature2.cpp m1 vs m2）。
        std::map<int, CCreatureScript*>::iterator found = m_scripts.find(native.m_id);
        if (found != m_scripts.end())
        {
            // P221：具名 trace 在 4.4 恒占深槽（ORIG 为浅槽）⇒ 构造即调用临时链。
            // [N5D] P139：三处同文串改 __PRETTY_FUNCTION__（ORIG 0x08c2ebc0 size 0x41）。
            cMyTrace(__PRETTY_FUNCTION__,
                     0xfbe, 5)("[%s][%d]",
                               __PRETTY_FUNCTION__,
                               0xfbe);
            return false;
        }
        m_scripts.insert(std::make_pair(native.m_id, script));
    }
L_true:
    return true;
}

// ORIG FindCreatureScript @ 0x833d49a：在 +0x18 map 按 creatureId 查脚本指针；
// 未命中 cMyTrace 后返回 0。
// [b28d-30] 复核 DIFF：我方多 10 条（find 迭代器 cv/转换构造形，P96/P270/N82 族），
// ORIG 见 class_func_reports/user_creature__CCreatureScriptMgr/FindCreatureScript.md。
#include "CCreatureScriptMgr/FindCreatureScript.inc"
{
    // round1 R19：ORIG 0x0833d4a2 段为 **非 const** `find` sret 直构进 it 槽
    // （-0x20）+ 内联 `end()`（-0x1c 临时）+ `_Rb_tree_iterator::eq` 单条比较；
    // const_iterator 会多出 iterator→const_iterator 转换构造（+2 lea/call，
    // P96/P270/P323）⇒ 改非 const iterator，`end()` 不落具名局部（L29 ①）。
    std::map<int, CCreatureScript*>::iterator it = m_scripts.find(creatureId);
    if (it == m_scripts.end())
    {
        // P221：ORIG 槽序 it(-0x20) < end(-0x1c) < trace(-0x18)（迭代器深、
        // 具名 trace 浅）；4.4 具名对象恒反向（trace 深 -0x28）⇒ 改「构造即调用」
        // 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xfe8, 5)("[%s][%d]",
                                                __PRETTY_FUNCTION__,
                                                0xfe8);
        return 0;
    }
    return it->second;
}

// ORIG RegisterCreatureIdAndItemId @ 0x833d54a。
// [b28d-30] 复核 NEAR：map 迭代器槽/转换临时差（P50/P96/N86 族），ORIG 0x0833d54a。
#include "CCreatureScriptMgr/RegisterCreatureIdAndItemId.inc"
{
    // P50/P96：ORIG find 返回 iterator 直存（const_iterator 会额外引入转换临时+
    // 拷贝构造，帧 +0x10/多两条 lea——实测 we-larger 10 条）。
    // [b28-BM7 判词] NEAR 残余 = N95/N82：类对象槽基跨版本差——4.1 把
    // find/end/insert 临时深排（-0x34/-0x30/-0x38），4.4 恒从 ebp-8 浅排
    // （-0x24/-0x28），指令/助记符/调用集全等，仅 disp 槽号成串平移。
    std::map<int, int>::iterator it = m_creatureIdToItemId.find(creatureId);
    if (it != m_creatureIdToItemId.end())
    {
        // P221：ORIG 槽序 it(-0x38) < end(-0x34) < trace(-0x30)；4.4 具名 trace
        // 恒深槽 ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xfff, 5)("[%s][%d]",
                                                __PRETTY_FUNCTION__,
                                                0xfff);
        return false;
    }
    m_creatureIdToItemId.insert(std::make_pair(creatureId, itemId));
    return true;
}

}  // namespace user_creature
