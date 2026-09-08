// ============================================================================
// ItemUpgradeVtableB2.cpp — WongWork::CItemUpgrade::ProcUpgrade（ORIG vtable
// _ZTVN8WongWork12CItemUpgradeE @0x08c9d310 槽 0）及其升级域依赖闭环。
//
// 本 TU 定义的 ORIG 符号（nm -C /home/wangyilei/下载/df_game_r 全部核实为真实
// T/W；报告路径 docs/class_func_reports/…，## 汇编 + ## 反编译 C 均已完整读取）：
//
//   WongWork::CItemUpgrade:
//     ProcUpgrade(MSG_ITEM_UPGRADE)   T 0x0854779c  WongWork__CItemUpgrade/ProcUpgrade.md
//     _getUpgradeCountByUpgradeItem   T 0x08548e68  …/_getUpgradeCountByUpgradeItem.md
//     _getUpgradeRateByUpgradeItem    T 0x08548ffa  …/_getUpgradeRateByUpgradeItem.md
//     _CheckTargetUsableLv            T 0x08548c3c  …/_CheckTargetUsableLv.md
//     _ProcPortableUpgradeItem        T 0x08548d3c  …/_ProcPortableUpgradeItem.md
//     _DoProcUpgrade                  T 0x0854755a  …/_DoProcUpgrade.md
//     _ProcPenalty                    T 0x08546bba  …/_ProcPenalty.md
//     _getAdditionalDisjointItem      T 0x08546938  …/_getAdditionalDisjointItem.md
//   expert_job::CAlchemist（静态）:
//     IsExistContinuousEffectItem     T 0x085d121e  expert_job__CAlchemist/IsExistContinuousEffectItem.md
//     GetItemUpgradeSuccessRate       T 0x085d1360  expert_job__CAlchemist/GetItemUpgradeSuccessRate.md
//   DisJoint:
//     GetResult                       T 0x084733f6  DisJoint/GetResult.md
//     _GetAdditionalItem              T 0x084732b4  DisJoint/_GetAdditionalItem.md
//     _GetAdditionalItemInfo          T 0x08473224  DisJoint/_GetAdditionalItemInfo.md
//   CEquipItem::getUpgradeProbIncrease W 0x0854b552（14 字节访问器；_DoProcUpgrade.md
//     引用、无独立报告——调用方报告已读后按 objdump 忠实转写）
//   DisJointResult::DisJointResult   W 0x082347a2（无独立报告；调用方 GetResult/
//     _ProcPenalty 报告已读后按 objdump 忠实转写）
// [b28d-solo-ItemUpgradeVtableB2 判词汇总 · 单文件泳道]
// 本 TU 8 件（明细 /tmp/b28d_solo_ItemUpgradeVtableB2_result.tsv）：
//   _ProcPortableUpgradeItem 85/85 **IDENTICAL**（前轮 AE + 本轮 stack 改 const
//     ⇒ 调 const 版 vector::operator[]，符号与 ORIG 全等，TU identical +1）；
//   DisJointResult::C1 19/19 **AE**（本轮翻正旧 N4：DisJoint.h 加 union 双视图
//     m_f[10]/{m_a[5],m_b[5]}，ctor 改写 m_a[i]/m_b[i] ⇒ 复现 ORIG
//     `add $0x4,%edx; movl $0,0x4(%eax,%edx,4)`）；
//   IsExistContinuousEffectItem 66/66 **AE**（本轮翻正旧 N18b：同布局视图
//     ContinuousItemIdxView 的 unsigned 首字段 ⇒ 复现两段装载 mem→%edx/param→%eax）；
//   DisJoint::GetResult 105/105 **AE**（本轮翻正旧 N85：内联 rarity、int 槽复用
//     sellPrice、内联 sell、去 money 具名槽、if 形最小值）；
//   _getAdditionalDisjointItem 108/102 N85①（4.1 float 拷贝经 flds/fstps、4.4 恒
//     mov；volatile 实测反劣）+ N82（槽号）+ 尾 3 岛（4.1 多出口 jmp+nop）；
//     P167 CLEAR 8/8、条件方向已用 jb 复现、make_pair 嵌套形/unsigned span/去 +0.5 已落；
//   _DoProcUpgrade 166/163 N82（槽号重排）+ rollWin setcc-fold（4.1 mov $1/$0 岛）；
//     P167 CLEAR 14/14；
//   _ProcPenalty 634/620 N82（§6.2 三件套：P167 CLEAR 87/87、gap 14、帧同）；
//   ProcUpgrade 1242/1244 N82 + P292（P167 CLEAR 115/115、gap -2；
//     _GetUpgradeInfo int→bool 属 CItemUpgrade.cpp 属主，跨 TU 不改）。
// 全部 8 件 P167 调用点差集 CLEAR（helper EXTRA×2 已删：missing_in_game 40→36）。
//   getItemSellPrice(int,int,int,bool) T 0x0898c86e（无独立报告；调用方 GetResult/
//     _ProcPenalty 报告已读后按 objdump 忠实转写）
//   g_alchemistInfo                 B 0x094f7778（数据符号；nm 尺寸 8，唯一读者按
//     低 32 位 int 读取）
//
// vtable 闭环说明：GCC 以类内首个虚函数为 key function；本 TU 定义 ProcUpgrade
// 后由编译器产出 _ZTVN8WongWork12CItemUpgradeE，消除当前链接的
// "undefined reference to `vtable for WongWork::CItemUpgrade'"
// （CItemUpgrade.cpp.o 析构与 CtorProducers_2C.cpp.o 构造两处引用）。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CItemAmplifier.h"
#include "CStackableItem.h"
#include "CGlobalEffectManager.h"   // s_globalEffectManager_ 完整类型
#include "CEquipItem.h"
#include "CGuildServerProxy.h"    // CEventBase / CEventManager / stADDINFO
#include "CInventory.h"           // CInventory / Store / CValueStatistic
#include "DB_ReqItemUpgradeLog.h"
#include "PvPTypes.h"             // CMTRand（+0x4e8 randInt）
#include "EventUpgradeJarInfo.h"  // Event_Upgrade_Jar
#include "CCubeStatistic.h"
#include "DisJoint.h"
#include "LogManager.h"
#include "CSystemTime.h"
#include "CUserProc.h"            // APSystem::CUserProc
#include "CEnvironment.h"
// WongWork::CItemUpgradeTable（CItemUpgrade 表区子对象；ProcUpgrade 取表直调
// 其 getUpgradeInfoTable W 0x0854b382，禁 inline 视图代返）。
#include "CItemGenerator.h"
// MSG_ITEM_UPGRADE 完整类型（CItemUpgrade_Separate.h 仅前向声明；ProcUpgrade
// 需具名读 m_upgradeType/m_targetSlot/m_targetItemIdx/m_ticketSlot/
// m_materialSlot/m_25/m_27/m_28）。
#include "PacketTypes.h"

#include <cmath>
#include <cstring>
// std::max（_getAdditionalDisjointItem 0x08546983 模板实调）
#include <algorithm>
#include <utility>
#include <vector>

extern int itemPriceRates_[3];    // B 0x094f7bfc（InitScriptProducers_4.cpp 定义）

// B 0x094f7778（nm 尺寸 8）。源码此前无任何定义/写点；唯一读者
// GetItemUpgradeSuccessRate 以 fildl 取低 32 位。按 int 定义并置零。
extern int g_alchemistInfo[2];   // B 0x094f7778（定义于 CExpertJobList.cpp int[2]）


namespace expert_job
{

// [b28d-solo-ItemUpgradeVtableB2] ContinuousItemInfo 同布局具名视图：
// ORIG IsExistContinuousEffectItem 0x085d12a2 `call deEv; mov (%eax),%edx;
// mov 0xc(%ebp),%eax; cmp %eax,%edx` 是**两段装载**比较 ⇒ 字段与 int 形参
// 异型（P248 档③/P274）；权威头 ContinuousItemInfo::m_itemIdx 为 int 且跨 TU
// 消费点众多（改头需全消费段联判），故本 TU 以同布局视图（unsigned 首字段）
// 表达，零头改动、零 ABI/布局风险。
struct ContinuousItemIdxView
{
    unsigned int m_itemIdx;      // +0x00（同 ContinuousItemInfo::m_itemIdx）
    unsigned int m_expireTime;   // +0x04
};

// ---- CAlchemist 静态成员（此前无声明；仅本 TU 使用）----
class CAlchemist
{
public:
    static bool IsExistContinuousEffectItem(CUser* user, int itemId);
    static float GetItemUpgradeSuccessRate();
};

}  // namespace expert_job

// ============================================================================
// expert_job::CAlchemist::IsExistContinuousEffectItem @ 0x085d121e
//   getCurCharacR 为空 → false；遍历当前角色持续道具表，命中 itemId 即以
//   「当前秒 < 到期秒」判定存在性。
// [b28d-solo-ItemUpgradeVtableB2] **AE 66/66**：旧判 N18b（成员==参数
//   mem-cmp 折叠）已翻正——ContinuousItemIdxView 同布局视图（unsigned 首字段
//   vs int 形参，P248 档③/P274）令 4.4 复现 ORIG 两段装载
//   `mov (%eax),%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`（探针 g 实证；
//   视图指针必须内联在条件里，具名会多地址槽）。
// ============================================================================
bool expert_job::CAlchemist::IsExistContinuousEffectItem(CUser* user,
                                                         int itemId)
{
    // [b28d-64] P214①/P56：ORIG 0x085d1232 `test;setne;test;je <尾 mov$0>` =
    // 正向包裹（负形早返产 sete）；命中支 ORIG 0x085d12b5/12bc 两独立
    // `mov $1/$0 + jmp` 岛 ⇒ if/return 两条（直返比较结果会折 setb 直跳）。
    // 迭代器槽：ORIG begin→-0x10（浅）/end→-0x14（深）。
    if (((const CUserCharacInfo*)user)->getCurCharacR() != 0)
    {
        const std::vector<ContinuousItemInfo>* list =
            ((const CUserCharacInfo*)user)->GetCurCharacEffectItemListR();
        std::vector<ContinuousItemInfo>::const_iterator it = list->begin();
        std::vector<ContinuousItemInfo>::const_iterator last = list->end();
        for (; it != last; ++it)
        {
            // 视图指针必须**内联**在条件表达式里（P183）：具名指针会多一个
            // 地址槽（-0xc），使 it/last 槽整体下移 4（ORIG 直用 deEv 的 eax）。
            if (reinterpret_cast<const ContinuousItemIdxView*>(&(*it))
                    ->m_itemIdx == itemId)
            {
                // P167 新判据（b28c C28L8）：ORIG 全函数只调 `deEv`
                // （`(*it).m` 形），不调 `ptEv`（`it->m` 形）⇒ 此处写
                // `(*it).m_expireTime` 保持 deEv×2 / ptEv×0。
                if ((unsigned)GlobalData::s_systemTime_.getCurSec() <
                    (*it).m_expireTime)
                {
                    return true;
                }
                return false;
            }
        }
    }
    return false;
}

// ============================================================================
// expert_job::CAlchemist::GetItemUpgradeSuccessRate @ 0x085d1360
//   return (float)g_alchemistInfo / *(float*)0x08cbfa3c (= 9840.0f)
// ============================================================================
float expert_job::CAlchemist::GetItemUpgradeSuccessRate()
{
    static const float divisor = 9840.0f;  // ORIG .rodata 0x08cbfa3c
    return (float)g_alchemistInfo[0] / divisor;   // 合并后 int[2]；低半区 [0] 为原标量
}

// ============================================================================
// getItemSellPrice @ 0x0898c86e（自由函数；nm 实查 size 0x8e）
//   【切片转写】game_func_report 对本符号的 ORIG 段取错（撞名，§6.3 假阳性清单
//   已报 Main），故按 objdump 0x0898c86e..0x0898c8fb 人工转写：
//     0x0898c874 `mov 0x14(%ebp),%eax; mov %al,-0x14(%ebp)` bool 实参落本地槽；
//     0x0898c87a `movb $0,-0x1` + `cmpb $0,-0x14; je` + `movb $2,-0x1` ⇒ sel 形；
//     0x0898c888 `cmp 0x10,%eax; jle; mov 0x10→0xc` ⇒ a=min(a,b)（写回形参槽）；
//     0x0898c896 `cmpl $0,0x10; jne` 后**两分支各自完整重发**
//     `movzbl sel; mov rates(,%eax,4); imul price; 0x10624dd3 magic /1000`
//     （0x0898c89c 与 0x0898c8c6 各 13 条）⇒ 提公共 base 局部会丢整段 13 insn
//     （P48/P183：-O0 无 CSE）。
// ============================================================================
int getItemSellPrice(int price, int a, int b, bool flag)
{
    unsigned char sel = 0;
    if (flag)
    {
        sel = 2;
    }

    if (a > b)
    {
        a = b;
    }

    if (b == 0)
    {
        return itemPriceRates_[sel] * price / 1000;
    }
    return itemPriceRates_[sel] * price / 1000 * a / b;
}

// ============================================================================
// CStackableItem 强化/增幅券访问器（0xf 字节级访问器；声明见 CStackableItem.h）
//   IsReinforceTicket      W 0x084b4222：movzbl +0x30f（m_char30f）
//   GetReinforceRate       W 0x084b4232：movzbl +0x312（m_char312）
//   IsAmplifyReinforceTicket W 0x084b4242：movzbl +0x313（m_char313）
//   GetAmplifyReinforceRate  W 0x084b4252：movzbl +0x316（m_char316）
//   GetReinforceVaule      T 0x0854b4f8：movzwl +0x310（m_short310）
//   GetAmplifyReinforceValue T 0x0854b508：movzwl +0x314（m_short314）
// ============================================================================
bool CStackableItem::IsReinforceTicket()
{
    return m_char30f;
}

unsigned char CStackableItem::GetReinforceRate()
{
    return (unsigned char)m_char312;
}

bool CStackableItem::IsAmplifyReinforceTicket()
{
    return m_char313;
}

unsigned char CStackableItem::GetAmplifyReinforceRate()
{
    return (unsigned char)m_char316;
}

unsigned short CStackableItem::GetReinforceVaule()
{
    return m_short310;
}

unsigned short CStackableItem::GetAmplifyReinforceValue()
{
    return m_short314;
}

// ============================================================================
// DisJointResult ctor @ 0x082347a2：循环 i=0..4 将两组 5 int 清零
// （等价整体清零 10 个 int）。
// [b28d-solo-ItemUpgradeVtableB2] 旧判 N4（P30 add-vs-lea）已翻正：ORIG 第二组
//   `add $0x4,%edx; movl $0,0x4(%eax,%edx,4)` 的偏移 0x14 = 索引 add 4（16B）
//   + disp 4，只在**独立成员数组**（m_b@+0x14）下标形下由 4.4 复现（探针 c5/c7）；
//   单数组 m_f[i+5] 恒折 `lea 5(%eax)`（c1）。DisJoint.h 已加 union 双视图
//   m_f[10] / {m_a[5], m_b[5]}（sizeof 0x28、全偏移不变）。
// ============================================================================
DisJointResult::DisJointResult()
{
    for (int i = 0; i <= 4; ++i)
    {
        m_a[i] = 0;
        m_b[i] = 0;
    }
}

// ============================================================================
// DisJoint::GetResult @ 0x084733f6
//   out.m_f[5] = m_cubeIndexVecs[4][0]；
//   out.m_f[0] = ROUND(((ROUND(1.1f*sellPrice) 经 getItemSellPrice 折算 ×1000 ÷
//                m_creationConst) * m_creationRate[rarity]) ÷ 1000.0f)，最小 1；
//   _GetAdditionalItem(item, &out.m_f[6], &out.m_f[1])。
//   物品缺失走 LogManager::logFormat(1,…,0x6e,"[DisJoint::GetResult] Null Item\n")
//   并返回 0x11。
// [b28d-solo-ItemUpgradeVtableB2] **AE 105/105**：旧判 N85 已翻正——①rarity
//   无槽（get_rarity 直入 %edx 参与 0x14c 索引）②sellPrice 用 int 槽复用
//   （fildl→*1.1f→RC=trunc fistpl 写回同槽，禁 float 局部）③getItemSellPrice
//   返回值无槽（直进 imul $0x3e8）④base 具名槽 -0xc ⑤money 不设具名局部
//   （4.4 临时槽形与 ORIG -0x20 同构）⑥最小值用 if 形（三目折 cmovg）。
// ============================================================================
int DisJoint::GetResult(int itemIdx, DisJointResult& out)
{
    CItem* script = G_CDataManager()->find_item(itemIdx);
    if (script == 0)
    {
        LogManager::logFormat(1, "DisJoint.cpp",
                              __PRETTY_FUNCTION__,
                              0x6e, "[DisJoint::GetResult] Null Item\n");
        return 0x11;
    }

    // [b28d-solo-ItemUpgradeVtableB2] P0/P183：ORIG 0x08473410..0x08473466
    // rarity 无栈槽（get_rarity 结果直入 %edx 参与索引）⇒ 不设具名局部。
    const float rate = m_creationRate[script->get_rarity()];
    out.m_f[5] = m_cubeIndexVecs[4][0];

    // ORIG 0x0847348a..0x084734a3：int 槽 -0x10 复用（GetSellPrice 结果 →
    // fildl → *1.1f → RC=trunc fistpl 写回同槽）⇒ 具名 int 写回赋值，
    // 禁 float 局部（多 fstps 且帧 +0x10）。
    int sellPrice = script->GetSellPrice();
    sellPrice = (int)(sellPrice * 1.1f);
    // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x08473445..0x0847346c：getItemSellPrice
    // 返回值**无槽**（直接 `imul $0x3e8,%eax` 进入除法）⇒ 不设 sell 具名局部。
    // ORIG 0x0847346f..0x08473482：整数除法商存具名槽 -0xc（fildl 直读）。
    const int base = (getItemSellPrice(sellPrice, 0, 0,
                                       script->IsRandomOption()) * 1000) /
                     m_creationConst;
    // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x0847350a `fistpl -0x20(%ebp)` +
    // `mov -0x20,%edx` 是编译器临时槽（不设 money 具名局部，4.4 直接
    // fistpl 到临时再写 out.m_f[0]，可省一个具名槽）。
    out.m_f[0] = (int)(((float)base * rate) / 1000.0f);
    // ORIG 0x08473520..0x0847352b `test %eax,%eax; jg` + movl $1 ⇒ if 形
    // （三目在 4.4 折 cmovg）。
    if (out.m_f[0] <= 0)
    {
        out.m_f[0] = 1;
    }

    _GetAdditionalItem(script, out.m_f[6], out.m_f[1]);
    return 0;
}

// ============================================================================
// CReduceUpgradeItemPay —— 强化减费重复事件（CEventBase 派生；与
// CItemUpgrade.cpp 内同形内联定义，弱符号合并）。
// ORIG GetReduceRate W 0x854b56e：返回 +0x04 short 经 cwtl 符号扩展。
// ============================================================================
class CReduceUpgradeItemPay : public CEventBase
{
public:
    int GetReduceRate() { return (short)(m_addInfo.m_info0); }
};

// [b28d-solo-ItemUpgradeVtableB2] P49/P167：原匿名 namespace 的
// upgradeRand/randIntBound/embeddedTable/embeddedUpgradeTable 四个 inline
// helper 已全部删除——-O0 下它们是**真实 call**（ORIG 无的 EXTRA 符号），
// 调用点改为 m_rand 字段直读 / 基类 getUpgradeInfoTable() 成员直调后
// p167 差集归零（missing_in_game 40→36）。
namespace WongWork
{

// ============================================================================
// _getUpgradeCountByUpgradeItem @ 0x08548e68
//   [b28d-58] AE（P46 叶块排放序 = case 书写序）：原 case 序 1d29/1c6a/1c6b/1c69…
//   使各 je 落点与 ORIG 叶块错位；改 ORIG 排放序 1c69/1c6a/1c6b/1d29/1e3f/1e40/1e86
//   后逐条全等（ORIG 0x08548ecd..0x08548ee5）。
//   默认按材料 idx 映射强化档位（仅非增幅物品）；材料为堆叠物时改读
//   ReinforceTicket/AmplifyReinforceTicket 数值（[0,0x1f] 越界归零）。
//   增幅券在目标无增幅能力时报 ENUM_ERROR 0x13。
// ============================================================================
unsigned short CItemUpgrade::_getUpgradeCountByUpgradeItem(
    const Inven_Item& target, const Inven_Item& material, ENUM_ERROR& err)
{
    short count = 0;
    // ORIG 0x08548e7c：hasAbility 每用点重取（P48，%al 直测）。
    // ORIG 为正向 `test %al,%al; jne`（P59）：then 空 else 才是 switch。
    if (target.m_amp.hasAbility())
    {
    }
    else
    {
        switch (material.m_addInfo)
        {
            // 叶块排放序 = case 书写序（P46）；照 ORIG +0x65..+0x95 排：
            // 1c69/1c6a/1c6b/1d29/1e3f/1e40/1e86（ORIG 0x08548ecd..ee5）。
            case 0x1c69: count = 7;  break;
            case 0x1c6a: count = 10; break;
            case 0x1c6b: count = 12; break;
            case 0x1d29: count = 7;  break;
            case 0x1e3f: count = 3;  break;
            case 0x1e40: count = 5;  break;
            case 0x1e86: count = 13; break;
            default:     count = 0;  break;
        }
    }

    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script != 0 && script->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)script;
        if (stack->IsReinforceTicket())
        {
            // ORIG 0x08548f70 `cwtl` + int 槽 -0x10 + `cmpl` 区间判定，末了
            // `mov %ax,-0x1a` 落 short ⇒ 票面值先入 int 局部（P10/P11）。
            const int value = (short)stack->GetReinforceVaule();
            if (value < 0 || 0x1f < value)
            {
                count = 0;
            }
            else
            {
                count = (short)value;
            }
        }
        else if (stack->IsAmplifyReinforceTicket())
        {
            if (!target.m_amp.hasAbility())
            {
                err = (ENUM_ERROR)0x13;
                return 0;
            }
            const int value = (short)stack->GetAmplifyReinforceValue();
            if (value < 0 || 0x1f < value)
            {
                count = 0;
            }
            else
            {
                count = (short)value;
            }
        }
    }
    return (unsigned short)count;
}

// ============================================================================
// _getUpgradeRateByUpgradeItem @ 0x08548ffa
//   同上结构；映射命中直接返回 100；票券读 Rate 字段（[0,100] 越界归零）。
// ============================================================================
unsigned char CItemUpgrade::_getUpgradeRateByUpgradeItem(
    const Inven_Item& target, const Inven_Item& material, ENUM_ERROR& err)
{
    char rate = 0;
    // ORIG 0x08549018：hasAbility 每用点重取（P48，结果 %al 直测无槽）。
    // ORIG 0x0854901b 为正向 `test %al,%al; jne`（P59）：then 为空 else 才是
    // switch，负向 ! 形会多出 xor $0x1 且 je 反向。
    if (target.m_amp.hasAbility())
    {
    }
    else
    {
        // ORIG 0x0854901d..0x08549059 是 switch 二分决策树（首比较 0x1d29、
        // `sub $0x1c69; cmp $0x2; ja` 小区间、0x1e3f/0x1e40 jb/jbe、0x1e86），
        // 命中共享叶 `movb $0x64,-0x19; movzbl; jmp 返回`；default 叶
        // `movb $0,-0x19` 后**继续**执行（不返回）⇒ 必须写 switch + case 升序
        // （P46/P163），布尔区间链会退化成 cmp 序列且丢 18 insn。
        switch (material.m_addInfo)
        {
            case 0x1c69:
            case 0x1c6a:
            case 0x1c6b:
            case 0x1d29:
            case 0x1e3f:
            case 0x1e40:
            case 0x1e86:
                rate = 100;
                return (unsigned char)rate;
            default:
                rate = 0;
                break;
        }
    }

    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script != 0 && script->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)script;
        if (stack->IsReinforceTicket())
        {
            // ORIG +0x0cb 用 `cmpb` 字节直接判定（char 槽 -0xd 存 rate），非 int。
            char value = (char)stack->GetReinforceRate();
            if (value < 0 || 100 < value)
            {
                rate = 0;
            }
            else
            {
                rate = value;
            }
        }
        else if (stack->IsAmplifyReinforceTicket())
        {
            if (!target.m_amp.hasAbility())
            {
                err = (ENUM_ERROR)0x13;
                return 0;
            }
            const int value = (char)stack->GetAmplifyReinforceRate();
            if (value < 0 || 100 < value)
            {
                rate = 0;
            }
            else
            {
                rate = (char)value;
            }
        }
    }
    return (unsigned char)rate;
}

// ============================================================================
// _CheckTargetUsableLv @ 0x08548c3c（const）
//   材料（param_1）脚本可堆叠且 +0x320 标志为 1 时，要求目标（param_2）为
//   可装备类型且其可用等级落在材料脚本 [m_int318, m_int31c] 区间内。
// ============================================================================
bool CItemUpgrade::_CheckTargetUsableLv(const Inven_Item& material,
                                        const Inven_Item& target) const
{
    // [b28d-64] P194：ORIG 各出口直接 `mov $0/$1,%eax; jmp end`（0x08548c69/
    // 0x08548c86/0x08548ca7/…），无汇聚 bool 槽；汇聚形（-0x19）令每出口
    // 改走 `movb $..,-0x19; jmp` 且末尾多 movzbl（+5 条）。
    CItem* matScript = G_CDataManager()->find_item(material.m_addInfo);
    if (matScript == 0)
    {
        return false;
    }
    if (!matScript->is_stackable())
    {
        return true;
    }
    const CStackableItem* stack = (const CStackableItem*)matScript;
    // [b28d-64] P248 档②：字段 bool 化后 `if (!field)` 直产 ORIG 0x08548c9c
    // `movzbl 0x320; xor $1; test %al; je`（char 字段恒折 `cmp $0x1,%al; je`，gap 1）。
    if (!stack->m_char320)
    {
        return true;
    }
    CItem* tgtScript = G_CDataManager()->find_item(target.m_addInfo);
    if (tgtScript == 0)
    {
        return false;
    }
    if (!target.isEquipableItemType())
    {
        return false;
    }
    // ORIG 0x08548cf0 `mov -0x1c,%eax; mov %eax,-0x18` 具名副本槽，再取
    // [m_int318, m_int31c] 区间与 getUsableLevel 比较。
    const CItem* usableScript = tgtScript;
    const int minLv = stack->m_int318;
    const int maxLv = stack->m_int31c;
    const int usable = usableScript->getUsableLevel();
    // [b28d-64] P272/P222：ORIG 0x08548d14 `mov -0x14; cmp -0xc; jg fail` +
    // `mov -0xc; cmp -0x10; jle ok` ⇒ 左操作数写 minLv、第二项写 usable>maxLv。
    if (minLv > usable || usable > maxLv)
    {
        return false;
    }
    return true;
}

// ============================================================================
// _ProcPortableUpgradeItem @ 0x08548d3c（const）
//   便携带升级券（action 类型 0x35）：校验通过后把 info 改写为
//   {recipe=材料 idx, count=1, discount=0} 并返回 0。
// [b28d-solo-ItemUpgradeVtableB2] **AE 85/85**（前轮判词复验，本轮改动无回归）。
// ============================================================================
int CItemUpgrade::_ProcPortableUpgradeItem(const Inven_Item& material,
                                           const Inven_Item& target,
                                           upgrade_info_t& info) const
{
    static const int MaxPortableItemActionIntCount = 2;  // ORIG _ZZNK8WongWork12CItemUpgrade24_ProcPortableUpgradeItem…E29MaxPortableItemActionIntCount
    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script == 0)
    {
        return 4;
    }
    if (!script->is_stackable())
    {
        return 0;
    }
    // [b28d-solo-ItemUpgradeVtableB2] P96/P270：ORIG 调 **const** 版
    // `_ZNKSt6vectorIiSaIiEEixEj` ⇒ 源码该指针是 const（verifyStackableAction
    // 已声明 const，CStackableItem.h:441）。
    const CStackableItem* stack = (const CStackableItem*)script;
    if (!stack->verifyStackableAction((StackableAction::Type)0x35))
    {
        return 0;
    }
    // [b28d-64] P0/P183：ORIG 0x08548dd9 无 vector 引用局部（引用引入 -0x18 指针槽，
    // 使后续槽整体下移 4）；直接成员表达式访问。up 槽 ORIG 0x08548e1e 为
    // `mov %al,-0x9`（单字节槽）⇒ 局部类型 unsigned char，比较处 (int) 提宽。
    if (stack->m_vec3a0.size() != MaxPortableItemActionIntCount)
    {
        return 4;
    }
    const int minUp = stack->m_vec3a0[0];
    const int maxUp = stack->m_vec3a0[1];
    const unsigned char up = target.GetUpgrade();
    if ((int)up < minUp || (int)up > maxUp)
    {
        return 0x13;
    }
    info.m_discountBase = 0;            // +0x34（具名字段）
    info.m_meterialCount = 1;           // +0x30（具名字段）
    info.m_recipeItem = material.m_addInfo;  // +0x2c（具名字段）
    return 0;
}

// ============================================================================
// _DoProcUpgrade @ 0x0854755a
//   以 info+0x20 的失败概率经事件/炼金/装备三段修正后掷骰：
//   roll >= prob → IncUpgrade 且返回 true（强化成功）；否则保持等级并返回
//   false（调用方走 _ProcPenalty）。
// [b28d-solo-ItemUpgradeVtableB2] DIFF 166/163 封顶（N82 + setcc-fold）：
//   P167 CLEAR 14/14，帧同（0x94）。已修：删 range 缓存（m_randMax 每用点
//   重读）、两处 `&&` 拆嵌套 if（复现双 test/je 短路口）、a0/a1 独立 max 局部、
//   reduced 左操作数改 m_randMax（复现 fildl→fstps→call→fmuls 求值序）、
//   内联 invenR/roll、恢复 wornIdx 具名槽。残余：①prob 等局部槽号整体重排
//   （4.4 首次使用序 vs 4.1 声明序，N82）②`rollWin = roll < prob` 4.4 恒
//   setb+槽+xor，ORIG 是 mov $1/$0 + test 岛（P197④ setcc-fold，call 结果
//   寄存器直比不可翻）。
// ============================================================================
bool CItemUpgrade::_DoProcUpgrade(CUser* user, Inven_Item* item,
                                  const upgrade_info_t* info)
{
    unsigned int prob =
        (unsigned int)info->m_prob20[0];   // info +0x20（失败概率，具名字段）
    GlobalData::s_pEvent_UpGrade_Jar->GetProbability(
        info->m_recipeItem,   // info +0x2c（材料 item idx，具名字段）
        prob);

    // [b28d-solo-ItemUpgradeVtableB2] P183/P225：ORIG 0x08547562 起 m_randMax
    // **每用点重读**（`mov 0x8(%ebp),%eax; mov 0x4ec(%eax),%eax`），无 range
    // 缓存局部；两处 `&&` 在 ORIG 是各自 `test/je 同一尾` 的短路跳
    // （0x08547569/0x08547576、0x08547588/0x085475a3），4.4 对 `a&&b` 会物化
    // bool 岛 ⇒ 拆嵌套 if 复现（P225 推广）。
    if (prob != 0)
    {
        if (GlobalData::s_globalEffectManager_->isAffectedEffect(3))
        {
            const float effect =
                GlobalData::s_globalEffectManager_->getEffectValue(3);
            // ORIG 0x08547595..0x085475c4：`fildl m_randMax; fmuls effect;
            // RC=trunc fistpl; sub m_randMax` 且两次独立重读 m_randMax。
            const int delta =
                (int)((float)m_randMax * effect) - m_randMax;
            unsigned int a0 = prob - (unsigned int)delta;  // local_74[0]
            unsigned int a1 = 0;                           // local_74[1]
            prob = std::max(a1, a0);   // _ZSt3maxIjERKT_S2_S2_
        }
    }

    if (prob != 0)
    {
        if (expert_job::CAlchemist::IsExistContinuousEffectItem(user, 0x27ac59))
        {
            // ORIG 0x085475d5..0x085475f6：先 fildl m_randMax → fstps 槽，
            // 再 call GetItemUpgradeSuccessRate → fmuls ⇒ 左操作数是 m_randMax
            // （求值序：`(float)m_randMax * rate`，禁把 call 写左侧）。
            const int reduced =
                (int)((float)m_randMax *
                      expert_job::CAlchemist::GetItemUpgradeSuccessRate());
            unsigned int a0 = prob - (unsigned int)reduced;  // local_68
            unsigned int a1 = 0;                             // local_64
            prob = std::max(a1, a0);
        }
    }

    // [b28d-solo-ItemUpgradeVtableB2] P183：ORIG 0x0854764f `call
    // getCurCharacInvenR; ... call GetInvenSlot` 无 invenR 具名局部。
    Inven_Item worn = ((CUserCharacInfo*)user)->getCurCharacInvenR()
                          ->GetInvenSlot(0, 0xb);
    // ORIG 0x08547675 `mov -0x5b(%ebp),%eax; mov %eax,-0x1c(%ebp)` ⇒ 有具名
    // wornIdx 局部（先落槽再传入 find_item）。
    const int wornIdx = (int)worn.m_addInfo;
    CItem* equipScript = G_CDataManager()->find_item(wornIdx);
    if (equipScript != 0 && prob != 0)
    {
        // W 0x0854b552：失败概率 -= 装备 +0x1d0 强化概率提升值。
        prob -= (unsigned)((CEquipItem*)equipScript)->getUpgradeProbIncrease();
        if ((unsigned)m_randMax < prob)   // +0x4ec（具名字段）
        {
            prob = 0;
        }
    }

    if (prob != 0)
    {
        // ORIG：mov 0x4ec（m_randMax）存栈 → mov 0x4e8（m_rand）→ randInt(&bound)；
        // 结果**无槽**（%eax 直比 prob，0x08547768 `cmp %edx,%eax`）。
        unsigned long boundStorage = (unsigned long)m_randMax;
        if ((unsigned int)m_rand->randInt(boundStorage) < prob)
        {
            return false;
        }
    }
    item->IncUpgrade();
    return true;
}

// ============================================================================
// ProcUpgrade @ 0x0854779c —— 强化主流程（vtable 槽 0）。
// 返回 0 成功；其余为错误码（4/8/9/0xd/0x10/0x11/0x13/0x16/0x17/0xea/0x3f4…）。
//
// 【batch-11 验收标注（§6.2 超大函数口径）】orig 1242 / ours 1246（gap 4）。
//   ① 调用点差集（P167）：ORIG direct 115 + indirect 5 ↔ ours 115 + 5，**归零**
//      （仅 Inven_Item C2×4 / upgrade_info_t C2×1 ↔ 4.4 发射 C1×5 的同体符号名
//      对称差，函数体等价，属跨 GCC 版本发形）；
//   ② 逐块比对补齐：4 处 Inven_Item「默认构造+赋值」物化块、map operator-> ×3、
//      GetRepeatEvent ×2、use_money 双分支各自重取 invenW、减费装备改走
//      getCurCharacInvenR(0,0xb)、回包 add_info 改取材料槽且重取 invenW、
//      randInt 上界改字面 99、去掉 ORIG 不存在的 GetRepeatEvent 判空防御
//      （gap 156 → 4）；
//   ③ 残余 4 insn 集中在 ticket 级 bool 汇聚岛（0x08547c51）与
//      /100 magic 除法展开（0x08548a98）⇒ 判**语义完成、byte 层
//      N76/N82/N18b 封顶**，勿再耗轮次。
// [b28d-solo-ItemUpgradeVtableB2 复核] DIFF 1242/1244 封顶（N82 + P292）：
//   ①P167 CLEAR 115/115（本轮删匿名 namespace helper×2，EXTRA 归零）；
//   ②gap -2（我方多 2 条，含 1 nop）；③帧 0x32c 同、残余为槽号整体重排
//   （N82）与两处 bool 物化（`_GetUpgradeInfo` int 返回的 `test/sete` vs
//   ORIG `xor $0x1`；callee 定义在 CItemUpgrade.cpp 属主 TU，P292 跨 TU
//   返回类型冲突不改）⇒ 语义完成、byte 层封顶。
//   MSG_ITEM_UPGRADE +0x1a/+0x1b/+0x1d/+0x21/+0x23 字段化已由 Main 代落
//   PacketTypes.h:807-812；本函数无裸偏移读（CEnvironment+0x378 已按 §4.3
//   具名化为 m_serverEnvir.m_serverGroup）。
//
// 物化口径（P183/P167 实证）：ORIG 对材料/保护券槽各发
//   Inven_Item C2（0x080cb854）+ GetInvenSlot 临时槽（-0x2c8）+ 15×movl+1×movb
//   逐字段拷贝，共四处独立物化块（-0x15e/-0x19b/-0x1d8/-0x215）；重建端一律
//   「默认构造 + 赋值」两步形，禁 `T x = f()` 直构（会折叠 sret 目标，丢整块）。
// ============================================================================
int CItemUpgrade::ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                              PacketGuard* pg)
{
    CInventory* invenW = ((CUserCharacInfo*)user)->getCurCharacInvenW();

    Inven_Item target = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                             msg->m_targetSlot);
    if (target.isEmpty())
    {
        return 4;
    }
    if (target.m_addInfo != msg->m_targetItemIdx)
    {
        return 4;
    }

    Inven_Item material;
    material = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_materialSlot);
    if (material.isEmpty())
    {
        return 0x16;
    }

    Event_Upgrade_Jar* jar = GlobalData::s_pEvent_UpGrade_Jar;
    if (!jar->CheckMaxLuckyLevel(target, material.m_addInfo))
    {
        return 0x3f4;
    }
    jar->CheckMaterialIndex(material.m_addInfo);

    ENUM_ERROR err = ENUM_ERROR_NONE;              // -0xdc
    msg->m_25 = _getUpgradeCountByUpgradeItem(target, material, err);
    if (err != ENUM_ERROR_NONE)
    {
        return (int)err;
    }
    msg->m_27 = _getUpgradeRateByUpgradeItem(target, material, err);
    if (err != ENUM_ERROR_NONE)
    {
        return (int)err;
    }

    CItem* script = G_CDataManager()->find_item(target.m_addInfo);
    if (script == 0)
    {
        return 4;
    }

    Inven_Item ticket;                             // -0x19b
    char ticketFlag = 0;                           // -0x45
    if (msg->m_ticketSlot != 0xffff)
    {
        if (!script->IsPossibleKiriProtection())   // CItem vtable+0x44
        {
            return 0xea;
        }
        ticket = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        if (ticket.isEmpty())
        {
            return 4;
        }
        std::map<unsigned int, stUpgradeItemTicketParam_t>& tickets =
            G_CDataManager()->m_serverParameter.m_ticket468;
        std::map<unsigned int, stUpgradeItemTicketParam_t>::iterator it =
            tickets.find(ticket.m_addInfo);
        if (it == tickets.end())
        {
            return 9;
        }
        // ORIG 每次 it->second 都重调 _Rb_tree_iterator::operator->（3 次：
        // 0x08547c15/0x08547c3b/0x08547c6b）、GetUpgrade 重调 2 次 ⇒ 表达式内
        // 逐点写，禁 &it->second 缓存（P183）。
        if (target.GetUpgrade() < it->second.m_view.m_minUpgrade ||
            it->second.m_view.m_maxUpgrade < target.GetUpgrade())
        {
            return 8;
        }
        ticketFlag = (char)it->second.m_view.m_ticketFlag;
    }

    // 升级表：增幅物品用 CItemAmplifier 表，否则表区子对象的 getUpgradeInfoTable。
    const upgrade_table_t* table;
    if (!target.m_amp.hasAbility())
    {
        // [b28d-solo-ItemUpgradeVtableB2] P49/P167：删匿名 namespace inline
        // helper（helper 在 -O0 是真实 call ⇒ ORIG 无的 EXTRA 符号）；基类
        // CItemUpgradeTable 成员直调，this 自动 +0x04 调整（ORIG 0x08547ca4
        // `add $0x4,%eax` + call getUpgradeInfoTable）。
        table = getUpgradeInfoTable();
    }
    else
    {
        table = GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    }

    upgrade_info_t info;                           // -0x284（ctor 自清零）
    const int level = ((const CUserCharacInfo*)user)->get_charac_level();
    if (!_GetUpgradeInfo(&target, script, *table, &info, level))
    {
        return 0xd;
    }

    // 【ORIG 怪癖】payBase/matCount 仅 upCount!=0 时赋值，其余路径沿用旧栈值。
    int payBase;                // -0x250 金钱基数
    int matCount;               // -0x254 材料消耗数量
    int ticketMatIdx;           // -0x258（ORIG 写入后无读者：死写入照抄，P171）
    if (msg->m_25 != 0)
    {
        payBase = 0;
        matCount = 1;
        ticketMatIdx = (int)material.m_addInfo;
    }
    (void)ticketMatIdx;

    if (!_CheckTargetUsableLv(material, target))
    {
        return 0x13;
    }
    const int portable = _ProcPortableUpgradeItem(material, target, info);
    if (portable != 0)
    {
        return portable;
    }

    if (ticket.m_addInfo == 0x28d30e || ticket.m_addInfo == 0x28d30f)
    {
        payBase = 0;
    }

    jar->SettingRecipeNUpConunt(material.m_addInfo, info,
                                target.GetUpgrade());

    if (msg->m_upgradeType == 0 && script->IsImpossibleUpgrade())
    {   // CItem vtable+0x30
        return 0x13;
    }
    if (msg->m_upgradeType == 1 && script->IsImpossibleAmplify())
    {   // CItem vtable+0x34
        return 0x13;
    }

    // 特殊券覆盖材料消耗数（第一处 @0x08547e89，删除前第二次见 @0x085483b4）。
    if (msg->m_ticketSlot != 0xffff)
    {
        Inven_Item ticketAgain;                    // -0x1d8
        ticketAgain =
            invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        // ORIG 各分支直发 movl $imm,-0x254（0x08547f9e/0x08547faa/0x08547fce/
        // 0x08547fda）⇒ if/else 独立赋值，禁三目（三目物化「先备值再 store」
        // 两条，本函数 4 处共 +4 insn）。
        if (ticketAgain.m_addInfo == 0x28d30f)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 0x32;
            }
            else
            {
                matCount = 10;
            }
        }
        else if (ticketAgain.m_addInfo == 0x28d30e)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 100;
            }
            else
            {
                matCount = 20;
            }
        }
    }

    int cond = 0;                                  // -0x3c
    cond = _CheckCondition(user, target, script, material, *table, info);
    if (cond != 0)
    {
        return cond;
    }
    if (msg->m_upgradeType == 0 && target.m_amp.hasAbility())
    {
        return 0x17;
    }
    if (msg->m_upgradeType == 1 && !target.m_amp.hasAbility())
    {
        return 0x17;
    }

    pg->put_byte(1);

    const int oldUp = (int)target.GetUpgrade();    // -0x38（int 槽）
    bool upgraded = false;                         // -0x32
    char winFlag = 0;                              // -0x31

    if (msg->m_25 != 0)
    {
        upgraded = true;
        // ORIG 上界是字面量 99（movl $0x63,-0xd0），非 _DoProcUpgrade 的 +0x4ec。
        unsigned long bound = 99;
        // [b28d-solo-ItemUpgradeVtableB2] P49/P167：同上，直读 m_rand 字段
        // （+0x4e8）消 helper call。
        if (m_rand->randInt(bound) < msg->m_27)
        {
            target.SetUpgrade((unsigned char)msg->m_25);
            winFlag = 1;
        }
        else
        {
            target.SetUpgrade((unsigned char)oldUp);
            winFlag = 0;
        }
    }
    else
    {
        upgraded = _DoProcUpgrade(user, &target, &info);

        int pay = payBase;                         // -0x2c
        // ORIG 无判空防御（0x085481bc 直接解引用返回值，P140）。
        CEventBase* repeat =
            GlobalData::s_event_manager->GetRepeatEvent(0x32);
        if (repeat->IsEventing((CUser*)0) && !target.m_amp.hasAbility())
        {
            // 第二次 GetRepeatEvent（0x0854820c）：ORIG 不缓存返回值（P48）。
            const int rate =
                ((CReduceUpgradeItemPay*)
                    GlobalData::s_event_manager->GetRepeatEvent(0x32))
                    ->GetReduceRate();
            if (rate > 0)
            {
                pay = payBase - (payBase * rate) / 100;
                if (pay < 0)
                {
                    pay = 0;
                }
            }
        }
        else
        {
            pay = payBase;
        }

        // 减费装备来自**右槽**（0x08548ad3 getCurCharacInvenR，type=0/slot=0xb）。
        Inven_Item worn =
            ((const CUserCharacInfo*)user)
                ->getCurCharacInvenR()
                ->GetInvenSlot((INVEN_TYPE)0, 0xb);
        const int wornIdx = (int)worn.m_addInfo;   // -0x28
        CItem* discScript = G_CDataManager()->find_item(wornIdx);
        if (discScript != 0)
        {
            const float discount =
                ((CEquipItem*)discScript)->getUpgradeCostDiscount();
            // _DAT_08c9d304 = -100.0f（objdump flds 立即数直读，P185）。
            pay = (int)((discount * (float)pay) / -100.0f + (float)pay);
        }

        if (target.m_amp.hasAbility())
        {
            if (((CUserCharacInfo*)user)->getCurCharacInvenW()->use_money(
                    pay, (eMoneySubReason)0x2e, true) != 1)
            {
                return 10;
            }
        }
        else
        {
            if (((CUserCharacInfo*)user)->getCurCharacInvenW()->use_money(
                    pay, (eMoneySubReason)0xd, true) != 1)
            {
                return 10;
            }
        }
    }

    // 特殊券覆盖材料消耗数（第二处 @0x085483b4）+ 删券。
    if (msg->m_ticketSlot != 0xffff)
    {
        Inven_Item ticketDel;                      // -0x215
        ticketDel = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        if (ticketDel.m_addInfo == 0x28d30f)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 0x32;
            }
            else
            {
                matCount = 10;
            }
        }
        else if (ticketDel.m_addInfo == 0x28d30e)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 100;
            }
            else
            {
                matCount = 20;
            }
        }
        if (((CUserCharacInfo*)user)->getCurCharacInvenW()->delete_item(
                INVEN_TYPE_EQUIP, msg->m_ticketSlot, 1, (eItemDelReason)3,
                true) != 1)
        {
            return 0x11;
        }
        user->m_historyLog.apply_Item(ticket.m_addInfo, target.m_addInfo,
                                      (eApplyItemReason)0);
    }

    if (((CUserCharacInfo*)user)->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, msg->m_materialSlot, matCount,
            (eItemDelReason)0xc, true) != 1)
    {
        return 0x11;
    }
    if (msg->m_25 != 0)
    {
        user->m_historyLog.apply_Item(material.m_addInfo, target.m_addInfo,
                                      (eApplyItemReason)1);
    }

    GetInstanceValueStatistic()->AddValueStatistic(
        (VALUE_STATISTIC_FIELD)0xe, user, (unsigned int)payBase);

    pg->put_byte((int)msg->m_upgradeType);
    pg->put_short((int)msg->m_materialSlot);
    // 回包的 add_info 取自**材料槽**当前物品，且 ORIG 在此重取 invenW
    // （0x08548ee7 call getCurCharacInvenW → 0x08548f0b GetInvenSlot(1, +0x23)）。
    pg->put_int(((CUserCharacInfo*)user)
                    ->getCurCharacInvenW()
                    ->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_materialSlot)
                    .get_add_info());
    pg->put_short((int)msg->m_ticketSlot);

    if (msg->m_25 == 0)
    {
        if (jar->isNewNPC())
        {
            pg->put_byte(2);
        }
        else if (jar->isFortune())
        {
            pg->put_byte(3);
        }
        else
        {
            pg->put_byte(0);
        }
    }
    else
    {
        pg->put_byte(1);
    }
    pg->put_byte(oldUp);

    if (table->m_bodyView.m_named.m_noticeUpgradeLevel <=
            (int)target.GetUpgrade() &&
        (msg->m_25 == 0 || winFlag != 0))
    {
        _NoticeUpgrade(user, target, upgraded);
    }

    if (target.GetUpgrade() > 12)
    {
        const unsigned int logUpgraded = (unsigned int)upgraded;  // -0x2d4
        const unsigned int logUpgrade =
            (unsigned int)target.GetUpgrade();                    // -0x2d0
        // [b28d-solo-ItemUpgradeVtableB2] §4.3 裸偏移具名化：ORIG 0x0854898c
        // `call G_CEnvironment; mov 0x378(%eax),%eax` —— +0x378 即
        // CEnvironment::m_serverEnvir（+0x198）+ Server_Envir::m_serverGroup
        // （+0x1e0），同一 4 字节 int 装载（CEnvironment.h:290 / :186 已具名）。
        const unsigned int envField =
            (unsigned int)G_CEnvironment()->m_serverEnvir.m_serverGroup;
        DB_ReqItemUpgradeLog::makeRequest(
            envField, target.m_addInfo,
            ((const CUserCharacInfo*)user)->getCurCharacNo(),
            ((const CUserCharacInfo*)user)->getCurCharacName(), logUpgrade,
            logUpgraded != 0);
    }

    if (upgraded)
    {
        if (msg->m_25 == 0)
        {
            pg->put_byte(0);
        }
        else if (winFlag == 0)
        {
            pg->put_byte(1);
        }
        else
        {
            pg->put_byte(0);
        }
        pg->put_byte((int)target.GetUpgrade());
        pg->put_short((int)msg->m_targetSlot);
        if (msg->m_25 == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)0xc, 0,
                (ENUM_PACKETCLASS)0);
        }
    }
    else
    {
        int sellPrice = 0;                         // -0xe4
        G_Store()->GetSellItemPrice(target, script,
                                    (short)target.m_addInfo2, false,
                                    sellPrice);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0xf, user, (unsigned int)sellPrice);

        const int penalty =
            _ProcPenalty(user, &target, script, msg->m_targetSlot, *table,
                         &info, pg, &ticket, ticketFlag);
        if (penalty != 0)
        {
            if (penalty != 0xd8)
            {
                return penalty;
            }
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)msg->m_materialSlot);
            user->SendUpdateItemList((CUser::eSendTarget)1,
                                     (ENUM_ITEMSPACE)0, 0);
            return penalty;
        }
    }

    user->m_historyLog.UpgradeItem(script->get_index(), oldUp,
                                   (int)target.GetUpgrade(), upgraded);

    if (script->IsRandomOption())                  // CItem vtable+0x4c
    {
        user->makeGuildItemUpgradMessage(msg->m_28, oldUp,
                                         (int)target.GetUpgrade(), upgraded);
    }
    else
    {
        user->makeGuildItemUpgradMessage(script->GetItemName(), oldUp,
                                         (int)target.GetUpgrade(), upgraded);
    }

    if (target.m_addInfo != 0)
    {
        invenW->update_item(INVEN_TYPE_EQUIP, msg->m_targetSlot, target);
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    return 0;
}

// ============================================================================
// _getAdditionalDisjointItem @ 0x08546938
//   grade 与表内基准差值达到阈值且强化级足够时，按 rarity 权重浮点计算
//   附加分解物数量并压入 items（{item=table[0x238], count}）。
// [b28d-solo-ItemUpgradeVtableB2] DIFF 108/102 封顶（N85① + N82 + 尾岛）：
//   P167 CLEAR 8/8；已修 grade 内联（%ebx 保活）、max 实参序、clamped>=gap
//   方向（复现 ORIG jb）、unsigned span（复现零扩展 fildll）、去 +0.5（ORIG
//   RC=trunc 纯截断）、make_pair 嵌套形（三连 call）。残余：①weight 的
//   `flds mem; fstps` vs 4.4 mov（volatile 实测反劣 +1）②槽号重排（N82）
//   ③fistpll 后 4.4 DCE 掉 `mov -0x3c,%edx` ④尾部 3 个 jmp+nop 岛（4.1
//   多出口，4.4 jump-thread 合并）。
// ============================================================================
void CItemUpgrade::_getAdditionalDisjointItem(unsigned int up,
                                              const CItem* itemScript,
                                              std::vector<std::pair<int, int> >& items,
                                              const upgrade_table_t& table)
{
    // 【切片转写 0x08546938..0x08546a95】表项全部走具名字段直访（inline
    // 访问器在 -O0 每点额外发 call）；**语义纠错**：pow 的底数是
    // `up - table.m_additionalMinUpgrade`（0x085469db..0x085469f8：先取 +0x258，
    // 与形参 up(0xc(%ebp)) 相减、符号扩展成 long long 后 fildll），
    // 此前误用钳零后的 grade 差。
    // [b28d-solo-ItemUpgradeVtableB2] P0/P48：ORIG 0x08546944 `mov %eax,%ebx`
    // ⇒ grade 无栈槽（值跨 get_rarity 调用由 %ebx 保活）⇒ 不设具名局部，
    // 直接内联进 diff 表达式。
    // 0x0854696f..0x0854698a：`lea -0x24(&diff); lea -0x20(&zero); call max;
    // mov (%eax)` ⇒ ORIG 用 std::max 模板（两实参各取地址，故 zero 必须是
    // 具名 int 局部）；写成 if 钳零会少 6 insn。
    // get_rarity **两次独立调用**（0x0854695a 算 grade 差、0x085469bf 选权重
    // 槽）⇒ 不命名 rarity 局部（P48/P183）。
    int diff =
        itemScript->get_grade() -
        table.m_bodyView.m_named.m_baseGrade[itemScript->get_rarity()];
    int zero = 0;
    // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x0854696f..0x0854698a 压参序：
    // 0x4(%esp)=&diff、(%esp)=&zero ⇒ 调用是 max(zero, diff)（4.4 右起求值
    // 时先取 &diff，与 ORIG 装载序一致；max 可交换，语义等价）。
    const int clamped = std::max(zero, diff);

    // ORIG 0x0854698d..0x085469a2：`mov -0xc,%edx; mov 0x23c(%eax),%eax;
    // cmp %eax,%edx; jb end` ⇒ 源码写 `clamped >= gap`（4.4 对 `a>=b` 发 jb）。
    if ((unsigned int)clamped >=
            (unsigned int)table.m_bodyView.m_named.m_additionalGradeGap &&
        (unsigned int)table.m_bodyView.m_named.m_additionalMinUpgrade < up)
    {
        // ORIG 的 make_pair 实参是 `Rm Rj`（unsigned long& / unsigned int&
        // 左值）⇒ pair 由转换 ctor C2<unsigned long,unsigned int> 落成
        // pair<int,int>（0x08546a5c/0x08546a71 两个 call）⇒ 两量必须是有类型
        // 的具名局部，不能是表达式直传。
        unsigned long itemId =
            (unsigned long)table.m_bodyView.m_named.m_additionalItemId;
        // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x085469bf..0x085469ca 为
        // `flds (%eax,%edx,4); fstps -0x4c(%ebp)`（4.1 的 float 拷贝经 x87），
        // 4.4 恒 mov 位直搬（volatile 形实测反而多 flds+fmulp 一条，且初始化
        // 仍为 mov）⇒ N85① 封顶（跨版本 float 拷贝 lowering 差异）。
        const float weight = table.m_bodyView.m_named
                                 .m_additionalWeight[itemScript->get_rarity()];

        // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x085469db..0x08546a01：
        // `mov 0x258(%eax),%eax; mov 0xc(%ebp),%edx; mov %edx,%ecx;
        //  sub %eax,%ecx; mov %ecx,%eax; mov $0x0,%edx; mov %eax,-0x40;
        //  mov %edx,-0x3c; fildll -0x40` = **unsigned int → double** 的零扩展
        // 转换（32 位 unsigned 不能走 fildl）⇒ 底数类型是 unsigned int，
        // 不是 (long long) 相减（那会出 sbb 链）。
        // ORIG 0x08546a02..0x08546a39：pow 结果直接 `fnstcw/fldcw(RC=trunc)`
        // 后 fistpl（截断，**无 +0.5**）；再 fildl + fmuls weight +
        // fistpll（64 位截断）⇒ 源码是两次 (int)/(long long) C cast。
        // span 无具名槽（ORIG 0x085469d1 用 %ecx/%eax 直传）⇒ 内联表达式。
        const long long scaled =
            (long long)((float)(int)std::pow(
                            (double)(unsigned int)(
                                up - table.m_bodyView.m_named
                                         .m_additionalMinUpgrade),
                            2.0) *
                        weight);
        unsigned int count = (unsigned int)scaled;
        if (count != 0)
        {
            // ORIG 0x08546a4a..0x08546a6e：make_pair<Rm,Rj>（两左值）→
            // pair<int,int> 转换 ctor → push_back(&&) 三连 call（P178 嵌套形；
            // 直写 make_pair 只发两 call）。
            items.push_back(
                std::pair<int, int>(std::make_pair(itemId, count)));
        }
    }
}

// ============================================================================
// _ProcPenalty @ 0x08546bba
//   惩罚分派：1=降级（保底）、2=降级（可到 0）、3=破坏（保护券或分解返料）。
//
// 【batch-16 验收标注（§6.2 口径）】orig 634 / ours 633（gap 41 → 1）。
//   本轮补齐：① 返料三元组遍历由 size()+operator[] 索引形改为 ORIG 的
//   begin/end 迭代器形（恢复 4 个迭代器调用、去掉 3×ix 与 1×size，P175）；
//   ② GetSellItemPrice 的短参改直读 Inven_Item::m_addInfo2（ORIG 无
//   get_add_info() 调用）；③ penaltyType==3 的增幅表改走真实成员调用
//   CItemUpgradeTable::getUpgradeInfoTable（inline 视图代返会丢该 call）。
//   残余 1 insn 与差集里 operator+ ×3 ↔ operator++ ×1 的符号名形差同源
//   （Ghidra 把 libstdc++ 的 operator+(difference_type) 渲染为 operator++），
//   已判语义完成、byte 层封顶。
// [b28d-solo-ItemUpgradeVtableB2] DIFF 634/620 封顶（N82，§6.2 三件套）：
//   ①P167 CLEAR 87/87（本轮补：三元组循环 `*it++`×3 复现 ppEi×3、SetUpgrade
//   两分支各自重调 GetUpgrade、insert_event_items 前重调 getCurCharacInvenW、
//   保护券分支去 invenW 缓存）；②gap 14（2.2%）；③帧 0x180 同、残余为槽号
//   重排与 `info->m_prob20[2]=1` 的 4.1 地址中转（-0x3c 指针槽，4.4 折单条
//   movl）+ bool 物化（setge vs jge/jns）。已修：penaltyType 正极性重排、
//   toZero 改 `A || B` 短路表达式（复现 jge/jns 双跳）。
// ============================================================================
int CItemUpgrade::_ProcPenalty(CUser* user, Inven_Item* item,
                               const CItem* itemScript, unsigned short slot,
                               const upgrade_table_t& table,
                               const upgrade_info_t* info, PacketGuard* pg,
                               Inven_Item* ticketCopy, char ticketFlag)
{
    // [b28d-solo-ItemUpgradeVtableB2] P222/P194：ORIG 0x08546bdb 先测
    // `ticketCopy->m_addInfo != 0`（正极性）再进票分支、`==0` 走 else
    // （0x08546bfd 表查询）；负极性写法令两臂布局互换。票分支内
    // `cmpb ticketFlag; jne` ⇒ if/else 独立赋值（非三目）。
    int penaltyType;
    if (ticketCopy->m_addInfo != 0)
    {
        if (ticketFlag == 0)
        {
            penaltyType = 1;
        }
        else
        {
            penaltyType = 2;
        }
    }
    else
    {
        penaltyType = table.GetPenaltyType(item->GetUpgrade(),
                                           (ENUM_RARITY)itemScript->get_rarity(),
                                           info);
        if (GlobalData::s_pEvent_UpGrade_Jar->isFortune())
        {
            penaltyType = 2;
        }
    }
    pg->put_byte((int)(char)penaltyType);

    if (penaltyType == 1)
    {
        pg->put_byte((int)item->GetUpgrade());
        pg->put_short((int)slot);
        return 0;
    }

    if (penaltyType == 2)
    {
        if (GlobalData::s_pEvent_UpGrade_Jar->isFortune())
        {
            const_cast<upgrade_info_t*>(info)->m_prob20[2] = 1;  // info +0x28
        }
        // [b28d-solo-ItemUpgradeVtableB2] P167/P48：ORIG 0x08546cd9 比较用
        // GetUpgrade（值留 %eax 无槽）、0x08546d22/0x08546d4a 两个 SetUpgrade
        // 分支**各自重新调用** GetUpgrade，且 else 支重读 info->m_prob20[2]
        // ⇒ drop/up 均无具名局部，全部内联表达式（此前缓存 up/drop 少 2 个
        // GetUpgrade 调用点，p167 MISSING -2）。
        // ORIG 0x08546ce1..0x08546cf5：`cmp; jge L1; cmpb ticketFlag;
        // jns L2; L1: mov $1,%eax; jmp; L2: mov $0,%eax; test %al` ⇒ 源码是
        // `A || B` 短路表达式直接初始化 bool（4.1 生 jge/jns 双跳 + mov 岛）。
        bool toZero = (info->m_prob20[2] >= (int)item->GetUpgrade()) ||
                      (ticketFlag < 0);
        if (toZero)
        {
            item->SetUpgrade(0);
        }
        else if (ticketFlag > 0)
        {
            item->SetUpgrade(
                (unsigned char)((int)item->GetUpgrade() - (int)ticketFlag));
        }
        else
        {
            item->SetUpgrade((unsigned char)((int)item->GetUpgrade() -
                                             info->m_prob20[2]));
        }
        pg->put_byte((int)item->GetUpgrade());
        pg->put_short((int)slot);
        return 0;
    }

    // ---- penaltyType == 3：破坏 ----
    if (!item->m_amp.hasAbility())
    {
        // 保护券链：0x28be7e → 0x1f72 → 0x20ba，末位 0x292091ac 优先覆盖。
        int protectSlot =
            ((const CUserCharacInfo*)user)->getCurCharacInvenR()->check_item_exist(0x28be7e);
        int slot172 =
            ((const CUserCharacInfo*)user)->getCurCharacInvenR()->check_item_exist(0x1f72);
        int slot20ba =
            ((const CUserCharacInfo*)user)->getCurCharacInvenR()->check_item_exist(0x20ba);
        protectSlot = protectSlot != -1
                          ? protectSlot
                          : (slot172 != -1 ? slot172
                                           : (slot20ba != -1 ? slot20ba : -1));
        const int slot292 =
            ((const CUserCharacInfo*)user)->getCurCharacInvenR()->check_item_exist(0x292091ac);
        if (slot292 != -1)
        {
            protectSlot = slot292;
        }
        if (protectSlot != -1)
        {
            // [b28d-solo-ItemUpgradeVtableB2] P167/P183：ORIG 0x08546ea3
            // （GetInvenSlot）与 0x08546f10（delete_item）各自重调
            // getCurCharacInvenW，无具名 invenW 缓存（缓存少 1 个调用点）。
            Inven_Item used = ((CUserCharacInfo*)user)
                                  ->getCurCharacInvenW()
                                  ->GetInvenSlot(INVEN_TYPE_EQUIP, protectSlot);
            if (used.m_addInfo != 0x28be7e && used.m_addInfo != 0x1f72 &&
                used.m_addInfo != 0x20ba && used.m_addInfo != 0x292091ac)
            {
                return 0x11;
            }
            if (((CUserCharacInfo*)user)
                    ->getCurCharacInvenW()
                    ->delete_item(INVEN_TYPE_EQUIP, protectSlot, 1,
                                  eItemDelReason_3, true) != 1)
            {
                return 0x11;
            }
            user->m_historyLog.apply_Item(used.m_addInfo, item->m_addInfo,
                                          eApplyItemReason_0);
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 protectSlot);
            item->SetUpgrade(0);
            ((CUserCharacInfo*)user)->getCurCharacInvenW()->update_item(
                INVEN_TYPE_EQUIP, slot, *item);
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)slot);
            return 0xd8;
        }
    }

    CInventory* invenW = ((CUserCharacInfo*)user)->getCurCharacInvenW();
    if (invenW->delete_item(INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_0xd,
                            true) != 1)
    {
        return 0x11;
    }
    const unsigned char savedUp = item->GetUpgrade();
    item->SetUpgrade(0);
    pg->put_byte(0);
    pg->put_short((int)slot);

    DisJointResult result;
    if (G_CDataManager()->m_disJoint->GetResult(item->m_addInfo, result) != 0)
    {
        pg->put_byte(0);
        return 0;
    }
    item->m_addInfo = 0;

    std::vector<std::pair<int, int> > items;
    items.reserve(6);
    for (int i = 0; i <= 4; ++i)
    {
        if (result.m_f[i + 4] != 0)
        {
            items.push_back(std::make_pair(result.m_f[i + 5], result.m_f[i]));
        }
    }
    _getAdditionalDisjointItem(savedUp, itemScript, items, table);
    if (item->m_amp.hasAbility())
    {
        // 表区子对象走**真实成员调用**（0x0854b382
        // CItemUpgradeTable::getUpgradeInfoTable）；inline 视图代返会丢该 call
        // （P167 差集实证 missing 1）。
        _getAdditionalDisjointItem(savedUp, itemScript, items,
                                   *getUpgradeInfoTable());
    }

    std::vector<int> slots;
    // ORIG 尾参 a/b 为 .rodata 空串 0x08c9c3a0（邮件标题/正文查找键）。
    static const char kEmptyKey[1] = "";
    // [b28d-solo-ItemUpgradeVtableB2] P167：ORIG 0x08547295 在
    // insert_event_items 前**重调** getCurCharacInvenW（不复用上面的 invenW）。
    const int rc = ((CUserCharacInfo*)user)->getCurCharacInvenW()
                       ->insert_event_items(items, slots, (eMoneyAddReason)7,
                                            eItemAddReason_0x13, false,
                                            kEmptyKey, kEmptyKey);
    pg->put_byte(rc);

    // insert_event_items 输出三元组 {slot, itemIdx, addInfo2/count}。
    // 【batch-16 P175 反向铁律】ORIG 0x085472xx 起走 **begin/end 迭代器**
    // （vector::begin、normal_iterator<int const*>(int*) 转换 ctor、每轮重取
    // end()、operator!=、operator++ ×3、operator*），我们写成 size()+operator[]
    // 索引形会丢这 4 个迭代器调用并多出 3×ix 与 1×size（P167 差集实测
    // missing 23 / extra 13）。三元组取数用 `*(it + n)` 形复现 3 次 operator++。
    // [b28d-solo-ItemUpgradeVtableB2] P167/P28：ORIG 每轮调 **ppEi ×3**
    // （MISSING -3 `normal_iterator::ppEi`、EXTRA +3 `pl`）⇒ 源码是三次
    // 后置 `*it++` 取三元组，不是 `it += 3` + `*(it+n)`。
    for (std::vector<int>::const_iterator it = slots.begin();
         it != slots.end(); )
    {
        const int outSlot = *it++;
        const int outIdx = *it++;
        const int outCnt = *it++;
        pg->put_short(outSlot);
        pg->put_item_idx((unsigned long)outIdx);
        pg->put_int(outCnt);

        CItem* outScript = G_CDataManager()->find_item(outIdx);
        if (outScript != 0)
        {
            Inven_Item tmp;
            tmp.m_addInfo = outIdx;
            tmp.set_add_info(outCnt);
            outScript->make_item(tmp);
            GetInstanceCubeStatistic()->collectCubeStatistics(
                outScript, outCnt, user, (CUBE_STATISTIC_FIELD)1);
            int price = 0;
            // ORIG 0x085474xx 直读 Inven_Item+0x07 低 16 位（`movzwl`/`movswl`），
            // 不走 get_add_info() 调用。
            G_Store()->GetSellItemPrice(tmp, outScript,
                                    (short)tmp.m_addInfo2, false,
                                        price);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)9, user, (unsigned int)price);
        }
    }
    return 0;
}

}  // namespace WongWork

// ============================================================================
// CEquipItem::getUpgradeProbIncrease @ 0x0854b552（W，14 字节访问器）
//   return *(int*)((char*)this + 0x1d0)（CEquipItem::m_field1d0）。
// ============================================================================
int CEquipItem::getUpgradeProbIncrease() const
{
    return m_field1d0;
}
