// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CreateDnfEventSciprt/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CreateDnfEventSciprt 实现（独立新 TU，全局命名空间类）。
// 依据 docs/class_func_reports/CreateDnfEventSciprtServer/{Load,Destroy}.md、
// docs/class_func_reports/CreateDnfEventSciprt/{loadScript,clearScript,
// GetItemInfo}.md 与 ORIG 反汇编逐段转写。
//
// 注册链（ARAD.cpp Init #8，待 ARAD 域批次接入）：
//   RegistScript(Singleton<CreateDnfEventSciprtServer>::Get(), 0x13)
#include "CreateDnfEventSciprt.h"

#include <cstdio>

#include "DNFLexWrapper.h"    // loadRDARScriptFile / unloadRDARScriptFile /
                              // ScanType / ScanInt / ScanTypeOrInt
#include "GameTypes.h"        // cMyTrace

namespace
{
typedef std::vector<CreateDnfEventSciprt::ItemInfo> ItemInfoVector;
}  // namespace

// ============================================================================
// CreateDnfEventSciprt
// ============================================================================

// ORIG _ZN20CreateDnfEventSciprtC1Ev @0x08ac000c：
// this+4 处 m_itemMap 默认构造（map Rb_tree impl 初始化）。
#include "CreateDnfEventSciprt/CreateDnfEventSciprt.inc"
{
}

// ORIG _ZN20CreateDnfEventSciprtD1Ev @0x08ac0020：m_itemMap 逆序析构。
#include "CreateDnfEventSciprt/dtor_CreateDnfEventSciprt.inc"
{
}

// ORIG loadScript @0x08ac0034（0x2f5 字节）。token 循环；
// 各段失败路径 → unloadRDARScriptFile + return 0；
// duplicate [level] → printf 后直接 return -1（不 unload，ORIG 同）。
// b28d-solo-CreateDnfEventSciprt：4.4 对 while 恒 loop-rotation（条件移至 body 末尾），
// ORIG 4.1 为「入口 jmp → continue 中继岛(nop+jmp / nop) → 条件头 → body → 回边 jmp」
// 布局（08ac0087/0089/008c/008d）⇒ 改 for(;;)+显式 goto 岛（探针 /tmp/probe_ls2.cpp
// loop3 逐条同形）；4 次 ScanType 失败各落独立 break 岛（08ac02be/02c1/02c4/02c7）。
// 终判 AE：AE 归一后 210=210 逐条相等（ndiff 184→0，gap 0），p167 32/32 CLEAR。
// 余差钥匙：ORIG 两处字符串判断为 operator== + `test;je`（无 xor）⇒ 4.4 对否定式
// `if (!(token == X))` 恒加 xor 物化（实测 ndiff 4→10），必须正向包裹 + else goto 岛。
#include "CreateDnfEventSciprt/loadScript.inc"
{
    if (!loadRDARScriptFile("Event/", "tw_CreateDnf.evt"))   // "tw_CreateDnf.evt" / "Event/"
        return -1;

    std::string token;              // -0x44
    bool isType = false;            // -0x45（ScanTypeOrInt 出参）
    bool isInt = false;             // -0x46
    int curDayLevel = 0;            // -0x4c（[day] 段记录）
    std::vector<ItemInfo> items;    // -0x58

    for (;;)
    {
        goto L_cond;                                    // 入口 jmp（08ac0087）
    L_contA:
        goto L_cond;                                    // continue 中继岛 1（08ac0089: nop;jmp）
    L_contB:
    L_cond:                                             // 条件头（08ac008d）
        if (!ScanType(token, true))
            break;                                      // → break 岛（08ac02be）

        // ORIG 用 operator== + je（无 xor 物化）：必须正向包裹，否定式
        // `if (!(token == X))` 会被 4.4 加 xor（探针实测 ndiff 4→10）。
        if (token == "[step]")                          // 08ac00ab
        {
            if (!ScanType(token, true))                 // 08ac00c2
                break;
            if (token == "[day]")                       // 08ac00e0
                curDayLevel = ScanInt((bool*)0);        // 08ac00f7
            if (!ScanType(token, true))                 // 08ac0106
                break;

            if (token == "[reward]")                    // 08ac0124
            {
                for (;;)                                // reward 体 08ac013b..
                {
                    int rewardIdx = ScanTypeOrInt(isType, token, isInt, true);
                    if (isType)                         // 08ac015f：出参为真 → 结束 [reward]
                        break;
                    int count = ScanInt((bool*)0);      // 08ac0167
                    items.push_back(ItemInfo(rewardIdx, count));   // ItemInfo C1 + push_back(move)
                }
            }

            if (!ScanType(token, true))                 // 08ac01a4
                break;
            if (token == "[/step]")                     // 08ac01c2
            {
                // [/step] 收口（08ac01dd..）：查重后经 new 空向量 swap(items) 再以引用拷贝入 map。
                if (m_itemMap.find(curDayLevel) != m_itemMap.end())
                {
                    printf("duplicate [level] %d", curDayLevel);       // 08ac022b
                    return -1;                          // 不调 unloadRDARScriptFile（ORIG 同）
                }
                std::vector<ItemInfo>* staged = new std::vector<ItemInfo>();  // operator new(0xc)
                staged->swap(items);
                m_itemMap.insert(std::pair<const int, std::vector<ItemInfo>*>(curDayLevel, staged));
                // 注【ORIG 原样】：staged 指针直接入 map（value 为 vector*），本体由
                // clearScript 的指针式回收（析构 + delete）承接。
            }
            else
            {
                goto L_contB;
            }
            goto L_cond;                                // 回边（08ac02b9）
        }
        else
        {
            goto L_contA;
        }
    }

    unloadRDARScriptFile();                             // 08ac02c8 收尾路径
    return 0;
}

// ORIG clearScript @0x08ac032a（0x7b 字节）。逐指令转写【ORIG 原样，含缺陷】：
// 循环体内无迭代器自增，且把 mapped value 首 4 字节当堆指针做显式析构 + delete。
#include "CreateDnfEventSciprt/clearScript.inc"
{
    std::map<int, std::vector<ItemInfo>* >::iterator it = m_itemMap.begin();
    std::map<int, std::vector<ItemInfo>* >::iterator itEnd = m_itemMap.end();  // ORIG：begin 后即缓存
    for (; it != itEnd /* 无 ++it，ORIG 原样 */; )
    {
        register std::vector<ItemInfo>* victim = it->second;   // ORIG：victim 驻留 ebx
        if (victim != NULL)                                     // 08ac036b
        {
            victim->~ItemInfoVector();                          // 显式 ~vector（08ac06fa）
            ::operator delete(victim);                          // _ZdlPv（087244f0）
        }
    }
    m_itemMap.clear();                                          // 08ac08e2
}

// ORIG GetItemInfo(vector&) @0x08ac04a4（0x13d 字节）：合并所有 day 组的全部元素。
#include "CreateDnfEventSciprt/GetItemInfo.inc"
{
    std::map<int, std::vector<ItemInfo>* >::iterator tmp = m_itemMap.begin();  // ORIG 冗余 begin（结果未用）
    std::map<int, std::vector<ItemInfo>* >::iterator itEnd = m_itemMap.end();
    out.clear();
    for (std::map<int, std::vector<ItemInfo>* >::iterator it = m_itemMap.begin(); it != itEnd; it++)  // ORIG：后缀 ++
    {
        unsigned int i = 0;                // ORIG：i 在外层循环体入口初始化
        unsigned int n = it->second->size();   // ORIG：外层不存 vec，链式取 size
        for (; i < n; ++i)
        {
            std::vector<ItemInfo>* vec = it->second;   // ORIG：vec 在内层才存入槽
            out.push_back(std::make_pair(vec->at(i).m_itemIndex, vec->at(i).m_count));
        }
    }
}

// ORIG GetItemInfo(vector&, int) @0x08ac03a6（0xfe 字节）：仅指定 day 组；
// find 未命中时不动 out（ORIG 同）。
#include "CreateDnfEventSciprt/GetItemInfo_1.inc"
{
    std::map<int, std::vector<ItemInfo>* >::iterator it = m_itemMap.find(dayLevel);
    if (it != m_itemMap.end())   // ORIG 结构：ne; je 跳过（命中才处理）
    {
        out.clear();
        std::vector<ItemInfo>* vec = it->second;
        unsigned int i = 0;                // ORIG：i 声明在 n 之前
        unsigned int n = vec->size();      // ORIG：size 只取一次并缓存
        for (; i < n; ++i)
        {
            out.push_back(std::make_pair(vec->at(i).m_itemIndex, vec->at(i).m_count));
        }
    }
}

// ============================================================================
// CreateDnfEventSciprtServer
// ============================================================================

// ORIG C1 @0x081648ac：先 m_script(this+4) ctor → ARAD::Singleton<T> C1（空）→
// ScriptInterface C1（vptr 0x8b2db78）→ 写自身 vptr 0x8b7cba8。
#include "CreateDnfEventSciprt/CreateDnfEventSciprtServer.inc"
{
}

// ORIG D1 @0x081648e2：写 vptr 0x8b7cba8 后析构 m_script(this+4)；无基析构调用。
#include "CreateDnfEventSciprt/dtor_CreateDnfEventSciprtServer.inc"
{
}

// ORIG Load @0x0816451e：PRETTY 串 "virtual bool CreateDnfEventSciprtServer::Load()"
// （.rodata 0x8b7c9a0），行号 0xd0 / 计数 9 / true,false → cMyTrace C1(PKc,iibb)；
// 打点 "- Loading EventCreateDnf script - "；return loadScript()==0（b28c-32：
// m_script 已改为第一个基类，直调基类成员）。
#include "CreateDnfEventSciprt/Load.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0xd0, 9, true, false);
    trace("\t- Loading EventCreateDnf script - ");
    if (loadScript())
        return false;
    return true;
}

// ORIG Destroy @0x08164590：mov $0x1,%eax; ret（undefined4 返 1；接口 void 化沿用
// AvatarFixedHiddenOptionServer 先例，见 ScriptInterface vtable 两槽契约）。
#include "CreateDnfEventSciprt/Destroy.inc"
{
    return 1;
}

// ============================================================================
// ORIG 单例链完整类族：_ZN4ARAD9SingletonI26CreateDnfEventSciprtServerEC1Ev（W
// 0x08164902）、Get（W 0x08164907）、guard var/obj（0x09404630/0x09404638）、
// GetPtr。本 TU 经下列可执行引用完成隐式实例化（inline 模板各 TU COMDAT 合并，
// 与 ARAD.cpp 将来的 #8 RegistScript 调用不冲突）；勿在他处重复显式实例化。
// ============================================================================
namespace
{
ARAD::ScriptInterface* instantiateCreateDnfEventSciprtServerSingleton()
{
    return ARAD::Singleton<CreateDnfEventSciprtServer>::Get();
}
}  // namespace
