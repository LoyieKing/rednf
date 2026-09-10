// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8_CU_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// W8_CU_A.cpp —— CUser Wave8 CUserOnlyOrig 残余批 A（抽奖/时序判定族，7 符号）
//
// 来源：docs/class_func_reports/CUser/{IsOldLotteryItem,lottery_item,
//       multibox_lottery_item}.md（## 汇编 + ## 反编译 C 全文）
//       + objdump 直转：
//         CUser::IsOldLotteryItem      T 0x08673342 (0x60)
//         CUser::inc_multiboxLotteryItemFailCnt W 0x080fdf56 (0x23)
//         CUser::SellItemTimeChecker   W 0x08230256 (0x55)
//         CUser::GetItemTimeChecker    W 0x082302ac (0x1f0)
//         CUser::GetItemSerialChecker  W 0x0823066a (0x14f)
//         CUser::lottery_item          T 0x086733a2 (0x15c6)
//         CUser::multibox_lottery_item T 0x08674968 (0x115d)
//
// NOTE(NEEDS_FIX, owner=CDataManager 域批次)：CDataManager::find_lottery_use_cost
// 当前声明/实现返回 bool（isNeedMoney 转发），ORIG mangled
// _ZNK12CDataManager21find_lottery_use_costEj 返回 int（花费数值——返回类型不编码
// 于 mangled）。本 TU 按正确语义以 int 使用其结果；宿主把签名改回 int 后无需再动
// 本文件（bool→int 整型提升编译可通过，值路径待修复）。
//
// NOTE(NEEDS_HEADER, owner=宿主裁决)：CParty.h 以 char 占位承载 +0xb24

// CBattle_Field(0xcd0) 子对象区（见 CParty.h 注释），ORIG ctor 0x082a5516 已实证。
// GetItemSerialChecker 的 party 分支需要读取该子对象的 m_visitedGotoFlag/
// m_hellMapIdx/get_cur_map()。本 TU 采用仓库既有成熟范式（DisPatcherCore.cpp:3333、
// ExpertJobOnlyMgr.cpp:86 同型 reinterpret_cast 视图）；待宿主把 CParty.h 战场区
// 正式嵌入 CBattle_Field 后统一 cutover 为具名子对象访问。
//
// 本 TU 依赖符号（owner 注记）：
//   CInGameAdvertisementManager::CheckFailMultiBoxLottery(CUser*,
//          std::vector<std::pair<Inven_Item,bool>>&)  ORIG T 0x080fcb3e：
//     定义在本 TU（见文件末尾）；ORIG 返回 void（0x080fcb3e 尾无 mov $0,%eax、
//     Ghidra void），权威头 CInGameAdvertisementManager.h:88 声明 bool —— 已上报
//     Main 待头修正（消费点 CUserOnlyOrig.cpp:4364 为注释、本 TU 调用忽略返回值）。
//   CInGameAdvertisementManager* InGameADManagerInstance()
//     ORIG T _Z23InGameADManagerInstancev @0x080fc9e9，owner 同上。
//   （二者的类最小视图在本 TU 末尾声明区给出，仅供指针上成员调用，
//     不产生任何静态数据/定义，无 ODR 风险。）
// ============================================================================

#include <cstring>

#include "CUser.h"
#include "CMCAPManager.h"
#include "CBoosterGage.h"
#include "CAvatarItemMgr.h"
#include "CCeraShop.h"
#include "CGenUniqueNo.h"
#include "Inven_Item.h"
#include "CItemList.h"
#include "CInventory.h"
#include "CItemGenerator.h"       // random_option::CRandomOptionItemHandle
#include "GamePODTypes.h"         // stAvatarExpansionInfo_t
#include "CSystemTime.h"
#include "GlobalData.h"           // s_pCeraShop / g_randomOptionHandle / s_systemTime_
#include "CBattle_Field.h"
#include "CParty.h"
// ORIG 全局空槽物品（B 0x943ddc0；唯一 owner 定义于 CAccountCargo.cpp）
extern Inven_Item g_emptySlot;
#include "GameWorld.h"

// ---------------------------------------------------------------------------
// 最小外部声明区（owner=W8_LongTail.LT08_AdvertisementMisc；本 TU 无定义）
// ---------------------------------------------------------------------------
class CInGameAdvertisementManager
{
public:
    void CheckFailMultiBoxLottery(
        CUser* user,
        std::vector<std::pair<Inven_Item, bool> >& rewardList);
};
extern CInGameAdvertisementManager* InGameADManagerInstance();

// ===========================================================================
// 1) CUser::IsOldLotteryItem(unsigned int) @0x08673342 —— 报告全文直转
// ===========================================================================
#include "W8_CU_A/IsOldLotteryItem.inc"
{
    // b28d-solo-W8_CU_A：N 封顶（N7/N18a 寄存器化 + 跨版本块布局；原 b28d-35
    //   P85/P209 判词同族）。ORIG 0x08673342：形参一次载入 %eax 后全程
    //   `cmp $imm,%eax` 复用，三处失败路径 tail-merge 到单一 mov $0 尾块
    //   (0x59)、成功汇 0x52。4.4.7 恒「形参槽内存直比 + 失败块就地散布」。
    //   本轮五形实测（探针 /tmp/probe_ol.cpp）：①当前嵌套 if 37 条；②register
    //   局部 37 条（值落 %ebx+push/pop，ORIG 为 %eax 无 push）；③显式 early
    //   return 36 条；④goto 尾标签 45 条（4.4 在 goto 岛补 nop+jmp）——四形
    //   mnemonic 序列均与 ORIG 31 条不符，语义/边界全等 ⇒ 封顶。
    // [round2-L30 复核·N18b（Main 反误判规则 2）] widthscan `cmp_l orig=0 our=6`：
    //   ORIG 实为**两段寄存器比较**（`mov 0xc(%ebp),%eax; cmp $0x1d56,%eax; ja`，
    //   形参一次载入 %eax 全程复用；我方 4.4 退化为 `cmpl $imm,0xc(%ebp)` 槽直比）
    //   ⇒ 判 N18b，非类型宽度问题（不改类型）。
    // [E04 2026-09-10 实验·回退] 失败出口合并为单一尾标签（P325 变体）：实测
    //   36 条（较原 37 少 1）但生成 `nop; jmp` 中继岛、形参仍未入寄存器 ⇒ 仍 DIFF
    //   且远离 ORIG 31 条，已回退。ORIG 的 tail-merge 共享出口（0x52/0x59）+
    //   形参一次载入 %eax 属 4.1 前端产物，4.4 不可复现 ⇒ N18b 维持。
    if (itemIdx < 0x1d57u) {
        if (itemIdx < 0x1d4cu) {
            if (itemIdx < 0x1bc6u) {
                return false;
            }
            if ((itemIdx > 0x1bf5u) && ((itemIdx - 0x1c15u) > 4u)) {
                return false;
            }
        }
    } else if (itemIdx < 0x1e5du) {
        if ((itemIdx < 0x1e44u) && ((itemIdx - 0x1dcau) > 5u)) {
            return false;
        }
    } else if ((itemIdx - 0x1ebau) > 1u) {
        return false;
    }
    return true;
}


// ===========================================================================
// 3) CUser::SellItemTimeChecker(unsigned int nowSec, unsigned int interval)
//    @0x08230256。字段：m_mcap.m_sellTimeChkBaseSec(+0x3c)/
//    m_mcap.m_sellTimeHitCount(+0x40)（本批于 CMCAPManager.h 拆 pad 具名化，
//    已广播默许）。
// ===========================================================================
#include "W8_CU_A/SellItemTimeChecker.inc"
{
    // ORIG 0x08230256：条件 = `(nowSec - m_sellTimeChkBaseSec) > interval`
    // （nowSec→edx→ecx 中转 sub %eax,%ecx，cmp interval/ja）——重置块后置
    // （ja 直跳），++ 块 fall + jmp 公共出口；重置块内先 hitCount=0 后 base=nowSec。
    // b28d-35：反条件 if/else 形（++ 为 then 直落、reset 为 else 后置）复现 ORIG
    //   25 条无对齐 nop 形；goto 形在 4.4 于 ja 目标处多发一条对齐 nop
    //   （探针 /tmp/st_probe2.cpp fE 实证：25 条逐条全等）。
    if ((nowSec - m_mcap.m_sellTimeChkBaseSec) <= interval)
    {
        ++m_mcap.m_sellTimeHitCount;
    }
    else
    {
        // 超窗重置：基准秒=本次时间、计数清零
        m_mcap.m_sellTimeHitCount = 0;
        m_mcap.m_sellTimeChkBaseSec = nowSec;
    }
    return static_cast<unsigned int>(m_mcap.m_sellTimeHitCount);
}

// ===========================================================================
// 3b) CUser::inc_multiboxLotteryItemFailCnt() @0x080fdf56
// ===========================================================================
#include "W8_CU_A/inc_multiboxLotteryItemFailCnt.inc"
{
    m_multiboxLotteryFailCnt += 1;
    return m_multiboxLotteryFailCnt;
}

// ===========================================================================
// 4) CUser::GetItemTimeChecker(unsigned int nowSec, unsigned short val1,
//                              unsigned short val2, unsigned int rangeSec)
//    @0x082302ac。双通道时序差窗（hack 时序检测）：
//    超 rangeSec → 重置；窗口内更新 min/max 并写 |max-min| 绝对值到 diff 字段。
// ===========================================================================
#include "W8_CU_A/GetItemTimeChecker.inc"
                              unsigned short val2, unsigned int rangeSec)
{
    // b28d-solo-W8_CU_A：IDENTICAL 复原（原 b28d-35 N82/P182 判词作废）。
    //   ORIG @0x082302ac 帧 0x8、条件 `ja` 落 reset 块（窗口内逻辑 fall-through）。
    //   三处修法：① 条件极性反写（`<= rangeSec` 窗口在前 / reset 后置）复现
    //   `cmp 0x18(%ebp),%eax; ja <reset>`；② 删具名 int a/b 局部（帧 0x18→0x8）
    //   改为「先写 diff 字段（16 位 sub）再对字段本身 abs 写回」——
    //   ORIG 0x08230398/0x08230409 两段 `movzwl 字段;cwtl;sar;xor;sub` 即此形；
    //   ③ 真语义 bug 修正：原「MaxB 用 min 判据 / MinB 用 max 判据」两条互换了
    //   字段（B 通道 min/max 语义反转）；按 ORIG 0x0823031a/0x08230344 恢复为
    //   MinA,MinB,MaxA,MaxB 顺序（reset 块同序，ORIG 0x08230459..0x0823048a）。
    if ((nowSec - m_mcap.m_timeChkBaseSec) <= rangeSec) {
        ++m_mcap.m_timeChkCount;
        if ((static_cast<int>(m_mcap.m_timeChkMinA) - (short)val1) > 0) {
            m_mcap.m_timeChkMinA_u = val1;
        }
        if ((static_cast<int>(m_mcap.m_timeChkMinB) - (short)val2) > 0) {
            m_mcap.m_timeChkMinB_u = val2;
        }
        if ((static_cast<int>(m_mcap.m_timeChkMaxA) - (short)val1) < 0) {
            m_mcap.m_timeChkMaxA_u = val1;
        }
        if ((static_cast<int>(m_mcap.m_timeChkMaxB) - (short)val2) < 0) {
            m_mcap.m_timeChkMaxB_u = val2;
        }

        m_mcap.m_timeChkDiffA = static_cast<short>(
            static_cast<short>(m_mcap.m_timeChkMaxA_u) -
            static_cast<short>(m_mcap.m_timeChkMinA_u));
        m_mcap.m_timeChkDiffB = static_cast<short>(
            static_cast<short>(m_mcap.m_timeChkMaxB_u) -
            static_cast<short>(m_mcap.m_timeChkMinB_u));
        m_mcap.m_timeChkDiffA = static_cast<short>(
            m_mcap.m_timeChkDiffA < 0 ? -m_mcap.m_timeChkDiffA
                                      : m_mcap.m_timeChkDiffA);
        m_mcap.m_timeChkDiffB = static_cast<short>(
            m_mcap.m_timeChkDiffB < 0 ? -m_mcap.m_timeChkDiffB
                                      : m_mcap.m_timeChkDiffB);
    } else {
        // 重置分支（ORIG 写序：diff 清零 → 计数清零 → 基准秒与四端点）
        m_mcap.m_timeChkDiffA = 0;
        m_mcap.m_timeChkDiffB = 0;
        m_mcap.m_timeChkCount = 0;
        m_mcap.m_timeChkBaseSec = nowSec;
        m_mcap.m_timeChkMinA_u = val1;
        m_mcap.m_timeChkMinB_u = val2;
        m_mcap.m_timeChkMaxA_u = val1;
        m_mcap.m_timeChkMaxB_u = val2;
    }
    return m_mcap.m_timeChkCount;
}

// ===========================================================================
// 5) CUser::GetItemSerialChecker(unsigned short serialIdx,
//                                unsigned short serialVal, int monsterIdx)
//    @0x0823066a。序列号差窗连续命中计数：idx 差∈[-23,23] 且 val 差∈[-17,17]
//    才保持计数；越窗重置。命中条件 = party 存在且 party->isMonsterDrop_item
//    且战场未做 visited-goto 校验（bf.m_visitedGotoFlag==0 或
//    bf.m_hellMapIdx != bf.get_cur_map()）。
// ===========================================================================
#include "W8_CU_A/GetItemSerialChecker.inc"
                                int monsterIdx)
{
    // b28d-solo-W8_CU_A：AE 复原（原 b28d-35「N82 帧/槽 + bool 物化」判词作废）。
    //   ORIG @0x0823066a 帧 0x34（与本重建同）；三处修法：
    //   ① 删具名 bool inWindow（4.4 恒物化 `movb %al,-0x12; movzbl; xor $1; test`
    //   多 4 条）⇒ 四条件直写 if 头，失败路径即 ORIG 的 `jl/jg <reset 尾块>`；
    //   ② 删具名 bool hit 与 `CBattle_Field& bf` 引用（引用强制 `add $0xb24`
    //   具名槽）⇒ 经 p->m_battleField 直接访问，复现 ORIG 0x0820729 段
    //   `movzbl 0xcec(%eax)`/`mov 0xce0(%eax),%ebx` disp 折叠；
    //   ③ if/else 结构（reset 为 else 后置）= ORIG 0x082077e 尾块布局。
    if (((int)(short)serialIdx - (int)m_mcap.m_serialLastIdx) >= -23 &&
        ((int)(short)serialIdx - (int)m_mcap.m_serialLastIdx) <= 23 &&
        ((int)(short)serialVal - (int)m_mcap.m_serialLastVal) >= -17 &&
        ((int)(short)serialVal - (int)m_mcap.m_serialLastVal) <= 17) {
        CParty* p = static_cast<CParty*>(GetParty());
        if (p != NULL && p->isMonsterDrop_item(monsterIdx) &&
            (!p->m_battleField.m_visitedGotoFlag ||
             p->m_battleField.m_hellMapIdx !=
                 p->m_battleField.get_cur_map())) {
            ++m_mcap.m_serialHitCount;
        }
    } else {
        // 重置：命中计数清零、记录本次基准对
        m_mcap.m_serialHitCount = 0;
        m_mcap.m_serialLastIdx_u = serialIdx;
        m_mcap.m_serialLastVal_u = serialVal;
    }
    return m_mcap.m_serialHitCount;
}

// ===========================================================================
// 6) CUser::lottery_item(int slot) @0x086733a2 —— 使用槽上 lottery 物品开奖。
//    金币奖励 / avatar / 生物蛋 / 强化三档概率（ServerParameterScript 内嵌表）/ 
//    useCost 扣款通知 / 主响应包 / 全服强化播报。全程照报告转写。
//    §6.2 巨件核销：P167 200/200 [0缺0多]；gap 1315→1330；残差归因 N 类
//    （N82 全帧槽平移 0x18/0x10 + 槽复用差、N-88/setne 物化假象（ORIG
//    cmpl/setne/test vs 4.4 cmp 直跳，P197④ 探针同型）、N22 比较方向反演
//    （get_rand_int 三段 seta↔setb 同语义折向）、bool 聚合变量槽物化
//    （movb/-0x51 槽 vs ORIG 寄存器 test）、short 形参槽中转（movswl 经
//    -0x22 中转）——语义与调用序列逐块比对零缺口，byte 层 N 封顶。
// ===========================================================================
// [round2-L30 复核·N95/N82] shift.py: frame ORIG=780 OURS=780，MIXED（相对序差，含 8-dword
//   结构拷贝展开位）⇒ 大件未改造，判 N；ORIG 0x086733a2（0x15c6）。
// [E04 2026-09-10 复核] p167 200/200 CLEAR（差集全为 C1/C2·D1/D2 同址别名 P93）⇒ 无真缺口；
//   gap +15 维持 N82/N95 槽序族。
// [F2a-C05② 复测·不适用] 件级 F2 旋钮逐条核对报告 5 处 ORIG `cmp -0xNN(%ebp),%eax`：
//   ① ORIG 0x086735... `cmp -0x40(%ebp),%eax; jle`（(prev+delta) vs 局部）我方对应点
//      已是同形 mem 直比（`cmp -0x4c(%ebp),%eax; setg`，仅槽号整体差 0xc，属 N82）；
//   ② `cmp -0x44(%ebp),%eax`（ORIG 0x086736xx）与 `cmp -0x30(%ebp),%eax; seta`
//      （ORIG 0x08673xx）的 mem 操作数是 **call 返回值局部**（gain_money / get_rand_int
//      的结果槽）⇒ C05② 边界（跨调用返回值不适用），且后者为无符号序 seta；
//   ③ 无一处属「两个具名局部比较」命中形 ⇒ 维持 N82 封顶，无改动、无回滚。
#include "W8_CU_A/lottery_item.inc"
{
    // b28d-solo-W8_CU_A：N 封顶（§6.2 巨件口径；原 b28d-35 判词沿用并复验）。
    //   ORIG @0x086733a2 size 0x15c6；P167 调用点差集 200/200 CLEAR（本轮复验）；
    //   帧 0x30c 与 ORIG 相同，但 4.4 槽分配整体浅移 0x18（resultSlotNo -0x74
    //   vs ORIG -0x5c、lotto -0x151 vs -0x139、curItem -0x18e vs -0x176）——
    //   属 N82/N95 槽序族；残余另含 setne 物化（N-88）与 N22 比较折向。
    int resultSlotNo = 0;                                        // local_60
    Inven_Item lotto;                                            // local_13d
    Inven_Item curItem;                                          // local_17a
    curItem = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    unsigned int ticketId = static_cast<unsigned int>(curItem.m_addInfo);

    CItem* costItem = G_CDataManager()->find_item(ticketId);     // local_58
    if (costItem == NULL) {
        return 0x15;
    }
    if (costItem->getUsableLevel() > get_charac_level()) {
        return 0xe;
    }

    // NOTE(NEEDS_FIX)：find_lottery_use_cost 见文件头注记
    int useCost = G_CDataManager()->find_lottery_use_cost(ticketId);  // local_50
    if (useCost != 0 && getCurCharacInvenR()->get_money() < useCost) {
        return 10;
    }

    G_CDataManager()->m_itemList->get_lotto_item(
        ticketId, reinterpret_cast<char*>(&lotto));

    // ---- 金币奖励分支：结果项无 id 且金额>0 ----
    if (lotto.m_addInfo == 0 && lotto.m_addInfo2 > 0) {
        int moneyGain = lotto.m_addInfo2;                        // local_44
        int limit = G_CDataManager()->GetMoneyLimitPerLevel(
            get_charac_level(), get_acc_name());                 // local_40
        if (getCurCharacInvenR()->get_money() + moneyGain > limit) {
            unsigned long charNo = getCurCharacNo();
            const char* accIdStr = NumberToString(get_acc_id(), 0);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x61b9, 4);
            trace("lottery_gold_item:m_id(%s), char_no(%d), money(%d)",
                  accIdStr, charNo, moneyGain);
            return 0x16;
        }
        int gained = getCurCharacInvenW()->gain_money(
            moneyGain, static_cast<eMoneyAddReason>(0x17), true, 0);  // local_3c
        if (gained < moneyGain) {
            SendMoneyFullReason(static_cast<ENUM_MONEY_FULL_REASON>(0),
                                static_cast<unsigned long>(moneyGain),
                                static_cast<unsigned long>(gained));
        }
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                               eItemDelReason_3, true)) {
            return 0x11;
        }
        PacketGuard out;                                         // local_bc
        out.put_header(1, 0x1d);
        out.put_byte(1);
        out.put_short(slot);
        out.put_short(resultSlotNo);
        out.put_int(0);
        out.put_int(gained);
        out.put_short(0);
        out.put_byte(0);
        out.put_byte(0);
        out.put_short(0);
        out.put_packet(g_emptySlot);
        out.finalize(true);
        Send(out);
        send_broadcast_lottery_gold_message(static_cast<unsigned long>(ticketId),
                                            moneyGain);
        return 0;
    }

    // ---- 结果道具抽取强化概率（costTicket 可堆叠且类型 0xd 时进入；
    //      SPS 字段 dm+0x5xx ↔ m_serverParameter(基 0x68) 相对偏移映射见头注）----
    unsigned int resultId = static_cast<unsigned int>(lotto.m_addInfo);
    CItem* costTicket = G_CDataManager()->find_item(ticketId);   // local_54(栈-0x54)
    CItem* resItem = G_CDataManager()->find_item(resultId);      // local_50(栈-0x50)

    // ORIG 条件精写（报告 0x051c..0x05d2）：
    if (costTicket != NULL && resItem != NULL) {
        bool condStack = costTicket->is_stackable() &&
                         costTicket->GetItemType() == 0xd;
        if (condStack) {
            bool upgradeGate =
                (lotto.m_field1 == '\x01') &&
                (resItem->GetItemType() != 0xb);
            if (upgradeGate) {
                int up = 0;                                      // local_34
                int roll = get_rand_int(10000);                  // local_30
                if (static_cast<unsigned int>(roll) <
                    static_cast<unsigned int>(
                        G_CDataManager()->m_serverParameter.m_508[10])) {
                    up = G_CDataManager()->m_serverParameter.m_508[8] +
                         get_rand_int(
                             G_CDataManager()->m_serverParameter.m_508[11]) /
                         1000;
                } else if (static_cast<unsigned int>(roll) <
                           static_cast<unsigned int>(
                               G_CDataManager()->m_serverParameter.m_508[14] +
                               G_CDataManager()->m_serverParameter.m_508[10])) {
                    up = G_CDataManager()->m_serverParameter.m_508[12] +
                         get_rand_int(
                             G_CDataManager()->m_serverParameter.m_508[15]) /
                         1000;
                } else {
                    up = G_CDataManager()->m_serverParameter.m_508[16] +
                         get_rand_int(
                             G_CDataManager()->m_serverParameter.m_508[19]) /
                         1000;
                }
                if (up < 0 || up > 0x1f) {
                    up = 0;
                }
                lotto.SetUpgrade(static_cast<unsigned char>(up));
            }
        }
    }

    if (resultId == 0u || resultId == 0xffffffffu) {
        return 0x11;
    }

    // ---- 插入容量预检 ----
    if (getCurCharacInvenR()->QuickEmptyCount() <= 0) {
        if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE_EQUIP,
                                                     1)) {
            return 4;
        }
        if (curItem.get_add_info() >= 2) {
            if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE_2,
                                                         1)) {
                return 4;
            }
        }
        if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE_3,
                                                     1)) {
            return 4;
        }
        if (!getCurCharacInvenR()->check_empty_count(Inven_Item::ITEM_TYPE_10,
                                                     1)) {
            return 4;
        }
    }

    // ---- 宠物罐子票（0x28bfa1）：读 4 个罐子槽，任一强化等级 >9 拒绝 ----
    if (ticketId == 0x28bfa1u) {
        Inven_Item jar1;
        getCurCharacInvenW()->GetInvenData(0x28bf9d, jar1);
        Inven_Item jar2;
        getCurCharacInvenW()->GetInvenData(0x28bf9e, jar2);
        Inven_Item jar3;
        getCurCharacInvenW()->GetInvenData(0x28bf9f, jar3);
        Inven_Item jar4;
        getCurCharacInvenW()->GetInvenData(0x28bfa0, jar4);
        if (jar1.m_addInfo2 > 9 || jar2.m_addInfo2 > 9 ||
            jar3.m_addInfo2 > 9 || jar4.m_addInfo2 > 9) {
            return 4;
        }
    }

    // ---- 消耗票（id != 0x28becb 即刻删除；否则延后） ----
    if (ticketId != 0x28becbu) {
        if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                               eItemDelReason_3, true)) {
            return 0x11;
        }
    }

    int remainOrAdd = 0;                                         // local_2c

    if (resItem != NULL && resItem->IsAvatarItem()) {
        // ---- avatar 分支 ----
        char ipgBuf[24];                                         // local_e0
        const char* ipg = WongWork::CGenUniqueNo::genIPGNo(
            WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
            static_cast<unsigned>(getCurCharacNo()), ipgBuf);
        resultSlotNo = getCurCharacInvenW()->AddAvatarItem(
            static_cast<int>(resultId), resItem->getUsablePeriod(), false,
            '\0', static_cast<char>(-1), ipg,
            static_cast<eAvatarItemAddReason>(7), false, 0);
        if (resultSlotNo == -1) {
            return 4;
        }
        int nowSec = GlobalData::s_systemTime_.getCurSec();      // local_24
        int avatarUid = getCurCharacInvenR()->GetInvenSlot(
            INVEN_TYPE_AVATAR, resultSlotNo).m_addInfo2;
        remainOrAdd = getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
            avatarUid, nowSec);
        lotto.SetUpgrade(0);
    } else if (resItem != NULL && resItem->IsCreatureItem() &&
               resItem->GetItemType() == 0x16) {
        // ---- 生物蛋分支 ----
        if (lotto.m_field1 != '\x05') {
            return 0x11;
        }
        if (user_creature::CCreatureItemConverter::SetEggItem(&lotto) != 1) {
            return 0x11;
        }
        resultSlotNo = getCurCharacInvenW()->insertItemIntoInventory(
            lotto, static_cast<eItemAddReason>(3), true, false);
        if (resultSlotNo < 0) {
            return 4;
        }
        getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
            &lotto, resultSlotNo, 0x19, resItem->getUsablePeriod(),
            resItem->getExpirationDate());
    } else {
        // ---- 普通道具（含随机选项生成）----
        if (resItem != NULL && resItem->IsRandomOption()) {
            if (!GlobalData::g_randomOptionHandle->give_option(
                    resultId,
                    static_cast<ENUM_RARITY>(resItem->get_rarity()),
                    resItem->getUsableLevel(), resItem->getItemGroupName(),
                    resItem->GetRandomOptionGrade(), &lotto.m_random.m_random)) {
                return 0x11;
            }
        }
        resultSlotNo = getCurCharacInvenW()->insertItemIntoInventory(
            lotto, static_cast<eItemAddReason>(3), true, true);
        if (resultSlotNo == -1) {
            return 4;
        }
        if (ticketId == 0x28becbu) {
            if (!getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                                   eItemDelReason_3, true)) {
                return 0x11;
            }
        }
        remainOrAdd = lotto.get_add_info();

        if (resItem != NULL) {
            int sellPrice = 0;                                   // local_e4
            short count = static_cast<short>(lotto.m_addInfo2);
            G_Store()->GetSellItemPrice(lotto, resItem, count, false,
                                        sellPrice);
            GetInstanceValueStatistic()->AddValueStatistic(
                static_cast<VALUE_STATISTIC_FIELD>(7), this,
                static_cast<unsigned int>(sellPrice));
        }
    }

    // ---- 老 lottery 物品清行动点 ----
    if (IsOldLotteryItem(ticketId)) {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, static_cast<APSystem::_ActionGroupIndex>(4), 0,
            static_cast<ENUM_PACKETCLASS>(0));
    }

    // ---- useCost 二次扣款与货币更新包 ----
    useCost = G_CDataManager()->find_lottery_use_cost(ticketId);
    if (useCost != 0) {
        getCurCharacInvenW()->use_money(
            useCost, static_cast<eMoneySubReason>(0x2d), true);
        PacketGuard notify;                                      // local_f0
        notify.clear();
        notify.put_header(0, 0xe);
        notify.put_byte(0);
        notify.put_short(1);
        getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_EQUIP, 0, notify);
        notify.finalize(true);
        Send(notify);
    }

    // ---- 主响应包 ----
    PacketGuard resp;                                            // local_c8
    resp.put_header(1, 0x1d);
    resp.put_byte(1);
    resp.put_short(slot);
    resp.put_short(resultSlotNo);
    resp.put_int(static_cast<int>(resultId));
    resp.put_int(remainOrAdd);
    resp.put_short(static_cast<unsigned short>(lotto.m_fieldb));
    resp.put_byte(lotto.GetItemAttr());
    resp.put_byte(lotto.m_amp.getAbilityType());
    resp.put_short(lotto.m_amp.getAbilityValue());
    if (lotto.isAvatarItemType()) {
        resp.put_int(0x1e);
        resp.put_binary(getCurCharacInvenR()->GetAvatarItemMgrR()
                            ->getJewelSocketData(
                                static_cast<int>(lotto.m_addInfo2)),
                        0x1e);
        resp.put_int(4);
        const char* expansion = static_cast<const char*>(
            getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                static_cast<int>(lotto.m_addInfo2)));
        if (expansion != NULL) {
            resp.put_binary(expansion, 4);
        } else {
            stAvatarExpansionInfo_t fresh;
            fresh.init();
            resp.put_binary(reinterpret_cast<const char*>(&fresh), 4);
        }
    }
    resp.put_packet(lotto);
    resp.finalize(true);
    Send(resp);

    if (lotto.GetUpgrade() != 0) {
        m_historyLog.UpgradeItem(static_cast<int>(resultId), 0,
                                 lotto.GetUpgrade(), true);
    }

    // ---- 强化成功全服播报（含稀度上限豁免表）----
    bool eligibleToShout = false;
    if (costTicket != NULL && resItem != NULL &&
        costTicket->is_stackable() && costTicket->GetItemType() == 0xd) {
        if (G_CDataManager()->m_serverParameter.m_558 <= resItem->get_rarity()) {
            eligibleToShout = true;                       // 高稀度直接播报
        } else if (resItem->IsAvatarItem() &&
                   G_CDataManager()->m_serverParameter
                       .m_55c[resItem->GetItemType()] != 0) {
            eligibleToShout = true;                       // 表命中 avatar 特例
        } else {
            eligibleToShout =
                (G_CDataManager()->m_serverParameter.m_578 <=
                 static_cast<int>(lotto.GetUpgrade()));  // 强化等级门槛
        }
    }

    if (eligibleToShout) {
        bool skipByPlace =
            (getMoveSpace() == static_cast<UserSpace::T>(1)) ||
            (resItem != NULL && resItem->IsRandomOption());
        if (!skipByPlace) {
            PacketGuard shout;                                    // local_100
            shout.put_header(0, 0x56);
            shout.put_byte(2);
            shout.put_byte(1);
            shout.put_short(
                static_cast<unsigned short>(get_unique_id() & 0xffff));
            shout.put_int(static_cast<int>(resultId));
            shout.put_byte(lotto.GetUpgrade());
            shout.finalize(true);
            G_GameWorld()->send_all(shout);
        }
    }
    return 0;
}

// ===========================================================================
// 7) CUser::multibox_lottery_item(int boxSlot, int keySlot) @0x08674968
// ===========================================================================
// [round2-L30 复核·N126] 残余=整帧槽号常数平移 0x10（ORIG -0x14d… vs 我方 -0x15d…；帧
//   0x25c vs 0x24c）⇒ 判据⑪ (b) 类，声明重排无效；ORIG 0x08674968（0x115d）。
// [E04 2026-09-10 复核] p167 141/141 CLEAR（差集仅 C1/C2·D1/D2 别名）；另比较点
//   ORIG `mov -0x14b(%ebp),%eax; cmp $0x2061,%eax` vs 我方 `cmpl $0x2061,-0x58(%ebp)`
//   = N18b 比较形族（形参/局部 mem 直比）。N126+N18b 维持。
#include "W8_CU_A/multibox_lottery_item.inc"
{
    // b28d-solo-W8_CU_A：N 封顶（§6.2 巨件口径；原 b28d-35 判词沿用并复验）。
    //   ORIG @0x08674968 size 0x115d；P167 调用点差集 141/141 CLEAR（本轮复验）；
    //   帧 0x25c vs 我方 0x24c（差 0x10，首局部 boxItem -0x14d vs -0x15d），
    //   P210 把 CheckFailMultiBoxLottery 定义后置实测帧不变 ⇒ 非「已见定义
    //   callee」成因，属 N95/N82 槽序族；P282 复核 vtable 槽序无真 bug
    //   （vcall_cand 件：ORIG 0x16/0x20/0x12 各 1-3 处 vs 我方少 1）。
    Inven_Item boxItem;                                          // local_151
    boxItem = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, boxSlot);
    unsigned int boxItemId = static_cast<unsigned int>(boxItem.m_addInfo);

    std::vector<std::pair<Inven_Item, bool> > results;           // local_7c
    int ret = 0;                                                 // local_50
    if (boxItemId == 0x2061u || boxItemId == 0x289843u) {
        ret = process_use_unlocked_multibox(boxSlot, keySlot, results);
    } else {
        ret = process_use_locked_multibox(boxSlot, keySlot, results);
    }
    if (ret != 0) {
        if (ret == 0x13) {
            reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                ->send_error_state_packet(this, boxItemId);
            return -1;
        }
        return ret;
    }

    // [E04 单点 spec·F2a 已试·N 回滚] 按 ORIG 0x08674b56（`call; mov %al,-0x45(%ebp);
    //   cmpb $0x0,-0x45(%ebp); je`，1 字节直存无 test/setne）试
    //   `char isFirst = (char)call; if (isFirst != 0)`:横测 ours
    //   `_ZN5CUser21multibox_lottery_itemEii` **1017→1024（+7 劣化）**、TU 四数
    //   27/71/0/5 不变 ⇒ 4.4 对 char 局部 + `!=0` 反产符号扩展/回读链（ORIG 的
    //   `cmpb $0x0,槽` 单条不可复现）⇒ 按「gap 不改善即回滚」恢复 bool 形式。
    const bool isFirst =
        reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
            ->cal_booster_gage(boxItemId, this) != 0;
    if (isFirst) {
        reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
            ->log_booster_gage_bonus_reward(this, results);
    }

    PacketGuard out;                                             // local_88
    out.put_header(1, 0xd3);
    out.put_byte(1);
    out.put_short(boxSlot);
    out.put_short(keySlot);
    out.put_short(static_cast<int>(results.size()));

    std::vector<std::pair<int, int> > updatedSlots;              // local_94

    for (std::vector<std::pair<Inven_Item, bool> >::iterator it =
             results.begin();
         it != results.end(); ) {
        Inven_Item item = (*it).first;
        int newSlot = -1;                                        // local_48
        int remainOut = 0;                                       // local_a0
        unsigned int itemId = static_cast<unsigned int>(item.m_addInfo);

        if (itemId == 1u) {
            // ---- 事件金币项 ----
            int amount = item.m_addInfo2;                        // local_34 初值
            if (amount > 0 && amount < 1000) {
                int addAmount = amount;
                if (isFirst) {
                    addAmount = static_cast<int>(
                                    G_CDataManager()->m_boosterGage
                                        .m_cMultiboxCount) *
                                amount;
                }
                unsigned int coin =
                    getCurCharacInvenR()->GetEventCoin() +
                    static_cast<unsigned int>(addAmount);
                getCurCharacInvenW()->SetEventCoin(coin);
                coin = getCurCharacInvenR()->GetEventCoin();
                m_historyLog.EventCoinAdd(static_cast<int>(coin), addAmount,
                                          static_cast<eCoinAddReason>(2));
                SaveMoneyCoin();
                SendCoin();
                out.put_short(-1);
                out.put_int(item.m_addInfo);
                out.put_int(amount);
                out.put_short(static_cast<unsigned short>(item.m_fieldb));
                out.put_byte(item.GetItemAttr());
                out.put_packet(item);
            }
            it++;                                                // ORIG 站点 1
            continue;
        }

        CItem* resItem = G_CDataManager()->find_item(itemId);    // local_38
        if (resItem == NULL) {
            return 0x11;
        }

        if (resItem->IsAvatarItem()) {
            char ipgBuf[24] = { 0 };                             // local_b8
            item.SetUpgrade(0);
            const char* ipg = WongWork::CGenUniqueNo::genIPGNo(
                WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
                static_cast<unsigned>(getCurCharacNo()), ipgBuf);
            newSlot =
                reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                    ->AddAvatarItem(updatedSlots, isFirst, this, item,
                                    resItem->getUsablePeriod(), false, '\0',
                                    static_cast<char>(-1), ipg,
                                    static_cast<eAvatarItemAddReason>(7),
                                    remainOut);
        } else if (resItem->IsCreatureItem() &&
                   resItem->GetItemType() == 0x16) {
            // ---- 生物蛋 ----
            if (item.m_field1 != '\x05') {
                newSlot = -1;
            } else if (
                user_creature::CCreatureItemConverter::SetEggItem(&item) == 1) {
                char ipgBuf[24] = { 0 };                         // local_d8+8
                const char* ipg = WongWork::CGenUniqueNo::genIPGNo(
                    WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
                    static_cast<unsigned>(getCurCharacNo()), ipgBuf);
                newSlot =
                    reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                        ->InsertCreatureItem(
                            updatedSlots, isFirst, this, item,
                            static_cast<eItemAddReason>(3), true, false,
                            resItem->getUsablePeriod(),
                            resItem->getExpirationDate(), ipg);
            } else {
                newSlot = -1;
            }
        } else {
            // ---- 普通 / 特殊商城品 ----
            int specialType = 0;                                 // local_d8[0]
            int specialValue = 0;                                // local_d8[1]
            bool specialFlag = false;                            // local_d9
            if (GlobalData::s_pCeraShop->IsSpecialItem(
                    itemId, specialType, specialValue, specialFlag)) {
                CItem* specItem = G_CDataManager()->find_item(itemId);
                int kind = specItem->GetItemType();
                if (kind != 0x10 && kind != 0x22) {
                    int featured = 0;                            // local_e0
                    int restrictCode =
                        G_CDataManager()->m_itemList->GetRestrictCode(
                            itemId);
                    long curSec = GlobalData::s_systemTime_.getCurSec();
                    std::vector<Inven_Item> produceVec;          // local_ec
                    WongWork::CCeraShop::stCeraShopItemParam_t param;  // local_104
                    if (GlobalData::s_pCeraShop->ProcessSpecialItem(
                            this, itemId, restrictCode, featured, curSec, true,
                            produceVec, param, '\0', '\0', isFirst, 1)) {
                        if (featured != 0) {
                            newSlot = -1;
                        }
                        if (restrictCode != 0 &&
                            !IsRestrictedGoods(restrictCode)) {
                            SetRestrictedGoods(restrictCode);
                            GlobalData::s_pCeraShop->SaveFeaturedIdx(
                                this, restrictCode, static_cast<int>(curSec), 0);
                            SendRestrictedGoods(true, restrictCode);
                        }
                    }
                    newSlot = 0xffff;   // 特殊品不落背包槽（ORIG 写回 0xffff）
                }
                if (kind == 0x10 || kind == 0x22) {
                    newSlot =
                        reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                            ->insertItemIntoInventory(
                                updatedSlots, isFirst, this, item,
                                static_cast<eItemAddReason>(3), true);
                }
            } else {
                newSlot =
                    reinterpret_cast<CBoosterGage*>(
                    GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)17))
                        ->insertItemIntoInventory(
                            updatedSlots, isFirst, this, item,
                            static_cast<eItemAddReason>(3), true);
            }

            if (newSlot != -1) {
                remainOut = item.get_add_info();
                if (resItem == NULL) {
                    newSlot = -1;
                } else {
                    int price = 0;                               // local_108
                    short count = static_cast<short>(item.m_addInfo2);
                    G_Store()->GetSellItemPrice(item, resItem, count, false,
                                                price);
                    if (isFirst) {
                        price = price * static_cast<int>(
                                          G_CDataManager()->m_boosterGage
                                              .m_cMultiboxCount);
                    }
                    GetInstanceValueStatistic()->AddValueStatistic(
                        static_cast<VALUE_STATISTIC_FIELD>(7), this,
                        static_cast<unsigned int>(price));
                }
            }
        }

        // ORIG 两个分支各自完整重复发送序列（调用计数按站点复现）
        if (newSlot == -1) {
            out.put_short(-1);
            out.put_int(item.m_addInfo);
            out.put_int(item.get_add_info());
            out.put_short(static_cast<unsigned short>(item.m_fieldb));
            out.put_byte(item.GetItemAttr());
            out.put_packet(item);
        } else {
            out.put_short(newSlot);
            out.put_int(item.m_addInfo);
            out.put_int(remainOut);
            out.put_short(static_cast<unsigned short>(item.m_fieldb));
            out.put_byte(item.GetItemAttr());
            out.put_packet(item);
        }
        it++;                                                    // ORIG 站点 2
    }

    out.put_byte(isFirst ? 1 : 0);
    out.finalize(true);
    Send(out);

    for (std::vector<std::pair<int, int> >::const_iterator itUpd =
             updatedSlots.begin();
         itUpd != updatedSlots.end(); itUpd++) {
        SendUpdateItem(eSendTarget_1,
                       static_cast<ENUM_ITEMSPACE>(itUpd->second),
                       itUpd->first);
    }

    InGameADManagerInstance()->CheckFailMultiBoxLottery(this, results);

    for (std::vector<std::pair<Inven_Item, bool> >::iterator itShout =
             results.begin();
         itShout != results.end(); itShout++) {
        std::pair<Inven_Item, bool> entry = *itShout;
        if (entry.second) {
            PacketGuard shout;                                   // local_114
            shout.put_header(0, 0x56);
            shout.put_byte(4);
            shout.put_byte(1);
            shout.put_short(
                static_cast<unsigned short>(get_unique_id() & 0xffff));
            shout.put_int(entry.first.m_addInfo);
            shout.put_int(static_cast<int>(boxItemId));
            shout.put_byte(entry.first.GetUpgrade());
            shout.finalize(true);
            G_GameWorld()->send_all(shout);
        }
    }

    use_multibox_statistic(boxItemId);
    return 0;
}

// ---------------------------------------------------------------------------
// CInGameAdvertisementManager::CheckFailMultiBoxLottery —— ORIG 0x080fcb3e
// _ZN27CInGameAdvertisementManager24CheckFailMultiBoxLotteryEP5CUserRSt6vectorISt4pairI10Inven_ItembESaIS5_EE
// 报告 docs/class_func_reports/CInGameAdvertisementManager/CheckFailMultiBoxLottery.md：
//   若 rewardList 全部为 multibox 广告箱物品(idx ∈ {0x1e3c,0x1e3d,0x1e3e})，
//   inc_multiboxLotteryItemFailCnt；连续失败 >2 时发送 put_header(0,0x1c6) 空通知包；
//   任一非广告物品 → set_multiboxLotteryItemFailCnt(0)。单例 owner=GameMain.cpp。
// 定义置于本 TU 末尾（曾试 P210 后置以翻 multibox 帧：实测帧 0x24c 不变，
//   故非「已见定义 callee」成因；见 multibox_lottery_item 判词 N95）。
// ---------------------------------------------------------------------------
#include "W8_CU_A/CheckFailMultiBoxLottery.inc"
    CUser* user,
    std::vector<std::pair<Inven_Item, bool> >& rewardList)
{
    // b28d-solo-W8_CU_A：按报告 0x080fcb3e 复原（原 b28d-35「N82 帧/槽 + bool
    //   物化块序」判词作废）。三处修法：① 循环体内补具名 `Inven_Item item =
    //   (*it).first;`（ORIG 0x080fcb81 段 15×dword+1×word 内联拷贝到 -0x66 槽，
    //   只读 m_addInfo 不产生该拷贝）；② 白名单判定改正极性
    //   `if (id==A||id==B||id==C) { allAdItems=true; } else { allAdItems=false;
    //   break; }`（ORIG 0x080fcc0b 三 je 汇同一 true 块 + 尾 jne false 块）；
    //   ③ `int failCnt = inc_...();` 具名槽（ORIG 0x080fcc92 `mov %eax,-0xc;
    //   cmpl $0x2,-0xc`）。
    // 残余 N 封顶：① Inven_Item 拷贝尾 4.1 用 word（`movzwl 0x3c; mov %ax`，
    //   越界读 +0x3d，同 CParty.cpp:5915 已证 ORIG 遗留）vs 4.4 byte ⇒ N78；
    //   [E04 2026-09-10 复核] p167 16/16 CLEAR（差集仅 PacketGuard C1/D1↔C2/D2 别名）
    //   ⇒ 无真缺口，gap +1 维持 N78+N95。
    //   ② it/guard 槽序反向（4.1 guard -0x28/it -0x1c，4.4 it -0x28/guard
    //   -0x24）⇒ N95 槽序族（4.4 按首次使用序分配，it 先于 guard 使用）。
    //   本轮已用循环体内 `char slotPad1[1]` 把 item 槽基从 -0x65 顶到 ORIG 的
    //   -0x66（N95 正例，拷贝段 15×dword 偏移全对齐）。语义/调用点
    //   （16/16 CLEAR）与块布局已全等。
    if (user == 0) {
        return;
    }
    if (rewardList.empty()) {
        return;
    }

    // ORIG：迭代器 for 循环（begin/end/++/de）
    bool allAdItems = false;
    for (std::vector<std::pair<Inven_Item, bool> >::iterator it =
             rewardList.begin(); it != rewardList.end(); it++) {
        char slotPad1[1];          // N95 试验：把 item 槽基顶到 ORIG 的 -0x66
        (void)slotPad1;
        Inven_Item item = (*it).first;
        if (item.m_addInfo == 0x1e3cu || item.m_addInfo == 0x1e3du ||
            item.m_addInfo == 0x1e3eu) {
            allAdItems = true;                               // ORIG +0xcd 置一
        } else {
            allAdItems = false;                              // ORIG +0xf1 置零后跳出
            break;
        }
    }

    if (!allAdItems) {
        user->set_multiboxLotteryItemFailCnt(0);             // 0x080fdf44
        return;
    }
    int failCnt = user->inc_multiboxLotteryItemFailCnt();    // 0x080fdf56
    if (failCnt <= 2) {
        return;
    }

    PacketGuard out;
    out.clear();                       // ORIG PacketGuardC1 后显式 clear
    out.put_header(0, 0x1c6);
    out.finalize(true);
    user->Send(out);
}
