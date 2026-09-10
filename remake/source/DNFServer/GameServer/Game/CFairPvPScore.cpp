// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CFairPvPScore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// fair_pvp::CFairPvPScore —— ORIG df_game_r 忠实转写
// 报告: docs/class_func_reports/fair_pvp__CFairPvPScore/*.md
//   ctor 0x84a47dc / D1 0x84a4850 / loadData 0x84a48f8 / getData 0x84a55a8 /
//   _saveData 0x84a57fe / _reset 0x84a5942 / UpdateScore 0x84a5a28 /
//   MakePacketSocre 0x84a5cc2 / ResetDaily W 0x84a645a(反汇编实证) /
//   JobKey ctor 0x84a643a / JobKey operator< 0x84a63f2
// W 内联组反汇编实证:
//   SetGiveItem 0x8234f74(写字节+0x5b 后 alter) / SetDailyPlayCount
//   0x8234f92(jns 钳负→0 写 int+0x50) / SetRemainBonusCount 0x8234fae(+0x5a)
// / GetDailyPlayCount 0x8568ec2 读 +0x50；SetLastTrainingSeal(Bonus)Count 与
//   Get* 三族同型读写字节 +0x58/+0x59/+0x5a。
// ============================================================================
#include "PacketGuard.h"              // CFairPvPScore.h 引用 PacketGuard&
#include "STEquipmentScript.h"        // 提供 guarded ENUM_CHARACTERJOB（DNF_ENUM_CHARACTERJOB_DEFINED）
#include "CFairPvPScore.h"

#include <string.h>

#include "GameWorld.h"                // G_GameWorld/GetChannelType
#include "MsgQueueMgr.h"              // s_msgq_mgr->put 完整类型
#include "GlobalData.h"               // s_stream_pool/s_msgq_mgr/s_systemTime_
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"       // PacketGuard 转发层 API
#include "SigTypes.h"                 // SIG_FAIR_PVP_SCORE（0x688）
#include "CItem.h"                    // CItem::make_item
#include "CInventory.h"               // insert_event_items/MakeItemPacket/getCurCharacInven*
#include "CUser.h"                    // Send/GetUID/get_charac_no/get_pvp_grade/GetServerGroup/...
#include "CUserCharacInfo.h"          // getPVPResultRefR/getCurCharFirstGrowType/get_charac_job/getCurCharacNo
#include "PvpResultType.h"            // 权威 PvpResultType（m_pvpGrade=+0x14，ORIG 084a4b94 mov 0x14(%eax)）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "ServerParameterScript.h"    // GetTraningSealGive/GetTraningFirstGive/get_pvp_item_give

extern CDataManager* G_CDataManager();

namespace fair_pvp
{

// ---- TU-local 线格式视图 ----------------------------------------------------
// getData/loadData 的条目基址为 (idx+2)*0x10，ORIG 对该基址用
// `lea 0x2(%ebx),%ecx; shl $0x4,%ecx` 后以 disp 0x10/0x14 访问（084a55fc/
// 084a563b 段）；4.4 若直接写 `((Entry*)(sig+(idx+2)*0x10))[1]` 会把 +0x10
// 折进基址（add $0x3; shl $4）。改用「双条目」具名视图令偏移全部落 disp。
// POD、无成员函数 ⇒ 不发射任何符号；sizeof/偏移与 SIG_FAIR_PVP_SCORE_ENTRY×2 一致。
struct FairPvpScoreEntryPair
{
    SIG_FAIR_PVP_SCORE_ENTRY m_e0;    // +0x00
    SIG_FAIR_PVP_SCORE_ENTRY m_e1;    // +0x10（= (idx+2)*0x10 + 0x10）
};

// ---- ORIG 0x84a47dc CFairPvPScore::CFairPvPScore() --------------------------
#include "CFairPvPScore/CFairPvPScore.inc"
}

// ---- ORIG 0x84a4850 ~CFairPvPScore() ---------------------------------------
#include "CFairPvPScore/dtor_CFairPvPScore.inc"
}

// ---- ORIG 0x84a645a ResetDaily()（W，反汇编实证：仅清 +0x50） --------------
#include "CFairPvPScore/ResetDaily.inc"
}

// ---- ORIG 0x84a63f2 JobKey::operator<(const JobKey&) const -----------------
bool JobKey::operator<(const JobKey& rhs) const
{
    if (m_job < rhs.m_job)
        return true;
    // [b28 P59 块序试验] ORIG 0x84a63f2：第二比较 `cmp;jne 尾mov$0`（!= 为跳走
    // 侧、== fall-through 进第三段）；false 出口共享物理尾块（84a6433 mov$0 在
    // 三段之后）。等值嵌套形令 4.4 反向排布尝试。
    if (m_job == rhs.m_job) {
        // ORIG 0x84a63f2: 第三比较 `cmp %al,%dl; jge 尾`（jge=跳 false 尾，
        // < 落 true 岛）。字段 P211 恢复 signed char，直接比较即原有符号序。
        if (m_sub < rhs.m_sub)
            return true;
    }
    return false;
}

// ---- ORIG 0x84a643a JobKey::JobKey(ENUM_CHARACTERJOB, unsigned char) -------
#include "CFairPvPScore/JobKey.inc"
}

// ---- ORIG 0x84a5942 _reset() ------------------------------------------------
#include "CFairPvPScore/_reset.inc"
}

// ---- ORIG 0x84a57fe _saveData(CUser*) --------------------------------------
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一，全二进制口径 12）：唯一差 =
// 虚槽调用（CUser 写流路径）ORIG 4.1 `mov (%eax),%edx; …; call *%edx` vs 4.4
// 恒 `call *%eax` + 实参装载寄存器对调（N7 间接 call 目标族，b26 NL28 实证）。
// 助记符/条数 87=87 全等；禁绑定形（P0/N96）。
#include "CFairPvPScore/_saveData.inc"
}

// ---- ORIG 0x84a55a8 getData(char*) const -----------------------------------
// b28d-solo-CFairPvPScore 判词：DIFF·N 封顶（ORIG 183 vs 215；归一化 NDIFF 83
// 全为 push esi 缺位引发的错位伪差 + 下列 N 族）。
// 已修：p167 CLEAR（12=12；恢复 `it->` 三次 ptEv 084a6764 + 第三段 `it->second`
// 整体结构赋值一次 ptEv）；索引单槽复用（三循环共用 -0xc）、迭代器 for 外初始化式、
// 去 entry 指针缓存（P183）、双条目视图消 4.4 把 +0x10 折进基址。
// 残余 N 归因：N7（ORIG 统计循环以 esi 缓存 this，4.4 恒用 ebx ⇒ push esi 缺位、
// 帧 0x30 vs 0x34、全篇 difflib 错位）；N4/P30（`(idx+2)*0x10` 基址 + disp 的
// scaled 写 `mov %dl,0x10(%ecx,%eax,1)` vs 4.4 先 lea 地址再 disp 写）；
// N18a/N82（寄存器分配与槽号）。结构复原（ORIG 084a55a8 逐段实证）：
// ① idx 单槽复用（三循环共用 -0xc；ORIG 头部 movl$0 与 for-init movl$0 成对）；
// ② 迭代器为 for 外初始化式（begin() 直构 sret，无 C1Ev）；
// ③ 条目基址 (idx+2)*0x10 每点重发（P183 禁指针缓存，旧版 entry 具名局部多一槽）；
// ④ it-> 三次 ptEv（084a6764），第三段 `it->second` 整体结构赋值（一次 ptEv）。
#include "CFairPvPScore/getData.inc"
}

// ---- ORIG 0x84a5a28 UpdateScore(int,int,int,int,int,int,int) ---------------
// b28d-solo-CFairPvPScore 判词：AE/IDENTICAL（已转出 TU DIFF 列表；211=211，
// 归一化 NDIFF 仅剩 1 条尾部对齐 nop = §6.1 豁免）。
// 修法：①**真语义 bug**——新键插入值应为本次 cnt0/cnt1/cnt2（ORIG 084a5ae8..
// 084a5af7 从 0x18/0x1c/0x20(%ebp) 取），旧转写误插 0（丢失首局计数）；
// ②P220 正极性 `it != end()` + then/else 交换（p167 差集 ne/eq 实证）；
// ③P178 嵌套实参形：`pair<const JobKey,COUNT>(make_pair(key, value))` 复现
// make_pair<JobKey&,COUNT&> + pair 移动构造两段；④FAIR_PVP_SCORE_MODE_COUNT 与
// m_modeStats 字段 unsigned 化（P274：unsigned 字段 + int 形参 ⇒ ORIG 084a5b4a
// `mov mem,%edx; mov param,%eax; add %eax,%edx` 三段形）。p167 CLEAR（16=16）。
#include "CFairPvPScore/UpdateScore.inc"
}

// ---- ORIG 0x84a5cc2 MakePacketSocre(PacketGuard&, int) const ---------------
// b28d-solo-CFairPvPScore 判词：AE/IDENTICAL（已转出 TU DIFF 列表；233=233，
// 归一化 NDIFF=0）。
// 修法：①P257 正向包裹 `if (mode == 1) { … }`（旧 `if (mode != 1) return;` 在
// 4.4 多出 `jmp <epilogue>; nop` 尾岛，ORIG 084a5e5b 为 `cmpl $1; jne <尾>`）；
// ②字段 unsigned 化（m_modeStats/m_field40/m_field44/m_winStreak/m_bestWinStreak
// ⇒ put_int 实参复现 ORIG 两段 `mov field,%eax; mov %eax,%edx`）。
// p167 CLEAR（37=37）。
#include "CFairPvPScore/MakePacketSocre.inc"
}

// ---- W 内联组（ORIG 地址见头文件注释，逐一反汇编实证） ---------------------
#include "CFairPvPScore/SetGiveItem.inc"
}

#include "CFairPvPScore/GetDailyPlayCount.inc"
}

#include "CFairPvPScore/SetDailyPlayCount.inc"
}

#include "CFairPvPScore/SetRemainBonusCount.inc"
}

#include "CFairPvPScore/SetLastTrainingSealCount.inc"
}

#include "CFairPvPScore/SetLastTrainingSealBonusCount.inc"
}

#include "CFairPvPScore/GetRemainBonusCount.inc"
}

#include "CFairPvPScore/GetLastTrainingSealCount.inc"
}

#include "CFairPvPScore/GetLastTrainingSealBonusCount.inc"
}

// b28d-21：删除**发明符号** `_ZN8fair_pvp12_GLOBAL__N_124IsFairPvpTrainingChannelEv`
// （ORIG nm 无此符号；P49：-O0 下 static inline helper 仍产真实 call 符号 = 既 DIFF
// 又造 missing）。ORIG 0x84a48f8 loadData 区间实测 4 次 `call G_GameWorld` +
// 4 次 `call GetChannelType`（0x84a4b??/0x84a4c?? 段）⇒ 两个使用点各直接写两次
// `G_GameWorld()->GetChannelType()`（P183 禁指针缓存，每点重发全链）。

// ---- ORIG 0x84a48f8 loadData(CUser*, char*) --------------------------------
// b28d-solo-CFairPvPScore 判词：DIFF·N 封顶（§6.2 三件套：①p167 CLEAR 91=91；
// ②gap 24（ORIG 848 vs 872）；③帧 0x22c vs 0x27c、全帧槽号平移 = N82/N95 族）。
// 修法（含 4 处真语义 bug）：
//   ①新键插入：第二实参直接引用线格式计数（ORIG 084a4929 `lea 0x14(%eax),%ebx`）
//     + P178 嵌套 make_pair/pair 移动构造（mangled make_pair<JobKey,COUNT&> 实证）；
//   ②区块 2 零检查改循环体内 break（ORIG 084a4a5c test/je 在 body 首）；
//   ③区块 3 改整体结构赋值（ORIG 084a4a9b 源/目标基址各算一次、3 读 3 写）；
//   ④MakeItemPacket 的 receiver 改 getCurCharacInvenR（旧用 W，**数据源真 bug**，
//     p167 MISSING `_ZNK15CUserCharacInfo18getCurCharacInvenREv` 实证）；
//   ⑤0x152 包 put_short（旧 put_byte，**包字段宽度真 bug**，MISSING put_short(int)）；
//   ⑥0x161 包复用同一 PacketGuard + clear()（消多余 C1/D1 与 _Unwind_Resume）；
//   ⑦第二封补 `strncpy(msgBuf,msg,0xff)`（旧漏调用，MISSING strncpy）；
//   ⑧缓冲改 `char[N]={0}` 数组零初始化（消 memset×4，P214③）；
//   ⑨G_CDataManager 每次重调（消 sps/dailyLimit 具名缓存，MISSING×2）；
//   ⑩`pair<int,int>(0xcf0, sealGive)` 直构（mangled `C1IiRiE` 实证；sealGive 非 const）；
//   ⑪单 Inven_Item + 单组缓冲复用两封（ORIG 084a51ec 单对象）、字段序
//     addInfo→find_item→addInfo2→make_item（ORIG 084a52a8/52de）。
// 残余 N 归因：N82/N95 帧·槽全帧平移（0x50）、N4/P30 寻址折叠、N7 寄存器分配。
// ORIG 0x84a48f8。
#include "CFairPvPScore/loadData.inc"
#define FAIR_PVP_ENTRY(i)                                                   \
    (reinterpret_cast<FairPvpScoreEntryPair*>(                              \
         reinterpret_cast<char*>(sig) + ((i) + 2) * 0x10)->m_e1)
    for (idx = 0;
         (idx < (int)(unsigned char)sig->m_jobScoreCount) && (idx <= 99);
         ++idx)
    {
        std::pair<std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::iterator, bool>
            ret = m_jobScore_.insert(
                std::pair<const JobKey, FAIR_PVP_SCORE_MODE_COUNT>(
                    std::make_pair(
                        JobKey((ENUM_CHARACTERJOB)(unsigned char)
                                   FAIR_PVP_ENTRY(idx).m_job,
                               FAIR_PVP_ENTRY(idx).m_sub),
                        *(FAIR_PVP_SCORE_MODE_COUNT*)&FAIR_PVP_ENTRY(idx).m_cnt0)));
        if (!ret.second)
        {
            LogManager::logFormat(
                1, "FairPvPScore.cpp",
                __PRETTY_FUNCTION__,
                0x20, "job_score_.insert error(%d,%d)",
                (int)(unsigned char)FAIR_PVP_ENTRY(idx).m_job,
                (int)(unsigned char)FAIR_PVP_ENTRY(idx).m_sub);
        }
    }

    // 区块 2：最近对局结果列表（ORIG 084a4a45..084a4a92：零检查在体内 break，
    // 循环条件只剩 idx <= 9；遇 0 截断）
    for (idx = 0; idx <= 9; ++idx)
    {
        if (sig->m_recentResults[idx] == '\0')
            break;
        m_recentResults_.push_back(sig->m_recentResults[idx]);
    }

    // 区块 3：两份模式统计记录（ORIG 084a4a92..084a4ae3 为整体结构赋值：
    // 源/目标基址各算一次、3 读 3 写）
    for (idx = 0; idx <= 1; ++idx)
    {
        *(FAIR_PVP_SCORE_MODE_COUNT*)&m_modeStats[idx][0] =
            *(FAIR_PVP_SCORE_MODE_COUNT*)&sig->m_modeStats[idx][0];
    }
    m_field40 = sig->m_field1c;
    m_field44 = sig->m_field20;
    m_winStreak = sig->m_winStreak;
    m_bestWinStreak = sig->m_bestWinStreak;
    m_dailyPlayCount = sig->m_dailyPlayCount;
    m_field54 = sig->m_lastPlayTime;   // int→unsigned 异型：ORIG 084a4b25 两段中转

    // 训练特训渠道（0xf/0x10）且无上次结算时间 → 补发入场奖励
    if ((G_GameWorld()->GetChannelType() == 0xf ||
         G_GameWorld()->GetChannelType() == 0x10) &&
        (int)sig->m_lastPlayTime == 0)
    {
        std::vector<std::pair<int, int> > items;
        // ORIG 084a4b86..084a4ba3：getPVPResultRefR 实参先求值（读 +0x14
        // m_pvpGrade），receiver 后求值；G_CDataManager 每次重调（禁 sps 缓存）。
        int sealGive =
            G_CDataManager()->m_serverParameter.GetTraningSealGive(
                pUser->getPVPResultRefR()->m_pvpGrade);
        if (sealGive > 0)
            // ORIG mangled pair<int,int>::C1<int,int&>(int&&,int&)：第一参字面量右值
            items.push_back(std::pair<int, int>(0xcf0, sealGive));
        G_CDataManager()->m_serverParameter.GetTraningFirstGive(
            pUser->get_charac_job(),
            (int)(signed char)pUser->getCurCharFirstGrowType(),
            pUser->get_pvp_grade(), items);

        if (!items.empty())
        {
            std::vector<int> slots;
            // ORIG 084a4c65 getCurCharacInvenW 作 insert_event_items 的 receiver；
            // 084a4d76 循环内每次重调 getCurCharacInvenR（P183 禁缓存）。
            int inserted = pUser->getCurCharacInvenW()->insert_event_items(
                items, slots, (eMoneyAddReason)3, (eItemAddReason)8, true,
                "game_server_msg_171", "game_server_msg_172");
            if (inserted >= 1 && slots.size() >= 3)
            {
                PacketGuard pkt;
                pkt.put_header(0, 0xe);
                pkt.put_byte(0);
                int fillIndex = pkt.get_index();
                int made = 0;
                pkt.put_short(made);           // ORIG 084a4d51 put_short(made) 占位
                for (size_t j = 0; j < slots.size(); j += 3)
                {
                    if (pUser->getCurCharacInvenR()->MakeItemPacket(
                            (INVEN_TYPE)1, slots[j], pkt))
                        ++made;
                }
                pkt.put_short(fillIndex, made);
                pkt.finalize(true);
                pUser->Send(pkt);
            }
        }
        m_field54 = GlobalData::s_systemTime_.getCurSec();
        alter();
    }

    // 剩余可领训练印章数（ORIG 读 CDataManager+0x49e0 =
    // m_serverParameter(+0x68)+0x4978 → FairMatchRooms 批已具名
    // m_schoolMatchDailyPlayLimit）。ORIG 084a4eaa/084a4ed1 两分支各重调
    // G_CDataManager（禁 dailyLimit 具名缓存）。
    if ((unsigned int)m_dailyPlayCount <
        (unsigned int)G_CDataManager()->m_serverParameter
            .m_schoolMatchDailyPlayLimit)
        m_remainBonusCount =
            (unsigned char)(G_CDataManager()->m_serverParameter
                                .m_schoolMatchDailyPlayLimit -
                            (unsigned char)m_dailyPlayCount);  // ORIG sub %al,%dl
    else
        m_remainBonusCount = 0;
    {
        // ORIG 084a4ef6 构造 -0x88 的 PacketGuard 后一直活到函数尾；
        // 0x152/0x161 两包复用同一对象（0x084a4fe5 clear）。
        PacketGuard pkt;
        pkt.put_header(0, 0x152);
        pkt.put_short((int)m_remainBonusCount);   // ORIG 084a4f30 put_short(int)
        pkt.finalize(true);
        pUser->Send(pkt);

        m_giveItem = sig->m_giveItem;

        // 特训渠道：可领但尚未领取的道具（发 0x161 提示包）
        if (G_GameWorld()->GetChannelType() == 0xf ||
            G_GameWorld()->GetChannelType() == 0x10)
        {
            if ((int)m_giveItem <=
                G_CDataManager()->m_serverParameter.get_pvp_item_give(
                    pUser->get_pvp_grade()))
            {
                pkt.clear();
                pkt.put_header(0, 0x161);
                pkt.finalize(true);
                pUser->Send(pkt);
            }
        }

    // 上次结算时间早于维护点 → 按累计成绩补发两封结算邮件
    if (m_field54 != 0 && m_field54 <= 0x4ddd89efu)
    {
        const int sumA = m_modeStats[0][0] + m_modeStats[0][1] + m_modeStats[0][2];
        const int bonusSumA50 = (sumA >= 10 && sumA <= 0x31) ? 10 : 0;
        const int bonusSumA60 = (sumA > 0x31) ? 60 : 0;
        const int bonusA15 = (m_modeStats[0][0] >= 10 && m_modeStats[0][0] <= 0x31) ? 15 : 0;
        const int bonusA90 = (m_modeStats[0][0] > 0x31) ? 90 : 0;
        const int sumB = m_modeStats[1][0] + m_modeStats[1][1] + m_modeStats[1][2];
        const int bonusSumB50 = (sumB >= 10 && sumB <= 0x31) ? 10 : 0;
        const int bonusSumB60 = (sumB > 0x31) ? 60 : 0;
        const int bonusB15 = (m_modeStats[1][0] >= 10 && m_modeStats[1][0] <= 0x31) ? 15 : 0;
        const int bonusB90 = (m_modeStats[1][0] > 0x31) ? 90 : 0;

        const int total = m_modeStats[1][0] * 2 + m_modeStats[0][0] * 2 +
                          m_modeStats[1][1] +
                          m_modeStats[1][2] + m_modeStats[1][0] * 3 + m_modeStats[0][1] +
                          m_modeStats[0][2] +
                          m_modeStats[0][0] * 2 + bonusSumA50 + bonusSumA60 + bonusA15 +
                          bonusA90 + bonusSumB50 + bonusSumB60 + bonusB15 +
                          bonusB90;

        // ORIG 084a51ec..084a54cd：单个 Inven_Item + 单组缓冲复用两封；
        // 缓冲为数组零初始化（4.1 内联 rep stos/慢路；旧版 memset 调用多 4 次）；
        // 字段序 = addInfo → find_item → addInfo2 → make_item（084a52a8/52de）。
        Inven_Item item;                       // 默认构造清零
        char titleBuf[21] = {0};               // ORIG 0x15 字节
        char msgBuf[256] = {0};
        item.m_addInfo = 0xcf0;                // ORIG -0x104(%ebp)= item+0x02
        CItem* def = G_CDataManager()->find_item(item.m_addInfo);
        if (def == NULL)
            return false;                      // ORIG ebx=0 早退（仍析构包守卫）
        item.m_addInfo2 = total + 200;         // ORIG -0xff(%ebp) = item+0x07

        def->make_item(item);
        {
            const char* title = g_scriptStringManager_.findString(
                4, "game_server_msg_171", 0);
            strncpy(titleBuf, title, 0x14);
            const char* msg = g_scriptStringManager_.findString(
                4, "game_server_msg_177", 0);
            strncpy(msgBuf, msg, 0xff);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0,
                (unsigned int)pUser->getCurCharacNo(), msgBuf,
                (int)strlen(msgBuf), 0, pUser->GetServerGroup(), true, false);
        }

        // —— 第二封：印章道具 0x289788，数量 1（ORIG 复用同一 item/缓冲） ——
        item.m_addInfo = 0x289788;
        CItem* def2 = G_CDataManager()->find_item(item.m_addInfo);
        if (def2 == NULL)
            return false;
        item.m_addInfo2 = 1;

        def2->make_item(item);
        {
            const char* title = g_scriptStringManager_.findString(
                4, "game_server_msg_171", 0);
            strncpy(titleBuf, title, 0x14);
            const char* msg = g_scriptStringManager_.findString(
                4, "game_server_msg_178", 0);
            strncpy(msgBuf, msg, 0xff);        // [b28d-solo] 旧版漏此调用（p167 MISSING strncpy）
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0,
                (unsigned int)pUser->getCurCharacNo(), msgBuf,
                (int)strlen(msgBuf), 0, pUser->GetServerGroup(), true, false);
        }

        m_field54 = GlobalData::s_systemTime_.getCurSec();
        alter();
    }
    }  // pkt 作用域（ORIG PacketGuard 活到函数尾）

#undef FAIR_PVP_ENTRY
    return true;
}

}  // namespace fair_pvp
