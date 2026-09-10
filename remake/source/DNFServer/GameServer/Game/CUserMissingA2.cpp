// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserMissingA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>  // memset

#include "CUser.h"                  // CUser/UserMercenaryInfoMgr/CHARAC_LOAD_MERCENARY/_Charac_info
#include "CInventory.h"             // CInventory::GetInvenRef(int,int) const、INVEN_TYPE_INVEN=0
#include "CDataManager.h"           // G_CDataManager / find_item(int) const
#include "CEquipItem.h"             // CEquipItem 完整类型（dynamic_cast 目标 + IsAvatarItem 槽 4）
#include "CFatigueBatteryHandle.h"  // 本类声明
#include "GameTypes.h"              // cMyTrace(name, line, flag) + operator()
#include "GlobalData.h"             // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"             // StreamPool::Acquire(char const*, int)
#include "W8LT_DBTask.h"            // DB_UpdateBingoEvent 权威声明（makeRequest/dispatch）
#include "MsgQueueMgr.h"            // MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)
#include "SigTypes.h"               // SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION
#include "DB_LoadCharacHotKeyOption.h"
#include "CItem.h"                // CItem::get_rarity（Reward 尾部）
#include "CItemList.h"            // CItemList::create_item / ENUM_ITEM_CREATE_TYPE
#include "CMailBoxHelper.h"       // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "CSystemTime.h"          // CSystemTime::getCurSec
#include "DB_MercenaryLogInfo.h"  // DB_MercenaryLogInfo::makeRequest
#include "PacketTypes.h"          // competition_reward_result 完整定义
#include "PvPTypes.h"             // CMTRand（probArea/probabilityRewardItem 局部 static）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include <stdio.h>                // sprintf / strncpy / strlen
#include "PacketGuard.h"           // PacketGuard（SendUnlimitChallengePacket）
#include "InterfacePacketBuf.h"    // put_header / put_byte / finalize
#include "CEventScriptMng.h"       // EventClassify::CEventScriptMng::process_level_up_reward
#include "Quest.h"               // Quest / RewardItemInfo 完整类型

// ----------------------------------------------------------------------------
// ORIG W 模板（本 TU 消费；符号 0x081b48da/<int> 0x081b4a66 / 0x081b495d）：
// 遍历累加 first 概率，dice<=累计 命中返回 second；未命中回退首元素 second。
// ----------------------------------------------------------------------------
template <typename T>
T FindRelativeProb(const std::vector<std::pair<int, T> >& vec, int dice)
{
    int sum = 0;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        sum += vec[i].first;
        if (dice <= sum)
            return vec[i].second;
    }
    return vec[0].second;
}

// ----------------------------------------------------------------------------
// ORIG W 0x081b495d：取 first<=level 中 first 最大的元素，返回其 second。
// ----------------------------------------------------------------------------
template <typename T>
T Find_max_value_lower_N(int level,
                         const std::vector<std::pair<int, T> >& vec)
{
    int bestIdx = 0;
    int bestFirst = 0;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        // [b28d-27] ORIG 0x081b495d：两条件各自 setcc 物化（P225 推广，`&&`
        // 合并 4.4 恒折 jg/jl 直跳）⇒ 拆嵌套 if。
        if (vec[i].first <= level)
        {
            if (vec[i].first >= bestFirst)
            {
                bestFirst = vec[i].first;
                bestIdx = i;
            }
        }
    }
    return vec[bestIdx].second;
}

// ----------------------------------------------------------------------------
// ORIG b 0x09407e80（.bss 全局 string，internal linkage；初值推断为空串，
// 运行期默认构造。GetRewardItem/Reward 用作兜底邮政消息）。
// ----------------------------------------------------------------------------
namespace MercenaryHardCodingSettingValue
{
    static std::string MERCENARY_BASE_POSTAL_MESSAGE;
    // ORIG r 0x08bb8754 = 0x2710 (10000)，_ZN31MercenaryHardCodingSettingValueL19ITEM_DROP_MAX_VALUEE
    static const int ITEM_DROP_MAX_VALUE = 10000;
}

// ----------------------------------------------------------------------------
// 11. ORIG T 0x081b2b34 sz 0x8f。m_vec40 同型检查；再判
// level >= m_areaLevel（ushort 符号扩展后与 level 比较）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckCompetitionAreaIndex.inc"
                                                      int level) const
{
    // ORIG：G_CDataManager() 在 empty()/size()/operator[] 三处各自重调（禁缓存，
    // P183/P48）；经非 const CDataManager* 直访 m_vec40 选非 const operator[]。
    // ORIG 0x081b2b53：`call empty; test %al,%al; jne L_true`（**无 xor**）+
    // 0x081b2b69 `cmp %edx,%eax; jae L_false` ⇒ 原码是正极性短路
    // `if (empty() || size < (unsigned)areaIdx) return 0;`（原实现写
    // !empty && … 出 xor$1 物化 + ja，块序全反）。bool 具名局部会额外占槽
    // （4.4 出 movb/cmpb 对，帧 0x34 vs ORIG 0x24）⇒ 必须无局部直写 if。
    if (G_CDataManager()->m_mercenarySystem.m_vec40.empty() ||
        G_CDataManager()->m_mercenarySystem.m_vec40.size() <
            static_cast<unsigned int>(static_cast<int>(areaIdx)))
        return false;
    if (level < static_cast<int>(static_cast<short>(
            G_CDataManager()->m_mercenarySystem
                .m_vec40[static_cast<int>(areaIdx)].m_areaLevel)))
        return false;
    return true;
}

// ----------------------------------------------------------------------------
// 2. ORIG T 0x08692e9c sz 0x55。
// cashedCharacJob==-1 → 1（首次加载放行）；否则两次调 GetCharacHotKeyType 比较，
// 相同→0、不同→1（类型变化才允许重新加载热键）。
// [P210] 本函数定义前置到 callee GetCharacHotKeyType 定义之前：4.4 对 TU 内
// 已见定义的 callee 精确继承 outgoing（帧塌缩 0x14→0x8），前置后恢复 ORIG
// 保守帧 sub $0x14（b28 P210 派生附注同法）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckLoadHotKeyCashedCharacType.inc"
{
    if (cashedCharacJob == -1)
        return true;

    // P0/N96 去绑定 + P226 裸语句表达式升格：`({ callee(); })` 单表达式形令 4.4
    // 将首结果升格 %ebx 跨次 call 保活并出 `mov %eax,%ebx; cmp %eax,%ebx;
    // sete %al; test; je` 物化形（probe p7.t7 实证 push ebx+sub 0x14 帧同 ORIG）。
    if (({ GetCharacHotKeyType(cashedCharacJob); }) == GetCharacHotKeyType(characJob))
        return false;
    return true;
}

// ----------------------------------------------------------------------------
// 1. ORIG T 0x08692ef2 sz 0x17。
// cmpl $0xa,param; jne→0；相等→1。返回值 eax 全宽（头文件声明 int，调用点
// LoadCharacTypeHotKey 以 mov %eax 全宽取值后截 byte；语义 characJob==10）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetCharacHotKeyType.inc"
{
    if (characJob == 10)
        return 1;
    return 0;
}

// ----------------------------------------------------------------------------
// 3. ORIG T 0x0868ea68 sz 0x141。雇佣兵 avatar 等级加成 = 全身 avatar 装备的最低
// 品阶：初值 10；getCurCharacR/getCurCharacInvenR 为空→0；否则 i∈[0,7] 遍历
// GetInvenRef(INVEN, i)：空引用/find_item 失败/dynamic_cast<CEquipItem*> 失败/
// IsAvatarItem()==0（vtable+0x10 槽 4，两处 dtor 后第 5 槽）任一即返 0；
// 否则取 min(get_grade)；循环后 >3 归零。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x0868ea68；p167 CLEAR（8/8），残差为帧/槽号与
// callee-saved 分配（4.1 保守帧 vs 4.4 紧凑帧），语义闭合。
// [b28d-solo-CUserMissingA2] 修复⇒NEAR：去 `CItem* item` 具名局部（dynamic_cast
// 直调 find_item 结果，ORIG 0x0868eb0f 无 item 槽）+ P222 正向包裹 equip 判定 +
// 尾部 `if (3 < best) return 0;` ⇒ 槽位集与 ORIG 全等（best/-0x1c、inven/-0x18、
// i/-0x14、ref/-0x10、equip/-0xc）。残差仅 N7（虚调用目标 4.4.7 恒 %eax vs ORIG
// %edx）+ .o 重定位伪差；p167 CLEAR 8/8。
#include "CUserMissingA2/CalcMercenaryAvatarBonus.inc"
{
    int best = 10;

    if (getCurCharacR() == 0)
        return 0;

    const CInventory* inven = getCurCharacInvenR();
    if (inven == 0)
        return 0;

    for (int i = 0; i < 8; ++i)
    {
        const Inven_Item* ref = inven->GetInvenRef(INVEN_TYPE_INVEN, i);
        if (ref == 0)
            return 0;

        // ref->m_addInfo（+0x2）为 item idx；ORIG 0x0868eaeb 在 find_item 返回 0 时
        // 直接给 equip 赋 0（`test %eax,%eax; jne L; mov $0,%eax; jmp after; L:
        // dynamic_cast`）⇒ 不设 `CItem* item` 具名局部（会多占一槽使全帧深 4）。
        // dynamic_cast<CEquipItem*>(0) 返回 0，语义与 ORIG 一致。
        CEquipItem* equip = dynamic_cast<CEquipItem*>(
            G_CDataManager()->find_item(ref->m_addInfo));
        // P222 正向包裹：ORIG 0x0868eb32 `cmpl $0,-0xc; je <+0x112 return-0 岛>`
        // （false 块归共享尾），4.4 对 `if (equip == 0) return 0;` 恒产
        // `jne 继续; mov $0,%eax; jmp` 反形 ⇒ 改正向包裹。
        if (equip != 0)
        {
            if (equip->IsAvatarItem())
            {
                if (equip->get_grade() < best)
                    best = equip->get_grade();
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    if (3 < best)
        return 0;
    return best;
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x0868ebaa sz 0xb1。
// bonus∈[0,3] 且 getCurCharacR() 非空时：characNo=*getCurCharacR()（+0 dword）→
// m_mercenaryMgr.FindIndexByCharacNo → !=-1 且 CheckCharacIndex((uchar)idx) →
// vector 元素 +0x32（m_avatarBonus）写 bonus。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/setAvatarTypeBonus.inc"
{
    if (bonus < 0 || 3 < bonus)
        return;

    if (getCurCharacR() == 0)
        return;

    int idx = m_mercenaryMgr.FindIndexByCharacNo(
        static_cast<unsigned int>(getCurCharacR()->m_characNo));
    if (idx == -1)
        return;
    if (!m_mercenaryMgr.CheckCharacIndex(static_cast<unsigned char>(idx)))
        return;

    m_mercenaryMgr.m_characs[idx].m_avatarBonus =
        static_cast<unsigned char>(bonus);
}

// ----------------------------------------------------------------------------
// 5. ORIG T 0x081b42ae sz 0x51。按 m_characs[i].m_characNo（首 dword 无符号比较）
// 线性查找；未命中返回 0xffffffff。循环条件 seta（size()>idx 无符号大于）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/FindIndexByCharacNo.inc"
{
    size_t idx = 0;
    while (m_characs.size() > idx)
    {
        if (static_cast<unsigned int>(m_characs[idx].m_characNo) == characNo)
            return static_cast<unsigned int>(idx);
        ++idx;
    }
    return 0xffffffffU;
}

// ----------------------------------------------------------------------------
// 6. ORIG T 0x081b4262 sz 0x4b。empty→1；size>=index（jae 无符号，含等）→1；
// size<index→0（汇编实证：empty 分支经 eax=0→je→返回 1，仅 size<index
// 落到返回 0）。ORIG 边界 index<=size 与调用点 FindIndexByCharacNo
// 命中下标恒 <size 相容。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckCharacIndex.inc"
{
    // ORIG 全形复现（同 CheckCompetitionPeriodIndex g2 形）：empty() 直判
    // test/jne；idx 拓宽后置（size call 后 movzbl -0xc,%edx；cmp %edx,%eax；jae）；
    // 尾 !invalid choose（探针 /tmp/probe_l8j3.o g2 同构实证）。
    if (m_characs.empty() || m_characs.size() < static_cast<unsigned int>(index))
        return false;
    return true;
}

// ORIG T 0x081b45ce：未找到项返回等待；命中项以 GetCompetitionState()==0 判等待。
#include "CUserMissingA2/IsWaitingStateCharac.inc"
{
    // b28（P211 同族）：index 用 int ⇒ vector operator[](size_t) 实参转换
    // 中转触发 RHS 值先载 = ORIG `mov -0xc,%edx; mov this,%eax`（探针
    // /tmp/l12p3.cpp w4 vs w1/w5）。
    const int index = (int)FindIndexByCharacNo(characNo);
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return true;
    return m_characs[index].GetCompetitionState() == 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081b4198：线性查找 characNo 相同项；命中且 level 不同则更新
// level 并 alter()，返回 0；命中 level 相同也返回 0；未命中返回 1。
// 循环条件 seta（size()>i 无符号），参数首 dword 每次迭代载入 ebx（P36）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckValidAppend.inc"
{
    size_t i = 0;
    while (m_characs.size() > i)
    {
        if (info.m_characNo == m_characs[i].m_characNo)
        {
            if (m_characs[i].m_level != info.m_level)
            {
                m_characs[i].m_level = info.m_level;
                m_characs[i].alter();
                return 0;
            }
            return 0;
        }
        ++i;
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081b44c0：user 空→0；FindIndexByCharacNo + CheckCharacIndex 收窄
// 校验；有效则 m_characs[idx].Reward(user, reward) 直传返回值（无规范化）。
// [b28d-27] 返回类型 int→bool（P9）：ORIG 尾 `call Reward; leave; ret` 直传单字节，
// 无 movzbl 规范化；CUser.h:791 已由 Main 代落（mangled 不含返回类型，ABI 零影响）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/ReturnCharac.inc"
                                        competition_reward_result& reward)
{
    if (user == 0)
        return 0;
    const int index = (int)FindIndexByCharacNo(characNo);  // b28 P211 同族
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return 0;
    return m_characs[index].Reward(user, reward);
}

// ----------------------------------------------------------------------------
// ORIG T 0x081b4536：无效下标→0；有效项 Competition(user,modeA,modeB) 成功→1。
// modeB/modeA 先 movsbl 入 esi/ebx 跨 operator[] 存活（P52）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CompetitionCharac.inc"
                                            char modeA, char modeB)
{
    const int index = (int)FindIndexByCharacNo(characNo);  // b28 P211 同族
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return 0;
    if (!m_characs.operator[](index).Competition(user, modeA, modeB))
        return 0;
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868e692：转发 UserMercenaryInfoMgr::ReturnCharac（receiver
// lea this+0x6ef6c）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/Return_Mercenary_Charac.inc"
                                    competition_reward_result& reward)
{
    m_mercenaryMgr.ReturnCharac(this, mercUid, reward);
    // ORIG 0x0868e692 为 void 转发（leave; ret 无 eax 设置），调用点取 %al 残留——
    // 此处不写 return 以复刻 ORIG 的未定义行为。
}

// ORIG T 0x081b2e86：返回 1=竞技中、0=未报名、2=已结束且已报名。
#include "CUserMissingA2/GetCompetitionState.inc"
{
    const int remaining = m_field2c - GlobalData::s_systemTime_.getCurSec();
    if (remaining > 0)
        return 1;
    if (m_field30 == 0xff)
        return 0;
    return 2;
}

// ----------------------------------------------------------------------------
// 6b. ORIG C1 W 0x08694c08 sz 0x86（报告缺失，objdump 取证）：与 Reset 字段
// 集一致但顺序不同 —— 标量在前（+0/+4/+0x24/+0x28/+0x2c 清零、
// +0x30/0x31=0xff、+0x32=0xa），两个 memset 在后（+6,0x1e / +0x34,0x20）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CHARAC_LOAD_MERCENARY.inc"
{
    m_characNo = 0;
    m_level = 0;
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0xff;
    m_field31 = 0xff;
    m_avatarBonus = 0xa;
    memset(m_name, 0, 0x1e);
    memset(m_avatarItems, 0, 0x20);
}

// ----------------------------------------------------------------------------
// 7. ORIG T 0x081b410a sz 0x85。字段清零/初始化顺序逐条对应汇编：
// +0 dword=0、+4 word=0、memset(+6,0,0x1e)、memset(+0x34,0,0x20)、
// +0x24 byte=0、+0x28 dword=0、+0x2c dword=0、+0x30=0xff、+0x31=0xff、+0x32=0xa。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/Reset.inc"
{
    m_characNo = 0;
    m_level = 0;
    memset(m_name, 0, sizeof(m_name));                // 0x1e
    memset(m_avatarItems, 0, sizeof(m_avatarItems));  // 0x20
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0xff;
    m_field31 = 0xff;
    m_avatarBonus = 0xa;
}

// ----------------------------------------------------------------------------
// 9. ORIG T 0x0842f710 sz 0xe3。DB 请求入队：
// Acquire("DBThread.cpp",0x749a) → CStreamGuard(stream,true) → opcode 800(0x320)
// → uid(int) → GetInBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION> 写 +0 accId(uint)、
// +4 hotkeyType(int) → put(DB_Q=2, guard)。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/makeRequest.inc"
                                            int hotkeyType)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x749a), true);
    guard.operator*()->operator<<(800);   // SIG_LOAD_CHARAC_HOTKEY_OPTION opcode
    guard.operator*()->operator<<(uid);

    struct View
    {
        unsigned int m_accId;             // +0x00（dispatch 作 SQL where 键）
        int m_hotkeyType;                 // +0x04
    };
    View* v = reinterpret_cast<View*>(
        guard.operator->()->GetInBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION>());
    v->m_accId = accId;
    v->m_hotkeyType = hotkeyType;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// CHARAC_LOAD_MERCENARY 竞技域 15 函数（ORIG 报告：docs/class_func_reports/
// CHARAC_LOAD_MERCENARY/*.md；alter/save_complete 报告缺失按 objdump 取证）。
// ============================================================================

// ----------------------------------------------------------------------------
// 10. ORIG T 0x081b2bc4 sz 0x59。G_CDataManager()->m_vec14：
// empty→0；!empty 且 size>=(signed)periodIdx→1；否则 0。movsbl 符号扩展。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CheckCompetitionPeriodIndex.inc"
{
    // ORIG：G_CDataManager() 在 empty() 判与 size() 判两处各自重调（禁缓存，P183/P48）。
    // ORIG 全形复现：empty() 直判 test/jne 无 xor；idx 拓宽后置（size call 后
    // movsbl -0xc,%edx；cmp %edx,%eax；jae）；尾 !invalid choose（探针
    // /tmp/probe_l8j3.o g2 实证 28 条逐条全中）。
    if (G_CDataManager()->m_mercenarySystem.m_vec14.empty()
        || G_CDataManager()->m_mercenarySystem.m_vec14.size()
               < static_cast<unsigned int>(static_cast<int>(periodIdx)))
        return false;
    return true;
}

// ----------------------------------------------------------------------------
// 12. ORIG T 0x081b2c94 sz 0x75。m_vec20 表按 m_avatarBonus 查 float。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetAvatarBonus.inc"
{
    // [b28d-27] ORIG 0x081b2c94：无 bool 槽——`vec.empty() || (signed char)size <
    // (signed char)m_avatarBonus` 直写条件产 eax 寄存器物化（mov $1/$0; test; je），
    // 比较为 8 位有符号（ORIG `cmp %al,%dl; jge`；探针 /tmp/d27_p5.cpp f4 逐条同形，
    // 32 位无符号形产 jae 见 f1）。
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec20;
    if (vec.empty() || static_cast<signed char>(vec.size()) <
                           static_cast<signed char>(m_avatarBonus))
        return 0.0f;
    return vec[static_cast<int>(static_cast<char>(m_avatarBonus))].second;
}

// ----------------------------------------------------------------------------
// 13. ORIG T 0x081b2c1e sz 0x75。m_vec14 表按 m_field31 查 float。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetPeriodBonus.inc"
{
    // [b28d-27] ORIG 0x081b2c1e：同 GetAvatarBonus（8 位有符号比较 + 无 bool 槽）。
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec14;
    if (vec.empty() || static_cast<signed char>(vec.size()) <
                           static_cast<signed char>(m_field31))
        return 0.0f;
    return vec[static_cast<int>(static_cast<char>(m_field31))].second;
}

// ----------------------------------------------------------------------------
// 14. ORIG T 0x081b2d6a sz 0x11b。base=Find_max_value_lower_N(...).second；
// rate=(int)(term*base*period*avatar)（fistpl 截断）；GM 打印调试；
// rate<1 归 -1。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/CalcItemDropRate.inc"
{
    // ORIG 0x081b2d70：`call G_CDataManager; add $0xa8e4,%eax; mov %eax,-0x20`
    // —— m_vec8 先取地址落具名引用局部槽（P0：有槽往返 ⇒ 原码有具名局部），
    // 再作 Find_max_value_lower_N 第二实参（原实现无该局部，实参位置直接
    // 求值出 lea %ecx，帧少 4 且槽号全平移）。
    const std::vector<std::pair<int, std::pair<int, int> > >& vec8 =
        G_CDataManager()->m_mercenarySystem.m_vec8;
    std::pair<int, int> basePair = Find_max_value_lower_N<std::pair<int, int> >(
        m_level, vec8);
    int base = basePair.second;
    float period = GetPeriodBonus();
    float avatar = GetAvatarBonus();
    int term = GetCompetitionPeriodTerm(0);
    // ORIG 0x081b2dd8：`fildl base; fmuls period; fmuls avatar; fildl term;
    // fmulp` ⇒ x87 求值序是 base*period*avatar*term（写 term*base*… 4.4 出
    // fildl term 先 + fmulp 前置，指令序与 ORIG 相反）。
    int rate = (int)((float)base * period * avatar * (float)term);
    if (user != 0 && user->isGMUser())
    {
        static char buffer[0x100];
        sprintf(buffer,
                "\nbase:%d * period:%.2f * avatar:%.2f * term:%d = %d\n",
                base, (double)period, (double)avatar, term, rate);
        SendMessageToCUser(user, buffer);
    }
    // ORIG 0x081b2e7a：`mov $0xffffffff,%eax; jmp` ⇒ 源码是 `return -1;`
    // 直接返回（写回 rate 局部再 return 会多一条 movl 到槽）。
    if (rate < 1)
        return -1;
    return rate;
}

// ----------------------------------------------------------------------------
// 15. ORIG T 0x081b2d0a sz 0x5f。dice<0 或 dice>m_field3c（0xa918）→0；
// 否则 FindRelativeProb<float>(m_vec30, dice)。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetRewardCritical.inc"
{
    if (dice < 0 || G_CDataManager()->m_mercenarySystem.m_field3c < dice)
        return 0.0f;
    // 引用局部槽形：ORIG `call; add $0xa90c,%eax; mov %eax,-0xc` + 传参三步
    // （探针 /tmp/probe_l8h3.o 实证 30 条逐条全中，含大偏移 add 形）。
    const std::vector<std::pair<int, float> >& probs = G_CDataManager()->m_mercenarySystem.m_vec30;
    return FindRelativeProb<float>(probs, dice);
}

// ----------------------------------------------------------------------------
// 16. ORIG T 0x081b2ed2 sz 0x5f。period 有效时返回
// vec14[periodIdx].first * m_field0（每期秒数）。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82附/P52：ORIG 0x081b2ed2；ORIG push esi + sub 0x20（值跨 call
// 保活 callee-saved），4.4 延迟装载 + 帧 0x24；p167 CLEAR（4/4），语义闭合。
// [b28d-solo-CUserMissingA2] 修复：整数乘操作数交换为 `m_field0 * vec[..].first`
// （P243 同族）⇒ 32/32 与 ORIG 逐条同形（m_field0 先入 %esi 跨 call 保活 +
// push esi/sub $0x20 帧），转 AE。
#include "CUserMissingA2/GetCompetitionPeriodSecond.inc"
{
    if (!CheckCompetitionPeriodIndex(periodIdx))
        return 0;
    // 【b28c-13 清绑】原 register asm("esi") 绑定按 P0 全域禁用，已删除。
    // ORIG 0x081b2f06：`call G_CDataManager; mov 0xa8dc(%eax),%esi`（右操作数
    // 先物化并升格 callee-saved，跨下一次 call 保活）⇒ P226 裸语句表达式形
    // 复现值升格（P200 方向：值作普通表达式直传，不经具名中间局部）。
    // P243 同族（整数乘）：ORIG 0x081b2efd 先求 m_field0（→%esi 跨 call 保活）再
    // 求 vec[...].first（→%eax），`imul %esi,%eax` ⇒ 源写「m_field0 * first」。
    return (int)G_CDataManager()->m_mercenarySystem.m_field0
           * G_CDataManager()->m_mercenarySystem
                 .m_vec14[static_cast<int>(periodIdx)].first;
}

// ----------------------------------------------------------------------------
// 17. ORIG T 0x081b2f32 sz 0x113。m_field31 期次检查 + 表越界→0；
// term=(getCurSec()-m_field28)/m_field0；term<期次表值→out=true；
// 否则 term=表值（else 分支二次 operator[] 照 ORIG）。
// ----------------------------------------------------------------------------
// [b28d-solo-CUserMissingA2②] **AE**（本波 DIFF→AE）：ORIG 0x081b2f32。
// 关键 = P226 裸语句表达式：分子段 ORIG 0x081b2fa5 先求 getCurSec（call→eax→
// `mov %eax,%edx`）再读 m_field28（`mov 0x28(%eax),%eax`），4.4 对裸
// `getCurSec() - m_field28` 恒先读内存（`mov 0x28(%edx),%edx; sub %edx,%eax`
// 单段）；改 `({ GlobalData::s_systemTime_.getCurSec(); }) - m_field28` 后逐条
// 复现 ORIG 6 条形。p167 CLEAR 8/8。
#include "CUserMissingA2/GetCompetitionPeriodTerm.inc"
{
    if (!CheckCompetitionPeriodIndex(static_cast<char>(m_field31)))
        return 0;
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec14;
    // ORIG 0x081b2f76：`call empty; test; jne L1; call size; cmp %al,%dl; jge L2;
    // L1: mov $1; L2: mov $0; test; je body` —— 8 位有符号比较 + 无具名 bool
    // （P158 反向：具名 bool 产 -0xd 槽往返，ORIG 用 %eax 直接物化）。
    if (vec.empty() ||
        static_cast<char>(vec.size()) < static_cast<char>(m_field31))
        return 0;
    // ORIG 0x081b2fcb：分子先落 -0xc 槽（`mov %eax,-0xc` + 回读 →%ebx），除法
    // 后再写回同一槽 ⇒ 两句式（P38 拆语句）；`divl`（无符号）⇒ 商按 unsigned 求。
    // [b28d-solo-CUserMissingA2②] P226 条件/实参向语句表达式：ORIG 0x081b2fa5 段
    // 先求 getCurSec（call→eax→mov edx,eax）再读 m_field28，4.4 恒先读内存
    // ⇒ 裸语句表达式强制左操作数先物化。
    int term = ({ GlobalData::s_systemTime_.getCurSec(); }) - m_field28;
    term = static_cast<int>(
        static_cast<unsigned int>(term)
        / static_cast<unsigned int>(
            G_CDataManager()->m_mercenarySystem.m_field0));
    if (term < vec[static_cast<int>(static_cast<char>(m_field31))].first)
    {
        if (isFinalPeriod != 0)
            *isFinalPeriod = true;
    }
    else
    {
        term = vec[static_cast<int>(static_cast<char>(m_field31))].first;
    }
    return term;
}

// ----------------------------------------------------------------------------
// 18. ORIG T 0x081b4190 sz 0x7。空函数（仅 prologue/epilogue）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/SendMessageToCUser.inc"
{
    // ORIG 0x081b4193：`sub $0x10,%esp`（空体但帧 0x10）⇒ 原码有一个未使用的
    // 0x10 字节局部（P31 dead local；b28c 未用局部占槽实证）。
    char buf[0x10];
    (void)buf;
}

// ----------------------------------------------------------------------------
// 19. ORIG W 0x081b47c8 sz 0xc：movb $1,0x24(%eax)（竞技报名后置位）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/alter.inc"
{
    m_field24 = 1;
}

// ----------------------------------------------------------------------------
// 20. ORIG W 0x08694c8e sz 0xc：movb $0,0x24(%eax)（结算后清除）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/save_complete.inc"
{
    m_field24 = 0;
}

// ----------------------------------------------------------------------------
// 21. ORIG T 0x081b3046 sz 0x22f。金奖励：
// base=Find_max_value_lower_N(...).first；gold=(int)(term*base*period*avatar+0.01)；
// criticalDice=randInt(m_field3c)；critical=GetRewardCritical；
// GM 打印；result.m_0=(int)(term*base*period*critical+0.01)；
// nCritical=(int)(gold*critical+0.01)；result.m_4=nCritical-result.m_0。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetRewardGold.inc"
                                         competition_reward_result& result) const
{
    // 引用局部槽形：ORIG `call; add $0xa8e4,%eax; mov %eax,-0x3c` 首句缓存
    // （同 GetRewardCritical 探针形）。
    const std::vector<std::pair<int, std::pair<int, int> > >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec8;
    unsigned long range;    // ORIG 槽 -0x40（高地址）：4.4 POD 槽逆序分配，
                            // 提前声明使其落 basePair sret 槽之后
    std::pair<int, int> basePair = Find_max_value_lower_N<std::pair<int, int> >(
        m_level, vec);
    int base = basePair.first;
    float period = GetPeriodBonus();
    float avatar = GetAvatarBonus();
    int term = GetCompetitionPeriodTerm(0);
    // 乘序按 ORIG x87 折叠序：fildl base→fmuls period→fildl term→fmulp→fmuls avatar
    int gold = (int)(base * period * term * avatar + 0.01f);

    static CMTRand probabilityRewardItem;
    range = static_cast<unsigned long>(
        G_CDataManager()->m_mercenarySystem.m_field3c);
    int criticalDice = probabilityRewardItem.randInt(range);
    float critical = GetRewardCritical(criticalDice);
    if (user->isGMUser())
    {
        static char buffer[0x100];
        sprintf(buffer,
                "term:%d, base gold:%d\nperiod:%.2f, avatar:%.2f\n"
                "%d * %.2f * %d * %.2f = %d\n"
                "critical Dice:%d/10,000 value:%.2f",
                term, base, (double)period, (double)avatar, base, (double)period,
                term, (double)avatar, gold, criticalDice, (double)critical);
        SendMessageToCUser(user, buffer);
    }
    int nCritical = (int)((float)gold * critical + 0.01f);
    result.m_0 = (int)(base * period * term * critical + 0.01f);
    result.m_4 = nCritical - result.m_0;
    return nCritical;
}

// ----------------------------------------------------------------------------
// 22. ORIG T 0x081b3276 sz 0x59d。物品奖励（长流程）：
// 重置 item、msg 置兜底邮政消息；area 校验；m_areaLevel==0 归零 m_field30；
// itemDropDice=randInt(10000)（GM 用户强制 0）；>=dropRate 失败；
// groupDice=randInt(m_groupIdSum)；GM 打印；按组累加选组（越界回绕 0）；
// msg=组名；mobList 非空走 mob 分支（map24 查独立掉落表），否则 item 分支
// （m_itemList 累加）；命中写 item+create_item+result.m_8=1。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（cv 旋钮待修）：ORIG 0x081b3276；p167 差集显示 ORIG 用 const
// 迭代器/const operator[]（_ZNKSt6vector...ixEj ×4、_ZNKSt17_Rb_tree_iterator...ptEv ×3、
// _ZNKSt6vector...emptyEv），我方用非 const 版（+4/+3/+2）⇒ N89/P96 封顶。
// [b28d-solo-CUserMissingA2] 修复（真语义 bug）：m_itemList 的 pair 语义为
// (概率, 道具id)——`sum2 += .first`、命中 `item.m_addInfo = .second`
// （ORIG 0x081b376c `mov (%eax),%eax` / 0x081b3790 `mov 0x4(%eax),%eax` 实证；
// 原实现两处互换 ⇒ 概率累加错、道具 id 取错）。另：pArea 表达式内联（消 -0x48
// 具名槽）、`itemDropDice >= dropRate` 比较方向翻正（ORIG `cmp dropRate,%eax; jl`）。
// [b28d-solo-CUserMissingA2②] 本波补齐两处真缺口 ⇒ **p167 50/50 CLEAR**：
// ① item 分支正向 empty 包裹 `if (!group.m_itemList.empty())`（ORIG 0x081b3786
//    `mov eax,[ebp-0x38]; add eax,0x14; call _ZNKSt6vectorISt4pairIiiESaIS1_EE5emptyEv;
//    xor $1; test; je <return-0>` 实证——原实现直接进 for 少一次 const empty）；
// ② dropLists 越界守卫改早返形 `if ((int)dropLists.size() <= it->second.first)
//    return false;`（ORIG 0x081b3636 `call size; mov ebx,eax; call ptEv; mov
//    eax,[eax+0x4]; cmp ebx,eax; setle; test; je <主流程>` 实证）。
// [b28d-solo-CUserMissingA2③] 本轮重核（dual_rank gap 6）⇒ 修真差异 + gap 6→1：
//   ① **dropDice 误建为函数级 static**：ORIG 0x081b3648 `mov %eax,-0x2c(%ebp)` /
//      0x081b3673 `mov -0x2c(%ebp),%eax` 是栈槽，且 nm 的 6 个 .bss 局部符号
//      （itemDropDice/groupDice/mobDice/itemDice/buffer/probabilityRewardItem）**不含**
//      dropDice ⇒ 原码是普通局部 int（原实现多写一个 .bss 符号且读写形态不同）。
//   ② groupSum/groupIdx 声明序翻正（ORIG 0x081b346e 先清 -0x40=groupSum、后清
//      -0x3c=groupIdx；原实现槽互换）。
//   ③ 复现 ORIG 形态：groupIdx 重复清零（0x081b347c）、mob/item 两分支在循环后补
//      显式 `return false` 使 4.4 分出独立 return-false 岛（ORIG 0x081b36f6/0x081b3700
//      /0x081b37fa/0x081b3803 四岛；原实现共享单一出口，少 5 条）。
//   残差 = 1 条 `jmp` 中继岛 + 尾部对齐 `nop`（§6.1 豁免）⇒ N112 封顶；
//   p167 50/50（MISSING/EXTRA 仅 CMTRand C1↔C2 同址别名，P262）。
#include "CUserMissingA2/GetRewardItem.inc"
    CUser* user, competition_reward_result& result, Inven_Item& item,
    std::string& msg)
{
    if (user == 0)
        return false;

    item.reset();
    msg = MercenaryHardCodingSettingValue::MERCENARY_BASE_POSTAL_MESSAGE;
    if (!CheckCompetitionAreaIndex(static_cast<char>(m_field30), m_level))
        return false;

    // ORIG 0x081b32d8：`call operator[]; movzwl 0x14(%eax),%eax` 无 pArea 具名槽
    // （P157/P183）⇒ 内联表达式直读。
    if (G_CDataManager()->m_mercenarySystem
            .m_vec40[static_cast<int>(static_cast<char>(m_field30))]
            .m_areaLevel == 0)
        m_field30 = 0;

    static CMTRand probabilityRewardItem;
    int dropRate = CalcItemDropRate(user);
    unsigned long range = 10000;
    static int itemDropDice;
    itemDropDice = probabilityRewardItem.randInt(range);
    if (user->m_field6ef68 != 0)
        itemDropDice = 0;
    // ORIG 0x081b33c6：`mov itemDropDice,%eax; cmp dropRate,%eax; jl <继续>`
    // ⇒ 源码比较方向为 itemDropDice < dropRate（P196 折痕：jg↔jl 同义）。
    if (itemDropDice >= dropRate)
        return false;

    const stMercenaryCompetitionArea* pArea2 = &G_CDataManager()
        ->m_mercenarySystem.m_vec40[static_cast<int>(static_cast<char>(m_field30))];
    unsigned long groupRange =
        static_cast<unsigned long>(pArea2->m_groupIdSum);
    static int groupDice;
    groupDice = probabilityRewardItem.randInt(groupRange);
    if (user->isGMUser())
    {
        static char buffer[0x100];
        sprintf(buffer, "dice: %d/%d\n", groupDice, pArea2->m_groupIdSum);
        SendMessageToCUser(user, buffer);
    }

    // ORIG 0x081b346e/0x081b3475/0x081b347c：groupSum 占 -0x40、groupIdx 占 -0x3c，
    // 且 groupIdx 连续清零两次 ⇒ 声明序 groupSum 在前 + 循环前一次显式重清（P198① 同族）。
    int groupSum = 0;
    unsigned int groupIdx = 0;
    groupIdx = 0;
    while (groupIdx < pArea2->m_groups.size())
    {
        groupSum += pArea2->m_groups[groupIdx].m_groupId;
        if (groupDice <= groupSum)
            break;
        ++groupIdx;
    }
    if (pArea2->m_groups.size() <= groupIdx)
        groupIdx = 0;

    const stMercenaryRewardGroup& group = pArea2->m_groups[groupIdx];
    msg = group.m_name;

    if (!group.m_mobList.empty())
    {
        unsigned long mobRange =
            static_cast<unsigned long>(group.m_field20);
        static int mobDice;
        mobDice = probabilityRewardItem.randInt(mobRange);
        int mobIdx = FindRelativeProb<int>(group.m_mobList, mobDice);

        std::map<int, std::pair<int, int> >::iterator it =
            G_CDataManager()->m_independentDrop.m_map24.find(mobIdx);
        if (it != G_CDataManager()->m_independentDrop.m_map24.end())
        {
            const std::vector<IndependentDropListInfo>& dropLists =
                G_CDataManager()->m_independentDrop.m_vec0;
            // [b28d-solo-CUserMissingA2②] ORIG 0x081b3636 段 `call size; mov
            // ebx,eax; call ptEv; mov eax,[eax+0x4]; cmp ebx,eax; setle; test;
            // je <主流程>`（有符号 size<=first 早返 return-0 岛）⇒ 反向早返形。
            if ((int)dropLists.size() <= it->second.first)
                return false;
            const std::vector<std::pair<int, int> >& list =
                dropLists[it->second.first].m_list04;
            unsigned long dropRange =
                static_cast<unsigned long>(it->second.second);
            // ORIG 0x081b3648/0x081b3673：dropDice 落栈槽 -0x2c（`mov %eax,-0x2c(%ebp)`
            // / `mov -0x2c(%ebp),%eax`），且 nm 无 .bss 符号 ⇒ 原码是**普通局部
            // int**（其余 itemDropDice/groupDice/mobDice/itemDice 才是函数级 static）。
            int dropDice = probabilityRewardItem.randInt(dropRange);
            int sum = 0;
            for (unsigned int j = 0; j < list.size(); ++j)
            {
                sum += list[j].second;
                // [b28d-solo-CUserMissingA2②] 复验：写 `sum >= dropDice` 反致块序
                // 错位（mnem ratio 0.9605→0.7289）⇒ 维持 ORIG 语义等价形。
                if (dropDice <= sum)
                {
                    item.m_addInfo = list[j].first;
                    item.m_addInfo2 = 1;
                    G_CDataManager()->m_itemList->create_item(
                        ENUM_ITEM_CREATE_TYPE_1, item, 1);
                    result.m_8 = 1;
                    return true;
                }
            }
            // ORIG 0x081b36f6 独立 return-false 岛（mob 循环未命中）与 0x081b3700
            // （it==end）分列两岛；显式 return 阻止 4.4 共享出口合并。
            return false;
        }
        return false;
    }

    // [b28d-solo-CUserMissingA2②] ORIG 0x081b3786 `mov eax,[ebp-0x38]; add
    // eax,0x14; call _ZNKSt6vectorISt4pairIiiESaIS1_EE5emptyEv; xor $1; test;
    // je <return-0>` = item 分支的正向 empty 包裹（p167 MISSING 该 const empty
    // 调用的根因；原实现直接进 for，语义上空表时同样返回 false 但少调用点）。
    if (!group.m_itemList.empty())
    {
        unsigned long itemRange =
            static_cast<unsigned long>(group.m_field24);
        static int itemDice;
        itemDice = probabilityRewardItem.randInt(itemRange);
        int sum2 = 0;
        for (unsigned int k = 0; k < group.m_itemList.size(); ++k)
        {
            // [b28d-solo-CUserMissingA2 真 bug 修正] ORIG 0x081b376c/0x081b3790：
            // m_itemList 的 pair 语义为 (概率, 道具id)——累加读 +0（first），命中取
            // +4（second）赋 m_addInfo（与 m_mobList 的 (id, prob) 相反）。原实现
            // 两处互换 ⇒ 概率累加错、道具 id 取错。
            sum2 += group.m_itemList[k].first;
            if (itemDice <= sum2)
            {
                item.m_addInfo = group.m_itemList[k].second;
                item.m_addInfo2 = 1;
                G_CDataManager()->m_itemList->create_item(
                    ENUM_ITEM_CREATE_TYPE_1, item, 1);
                result.m_8 = 1;
                return true;
            }
        }
        // ORIG 0x081b37fa `jmp <0x081b3803>` + 0x081b3803 岛：循环未命中与 empty
        // 失败各落独立 return 岛。4.4 尾块合并使循环退出直落外层 return-false 块
        // （少 1 条 jmp）。goto 尾标签变体实测可补该 jmp 但整帧槽号平移 8
        // （归一化 diff 5→19 处，净负，已回滚）⇒ N112/N121 封顶。
        return false;
    }
    return false;
}

// ----------------------------------------------------------------------------
// 23. ORIG T 0x081b3b68 sz 0x5a2。结算主流程：GetCompetitionState 非 0 才继续；
// 局部 static emptyItem/mailTitle/serverMessage/strMessage；try{GetRewardGold+
// GetRewardItem}catch 复位+alter+cMyTrace 返回 1；gold<1 走 MercenaryReturn
// 清理；否则发系统邮件（ReqDBSendNewSystemMail）+ 日志 + DB 记录。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82/N89：ORIG 0x081b3b68；p167 CLEAR（49/49），残差为 STL 实例
// 形态与帧/槽分配，语义闭合。
// [b28d-solo-CUserMissingA2] 修复：`bool isFinal2` 去预清（ORIG 无 movb $0，
// 直读 movzbl -0x3a）+ `emptyItem.m_addInfo` 两处独立读取（P48 无 CSE，消
// itemIdx 具名槽）。p167 CLEAR 49/49；残差为槽序（4.4 首次使用序）+ STL 实例
// 形态 ⇒ N82/N89 封顶。
#include "CUserMissingA2/Reward.inc"
                                   competition_reward_result& result)
{
    if (user == 0)
        return 0;
    if (GetCompetitionState() == 0)
        return 0;

    static Inven_Item emptyItem;
    static std::string mailTitle(
        g_scriptStringManager_.findString(4, "game_server_msg_225", 0),
        std::allocator<char>());
    static std::string serverMessage;
    static char strMessage[0x100];

    int gold = 0;
    bool bItemGot = false;
    // [b28d-solo-CUserMissingA2②] ORIG 0x081b3dca `mov BYTE PTR [ebp-0x3a],0x0`
    // 与 gold/bItemGot 预清同处（try 之前）⇒ isFinal 是函数头具名局部（P212），
    // 非块内声明。
    bool isFinal = false;
    try
    {
        gold = GetRewardGold(user, result);
        bItemGot = GetRewardItem(user, result, emptyItem, serverMessage);
        if (bItemGot != true)
            serverMessage =
                MercenaryHardCodingSettingValue::MERCENARY_BASE_POSTAL_MESSAGE;
    }
    catch (...)
    {
        m_field28 = 0;
        m_field2c = 0;
        m_field30 = 0xff;
        m_field31 = 0xff;
        alter();
        cMyTrace(__PRETTY_FUNCTION__, 0x1ea, 5)(
            "mercenary Line:%d exception\n", 0x1ea);
        return 1;
    }

    if (gold < 1)
    {
        int term = GetCompetitionPeriodTerm(&isFinal);
        user->m_historyLog.MercenaryReturn(m_characNo, m_field28, m_field2c,
                                           term, gold, 0);
        m_field28 = 0;
        m_field2c = 0;
        m_field30 = 0xff;
        m_field31 = 0xff;
        alter();
        return 1;
    }

    const char* msgText = g_scriptStringManager_.findString(
        4, serverMessage.c_str(), 0);
    strncpy(strMessage, msgText, 0xff);
    size_t msgLen = strlen(strMessage);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        mailTitle.c_str(), emptyItem, gold, m_characNo, strMessage, msgLen,
        0x1e, user->GetServerGroup(), false, false);

    // ORIG 0x081b3f?? 无预清（`movzbl -0x3a,%eax; test` 直读，GetCompetitionPeriodTerm
    // 命中路径必写）⇒ 不初始化（P198① 反向：ORIG 无 movb $0）。
    bool isFinal2;
    int term2 = GetCompetitionPeriodTerm(&isFinal2);
    user->m_historyLog.MercenaryReturn(m_characNo, m_field28, m_field2c,
                                       term2, gold, 0);

    char status = isFinal2 ? 2 : 1;
    // ORIG 0x081b3fc?：emptyItem.m_addInfo 两处各自独立读取（P48 无 CSE，
    // `mov 0x9407ec2,%eax` ×2），不设 itemIdx 具名局部。
    CItem* pItem = G_CDataManager()->find_item(emptyItem.m_addInfo);
    int rarity = -1;
    if (pItem != 0)
        rarity = pItem->get_rarity();
    DB_MercenaryLogInfo::makeRequest(
        user->get_acc_id(), m_characNo, m_field28,
        GlobalData::s_systemTime_.getCurSec(), status, m_field31, m_field30,
        gold, emptyItem.m_addInfo, rarity);

    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0xff;
    m_field31 = 0xff;
    alter();
    return 1;
}

// ----------------------------------------------------------------------------
// 24. ORIG T 0x081b3814 sz 0x353。竞技报名：Reward 预结算+状态/期次/地区校验；
// area m_worldMapIdx==0xffff 时随机选可报名地区（probArea + 有效地区表）；
// GetCompetitionPeriodSecond 非 0 → alter + 记录时间/期次/地区 + DB 日志。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82/N86：ORIG 0x081b3814；p167 CLEAR（30/30），残差为容器迭代器
// 临时槽 + 帧分配，语义闭合。
// [b28d-solo-CUserMissingA2] 修复：`if (!CheckCompetitionAreaIndex(...)) areaIdx=0;
// else areaIdx=idx;` 否定形复现 ORIG 0x081b3a4a `xor $1,%eax` 物化 + pArea 表达式
// 内联（消具名槽）⇒ 全段 diff 行 254→200。p167 CLEAR 30/30；残差帧 0x8c vs 0x7c +
// 槽序（N82/N93）封顶。
#include "CUserMissingA2/Competition.inc"
                                        char areaIdx)
{
    if (user == 0)
        return false;

    competition_reward_result result;
    Reward(user, result);
    if (GetCompetitionState() != 0)
        return false;
    if (!CheckCompetitionPeriodIndex(periodIdx))
        return false;
    if (!CheckCompetitionAreaIndex(areaIdx, m_level))
        return false;

    // ORIG 0x081b38d1：`call operator[]; movzwl 0x14(%eax),%eax` 直接内联
    // （无 pArea 具名局部槽）⇒ 表达式直读（P157/P183）。
    if (G_CDataManager()->m_mercenarySystem
            .m_vec40[static_cast<int>(areaIdx)].m_worldMapIdx == 0xffff)
    {
        static CMTRand probArea;
        std::vector<int> validIdx;
        for (int i = 0; i < (int)G_CDataManager()
                 ->m_mercenarySystem.m_vec40.size(); ++i)
        {
            stMercenaryCompetitionArea* pA = &G_CDataManager()
                ->m_mercenarySystem.m_vec40[i];
            // [b28d-solo-CUserMissingA2②] P287 同族：ORIG 0x081b3990 `movzx
            // eax,[eax+0x14]; test ax,ax` + 0x081b399c `cmp dx,ax; jg`（16 位
            // 有符号比较），4.4 默认提升 32 位（cwde）⇒ 源侧加 (short) 收窄。
            if ((short)pA->m_areaLevel != 0 &&
                (short)pA->m_areaLevel <= m_level)
                validIdx.push_back(i);
        }
        unsigned long range = validIdx.size() - 1;
        int pick = probArea.randInt(range);
        int idx = validIdx[pick];
        // ORIG 0x081b3a4b：`call CheckCompetitionAreaIndex; xor $1,%eax; test %al,%al;
        // je <idx 分支>`（bool 取反物化，P22/P292）⇒ 源码是否定形 if(!cond)。
        if (!CheckCompetitionAreaIndex(static_cast<char>(idx), m_level))
            areaIdx = 0;
        else
            areaIdx = static_cast<char>(idx);
    }

    // [b28d-solo-CUserMissingA2②] ORIG 0x081b3b?? `mov eax,[eax+0x28]; mov
    // edx,eax; add edx,[ebp-0x28]`（add mem 直加）；m_field28 已 unsigned，
    // 4.4 对 unsigned+int 生 lea ⇒ 试 seconds 同型化。
    unsigned int seconds = GetCompetitionPeriodSecond(periodIdx);
    if (seconds == 0)
        return false;

    alter();
    m_field28 = GlobalData::s_systemTime_.getCurSec();
    m_field2c = m_field28 + seconds;
    m_field30 = areaIdx;
    m_field31 = periodIdx;
    DB_MercenaryLogInfo::makeRequest(user->get_acc_id(), m_characNo, m_field28,
                                     m_field2c, 0, periodIdx, areaIdx,
                                     -1, -1, -1);
    return true;
}

// ============================================================================
// Wave15 批：CUser 仅ORIG 强符号补齐（逐报告核对；真实语义/签名）
// ============================================================================

// ----------------------------------------------------------------------------
// ORIG W 0x08697474 _ZN5CUser6GetETCEs：map 取键值后删除。
// 汇编：end()→find(key)→eq 返 0；op[](key) 取 int；erase(key)；返 val。
// ----------------------------------------------------------------------------
// [b28d-27] NEAR N93：ORIG 0x08697474；ORIG end()槽 -0x14 / find()槽 -0x10（4.1
// 先声明占深槽），4.4 恒反向（先声明浅槽）；交换声明序会改调用序（ORIG end 先、
// find 后）⇒ 封顶。p167 CLEAR（5/5），语义闭合。
// [b28d-solo-CUserMissingA2] 复验：p167 CLEAR 5/5。两形实测——①交换声明序（it
// 先/e 后）⇒ NEAR→DIFF（调用序反）；②it 包内层块作用域 ⇒ 槽序仍反。ORIG
// end=-0x14 / find=-0x10（4.1 先声明深），4.4 恒反向 ⇒ N93 封顶。
#include "CUserMissingA2/GetETC.inc"
{
    std::map<short, int>::iterator e = m_mapEtc.end();
    std::map<short, int>::iterator it = m_mapEtc.find(key);
    if (it == e)
        return 0;
    int val = m_mapEtc[key];
    m_mapEtc.erase(key);
    return val;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868dd44 _ZN5CUser17get_SoloPlay_modeEv：
// GetGameOptionRef()→get_etc_option(0x1b) short→int 符号扩展返回。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/get_SoloPlay_mode.inc"
{
    CGameOption* opt = GetGameOptionRef();
    short v = opt->get_etc_option((ENUM_OPTION_ETC)0x1b);
    return (int)v;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868a4b6 _ZN5CUser15set_MailMsgDataEP16MSG_MAILBOX_SEND：
// movb $0,0x8e3ec（mail 标志清 0）+ memcpy(this+0x8e101, msg, 0x145)。
// 0x8e101 起块视图沿用 m_pad8e101（0x2eb=0x8e3ec-0x8e101，先例见 CUserOnlyOrig）。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/set_MailMsgData.inc"
{
    m_pad8e101[0x2eb] = 0;                       // ORIG movb $0x0,0x8e3ec(%eax)
    memcpy(m_pad8e101, msg, 0x145);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08686f58 _ZN5CUser26SendUnlimitChallengePacketEv：
// PacketGuard：header(0,0xa4) + byte(1) + finalize(true) + Send。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/SendUnlimitChallengePacket.inc"
{
    PacketGuard pkt;
    pkt.put_header(0, 0xa4);
    pkt.put_byte(1);
    pkt.finalize(true);
    Send(pkt);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08688d26 _ZN5CUser16GetItemLimitInfoEi：
// begin/end 双迭代器（end 缓存）；元素 +0x15==itemIdx 返回元素指针；否则 0。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/GetItemLimitInfo.inc"
{
    for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin(),
         e = m_limitItemUsage.end(); it != e; it++)
    {
        if (it->m_field15 == itemIdx)
            return &*it;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08688c70 _ZN5CUser17IsLimitItemUsableEi：
// 命中条目按 isUsable() 判定（真返 1/假返 0）；无条目视为可用返 1。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/IsLimitItemUsable.inc"
{
    for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin(),
         e = m_limitItemUsage.end(); it != e; it++)
    {
        if (it->m_field15 == itemIdx)
        {
            if (it->isUsable())
                return 1;
            return 0;
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0867925a _ZN5CUser20processGrowTypeEventEhh21eChangeGrowTypeReason：
// reason==1 忽略；转 _onChangeGrowType；CEventScriptMng::process_level_up_reward
// (this,true)；p2==0 时 processLevelUpEventReward(true)。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/processGrowTypeEvent.inc"
                                 eChangeGrowTypeReason reason)
{
    if (reason == (eChangeGrowTypeReason)1)
        return;
    _onChangeGrowType(p1, p2, reason);
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->process_level_up_reward(this, true);
    if (p2 == 0)
        processLevelUpEventReward(true);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868e6c0 _ZN5CUser28Competition_Mercenary_CharacEjcc：
// 转发 m_mercenaryMgr.CompetitionCharac(this, slot, modeA, modeB)。
// ----------------------------------------------------------------------------
#include "CUserMissingA2/Competition_Mercenary_Charac.inc"
{
    // ORIG 0x0868e6c0：透传 CompetitionCharac 的 0/1（Ghidra 因 leave/ret 误判 void；
    // 调用点 Dispatcher_Mercenary_Competition::process 读 %al 写 result+4 实证）。
    return m_mercenaryMgr.CompetitionCharac(this, slot, modeA, modeB);
}

// ----------------------------------------------------------------------------
// ORIG T 0x086914cc _ZN5CUser15updateBingoDataEv：
// m_needSave 置位时 DB_UpdateBingoEvent::makeRequest(GetUID(), acc_id,
// m_eventId, m_isWin)（ORIG 签名 Eijib），随后 m_needSave 清 0。
// getBingoData() 每次独立调用（P48 无 CSE）。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N18a 反向：ORIG 0x086914cc；ORIG `mov (%eax),%esi` 单段直落
// callee-saved，4.4 恒经 %eax 两段；旧探针族（具名/内联/register）均不翻 ⇒ 封顶。
// p167 CLEAR（7/7），语义闭合。
// [b28d-solo-CUserMissingA2] 复验 44/43：P226 裸语句表达式、P231 纯 register、
// 删 `static_cast<int>` 三形实测均不翻（register 反使 isWin 装载序劣化），
// ORIG `mov (%eax),%esi` 单段直落 callee-saved 不可造 ⇒ N18a 反向封顶；
// p167 CLEAR 7/7。
#include "CUserMissingA2/updateBingoData.inc"
{
    // 语义闭合（ndiff 43/44 残 1 条定性）：ORIG `mov (%eax),%esi` 为 m_needSave
    // 内存加载直落 callee-saved %esi；4.4 恒经 %eax 中转两步（N18a 反向，
    // 与前序 getCurCharacInvenCheckSum `mov 0x58(%eax),%ebx` 同型，探针族全败）。
    if (static_cast<BingoData*>(getBingoData())->m_needSave)
    {
        DB_UpdateBingoEvent::makeRequest(
            GetUID(), get_acc_id(),
            // [Lane14C] ORIG 0x0869145a `mov (%eax),%esi` 单段直落 callee-saved：
            // 字段 m_eventId 为 unsigned int 而 makeRequest 第 3 形参为 int
            // （mangled Eijib）⇒ 4.4 对异型实参插转换中转（`mov (%eax),%eax;
            // mov %eax,%esi`）。探针 p2(异型→两段)/p6(同型 int 视图→单段) 实证
            // ⇒ 以 int 左值视图读取（P266 同族左值视图旋钮；sizeof/偏移/符号零变化）。
            *reinterpret_cast<int*>(
                &static_cast<BingoData*>(getBingoData())->m_eventId),
            static_cast<BingoData*>(getBingoData())->m_isWin);
        static_cast<BingoData*>(getBingoData())->m_needSave = 0;
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08651de6 _ZN5CUser17UpdateBloodRecordEv：
// 遍历当前角色 _Charac_info.m_bloodBestRecords（+0xecc），每项经 stream 发
// SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD（sig+0=characNo、+4=m_time、+8=m_round、
// +0xc=key）到 LOGDB 队列；末尾 setBloodBestRecordUpdate(false)。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 3）：ORIG 0x08651de6。
// [b28d-solo-CUserMissingA2] 已修复为 **AE**（127/127；p167 24/24 CLEAR）：
// [b28d-solo-CUserMissingA2] 迭代器族归位：ORIG 0x08651de6 用**非 const**
// `_Rb_tree_iterator`（p167：MISSING 非 const begin/end/ptEv/ppEv/ne，EXTRA 我方
// const 版全族）⇒ 原码 cur 为非 const 指针；本函数只读遍历，用 const_cast 取回
// 非 const 视图（不改共享头，避免全仓重编译）。循环条件形 = `it != map.end()`
// （ORIG end 调用点在循环底部，无 end 具名缓存）。
// [b28d-solo-CUserMissingA2] store 序：ORIG 4 处 store 全为 RHS-first + disp 直存
// （`mov RHS,%edx; mov sig,%eax; mov %edx,disp(%eax)`）⇒ 原码是**具名视图结构成员
// 赋值**（裸 `*(unsigned int*)((char*)sig+K)` 产 lea+ebx 跨 call 缓存，DIFF）；
// 字段类型须与 RHS 精确同型（probe /tmp/b28d_solo/probe_view4.cpp fK 逐条实证）。
struct BloodBestRecordSigView
{
    unsigned int m_characNo;   // +0x00（getCurCharacNo 返回 unsigned int，同型免中转）
    unsigned int m_time;       // +0x04（stBloodBestRecord::m_time）
    int m_round;               // +0x08（stBloodBestRecord::m_round）
    int m_key;                 // +0x0c（pair::first）
};

#include "CUserMissingA2/UpdateBloodRecord.inc"
{
    _Charac_info* cur = const_cast<_Charac_info*>(getCurCharacR());
    if (cur == 0)
        return;
    for (std::map<int, stBloodBestRecord>::iterator it = cur->m_bloodBestRecords.begin();
         it != cur->m_bloodBestRecords.end(); ++it)
    {
        // P259：ORIG 0x08651e43 无 stream 具名局部（Acquire 返回值直传 C1 的
        // 0x4(%esp)）⇒ 必须内联实参，否则多一个 -0x10 槽并使全帧槽号平移 -4。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bbb), true);
        // P132/P103：ORIG 0x08651e60/0x08651e9f 显式调 CStreamGuard::operator*()
        // （`de`×2）与 operator->()（`pt`×1）再转发 ⇒ 必须保留代理链
        // （直接 `guard <<`/`guard.GetInBuffer` 少 3 条 call）。
        (*guard)->operator<<(0x140);
        (*guard)->operator<<((int)GetUID());
        BloodBestRecordSigView* sig = reinterpret_cast<BloodBestRecordSigView*>(
            guard.operator->()->GetInBuffer<SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>());
        memset(sig, 0, 0x10);
        sig->m_characNo = getCurCharacNo();
        sig->m_time = it->second.m_time;
        sig->m_round = it->second.m_round;
        sig->m_key = it->first;
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    setBloodBestRecordUpdate(false);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0865da4e _ZN5CUser12SetEquipmentEiPviS0_：
// for(i=0..9) 检查装备槽 i 过期（m_addInfo!=0 且 get_add_info()>=1 且
// GetDateTimeTick()>addInfo）；过期则把 m_characList[slot].m_characNo 记入
// outData 表并 reset 该槽、计数+1；循环后对 m_characList[slot]+0xda 的
// CInventory 调 SetInvenData(0, invenData, size)。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x0865da4e；p167 CLEAR（7/7），残差为帧/槽分配，
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复：P226 条件语句表达式复现 `je skip`×3 + `mov
// $1/%eax … mov $0/%eax; test` 物化岛（消 expired bool 槽）；P311 具名视图
// OutDataView/OutTailView 复现 `mov %edx,0x4(%eax,%ebx,4)` /
// `mov %cl,0x5a4(%edx,%eax,1)` 单条 scaled/disp store（消 shl+add 地址链）；
// 第三条件操作数交换为 `get_add_info() < GetDateTimeTick()` 复现 ORIG 求值序。
// 残差仅 N7 寄存器分配：4.4 跨 operator[] 保活基址+索引（push %esi；ORIG 只保活
// 索引 %ebx、基址重载）；p167 CLEAR 7/7。
#include "CUserMissingA2/SetEquipment.inc"
{
    int* cnt = (int*)outData;
    Inven_Item* items = (Inven_Item*)invenData;
    for (int i = 0; i <= 9; ++i)
    {
        // P226 条件向语句表达式（b28c C28L9 扩展）：ORIG 0x0865da4e 对短路链发
        // `je skip`×3 + `mov $1,%eax; jmp; skip: mov $0,%eax; test %al,%al; je`
        // 寄存器物化岛（无 bool 槽）；写具名 bool 会落槽（movb/cmpb 对 + 帧 +4）。
        // 表达式内保持逐次重读（P157/P183，get_add_info 两次独立调用）。
        if (({ items[i].m_addInfo != 0 && items[i].get_add_info() >= 1
                && items[i].get_add_info() < OS_API::GetDateTimeTick(); }))
        {
            // P311/P291：ORIG 对两个写出均发单条 scaled/disp store
            // （`mov %edx,0x4(%eax,%ebx,4)` / `mov %cl,0x5a4(%edx,%eax,1)`）；
            // 裸指针下标 4.4 恒折 shl+add 地址链 ⇒ 用具名视图成员数组。
            struct OutDataView
            {
                int m_count;      // +0x00（*cnt）
                int m_items[1];   // +0x04
            };
            struct OutTailView
            {
                char m_pad[0x5a4];  // +0x000
                char m_flags[1];    // +0x5a4
            };
            // [b28d-solo-CUserMissingA2②] 复验：去 cnt 局部（形参直用）⇒ 我方
            // 仍把 base 保活 callee-saved（`mov ebx,[ebp+0x18]`）且丢序言 spill
            // 两条，劣化 ⇒ 维持 cnt 具名局部（ORIG 0x0865da5c -0x14 槽 + 序言
            // spill 实证）。残余 = 4.4 跨 operator[] 保活 base+索引（push %esi；
            // ORIG 只保活索引 %ebx、base 重载）⇒ N7 封顶。
            reinterpret_cast<OutDataView*>(cnt)->m_items[*cnt] =
                m_characList[slot].m_characNo;
            reinterpret_cast<OutTailView*>(cnt)->m_flags[*cnt] = (char)i;
            items[i].reset();
            *cnt = *cnt + 1;
        }
    }
    ((CInventory*)((char*)&m_characList[slot] + 0xda))
        ->SetInvenData(0, invenData, size);
}

// ----------------------------------------------------------------------------
// ORIG T 0x086925c4 _ZN5CUser20processFatigueCharacEi：
// 按职业/已用疲劳匹配疲劳礼物；命中则取脚本文案、构造 Inven_Item、snprintf
// 疲劳消息，itemIndex!=0 时经 CMailBoxHelper 发系统邮件。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x086925c4；p167 CLEAR（29/29），残差为帧/槽与
// 寄存器分配，语义闭合。
// [b28d-solo-CUserMissingA2] 修复：变参位内联 getCurCharacUsedFatigue()（ORIG
// 0x086926d8 `movzwl %ax,%ebx` 无局部槽）。残差为跨版本槽复用：ORIG 的 itemIndex
// 槽与 item.m_addInfo(-0x5f) 重叠（4.1 允许，4.4 独立分配）+ 帧 0x1bc vs 0x190
// ⇒ N82 封顶；p167 CLEAR 29/29。
#include "CUserMissingA2/processFatigueCharac.inc"
{
    int job = get_charac_job();
    int giftId = 0;
    giftId = G_CDataManager()->get_useJobFatigueMatchGift(
        (char)job, (int)getCurCharacUsedFatigue(), usedMin);
    if (giftId == 0)
        return;

    std::string title;
    std::string fmtKey;
    G_CDataManager()->get_useJobFatigueMatchGiftMsg(job, title, fmtKey);

    Inven_Item item;
    CItem* def = G_CDataManager()->find_item(giftId);
    int srvGrp = GetServerGroupToPvP();
    // [Lane14C] ORIG 0x08692689 `cmpl $0,-0x14(%ebp); je <+0x21f 共享清理出口>`
    // = 正向包裹形（P222/P257）：def==0 直接落共享清理出口、不执行 snprintf/
    // findString（原「if (def == 0) return;」平铺形使 4.4 生成反向 jne + 独立
    // 出口块，帧/块数 168 vs ORIG 162）。
    if (def != 0)
    {
        int itemIndex = def->get_index();
        def->make_item(item);
        item.set_add_info(1);

        char msg[256];
        // ORIG 0x086926d8：`call getCurCharacUsedFatigue; movzwl %ax,%ebx`（无局部槽，
        // 值在 %ebx 跨 c_str/findString 保活）⇒ 变参位内联（P44/P0）。
        OS_API::snprintf(msg, 0xff,
                         g_scriptStringManager_.findString(4, fmtKey.c_str(), 0),
                         getCurCharacUsedFatigue());
        if (itemIndex != 0)
        {
            int expire = def->getExpirationDate();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, title.c_str(), 0),
                item, 0, getCurCharacNo(), msg, (int)strlen(msg), expire,
                (ENUM_SERVER_GROUP)srvGrp, false, true);
        }
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x086792d6 _ZN5CUser19processAwakenRewardEc：
// switch(stage) 设定邮件正文/标题脚本文案（jump table），命中后构造
// Inven_Item(0x3c0, sealCount=1) 并 ReqDBSendNewSystemMail；default 直接返回。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N7+P52：ORIG 0x086792d6；虚调用 callee 寄存器 %eax↔%edx 互换
// （4.4 间接 call 目标恒 %eax）+ 值先物化 callee-saved 缺失；p167 CLEAR（32/32），
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复 P290：rewardItemId int→unsigned ⇒ find_item
// 实参升格 %ebx 跨 G_CDataManager call 保活（ORIG 0x08679609 对齐）；残余仅 N7
// （本 TU 路由 4.4.7 ⇒ 间接 call 目标恒 %eax），201/201。
#include "CUserMissingA2/processAwakenReward.inc"
{
    unsigned int rewardItemId = 0x3c0;
    int sealCount = 1;
    char mailMsg[256];
    char mailTitle[21];
    switch (stage)
    {
    case 0:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_1", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_1", 0), 0x14);
        break;
    case 1:
    case 7:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_2", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_2", 0), 0x14);
        break;
    case 2:
    case 5:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_3", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_3", 0), 0x14);
        break;
    case 3:
    case 8:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_4", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_4", 0), 0x14);
        break;
    case 4:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_5", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_5", 0), 0x14);
        break;
    case 6:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_6", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_6", 0), 0x14);
        break;
    default:
        return;
    }

    Inven_Item item;
    CItem* def = G_CDataManager()->find_item(rewardItemId);
    if (def == 0)
        return;
    item.m_addInfo = rewardItemId;
    def->make_item(item);
    item.set_add_info(sealCount);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        mailTitle, item, 0, getCurCharacNo(), mailMsg, (int)strlen(mailMsg),
        12, GetServerGroup(), false, false);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868e36a _ZN5CUser17SaveMercenaryDataEv：
// m_mercenaryMgr.m_characs 空则返 1；否则 stream 写 0x2b3+GetUID，
// SIG_MERCENARY_DB_DATA 清零后逐项（+0x24 脏标记）memcpy 0x54 到
// db+4+row*0x54 并 save_complete()；入 LOGDB 队列，返回 1。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 4）：ORIG 0x0868e36a；ORIG 多 4 条调用（STL 族），
// 我方实现语义等价但实例/形式不同（N89 类）+ 帧差。
// [b28d-solo-CUserMissingA2] 复验 136/132：p167 20/20 CLEAR；残余为槽序
// （4.4 首次使用序 vs 4.1）+ `imul $0x54` 强度归约（4.4 恒 shl/add 链）+
// `*db` store 序 ⇒ N82/N101 封顶。
// [b28d-solo-CUserMissingA2] p167 归位：ORIG 0x0868e36a 调 CStreamGuard::operator*
// ×2 + operator-> ×1（代理链）、operator[] ×3（P157 逐次重读，非引用缓存）、
// 结构整体赋值 `rep movsl 0x15`（非 memcpy）；row 为每次从 *db 读的具名局部。
struct MercenaryDbHeadView
{
    unsigned int m_count;   // +0x00（SIG_MERCENARY_DB_DATA 首 dword = 行数）
};

#include "CUserMissingA2/SaveMercenaryData.inc"
{
    if (m_mercenaryMgr.m_characs.empty())
        return 1;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("user.cpp", 0xab3b), true);
    (*guard)->operator<<(0x2b3);
    (*guard)->operator<<((int)GetUID());
    SIG_MERCENARY_DB_DATA* db =
        guard.operator->()->GetInBuffer<SIG_MERCENARY_DB_DATA>();
    memset(db, 0, 0xbd4);

    for (unsigned int i = 0; i < m_mercenaryMgr.m_characs.size(); ++i)
    {
        if (m_mercenaryMgr.m_characs[i].m_field24 != 0)
        {
            // [b28d-solo-CUserMissingA2②] P212/P309 复验：row 提函数头（含预清
            // `=0`）后 4.4 仍给 -0x24（guard 占最深 -0x2c），ORIG 0x0868e3a2 row
            // 落 -0x2c / guard -0x28 ⇒ 4.1 按声明序、4.4 按分配序，不可翻，记 N82。
            unsigned int row =
                reinterpret_cast<MercenaryDbHeadView*>(db)->m_count;
            // [b28d-solo-CUserMissingA2②] P205 复验：下标形与 `ptr+row` 指针加法形
            // 4.4 均强度归约为 shl/add 链（ORIG 0x0868e4c4 `imul eax,ebx,0x54`）
            // ⇒ N101 封顶，维持下标形。
            ((CHARAC_LOAD_MERCENARY*)((char*)db + 4))[row] =
                m_mercenaryMgr.m_characs[i];
            reinterpret_cast<MercenaryDbHeadView*>(db)->m_count = row + 1;
            m_mercenaryMgr.m_characs[i].save_complete();
        }
    }
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0868f726 _ZN5CUser20updatePcRoomPlayTimeEv：
// 取事件脚本管理器 get_event_entity(0x5c) 为 CPcRoomPlayTime；verify 通过
// （==0）时 collectPcRoomPlayTime() 并 DB_SavePcRoomPlayTimeEvent::makeRequest
// (accId, totalPlayTime, rewardCnt)。
// ----------------------------------------------------------------------------
class DB_SavePcRoomPlayTimeEvent
{
public:
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};

#include "CUserMissingA2/updatePcRoomPlayTime.inc"
{
    EventClassify::CPcRoomPlayTime* entity =
        static_cast<EventClassify::CPcRoomPlayTime*>(
            static_cast<EventClassify::CEventScriptMng*>(
                G_CDataManager()->get_event_script_mng())
            ->get_event_entity(0x5c));
    if (entity == 0)
        return;
    if (entity->verifyUserPcRoomPlayEvent(this) == 0)
    {
        collectPcRoomPlayTime();
        DB_SavePcRoomPlayTimeEvent::makeRequest(
            get_acc_id(), (unsigned int)getTotalPcRoomPlayTime(),
            getPcRoomPlayTimeRewardCnt());
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08664e8e _ZN5CUser17_fnStatQuestClearEjPc（enumQuestClear 静态回调）：
// ctx=[this, elixirIds*, outList*]；find_quest 命中且任务种类（+0xb8）为
// 0/0x19 时，对奖励表×elixirIds 双层遍历，匹配项 add_matched_list 到 outList。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 8 族）：ORIG 0x08664e8e；ORIG 用迭代器遍历
// （begin/end/C1Ev/ptEv/deEv/ppEv/ne 共 14 次），我方索引遍历 ⇒ 符号族不同
// （P175/N89 类），语义等价。
// [b28d-solo-CUserMissingA2] 复验：const_iterator（rewards 指针 const 化）+ P50
// 先声明后赋值 + 缓存 end + 4 迭代器全头部声明。p167 差 2 = ORIG 非 const
// `vector<int>::begin()` + `C1IPiE` 转换构造实例未发射（N89 实例差异，语义等价：
// 我方同点位有 begin/C1IPiE 调用）；残差为 4.1 内层 for-init 旋转到外层体末尾、
// 4.4 恒置开头 ⇒ N164/N82 封顶。
// [b28d-solo-CUserMissingA2②] 本波：getUserQuestRewardItem 已 static（Main 代落
// CUser.h:3037）⇒ 调用点压 2 实参（[esp]=q/[esp+4]=user），与 ORIG 0x08664ecf
// 一致；rewards 补两句式预清（ORIG 0x08664f07 `mov [ebp-0xc],0x0`）。p167 22/24：
// MISSING = ORIG 额外一次 `vector<int>::begin()`+`C1IPiE`（ORIG 0x08664fe8 段内层
// for-init 重复发射，4.4 恒单次，语义等价）。context 自增：`context += 4` /
// `context = context + 4` / `&context[4]` / `(char*)((int*)context+1)` 四形 4.4
// 均折 `add DWORD PTR [ebp+0xc],0x4`（P322② 同族、形参无 volatile 位点），ORIG
// `lea edx,[eax+4]; mov [ebp+0xc],edx` 不可造 ⇒ N 封顶。
// [b28d-solo-CUserMissingA2③] 本轮重核（dual_rank gap 16）⇒ 修真缺口 + gap 16→3：
// ORIG 初始化段（0x08664f53/0x08664f6e/0x08664f89/0x08664fb6）求外层 begin/end 与
// 内层 begin/end 各一次，此后**每轮外层迭代只重求内层 begin**（0x0866505a 段，位于
// ++ri 之前；内层 end 不再重求）⇒ 原实现把内层 begin+end 都写在体内，静态少 13 条。
// [④ 2026-09-09] 进一步复现 ORIG 块序：内层条件在体外（0x08665044）+ `ppEv` 在体内
// 末尾（0x08665039），begin 重求位于**内层循环之后、外层 ++ri 之前** ⇒ 源形改为
// `while (ii != ie) { …; ++ii; } ii = elixirIds->begin();`（原 for-init 形把 begin
// 重求放外层体开头，difflib 报整块移动）。改后行级 diff 循环段逐行对齐，仅剩 call
// 地址（.o 未重定位）差异。
// 残差 3 条 = 三处 context 形参自增（4.1 `mov;lea;mov;mov` 5 条/处 vs 4.4
// `mov;mov;mov;add mem` 4 条/处）；/tmp/probe_ctx{,2,3,4,5,6,7,8,9,10}.cpp 十组探针
// （4.1.2 与 4.4.7 双编译）穷举 `context++`/`+=4`/`=c+4`/`&c[4]`/`(int)c+4`/`*p++`/
// 引用 cast/逗号形，两侧均不出 `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)` 写回形
// ⇒ 跨版本 add-vs-lea 结构性差异（N1/N107 族），语义等价（三处均 +4 推进）。
// p167 24/24（MISSING/EXTRA 仅 `__normal_iterator` C1↔C2 同址别名，P262）。
#include "CUserMissingA2/_fnStatQuestClear.inc"
{
    // [b28d-solo-CUserMissingA2②] ORIG 0x08664e9e/0x08664eaa/0x08664eb6 对 context
    // 逐次 `mov 0xc(%ebp),%eax; lea 0x4(%eax),%edx; mov %edx,0xc(%ebp); mov (%eax),%eax`
    // = 读旧指针 + 形参就地推进（每处均写回，第三处同样推进）。
    // 4.4 对形参 `context += 4`/`context++` 恒折单条 `add DWORD PTR [ebp+0xc],0x4`；
    // 4.1 的 `mov/lea/mov` 三连（探针 /tmp/probe_ctx*.cpp 四组穷举：4.1/4.4 均不出 lea 写回形）
    // ⇒ 该 1 条/处之差属跨版本 add-vs-lea 结构性差异（N1/N107 族），语义等价。
    CUser* user = *(CUser**)context;
    context += 4;
    std::vector<int>* elixirIds = *(std::vector<int>**)context;
    context += 4;
    std::vector<std::pair<int, int> >* outList =
        *(std::vector<std::pair<int, int> >**)context;
    context += 4;

    Quest* q = G_CDataManager()->find_quest((int)questIdx);
    if (q == 0)
        return 1;
    if (q->m_fieldB8 != 0 && q->m_fieldB8 != 0x19)
        return 1;

    // [b28d-solo-CUserMissingA2②] ORIG 0x08664f07 `mov DWORD PTR [ebp-0xc],0x0`
    // 预清 + 0x08664f13 赋值 ⇒ 两句式具名局部（P198①）。
    const std::vector<RewardItemInfo>* rewards = 0;
    rewards = user->getUserQuestRewardItem(q, user);
    // P50 复活（b28c C28L29/C28L14）：ORIG 0x08664e8e 有独立 `const_iterator::C1Ev`
    // （×4）+ `C1IPiE` 转换构造（×2）+ 纯 const `ne` ⇒ 原码是「先声明后赋值 +
    // 缓存 end」形（初始化式形会被 4.4 折掉 C1Ev 并改 ne 重载）。
    // [③] 初始化顺序照 ORIG：外层 begin/end（0x08664f53/0x08664f6e）先、内层
    // begin/end（0x08664f89/0x08664fb6）后；ie 只求这一次。
    std::vector<RewardItemInfo>::const_iterator ri;
    std::vector<RewardItemInfo>::const_iterator re;
    std::vector<int>::const_iterator ii;
    std::vector<int>::const_iterator ie;
    ri = rewards->begin();
    re = rewards->end();
    ii = elixirIds->begin();  // ORIG 0x08664f89 初始化段的一次 begin
    ie = elixirIds->end();    // ORIG 0x08664fb6：内层 end 全程只求这一次
    for (; ri != re; ++ri)
    {
        // ORIG 0x08665044 内层条件在体外、0x08665039 `ppEv` 在体内末尾；begin 重求
        // （0x0866505a）位于**内层循环之后、外层 ++ri 之前**（非内层 for-init）。
        while (ii != ie)
        {
            if (ri->m_itemIdx == *ii)
            {
                user->add_matched_list(*outList, ri->m_itemIdx, ri->m_count);
            }
            ++ii;
        }
        ii = elixirIds->begin();  // ORIG 0x0866505a：每轮外层末重求内层 begin
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG _ZL14elixir_item_id（df_game_r .rodata 8×int；值 1039..1046 实证）。
// 精灵药水道具 id 表，calc_quest_stat 的匹配键集。
// ----------------------------------------------------------------------------
namespace
{
const int elixir_item_id[8] = { 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046 };
}

// ----------------------------------------------------------------------------
// ORIG T 0x08664c50 _ZN5CUser15calc_quest_statER15_Additioal_info：
// 精灵药水表→count_specific_items 统计持有；UserQuest+4 的 CQuestClear 经
// enumQuestClear(_fnStatQuestClear, this, ids, rewards) 收集可完成奖励；
// complete_reward_list 按持有补齐；逐项 apply_elixir_item 到 _Additioal_info。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x08664c50；p167 CLEAR（18/18），残差为帧/槽分配，
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复⇒AE：qc 表达式内联进 enumQuestClear 实参
// （ORIG 0x08664c9e `lea 0x4(%eax),%edx`，无具名局部）⇒ 消 -0x14 槽、全帧槽号
// 与 ORIG 对齐，DIFF→AE（p167 CLEAR 18/18）。
#include "CUserMissingA2/calc_quest_stat.inc"
{
    std::vector<int> elixirIds;
    for (int i = 0; i <= 7; ++i)
        elixirIds.push_back(elixir_item_id[i]);

    std::vector<std::pair<int, int> > ownCounts;
    count_specific_items(elixirIds, ownCounts);

    std::vector<std::pair<int, int> > rewards;
    // ORIG 0x08664c9e：`call getCurCharacQuestR; lea 0x4(%eax),%edx` 直接内联进
    // 实参装载（无 qc 具名局部）⇒ 表达式内联，避免多占 -0x14 槽使全帧深 4。
    ((WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4))
        ->enumQuestClear(_fnStatQuestClear, this, &elixirIds, &rewards);

    complete_reward_list(rewards, ownCounts);

    for (size_t i = 0; i < rewards.size(); ++i)
    {
        apply_elixir_item(ai, rewards[i]);
    }
}
