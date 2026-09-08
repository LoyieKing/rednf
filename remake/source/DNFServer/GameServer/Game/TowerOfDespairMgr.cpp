// TowerOfDespairMgr.cpp —— TOD 域首批叶子实现（PartyDepsB2 批次）。
// 逐指令依据：
//   TOD_Layer::TOD_Layer            docs/class_func_reports/TOD_Layer/TOD_Layer.md
//   TowerOfDespairMgr::SendAPCInfo  docs/class_func_reports/TowerOfDespairMgr/SendAPCInfo.md
#include "TowerOfDespairMgr.h"
#include <string.h>

#include "GameTypes.h"   // cMyTrace(char const*, int, int) / operator()(char const*, ...)

TOD_Layer::TOD_Layer(unsigned short layer)
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

void TowerOfDespairMgr::SendAPCInfo(TOD_Layer const& layer, CUser* user)
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
bool TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex(int idx) const
{
    std::set<int>::const_iterator it = m_layerSet.find(idx);
    return m_layerSet.end() != it;
}

// SendingAPC_Info::set_data —— ORIG T 0x085fe8e0 [0x085fe8e0,0x085fe905]
// rep movsl 0x2f 次（0xbc 字节）到 this+0x17338（m_apcInfo）。
void SendingAPC_Info::set_data(_APCInfo const& info)
{
    // P83：ORIG 0x085fe8fd `rep movsl`（0x2f 次）＝结构整体赋值内联；
    // 显式 memcpy 生 call memcpy。
    m_apcInfo = info;                              // 0xbc
}

// TowerOfDespair_APC_Mgr::Set_APC_Info —— ORIG T 0x085fed2e [0x085fed2e,0x085feecf]
// map find；命中→复用条目 set_data；未命中→构造局部 SendingAPC_Info + set_data
// 后 make_pair/insert，随后析构局部（try 块内 set_data/insert 链）。
void TowerOfDespair_APC_Mgr::Set_APC_Info(TOD_Layer const& layer, _APCInfo const& info)
{
    // b28d-49 @0x85fed2e：P167 差集定位 —— ORIG 有 iterator::C1Ev（默认构造）+
    // operator!=（正极性），我方缺 C1Ev 且多发 operator==。⇒ P50 复活
    // 「先声明后赋值」+ P220 正极性 `it != end()`，命中分支（set_data）为 then。
    // 二次复核 @0x85ff1aa：ORIG 求值序 = C1Ev(it) → end()（具名槽 -0x2e804）→
    // find() → it=结果（-0x2e808）⇒ 必须有具名 end 局部（P28 双旋钮之「命名 it
    // 固定 find 先求值」的反向：此处 end 先于 find）。
    // b28d-49 判词：NEAR 封顶（N93 槽方向/求值序绑定）——4.4 把先声明的 it 放浅槽
    // -0x2e804、end 放深槽 -0x2e808（ORIG 相反）；交换声明序可对槽但求值序反
    //（end→C1Ev→find vs ORIG C1Ev→end→find），两旋钮互斥。P167 CLEAR。@0x85fed2e
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

void TowerOfDespair_APC_Mgr::Reset_APC_Info()
{
    m_APCInfoMap.clear();
}

// TowerOfDespairMgr::ResetAPCInfo —— ORIG T 0x0864431e [0x0864431e,0x08644337]
// 转发 +0x354 APC 管理器 Reset_APC_Info（mov 0x354(%eax),%eax; call）。
void TowerOfDespairMgr::ResetAPCInfo()
{
    m_apcMgr->Reset_APC_Info();
}

// TowerOfDespairMgr::SetAPCInfo —— ORIG T 0x086442ca [0x086442ca,0x0864431d]
// vector<unsigned int>::operator[](m_npcLayers, idx) 构造 TOD_Layer，
// 转发 +0x354 APC 管理器 Set_APC_Info。
void TowerOfDespairMgr::SetAPCInfo(unsigned int idx, _APCInfo const& info)
{
    TOD_Layer layer(m_npcLayers[idx]);
    m_apcMgr->Set_APC_Info(layer, info);
}

// TowerOfDespairMgr::Reload_APC_Info —— ORIG T 0x08644360 [0x08644360,0x08644365]
// 空函数：push/mov/pop/ret（无成员访问），语义为空体。
void TowerOfDespairMgr::Reload_APC_Info()
{
}

// TowerOfDespairMgr::_layerInfo —— ORIG T 0x08644470 [0x08644470,0x08644474]
// 空函数：push/mov/pop/ret（无成员访问），语义为空体。
void TowerOfDespairMgr::_layerInfo(CUser* user)
{
}
