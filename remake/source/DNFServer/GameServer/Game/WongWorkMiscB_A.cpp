// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkMiscB_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// WongWorkMiscB_A.cpp — G7 杂项第二组（Avatar/Premium/Compound）
//
// 逐指令忠实转写以下 ORIG df_game_r 真实符号（定义唯一归属本 TU；
// 对应权威头为纯追加声明）：
//   WongWork::Avatar_Item::setColor(short,short)                 W 0x082fa2fa
//   WongWork::Avatar_Item::updateEmblemSocket(uint,uint)          W 0x082fa2a6
//   WongWork::Avatar_Item::~Avatar_Item() D1/D2                   W 0x082fa354
//   WongWork::CAvatarItemMgr::CAvatarItemMgr(const&)              W 0x0869937c
//   WongWork::CHandlePremium::handleReturn2CharacView             T 0x086af96e
//   WongWork::CHandlePremium::handleRemoveOneDayLethePremium      T 0x086b0594
//   WongWork::CCompoundAvatar::tagCompoundInfo::{C1,C2}           W 0x08334344
//   WongWork::CCompoundAvatar::tagCompoundInfo::clear             W 0x0833424a
//   WongWork::CCompoundAvatar::tagCompoundInfo::{D1,D2}           W 0x08334388
//   WongWork::CCompoundAvatar::_SendError                         T 0x08333a76
//   WongWork::CCompoundAvatar::_SendResult                        T 0x08333a9c
//   WongWork::CCompoundAvatar::ProcCompound                       T 0x083338c2
//
// 报告依据：docs/class_func_reports/WongWork__{CHandlePremium,CCompoundAvatar}/
// 及 objdump 反汇编（地址见各行注释）。
// ============================================================================

#include "AvatarItem.h"
#include "CAvatarItemMgr.h"
#include "CHandlePremium.h"
#include "CCompoundAvatar.h"

#include <vector>

#include "CUser.h"                  // CUser / SUserPremiumInfo / SendCmdErrorPacket
#include "CUserCharacInfo.h"        // SetOneDayLetheFlag / getCurCharacName / getCurCharacInvenR
#include "CDataManager.h"           // G_CDataManager()->getItemList()
#include "CItemList.h"             // CItemList::GetRestrictCode
#include "CPremiumLetheManager.h"  // CPremiumLetheManager::NOT_USE_LETHE_STATE
#include "GameTypes.h"             // cMyTrace
#include "GlobalData.h"            // GlobalData::s_systemTime_ (CSystemTime)
#include "CInventory.h"            // GetInvenSlot / GetAvatarItemMgrR
#include "SPremiumInfo.h"           // g_SPremiumInfo（GetAddQuestItemDropRate 表源）
#include "STStackableScript.h"      // STEnchantSystemMapKey/Data 权威定义
#include "CItemAmplifier.h"         // upgrade_table_t 权威布局（+0x48c/+0x490 视图）
#include "CStackableItem.h"         // ProcUpgradeChronicle 材料券脚本（GetEmancipate 视图）
#include "CEquipItem.h"             // m_vec198/m_vec1a4（emancipate 材料/结果表）
// ---------------------------------------------------------------------------
// CEquipItem::GetEmancipateMaterialList/GetEmancipateResultList —— ORIG 弱符号
// W 0x0854b528 / 0x0854b536：push ebp; mov esp,ebp; mov 0x8(%ebp),%eax;
// add $0x198/$0x1a4,%eax; pop ebp; ret —— 直返 &m_vec198 / &m_vec1a4（P1 视图）。
// 本 TU 独占定义（与 CEquipItem.h 声明配套），调用方 ProcUpgradeChronicle 使用。
#include "WongWorkMiscB_A/GetEmancipateMaterialList.inc"
}

#include "WongWorkMiscB_A/GetEmancipateResultList.inc"
}

namespace WongWork
{

// ---------------------------------------------------------------------------
// Avatar_Item — ORIG setColorEss 名称族：ORIG 真名 setColor(short,short)
// _ZN8WongWork11Avatar_Item8setColorEss W 0x082fa2fa：
//   写 packed m_color1(+0x3f)/m_color2(+0x41)。
#include "WongWorkMiscB_A/setColor.inc"
}

// _ZN8WongWork11Avatar_Item18updateEmblemSocketEjj W 0x082fa2a6：
//   以 grade 索引 m_emblemInfo.stEmblemSocket[grade]（pack(1) 下元素 6 字节，
//   起始 +0x21）；socketType(u16)==0 → return 0；否则写入 itemIdx 并 return 1。
#include "WongWorkMiscB_A/updateEmblemSocket.inc"
}

// _ZN8WongWork11Avatar_ItemC1Ev/C2Ev W 0x082fa34e：ORIG 空体（nm 实证重建遗漏）。
#include "WongWorkMiscB_A/Avatar_Item.inc"
#include "WongWorkMiscB_A/dtor_Avatar_Item.inc"
#include "WongWorkMiscB_A/CAvatarItemMgr.inc"
}

#include "WongWorkMiscB_A/CAvatarItemMgr_1.inc"
}

// ---------------------------------------------------------------------------
// CHandlePremium::handleReturn2CharacView —— T 0x086af96e。
// 取 premium 列表(type 上限 0x270f)，逐项：type!=0 且 state==3(已过期) 时以
// handleSetUserPremium(type,0,0,0,true) 重设（链路收尾重算）。
#include "WongWorkMiscB_A/handleReturn2CharacView.inc"
}

// ---------------------------------------------------------------------------
// CHandlePremium::handleRemoveOneDayLethePremium —— T 0x086b0594。
// isAffectedPremium(0x21) 时撤销该 premium、取限制物品码并移除受限物品、通知；
// 无条件置非使用 Lethe 状态并 trace 角色名。
// [b28d-solo-WongWorkMiscB_A] 两处修正：
//   ① `getItemList()` 为头内 inline，4.4 -O0 仍发 out-of-line call（ORIG 4.1 内联为
//      `mov 0xc(%eax),%eax`）⇒ 调用点改直访公有成员 `m_itemList`（p167 EXTRA
//      `CDataManager::getItemList()` 归零）。
//   ② ORIG 尾段为 P227 全表达式临时链：`call getCurCharacName; mov %eax,%ebx;`
//      跨 cMyTrace ctor 保活，ctor 后才压 op() 实参 ⇒ 具名 trace 改临时链 +
//      `__PRETTY_FUNCTION__`（P139：ORIG `_ZZ…E19__PRETTY_FUNCTION__`）。
#include "WongWorkMiscB_A/handleRemoveOneDayLethePremium.inc"
}

// ---------------------------------------------------------------------------
// tagCompoundInfo 生命周期 —— clear W 0x0833424a / C1 0x08334344 / D1 0x08334388。
#include "WongWorkMiscB_A/tagCompoundInfo.inc"
}

#include "WongWorkMiscB_A/clear.inc"
}

// D1/D2：仅析构 vector 子对象（成员隐式析构等价）。
#include "WongWorkMiscB_A/dtor_tagCompoundInfo.inc"
#include "WongWorkMiscB_A/_SendError.inc"
}

// ---------------------------------------------------------------------------
// _SendResult —— T 0x08333a9c：组 0x66 结果包。
// [b28d-solo-WongWorkMiscB_A] 226=226 全等（p167 44/44 CLEAR）。修法：
//   ①结果循环改迭代器形（P214④/P28：begin/end 头部预求值 + while 尾测 +
//     三次后置 `*it++` ⇒ ORIG ppEi×3 + de×3）；
//   ②去 inven/avatarMgr/jewelData/remainDate 具名局部（P52/P183：ORIG 每处重调
//     getCurCharacInvenR，GetAvatarItemMgrR 内联在链中，实参升格 callee-saved）；
//   ③`resultSlot.m_addInfo` 去 `(int)(short)` 冗余 cast（unsigned int 直传）；
//   ④返回类型 void→int（P159：ORIG 尾 `mov $0x1,%ebx; …; mov %ebx,%eax`）；
//   ⑤else 分支 put_binary 实参 (char*)→(const char*)（p167 MISSING/EXTRA 各 1 归零）。
#include "WongWorkMiscB_A/_SendResult.inc"
}

// ---------------------------------------------------------------------------
// ProcCompound —— T 0x083338c2。
// Make → CheckCondition(错误抛 ENUM_ERROR) → Core(同) → Result + 统计；
// 全程捕获 ENUM_ERROR 转 _SendError。
// [b28d-solo-WongWorkMiscB_A] ORIG 0x083338c2 逐点对应：①error 预清 0 后赋值
// （P198①：`movl $0x0,-0x14` 在 _CheckCondition 之前）；②错误判定用
// `while (error != 0) { throw; }` 复现 4.1 `setne %al; test %al,%al; je`
// 物化（P225；4.4 对槽比较恒直跳）；③table 具名局部（ORIG 有 -0x10 槽往返，
// P0 槽-局部律）。
#include "WongWorkMiscB_A/ProcCompound.inc"
}

// ---------------------------------------------------------------------------
// CUserPremium::GetGoldBonus —— ORIG _ZN8WongWork12CUserPremium12GetGoldBonusEi
// W 0x08694a64（非 const）：this+0x8bc 字节非 0 → level*level*100，否则 0。
#include "WongWorkMiscB_A/GetGoldBonus.inc"
}

// ---------------------------------------------------------------------------
// CUserPremium::GetAddQuestItemDropRate —— ORIG …23GetAddQuestItemDropRateEi
// W 0x08694a28：level∉[0,0x69] → 0；否则 g_SPremiumInfo[level].m_field48
// （ORIG 基址 0x09483a88 = g_SPremiumInfo + 0x48，元素步长 0x120）。
#include "WongWorkMiscB_A/GetAddQuestItemDropRate.inc"
}

// ---------------------------------------------------------------------------
// CItemUpgrade::find3rdChronicleEnchantData —— ORIG T 0x0854a916。
// itemScript=CDataManager::find_item(typeIdx)；空→0；map@item+0x2f4 查 key，
// 未命中→0；命中返回 &it->second（Ghidra 报告 node+0x10 语义即 second 地址）。
// [b28d-solo-WongWorkMiscB_A] ①去 `EnchantMap&` 具名引用（P0/P183：ORIG 无引用槽，
// find/end 各自重算 `mov itemScript,%eax; lea 0x2f4(%eax),%ecx`）；②+0x2f4 改用
// 权威成员 `CStackableItem::m_enchantMap`（§4.3 去裸偏移）；③条件改正极性
// `if (it != end) return &it->second; return 0;`（P220/P47：ORIG `je` 落失败尾块）。
#include "WongWorkMiscB_A/find3rdChronicleEnchantData.inc"
}

// ---------------------------------------------------------------------------
// CItemUpgrade::_getAdditionalDisjointItem(bool 版) —— ORIG T 0x08546a96。
// [b28d-solo-WongWorkMiscB_A] ORIG 0x08546a96 逐点对应：
//   ①at() 结果为**值拷贝**局部 `STResultItemCnt elem`（trivial 拷贝展开 4+2 两段
//     `mov (%eax),%edx; mov %edx,-0x2e; movzwl 0x4(%eax),%eax; mov %ax,-0x2a`），
//     非引用（引用多 1 指针槽 + 每次回读，P0）；
//   ②slotIdx/count 类型由 make_pair mangled `IRmRjE` 定：`unsigned long`/`unsigned int`，
//     count 预清 0 + 分支赋值（P198①）；
//   ③PRETTY 改 `__PRETTY_FUNCTION__`（ORIG `movl $sym,0x4(%esp)` 单条，P139）。
#include "WongWorkMiscB_A/_getAdditionalDisjointItem.inc"
}

// ---------------------------------------------------------------------------
// CItemUpgrade::ProcUpgradeChronicle —— ORIG T 0x085491d6
// _ZN8WongWork12CItemUpgrade20ProcUpgradeChronicleEP5CUsersjsjcRSt6vectorIiSaIiEE
// 三代编年史升级：A 槽材料券 + B 槽目标装备；扣 emancipate 材料、按
// chanceGrade 抽 equipGrade 样本、构造结果物品删旧插新。错误码 0x16/0x11。
// [b28d-solo-WongWorkMiscB_A] 349=349 NEAR、p167 44/44 CLEAR。修法：
//   ①invenW 除前两次 GetInvenSlot 外 4 处重调 getCurCharacInvenW()（P183）；
//   ②shadow.setCopy 改 getCurCharacInvenRefR()；③grade 表去 5 路 switch →
//   `m_grade[chanceGrade][r]` 单一 operator[] 调用点；④GetEmancipate() + (short)；
//   ⑤results 非 const 引用 + `*begin()` operator* + pair 拷贝（P96/P220）；
//   ⑥嵌套 if → 提前返回形（复现 ORIG seta/jbe 无符号比较与 je 落主体）；
//   ⑦P290 resultIdx 改 unsigned int（升格 callee-saved 跨 receiver call）；
//   ⑧itemType 预清 0xb（P198①）；⑨newItem 字段具名直写（去 set_add_info call）。
//   残余 N115：ORIG shadowMem 落 -0xca0b（4.1 非 4 对齐分配）vs 4.4 恒 4 对齐
//   -0xca0c ⇒ 3 处 lea 立即数差 1；aligned(1) 探针无效，其余帧/槽/调用点全等。
#include "WongWorkMiscB_A/ProcUpgradeChronicle.inc"
}

}  // namespace WongWork

