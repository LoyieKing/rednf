// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFairPvPScore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

// ---- ORIG 0x84a47dc CFairPvPScore::CFairPvPScore() --------------------------
#include "CFairPvPScore/CFairPvPScore.inc"
    : charac_expand::CData()
{
}

// ---- ORIG 0x84a4850 ~CFairPvPScore() ---------------------------------------
#include "CFairPvPScore/dtor_CFairPvPScore.inc"
{
}

// ---- ORIG 0x84a645a ResetDaily()（W，反汇编实证：仅清 +0x50） --------------
#include "CFairPvPScore/ResetDaily.inc"
{
    m_dailyPlayCount = 0;
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
{
    // [b28 P211 AE] 字段 enum/schar 化后本体与 ORIG 0x84a643a 13 条逐字同形
    // （探针 /tmp/l2_p2.cpp JK 实证）：store1 this→value→movl；store2 RHS 先
    // movzbl→this 重载→movb。旧 B24D N 判词作废。
    m_job = job;
    m_sub = sub;
}

// ---- ORIG 0x84a5942 _reset() ------------------------------------------------
#include "CFairPvPScore/_reset.inc"
{
    m_jobScore_.clear();
    m_recentResults_.clear();
    for (int i = 0; i < 2; ++i)
    {
        m_modeStats[i][0] = 0;
        m_modeStats[i][1] = 0;
        m_modeStats[i][2] = 0;
    }
    m_field40 = 0;
    m_field44 = 0;
    m_winStreak = 0;
    m_bestWinStreak = 0;
    m_dailyPlayCount = 0;
    m_field54 = 0;
    m_lastTrainingSealCount = 0;
    m_lastTrainingSealBonusCount = 0;
    m_remainBonusCount = 0;
    m_giveItem = 0;
}

// ---- ORIG 0x84a57fe _saveData(CUser*) --------------------------------------
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一，全二进制口径 12）：唯一差 =
// 虚槽调用（CUser 写流路径）ORIG 4.1 `mov (%eax),%edx; …; call *%edx` vs 4.4
// 恒 `call *%eax` + 实参装载寄存器对调（N7 间接 call 目标族，b26 NL28 实证）。
// 助记符/条数 87=87 全等；禁绑定形（P0/N96）。
#include "CFairPvPScore/_saveData.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("FairPvPScore.cpp", 0x10a),
                       true);

    **guard << 0x261;                       // SIG_FAIR_PVP_SCORE 头
    **guard << (int)pUser->GetUID();

    SIG_FAIR_PVP_SCORE* sig =
        guard.operator->()->GetInBuffer<SIG_FAIR_PVP_SCORE>();
    memset(sig, 0, sizeof(SIG_FAIR_PVP_SCORE));          // 0x688
    sig->m_characNo = pUser->get_charac_no(-1);          // ORIG *(int*)local_10 = m_characNo(+0x00)
    this->getData(reinterpret_cast<char*>(sig));         // ORIG vtable+8 (getData)；线格式序列化
    sig->m_lastPlayTime = GlobalData::s_systemTime_.getCurSec();   // +0x680

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return true;                                          // ORIG mov $0x1,%ebx
}

// ---- ORIG 0x84a55a8 getData(char*) const -----------------------------------
// [L6S4 round5b] 判词更新：ORIG 183 vs 我方 **184**（gap −1；改前 215）。
// 本轮修法（两处，均为 P291/P183 族）：
//   ① 条目写侧：cast 指针算术（`((Pair*)(sig+(idx+2)*0x10))->m_e1`）⇒ 4.4 把 sig
//      折进基址、每字段 6 条重算；改权威成员数组下标 `sig->m_jobScore[idx]`（同址）
//      ⇒ 复现 ORIG「索引寄存器 + 基址寄存器 + disp」形（仅常量拆分差：4.4
//      `lea 0x3/disp 0` vs ORIG `lea 0x2/disp 0x10`）。
//   ② 模式统计段：逐字段写（+27 条）改成员整体赋值
//      `*(COUNT*)&sig->m_modeStats[idx][0] = *(COUNT*)&m_modeStats[idx][0]`。
// 残余 1 条 + 错位归因（均 4.4 lowering）：
//   · N7（ORIG 统计段以 esi 缓存 this：`mov 0x8(%ebp),%esi` ⇒ prologue 多 push esi、
//     epilogue 多 pop esi，我方用 ebx/重载；帧 0x30 vs 0x34）；
//   · N4/P30（`(idx+2)*0x10` 的常量拆分习惯 + ORIG 写侧 `0x4(%eax,%ecx,1)` 索引/基址
//     分离 vs 4.4 合并为单指针：整体赋值段 25 vs 23 条）；
//   · 尾部 `sig->m_dailyPlayCount = m_dailyPlayCount` 4.4 多 1 条 `mov %eax,%edx` 中转。
// 试形记录（本轮）：+0x10 起 100 元素具名数组视图 ⇒ 4.4 产 `lea 0x2; lea 0x1` 两段，
// 劣化到 191 ⇒ 回滚。结构复原（ORIG 084a55a8 逐段实证）：
// ① idx 单槽复用（三循环共用 -0xc；ORIG 头部 movl$0 与 for-init movl$0 成对）；
// ② 迭代器为 for 外初始化式（begin() 直构 sret，无 C1Ev）；
// ③ 条目基址 (idx+2)*0x10 每点重发（P183 禁指针缓存，旧版 entry 具名局部多一槽）；
// ④ it-> 三次 ptEv（084a6764），第三段 `it->second` 整体结构赋值（一次 ptEv）。
// [L6S5 P211 试形·回滚] 尾部 m_dailyPlayCount 异型 store：改
//   `*(unsigned int*)&sig->m_dailyPlayCount = m_dailyPlayCount;`（同型别名）产
//   `add $0x67c,%eax; mov 0x8(%ebp),%edx; mov 0x50(%edx),%edx; mov %edx,(%eax)`（4 条），
//   与 ORIG 084a57c4 的 4 条（`mov 0x8(%ebp),%eax; mov 0x50(%eax),%edx;
//   mov -0x10(%ebp),%eax; mov %edx,0x67c(%eax)`）**形态不同**且总数不变（183/184）
//   ⇒ 回滚（本条属 N7/N4，源码不可控）。
#include "CFairPvPScore/getData.inc"
{
    SIG_FAIR_PVP_SCORE* sig = reinterpret_cast<SIG_FAIR_PVP_SCORE*>(pData);
    int idx = 0;
    std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::const_iterator it =
        m_jobScore_.begin();
    for (idx = 0; it != m_jobScore_.end() && idx <= 99; ++it, ++idx)
    {
        // [L6S4 P291/P183] ORIG 084a55e1 段为「索引寄存器 + 基址寄存器 + disp」：
        //   `mov -0x10(%ebp),%eax; lea 0x2(%ebx),%ecx; shl $0x4,%ecx;
        //    mov %dl,0x10(%ecx,%eax,1)`（值=%dl）/ 0x11 / 0x14..0x1c。
        // 旧 cast 指针算术形把 sig 折进基址 ⇒ 每字段 6 条重算（215 条）。
        // 权威成员数组下标形（m_jobScore@+0x30）降到 184 条（ORIG 183）：
        // 唯一残差 = 常量拆分习惯差（4.4 `lea 0x3/disp 0` vs ORIG `lea 0x2/disp 0x10`，
        // 4.1 把 +0x30 拆成 2*0x10+0x10）；「+0x10 起 100 元素」视图试形反而劣化
        // 到 191（4.4 产 `lea 0x2; lea 0x1` 两段）⇒ 回滚。
        sig->m_jobScore[idx].m_job = (unsigned char)(unsigned int)it->first.m_job;
        sig->m_jobScore[idx].m_sub = (unsigned char)(unsigned int)it->first.m_sub;
        *(FAIR_PVP_SCORE_MODE_COUNT*)&sig->m_jobScore[idx].m_cnt0 = it->second;
        ++sig->m_jobScoreCount;                          // 字节自增（ORIG movzbl/lea/movb）
    }

    std::list<unsigned char>::const_iterator it2 = m_recentResults_.begin();
    // ORIG 084a56f0/084a56fa：先 `addl $1,-0xc` 再 operator++（increment 序与 map 循环相反）。
    for (idx = 0; it2 != m_recentResults_.end() && idx <= 9; ++idx, ++it2)
        sig->m_recentResults[idx] = *it2;

    for (idx = 0; idx <= 1; ++idx)
    {
        // [L6S4] ORIG 084a5674 段为整体结构赋值（源/目标基址各算一次、3 读 3 写：
        //   `mov 0x8(%edx),%ebx; mov %ebx,0x4(%eax,%ecx,1)` …）⇒ 逐字段写会
        // 每字段重算两侧基址（+27 条）。与 loadData 区块 3 同一复现形。
        // [L6S4] 方向必须为「成员 → 线格式」（sig 为目标）——上一编辑曾误交换，
        // 已回滚（该形若反向即真语义 bug，勿再试）。
        *(FAIR_PVP_SCORE_MODE_COUNT*)&sig->m_modeStats[idx][0] =
            *(FAIR_PVP_SCORE_MODE_COUNT*)&m_modeStats[idx][0];
    }
    sig->m_field1c = m_field40;
    sig->m_field20 = m_field44;
    sig->m_winStreak = m_winStreak;
    sig->m_bestWinStreak = m_bestWinStreak;
    // [L6S5 试形·回滚] P211 类型轴：`*(unsigned int*)&sig->m_dailyPlayCount = ...`
    //   产 4 条（`add $0x67c,%eax; ...; mov %edx,(%eax)`），与 ORIG 084a57c4
    //   的 4 条（`mov 0x8(%ebp),%eax; mov 0x50(%eax),%edx; mov -0x10(%ebp),%eax;
    //   mov %edx,0x67c(%eax)`）**形态不同**且总数不变（183/184）⇒ 回滚。
    sig->m_dailyPlayCount = m_dailyPlayCount;
    sig->m_lastPlayTime = m_field54;   // unsigned→int 异型：ORIG 084a57d3 两段中转
    sig->m_giveItem = m_giveItem;
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
                                int cnt0, int cnt1, int cnt2, int resultKind)
{
    if (mode == 1)
    {
        JobKey key((ENUM_CHARACTERJOB)jobId, (unsigned char)subKey);
        std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::iterator it =
            m_jobScore_.find(key);
        if (it != m_jobScore_.end())
        {
            it->second.m_cnt0 += cnt0;
            it->second.m_cnt1 += cnt1;
            it->second.m_cnt2 += cnt2;
        }
        else
        {
            // ORIG 084a5ae8..084a5b44：新键插入值 = 本次 cnt0/cnt1/cnt2
            // （旧转写误插 0，属真语义 bug）；插入走 make_pair 临时 + pair
            // 移动构造两段（P178 嵌套实参形，p167 差集实证）。
            FAIR_PVP_SCORE_MODE_COUNT value;
            value.m_cnt0 = cnt0;
            value.m_cnt1 = cnt1;
            value.m_cnt2 = cnt2;
            m_jobScore_.insert(
                std::pair<const JobKey, FAIR_PVP_SCORE_MODE_COUNT>(
                    std::make_pair(key, value)));
        }
        m_modeStats[0][0] += cnt0;
        m_modeStats[0][1] += cnt1;
        m_modeStats[0][2] += cnt2;
    }
    else
    {
        if (resultKind == 2)
            ++m_field40;
        else if (resultKind == 3)
            ++m_field44;
        m_modeStats[1][0] += cnt0;
        m_modeStats[1][1] += cnt1;
        m_modeStats[1][2] += cnt2;
    }

    if (cnt0 != 0)
        m_recentResults_.push_back(1);
    else if (cnt1 != 0)
        m_recentResults_.push_back(2);
    else if (cnt2 != 0)
        m_recentResults_.push_back(3);

    if (m_recentResults_.size() > 10)
        m_recentResults_.pop_front();

    if (cnt1 != 0)
    {
        m_winStreak = 0;
    }
    else if (cnt0 != 0)
    {
        ++m_winStreak;
        if (m_bestWinStreak < m_winStreak)
            m_bestWinStreak = m_winStreak;
    }

    ++m_dailyPlayCount;
    alter();
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
{
    pkt.put_byte(mode);
    pkt.put_int(m_modeStats[0][0]);
    pkt.put_int(m_modeStats[1][0]);

    int winCnt = 0;
    int loseCnt = 0;
    int drawCnt = 0;
    for (std::list<unsigned char>::const_iterator it = m_recentResults_.begin();
         it != m_recentResults_.end(); ++it)
    {
        if (*it == 1)
            ++winCnt;
        else if (*it == 2)
            ++loseCnt;
        else if (*it == 3)
            ++drawCnt;
    }
    pkt.put_byte(winCnt);
    pkt.put_byte(loseCnt);
    pkt.put_byte(drawCnt);
    pkt.put_int(m_winStreak);
    pkt.put_int(m_field40);
    pkt.put_int(m_field44);
    pkt.put_int(m_bestWinStreak);

    // ORIG 084a5e5b `cmpl $1; jne <尾 leave/ret>` ⇒ 正向包裹（早返形多 jmp+nop 岛，P257）。
    if (mode == 1)
    {
        pkt.put_int(m_modeStats[0][1]);
        pkt.put_int(m_modeStats[0][2]);
        pkt.put_int(m_modeStats[1][1]);
        pkt.put_int(m_modeStats[1][2]);
        pkt.put_byte((int)m_jobScore_.size());
        for (std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::const_iterator it =
                 m_jobScore_.begin();
             it != m_jobScore_.end(); ++it)
        {
            pkt.put_byte(it->first.m_job);                     // ORIG emit dword 低字节
            pkt.put_byte((int)(signed char)it->first.m_sub);   // ORIG movsbl
            pkt.put_int(it->second.m_cnt0);
            pkt.put_int(it->second.m_cnt1);
            pkt.put_int(it->second.m_cnt2);
        }
    }
}

// ---- W 内联组（ORIG 地址见头文件注释，逐一反汇编实证） ---------------------
#include "CFairPvPScore/SetGiveItem.inc"
{
    m_giveItem = (unsigned char)v;
    alter();                                           // ORIG call CData::alter
}

#include "CFairPvPScore/GetDailyPlayCount.inc"
{
    return m_dailyPlayCount;
}

#include "CFairPvPScore/SetDailyPlayCount.inc"
{
    // [B24D判词 N] 语义正确：if(v<0)v=0(ORIG jns 钳负)；m_dailyPlayCount=v；
    // diff=mov 0x8(%ebp),%eax 与 mov 0xc(%ebp),%edx 装载序互换（GCC4.1 vs 4.4），
    // 10/10 纯 lowering，语义等价。report NO 由 nm/objdump diff 兜底。
    if (v < 0)
        v = 0;                                         // ORIG jns/clear
    m_dailyPlayCount = v;
}

#include "CFairPvPScore/SetRemainBonusCount.inc"
{
    m_remainBonusCount = (unsigned char)v;
}

#include "CFairPvPScore/SetLastTrainingSealCount.inc"
{
    m_lastTrainingSealCount = (unsigned char)v;
}

#include "CFairPvPScore/SetLastTrainingSealBonusCount.inc"
{
    m_lastTrainingSealBonusCount = (unsigned char)v;
}

#include "CFairPvPScore/GetRemainBonusCount.inc"
{
    return (int)m_remainBonusCount;
}

#include "CFairPvPScore/GetLastTrainingSealCount.inc"
{
    return (int)m_lastTrainingSealCount;
}

#include "CFairPvPScore/GetLastTrainingSealBonusCount.inc"
{
    return (int)m_lastTrainingSealBonusCount;
}

// b28d-21：删除**发明符号** `_ZN8fair_pvp12_GLOBAL__N_124IsFairPvpTrainingChannelEv`
// （ORIG nm 无此符号；P49：-O0 下 static inline helper 仍产真实 call 符号 = 既 DIFF
// 又造 missing）。ORIG 0x84a48f8 loadData 区间实测 4 次 `call G_GameWorld` +
// 4 次 `call GetChannelType`（0x84a4b??/0x84a4c?? 段）⇒ 两个使用点各直接写两次
// `G_GameWorld()->GetChannelType()`（P183 禁指针缓存，每点重发全链）。

// ---- ORIG 0x84a48f8 loadData(CUser*, char*) --------------------------------
// b28d-solo-CFairPvPScore 判词：DIFF·N 封顶（§6.2 三件套：①p167 CLEAR 91=91；
// ②gap（ORIG 848 vs 872）；③帧 0x22c vs 0x27c、全帧槽号平移 = N82/N95 族）。
// [L6S4 round5b] gap 改善：872 → **867**（ORIG 849；gap −24 → −18），修法 =
//   区块 1 条目访问宏改权威成员数组下标（`sig->m_jobScore[i]`）：旧 cast 指针算术
//   把 sig 折进基址 ⇒ 6 个访问点各多 1 条 `lea (%ecx,%eax,1)`；改后复现 ORIG
//   「索引寄存器 ecx + 基址寄存器 eax + disp 0x10」形（常量拆分差仍在：4.4
//   `lea 0x3/disp 0` vs ORIG `lea 0x2/disp 0x10`）。⇒ 本 TU 双条目视图
//   FairPvpScoreEntryPair 已无使用点，删除。
// 残余 −18 归因（超大函数，§6.2 口径）：total 段 ORIG 用槽 -0x24 逐项累加
//   （`mov -0x24,%eax; lea (%edx,%eax,1),%eax; mov %eax,-0x24`，每项 6 条）vs
//   我方单表达式寄存器累加（每项 3~4 条）⇒ 该段我方**更省**；净差全部来自块间
//   帧/槽平移（N82/N95，帧 0x22c vs 0x27c）与 N4/P30 寻址折叠、N7 寄存器分配。
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
// [L6S5 round] 867 → **857**（ORIG 848/849；gap −19 → **−9**；帧 0x27c → 0x24c，
//   槽平移 0x38 → 0x08）：
//   ① P329：两处 `const char* title/msg = findString(...)` 具名中转删掉、表达式就地
//      内联进 strncpy 实参位（ORIG `call findString; mov %eax,0x4(%esp)` 实证）⇒ −8 条；
//   ② P183：8 个 `const int bonus*` 各占 4B 具名槽（-0x28..-0x48）⇒ 删名内联 ⇒ −16 条；
//   ③ 形态复原：total 改回 ORIG 的**逐项 += 累加**（`movl $0x0,-0x24` 起步，序列
//      2*A0 → (A1+A2) → 3*B0 → (B1+B2) → bonusA×4 → sumB → bonusB×4 →
//      **2*A0 → 2*B0（尾部两项）**）⇒ +14 条，但块 65 差 −15 → −1、帧再降。
//      数值逐项核对 = 与旧单表达式**等价**（尾部 2*A0/2*B0 即旧式开头两项的位移），
//      无语义差。
//   残余 −9（15 块，幅 ±1~3）：块 0/6 的 N4/P30 常量拆分（我方 `(idx+3)*0x10+4` vs
//      ORIG `(idx+2)*0x10+0x14`）、块 44-49 EH/析构尾块序（净 −1）、块 68 失败返回
//      多 `mov $0x0,%esi`（N104 族）、块 18/20/79/93 寄存器角色（N7）。
// ORIG 0x84a48f8。
#include "CFairPvPScore/loadData.inc"
{
    SIG_FAIR_PVP_SCORE* sig = reinterpret_cast<SIG_FAIR_PVP_SCORE*>(pData);
    int idx = 0;

    // 区块 1：职业计分表（entry stride 0x10，起于 (idx+2)*0x10）
    // ORIG 084a491d..084a4a45：count 每轮重读 sig->m_jobScoreCount（无局部）；
    // key 为 make_pair 实参临时（mangled make_pair<JobKey,COUNT&> 实证）；
    // 第二实参直接引用线格式计数（`lea 0x14(%eax),%ebx`），旧版复制到局部 value
    // 会多 3 store 且丢失引用语义。
// [L6S4 P291/P183] ORIG 084a4a1c 段为「索引寄存器 + 基址寄存器 + disp」形：
//   `mov -0x34(%ebp),%ecx; mov -0x38(%ebp),%eax; add $0x2,%ecx; shl $0x4,%ecx;
//    movzbl 0x10(%ecx,%eax,1),%eax`（eax=sig 独立于索引 ecx）；旧 cast 指针
//   算术形把 sig 折进基址 ⇒ 每个访问点多 1 条 `lea (%ecx,%eax,1)`（6 处）。
// 改用权威成员数组下标：m_jobScore@+0x30 与 (idx+2)*0x10+0x10 同址。
#define FAIR_PVP_ENTRY(i) (sig->m_jobScore[i])
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
        // [L6S5 P183 + 形态复原] ORIG 084a4fxx..084a51xx：total 为**逐项 += 累加**
        //   （`movl $0x0,-0x24` 起步；每项 `mov -0x24,%eax; lea (%edx,%eax,1),%eax;
        //   mov %eax,-0x24`）。ORIG 累加序列（行号=块内序）：
        //   2*A0 → (A1+A2) → 3*B0 → (B1+B2) → bonusSumA50/60 → bonusA15/90
        //   → sumB → bonusSumB50/60 → bonusB15/90 → **2*A0 → 2*B0（尾部两项）**。
        //   旧版写成含 8 项的单一表达式（寄存器累加，更省）⇒ 与 ORIG 形态背离且
        //   无法对齐 -0x24/-0x20 槽（帧差主体）；数值等价（尾部 2*A0/2*B0 即旧式
        //   开头两项的位移）。8 个 bonus 项不用具名槽（ORIG 直接 `add %eax,-0x24`）。
        int total = 0;
        total += m_modeStats[0][0] * 2;
        total += m_modeStats[0][1] + m_modeStats[0][2];
        total += m_modeStats[1][0] * 3;
        total += m_modeStats[1][1] + m_modeStats[1][2];
        const int sumA = m_modeStats[0][0] + m_modeStats[0][1] + m_modeStats[0][2];
        total += (sumA >= 10 && sumA <= 0x31) ? 10 : 0;
        total += (sumA > 0x31) ? 60 : 0;
        total += (m_modeStats[0][0] >= 10 && m_modeStats[0][0] <= 0x31) ? 15 : 0;
        total += (m_modeStats[0][0] > 0x31) ? 90 : 0;
        const int sumB = m_modeStats[1][0] + m_modeStats[1][1] + m_modeStats[1][2];
        total += (sumB >= 10 && sumB <= 0x31) ? 10 : 0;
        total += (sumB > 0x31) ? 60 : 0;
        total += (m_modeStats[1][0] >= 10 && m_modeStats[1][0] <= 0x31) ? 15 : 0;
        total += (m_modeStats[1][0] > 0x31) ? 90 : 0;
        total += m_modeStats[0][0] * 2;
        total += m_modeStats[1][0] * 2;

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
            // [L6S5 P329] ORIG 084a51xx：findString 结果**直接**入 0x4(%esp) 喂
            //   strncpy（无具名 `title`/`msg` 中转槽）。
            strncpy(titleBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_171", 0), 0x14);
            strncpy(msgBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_177", 0), 0xff);
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
            // [L6S5 P329] 同上：第二封（178）同样内联。
            strncpy(titleBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_171", 0), 0x14);
            strncpy(msgBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_178", 0), 0xff);  // [b28d-solo] 旧版漏此调用（p167 MISSING strncpy）
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
