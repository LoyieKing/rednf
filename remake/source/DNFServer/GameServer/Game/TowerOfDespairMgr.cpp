// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TowerOfDespairMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// TowerOfDespairMgr.cpp —— TOD 域首批叶子实现（PartyDepsB2 批次）。
// 逐指令依据：
//   TOD_Layer::TOD_Layer            docs/class_func_reports/TOD_Layer/TOD_Layer.md
//   TowerOfDespairMgr::SendAPCInfo  docs/class_func_reports/TowerOfDespairMgr/SendAPCInfo.md
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN9TOD_LayerC2Et`（ORIG 0x85ff0dd）；TOD_Layer ctor 已是首强符号。
#include <iostream>

#include "TowerOfDespairMgr.h"
#include <string.h>

#include "GameTypes.h"   // cMyTrace(char const*, int, int) / operator()(char const*, ...)

#include "TowerOfDespairMgr/TOD_Layer.inc"
{
    m_layer = layer;
    if (m_layer > 100)
    {
        // [b28c-35] ORIG 0x085fe7d8：layer 在 cMyTrace::C1 调用**前**读入 %ebx
        // （`movzwl -0x1c(%ebp),%ebx` 跨 ctor 保活，push %ebx; sub $0x34）；具名
        // 对象形 4.4 改在 ctor 后重读槽（无 push ebx，帧 0x38）。P227 全表达式
        // 临时链复现。
        cMyTrace(__PRETTY_FUNCTION__, 0x10, 5)(
            "@TOD: invalid layer setting : %d", (unsigned int)layer);
        m_layer = 1;
    }
}

#include "TowerOfDespairMgr/SendAPCInfo.inc"
{
    // ORIG 0x08644338：尾调用 +0x354 处 APC 管理器（无返回值处理）。
    m_apcMgr->Send_APC_Info(layer, user);
}

// TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex —— ORIG T 0x08644272
// [0x08644272,0x086442c9]：m_layerSet(+0x328, set<int>).find(idx) != end()。
// const 成员：find/end 返回 _Rb_tree_const_iterator（与报告一致），
// operator!= 结果直返（无规范化，返回类型 bool）。
// 注意：find 必须独立语句先行——单语句 `find(idx) != end()` 被 gcc
// 从右向左求值（先 end 后 find），与 ORIG 指令序相反；
// 且 ORIG operator!= 的 this=end()、实参=it（写 `end() != it`）。
#include "TowerOfDespairMgr/IsTowerOfDespairDungeon_ByIndex.inc"
{
    std::set<int>::const_iterator it = m_layerSet.find(idx);
    return m_layerSet.end() != it;
}

// SendingAPC_Info::set_data —— ORIG T 0x085fe8e0 [0x085fe8e0,0x085fe905]
// rep movsl 0x2f 次（0xbc 字节）到 this+0x17338（m_apcInfo）。
#include "TowerOfDespairMgr/set_data.inc"
{
    // P83：ORIG 0x085fe8fd `rep movsl`（0x2f 次）＝结构整体赋值内联；
    // 显式 memcpy 生 call memcpy。
    m_apcInfo = info;                              // 0xbc
}

// TowerOfDespair_APC_Mgr::Set_APC_Info —— ORIG T 0x085fed2e [0x085fed2e,0x085feecf]
// map find；命中→复用条目 set_data；未命中→构造局部 SendingAPC_Info + set_data
// 后 make_pair/insert，随后析构局部（try 块内 set_data/insert 链）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N93）→ 试=复核 111/111：两处 `lea` 目标槽互换
// （ORIG 首执行的 `_Rb_tree_iterator::C1Ev`(0x85ff170) 作用 -0x2e808、2 实参 call(0x85ff17e
// 系 map::find 段) 作用 -0x2e804；我方 -0x2e804/-0x2e808 相反）⇒ 声明序↔执行序互斥
// （前轮 9 形已实测，B 形退化为 DIFF 已回滚）⇒ 维持（新=NEAR gap=0, blocks=5）。
#include "TowerOfDespairMgr/Set_APC_Info.inc"
{
    // b28d-49 @0x85fed2e：P167 差集定位 —— ORIG 有 iterator::C1Ev（默认构造）+
    // operator!=（正极性），我方缺 C1Ev 且多发 operator==。⇒ P50 复活
    // 「先声明后赋值」+ P220 正极性 `it != end()`，命中分支（set_data）为 then。
    // 二次复核 @0x85ff1aa：ORIG 求值序 = C1Ev(it) → end()（具名槽 -0x2e804）→
    // find() → it=结果（-0x2e808）⇒ 必须有具名 end 局部（P28 双旋钮之「命名 it
    // 固定 find 先求值」的反向：此处 end 先于 find）。
    // [P3Wave4 2026-09-11 9 形实测·已回滚 B 形] 本波试「mapEnd 先声明带初始化 +
    // it 后声明」形（槽序与 ORIG 全对：end -0x2e804 / it -0x2e808，111=111，
    // 行级 diff 10→6 行），但 **verdict 由 NEAR 退化为 DIFF**（C1Ev(it) 位置从
    // end() 之前挪到之后，mnemonic 序错位）⇒ 触犯 rule4，已回滚回 A 形（NEAR）。
    // ORIG 0x085fed39 = C1Ev(it) 在 end() 之前且 it 占深槽 -0x2e808，而 4.4 声明序
    // 与槽序强绑定（先声明=浅槽）⇒ 「C1Ev 在前」与「it 占深槽」两旋钮互斥（N93）。
    // 其它形读数（本次实测）：匿名 end() 临时 117 条/30 行；引用绑定 113 条/42、46 行；
    // end 先声明后赋值 116 条/15 行；it 初始化形 106 条/9 行 —— 均劣于 A 形 NEAR。
    // b28d-49 判词：NEAR 封顶（N93 槽方向/求值序绑定）。P167 CLEAR。@0x85fed2e
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;
    std::map<TOD_Layer, SendingAPC_Info>::iterator mapEnd = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != mapEnd)
    {
        it->second.set_data(info);
    }
    else
    {
        SendingAPC_Info value;
        value.set_data(info);
        m_APCInfoMap.insert(std::make_pair(layer, value));
    }
}

#include "TowerOfDespairMgr/Reset_APC_Info.inc"
{
    m_APCInfoMap.clear();
}

// TowerOfDespairMgr::ResetAPCInfo —— ORIG T 0x0864431e [0x0864431e,0x08644337]
// 转发 +0x354 APC 管理器 Reset_APC_Info（mov 0x354(%eax),%eax; call）。
#include "TowerOfDespairMgr/ResetAPCInfo.inc"
{
    m_apcMgr->Reset_APC_Info();
}

// TowerOfDespairMgr::SetAPCInfo —— ORIG T 0x086442ca [0x086442ca,0x0864431d]
// vector<unsigned int>::operator[](m_npcLayers, idx) 构造 TOD_Layer，
// 转发 +0x354 APC 管理器 Set_APC_Info。
#include "TowerOfDespairMgr/SetAPCInfo.inc"
{
    TOD_Layer layer(m_npcLayers[idx]);
    m_apcMgr->Set_APC_Info(layer, info);
}

// TowerOfDespairMgr::Reload_APC_Info —— ORIG T 0x08644360 [0x08644360,0x08644365]
// 空函数：push/mov/pop/ret（无成员访问），语义为空体。
#include "TowerOfDespairMgr/Reload_APC_Info.inc"
{
}

// TowerOfDespairMgr::_layerInfo —— ORIG T 0x08644470 [0x08644470,0x08644474]
// 空函数：push/mov/pop/ret（无成员访问），语义为空体。
#include "TowerOfDespairMgr/_layerInfo.inc"
{
}
