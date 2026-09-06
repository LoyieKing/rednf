// ============================================================================
// CUserMissingA2.cpp —— CUser 未定义方法补齐（独占 TU；不改 CUser.cpp/CMake）。
//
// 逐份完整读取报告（## 汇编 + ## 反编译 C）后转写：
//   1. CUser::GetCharacHotKeyType            docs/.../CUser/GetCharacHotKeyType.md
//                                            ORIG T 0x08692ef2 _ZN5CUser19GetCharacHotKeyTypeEi
//   2. CUser::CheckLoadHotKeyCashedCharacType .../CUser/CheckLoadHotKeyCashedCharacType.md
//                                            ORIG T 0x08692e9c _ZN5CUser31CheckLoadHotKeyCashedCharacTypeEii
//   3. CUser::CalcMercenaryAvatarBonus       .../CUser/CalcMercenaryAvatarBonus.md
//                                            ORIG T 0x0868ea68 _ZNK5CUser24CalcMercenaryAvatarBonusEv
//   4. CUser::setAvatarTypeBonus             .../CUser/setAvatarTypeBonus.md
//                                            ORIG T 0x0868ebaa _ZN5CUser18setAvatarTypeBonusEc
//   5. UserMercenaryInfoMgr::FindIndexByCharacNo（4 的依赖，ORIG T 0x081b42ae）
//   6. UserMercenaryInfoMgr::CheckCharacIndex    （4 的依赖，ORIG T 0x081b4262）
//   7. CHARAC_LOAD_MERCENARY::Reset          .../CHARAC_LOAD_MERCENARY/Reset.md
//                                            ORIG T 0x081b410a（布局自证）
//   8.（已迁回 CFatigueBatteryHandle.cpp 本类 TU；此处不再定义）
//                                            .../CFatigueBatteryHandle/ActiveFatigueBattery_EndLevel.md
//                                            ORIG T 0x084a81f4
//   9. DB_LoadCharacHotKeyOption::makeRequest .../DB_LoadCharacHotKeyOption/makeRequest.md
//                                            ORIG T 0x0842f710
//
// eventDeliveryBuyItem（ORIG T 0x0866463c）已由 CUserDepsB.cpp 定义，本 TU 不重复。
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
        if (vec[i].first <= level && vec[i].first >= bestFirst)
        {
            bestFirst = vec[i].first;
            bestIdx = i;
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
// 1. ORIG T 0x08692ef2 sz 0x17。
// cmpl $0xa,param; jne→0；相等→1。返回值 eax 全宽（头文件声明 int，调用点
// LoadCharacTypeHotKey 以 mov %eax 全宽取值后截 byte；语义 characJob==10）。
// ----------------------------------------------------------------------------
int CUser::GetCharacHotKeyType(int characJob)
{
    if (characJob == 10)
        return 1;
    return 0;
}

// ----------------------------------------------------------------------------
// 2. ORIG T 0x08692e9c sz 0x55。
// cashedCharacJob==-1 → 1（首次加载放行）；否则两次调 GetCharacHotKeyType 比较，
// 相同→0、不同→1（类型变化才允许重新加载热键）。
// ----------------------------------------------------------------------------
bool CUser::CheckLoadHotKeyCashedCharacType(int cashedCharacJob, int characJob)
{
    if (cashedCharacJob == -1)
        return true;

    int cachedType = GetCharacHotKeyType(cashedCharacJob);
    int currentType = GetCharacHotKeyType(characJob);
    if (cachedType == currentType)
        return false;
    return true;
}

// ----------------------------------------------------------------------------
// 3. ORIG T 0x0868ea68 sz 0x141。雇佣兵 avatar 等级加成 = 全身 avatar 装备的最低
// 品阶：初值 10；getCurCharacR/getCurCharacInvenR 为空→0；否则 i∈[0,7] 遍历
// GetInvenRef(INVEN, i)：空引用/find_item 失败/dynamic_cast<CEquipItem*> 失败/
// IsAvatarItem()==0（vtable+0x10 槽 4，两处 dtor 后第 5 槽）任一即返 0；
// 否则取 min(get_grade)；循环后 >3 归零。
// ----------------------------------------------------------------------------
int CUser::CalcMercenaryAvatarBonus() const
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

        // *(int*)(ref+2)：m_addInfo（item idx）
        CEquipItem* equip;
        if (CItem* item = G_CDataManager()->find_item(ref->m_addInfo))
            equip = dynamic_cast<CEquipItem*>(item);
        if (equip == 0)
            return 0;
        if (equip->IsAvatarItem())
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

    if (3 < best)
        best = 0;

    return best;
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x0868ebaa sz 0xb1。
// bonus∈[0,3] 且 getCurCharacR() 非空时：characNo=*getCurCharacR()（+0 dword）→
// m_mercenaryMgr.FindIndexByCharacNo → !=-1 且 CheckCharacIndex((uchar)idx) →
// vector 元素 +0x32（m_avatarBonus）写 bonus。
// ----------------------------------------------------------------------------
void CUser::setAvatarTypeBonus(char bonus)
{
    if (bonus < 0 || 3 < bonus)
        return;

    if (getCurCharacR() == 0)
        return;

    unsigned int characNo =
        static_cast<unsigned int>(getCurCharacR()->m_characNo);
    int idx = m_mercenaryMgr.FindIndexByCharacNo(characNo);
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
unsigned int UserMercenaryInfoMgr::FindIndexByCharacNo(unsigned int characNo) const
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
bool UserMercenaryInfoMgr::CheckCharacIndex(unsigned char index) const
{
    // ORIG 形态：empty→flag=0；!empty 且 index<=size→flag=0；否则 flag=1；返 !flag。
    bool bInvalid;
    if (m_characs.empty())
        bInvalid = false;
    else if (static_cast<unsigned int>(index) <= m_characs.size())
        bInvalid = false;
    else
        bInvalid = true;
    return !bInvalid;
}

// ORIG T 0x081b45ce：未找到项返回等待；命中项以 GetCompetitionState()==0 判等待。
bool UserMercenaryInfoMgr::IsWaitingStateCharac(unsigned int characNo)
{
    const unsigned int index = FindIndexByCharacNo(characNo);
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return true;
    return m_characs[index].GetCompetitionState() == 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081b4198：线性查找 characNo 相同项；命中且 level 不同则更新
// level 并 alter()，返回 0；命中 level 相同也返回 0；未命中返回 1。
// 循环条件 seta（size()>i 无符号），参数首 dword 每次迭代载入 ebx（P36）。
// ----------------------------------------------------------------------------
int UserMercenaryInfoMgr::CheckValidAppend(const CHARAC_LOAD_MERCENARY& info)
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
// ----------------------------------------------------------------------------
int UserMercenaryInfoMgr::ReturnCharac(CUser* user, unsigned int characNo,
                                       competition_reward_result& reward)
{
    if (user == 0)
        return 0;
    const unsigned int index = FindIndexByCharacNo(characNo);
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return 0;
    return m_characs[index].Reward(user, reward);
}

// ----------------------------------------------------------------------------
// ORIG T 0x081b4536：无效下标→0；有效项 Competition(user,modeA,modeB) 成功→1。
// modeB/modeA 先 movsbl 入 esi/ebx 跨 operator[] 存活（P52）。
// ----------------------------------------------------------------------------
int UserMercenaryInfoMgr::CompetitionCharac(CUser* user, unsigned int characNo,
                                            char modeA, char modeB)
{
    const unsigned int index = FindIndexByCharacNo(characNo);
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
bool CUser::Return_Mercenary_Charac(unsigned int mercUid,
                                    competition_reward_result& reward)
{
    m_mercenaryMgr.ReturnCharac(this, mercUid, reward);
    // ORIG 0x0868e692 为 void 转发（leave; ret 无 eax 设置），调用点取 %al 残留——
    // 此处不写 return 以复刻 ORIG 的未定义行为。
}

// ORIG T 0x081b2e86：返回 1=竞技中、0=未报名、2=已结束且已报名。
int CHARAC_LOAD_MERCENARY::GetCompetitionState() const
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
CHARAC_LOAD_MERCENARY::CHARAC_LOAD_MERCENARY()
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
void CHARAC_LOAD_MERCENARY::Reset()
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
void DB_LoadCharacHotKeyOption::makeRequest(int uid, unsigned int accId,
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
bool CHARAC_LOAD_MERCENARY::CheckCompetitionPeriodIndex(char periodIdx) const
{
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec14;
    bool bInvalid;
    if (vec.empty())
        bInvalid = true;
    else if (static_cast<unsigned int>(static_cast<int>(periodIdx)) <= vec.size())
        bInvalid = false;
    else
        bInvalid = true;
    return !bInvalid;
}

// ----------------------------------------------------------------------------
// 11. ORIG T 0x081b2b34 sz 0x8f。m_vec40 同型检查；再判
// level >= m_areaLevel（ushort 符号扩展后与 level 比较）。
// ----------------------------------------------------------------------------
bool CHARAC_LOAD_MERCENARY::CheckCompetitionAreaIndex(char areaIdx,
                                                      int level) const
{
    const std::vector<stMercenaryCompetitionArea>& vec =
        G_CDataManager()->m_mercenarySystem.m_vec40;
    bool bInvalid;
    if (vec.empty())
        bInvalid = true;
    else if (static_cast<unsigned int>(static_cast<int>(areaIdx)) <= vec.size())
        bInvalid = false;
    else
        bInvalid = true;
    if (bInvalid)
        return false;
    if (level < static_cast<int>(static_cast<short>(vec[static_cast<int>(areaIdx)].m_areaLevel)))
        return false;
    return true;
}

// ----------------------------------------------------------------------------
// 12. ORIG T 0x081b2c94 sz 0x75。m_vec20 表按 m_avatarBonus 查 float。
// ----------------------------------------------------------------------------
float CHARAC_LOAD_MERCENARY::GetAvatarBonus() const
{
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec20;
    bool bInvalid;
    if (vec.empty())
        bInvalid = true;
    else if (static_cast<unsigned int>(m_avatarBonus) <= vec.size())
        bInvalid = false;
    else
        bInvalid = true;
    if (bInvalid)
        return 0.0f;
    return vec[static_cast<int>(static_cast<char>(m_avatarBonus))].second;
}

// ----------------------------------------------------------------------------
// 13. ORIG T 0x081b2c1e sz 0x75。m_vec14 表按 m_field31 查 float。
// ----------------------------------------------------------------------------
float CHARAC_LOAD_MERCENARY::GetPeriodBonus() const
{
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec14;
    bool bInvalid;
    if (vec.empty())
        bInvalid = true;
    else if (static_cast<unsigned int>(m_field31) <= vec.size())
        bInvalid = false;
    else
        bInvalid = true;
    if (bInvalid)
        return 0.0f;
    return vec[static_cast<int>(static_cast<char>(m_field31))].second;
}

// ----------------------------------------------------------------------------
// 14. ORIG T 0x081b2d6a sz 0x11b。base=Find_max_value_lower_N(...).second；
// rate=(int)(term*base*period*avatar)（fistpl 截断）；GM 打印调试；
// rate<1 归 -1。
// ----------------------------------------------------------------------------
int CHARAC_LOAD_MERCENARY::CalcItemDropRate(CUser* user) const
{
    std::pair<int, int> basePair = Find_max_value_lower_N<std::pair<int, int> >(
        m_level, G_CDataManager()->m_mercenarySystem.m_vec8);
    int base = basePair.second;
    float period = GetPeriodBonus();
    float avatar = GetAvatarBonus();
    int term = GetCompetitionPeriodTerm(0);
    int rate = (int)((float)term * (float)base * period * avatar);
    if (user != 0 && user->isGMUser())
    {
        static char buffer[0x100];
        sprintf(buffer,
                "\nbase:%d * period:%.2f * avatar:%.2f * term:%d = %d\n",
                base, (double)period, (double)avatar, term, rate);
        SendMessageToCUser(user, buffer);
    }
    if (rate < 1)
        rate = -1;
    return rate;
}

// ----------------------------------------------------------------------------
// 15. ORIG T 0x081b2d0a sz 0x5f。dice<0 或 dice>m_field3c（0xa918）→0；
// 否则 FindRelativeProb<float>(m_vec30, dice)。
// ----------------------------------------------------------------------------
float CHARAC_LOAD_MERCENARY::GetRewardCritical(int dice) const
{
    if (dice < 0 || G_CDataManager()->m_mercenarySystem.m_field3c < dice)
        return 0.0f;
    return FindRelativeProb<float>(G_CDataManager()->m_mercenarySystem.m_vec30,
                                   dice);
}

// ----------------------------------------------------------------------------
// 16. ORIG T 0x081b2ed2 sz 0x5f。period 有效时返回
// vec14[periodIdx].first * m_field0（每期秒数）。
// ----------------------------------------------------------------------------
int CHARAC_LOAD_MERCENARY::GetCompetitionPeriodSecond(char periodIdx) const
{
    if (!CheckCompetitionPeriodIndex(periodIdx))
        return 0;
    int periodSec = G_CDataManager()->m_mercenarySystem.m_field0;
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec14;
    return vec[static_cast<int>(periodIdx)].first * periodSec;
}

// ----------------------------------------------------------------------------
// 17. ORIG T 0x081b2f32 sz 0x113。m_field31 期次检查 + 表越界→0；
// term=(getCurSec()-m_field28)/m_field0；term<期次表值→out=true；
// 否则 term=表值（else 分支二次 operator[] 照 ORIG）。
// ----------------------------------------------------------------------------
int CHARAC_LOAD_MERCENARY::GetCompetitionPeriodTerm(bool* isFinalPeriod) const
{
    if (!CheckCompetitionPeriodIndex(static_cast<char>(m_field31)))
        return 0;
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec14;
    bool bInvalid;
    if (vec.empty())
        bInvalid = true;
    else if (static_cast<unsigned int>(static_cast<char>(m_field31)) <= vec.size())
        bInvalid = false;
    else
        bInvalid = true;
    if (bInvalid)
        return 0;
    int term = (GlobalData::s_systemTime_.getCurSec() - m_field28) /
               G_CDataManager()->m_mercenarySystem.m_field0;
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
void CHARAC_LOAD_MERCENARY::SendMessageToCUser(CUser*, char*) const
{
}

// ----------------------------------------------------------------------------
// 19. ORIG W 0x081b47c8 sz 0xc：movb $1,0x24(%eax)（竞技报名后置位）。
// ----------------------------------------------------------------------------
void CHARAC_LOAD_MERCENARY::alter()
{
    m_field24 = 1;
}

// ----------------------------------------------------------------------------
// 20. ORIG W 0x08694c8e sz 0xc：movb $0,0x24(%eax)（结算后清除）。
// ----------------------------------------------------------------------------
void CHARAC_LOAD_MERCENARY::save_complete()
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
int CHARAC_LOAD_MERCENARY::GetRewardGold(CUser* user,
                                         competition_reward_result& result) const
{
    std::pair<int, int> basePair = Find_max_value_lower_N<std::pair<int, int> >(
        m_level, G_CDataManager()->m_mercenarySystem.m_vec8);
    int base = basePair.first;
    float period = GetPeriodBonus();
    float avatar = GetAvatarBonus();
    int term = GetCompetitionPeriodTerm(0);
    int gold = (int)((float)term * (float)base * period * avatar + 0.01f);

    static CMTRand probabilityRewardItem;
    unsigned long range =
        static_cast<unsigned long>(G_CDataManager()->m_mercenarySystem.m_field3c);
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
    result.m_0 = (int)((float)term * (float)base * period * critical + 0.01f);
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
bool CHARAC_LOAD_MERCENARY::GetRewardItem(
    CUser* user, competition_reward_result& result, Inven_Item& item,
    std::string& msg)
{
    if (user == 0)
        return false;

    item.reset();
    msg = MercenaryHardCodingSettingValue::MERCENARY_BASE_POSTAL_MESSAGE;
    if (!CheckCompetitionAreaIndex(static_cast<char>(m_field30), m_level))
        return false;

    stMercenaryCompetitionArea* pArea = &G_CDataManager()
        ->m_mercenarySystem.m_vec40[static_cast<int>(static_cast<char>(m_field30))];
    if (pArea->m_areaLevel == 0)
        m_field30 = 0;

    static CMTRand probabilityRewardItem;
    int dropRate = CalcItemDropRate(user);
    unsigned long range = 10000;
    static int itemDropDice;
    itemDropDice = probabilityRewardItem.randInt(range);
    if (user->m_field6ef68 != 0)
        itemDropDice = 0;
    if (dropRate <= itemDropDice)
        return false;

    stMercenaryCompetitionArea* pArea2 = &G_CDataManager()
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

    unsigned int groupIdx = 0;
    int groupSum = 0;
    while (groupIdx < pArea2->m_groups.size())
    {
        groupSum += pArea2->m_groups[groupIdx].m_groupId;
        if (groupDice <= groupSum)
            break;
        ++groupIdx;
    }
    if (pArea2->m_groups.size() <= groupIdx)
        groupIdx = 0;

    stMercenaryRewardGroup& group = pArea2->m_groups[groupIdx];
    msg = group.m_name;

    if (!group.m_mobList.empty())
    {
        unsigned long mobRange =
            static_cast<unsigned long>(group.m_field20);
        static int mobDice;
        mobDice = probabilityRewardItem.randInt(mobRange);
        int mobIdx = FindRelativeProb<int>(group.m_mobList, mobDice);

        std::map<int, std::pair<int, int> >::const_iterator it =
            G_CDataManager()->m_independentDrop.m_map24.find(mobIdx);
        if (it != G_CDataManager()->m_independentDrop.m_map24.end())
        {
            std::vector<IndependentDropListInfo>& dropLists =
                G_CDataManager()->m_independentDrop.m_vec0;
            if (it->second.first < (int)dropLists.size())
            {
                std::vector<std::pair<int, int> >& list =
                    dropLists[it->second.first].m_list04;
                unsigned long dropRange =
                    static_cast<unsigned long>(it->second.second);
                static int dropDice;
                dropDice = probabilityRewardItem.randInt(dropRange);
                int sum = 0;
                for (unsigned int j = 0; j < list.size(); ++j)
                {
                    sum += list[j].second;
                    if (dropDice <= sum)
                    {
                        item.m_addInfo = list[j].first;
                        item.m_addInfo2 = 1;
                        G_CDataManager()->getItemList()->create_item(
                            ENUM_ITEM_CREATE_TYPE_1, item, 1);
                        result.m_8 = 1;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    unsigned long itemRange = static_cast<unsigned long>(group.m_field24);
    static int itemDice;
    itemDice = probabilityRewardItem.randInt(itemRange);
    int sum2 = 0;
    for (unsigned int k = 0; k < group.m_itemList.size(); ++k)
    {
        sum2 += group.m_itemList[k].second;
        if (itemDice <= sum2)
        {
            item.m_addInfo = group.m_itemList[k].first;
            item.m_addInfo2 = 1;
            G_CDataManager()->getItemList()->create_item(
                ENUM_ITEM_CREATE_TYPE_1, item, 1);
            result.m_8 = 1;
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
// 23. ORIG T 0x081b3b68 sz 0x5a2。结算主流程：GetCompetitionState 非 0 才继续；
// 局部 static emptyItem/mailTitle/serverMessage/strMessage；try{GetRewardGold+
// GetRewardItem}catch 复位+alter+cMyTrace 返回 1；gold<1 走 MercenaryReturn
// 清理；否则发系统邮件（ReqDBSendNewSystemMail）+ 日志 + DB 记录。
// ----------------------------------------------------------------------------
int CHARAC_LOAD_MERCENARY::Reward(CUser* user,
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
        bool isFinal = false;
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

    bool isFinal2 = false;
    int term2 = GetCompetitionPeriodTerm(&isFinal2);
    user->m_historyLog.MercenaryReturn(m_characNo, m_field28, m_field2c,
                                       term2, gold, 0);

    char status = isFinal2 ? 2 : 1;
    int itemIdx = emptyItem.m_addInfo;
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    int rarity = -1;
    if (pItem != 0)
        rarity = pItem->get_rarity();
    DB_MercenaryLogInfo::makeRequest(
        user->get_acc_id(), m_characNo, m_field28,
        GlobalData::s_systemTime_.getCurSec(), status, m_field31, m_field30,
        gold, itemIdx, rarity);

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
bool CHARAC_LOAD_MERCENARY::Competition(CUser* user, char periodIdx,
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

    stMercenaryCompetitionArea* pArea = &G_CDataManager()
        ->m_mercenarySystem.m_vec40[static_cast<int>(areaIdx)];
    if (pArea->m_worldMapIdx == 0xffff)
    {
        static CMTRand probArea;
        std::vector<int> validIdx;
        for (int i = 0; i < (int)G_CDataManager()
                 ->m_mercenarySystem.m_vec40.size(); ++i)
        {
            stMercenaryCompetitionArea* pA = &G_CDataManager()
                ->m_mercenarySystem.m_vec40[i];
            if (pA->m_areaLevel != 0 && pA->m_areaLevel <= m_level)
                validIdx.push_back(i);
        }
        unsigned long range = validIdx.size() - 1;
        int pick = probArea.randInt(range);
        int idx = validIdx[pick];
        if (CheckCompetitionAreaIndex(static_cast<char>(idx), m_level))
            areaIdx = static_cast<char>(idx);
        else
            areaIdx = 0;
    }

    int seconds = GetCompetitionPeriodSecond(periodIdx);
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
int CUser::GetETC(short key)
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
int CUser::get_SoloPlay_mode()
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
void CUser::set_MailMsgData(MSG_MAILBOX_SEND* msg)
{
    m_pad8e101[0x2eb] = 0;                       // ORIG movb $0x0,0x8e3ec(%eax)
    memcpy(m_pad8e101, msg, 0x145);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08686f58 _ZN5CUser26SendUnlimitChallengePacketEv：
// PacketGuard：header(0,0xa4) + byte(1) + finalize(true) + Send。
// ----------------------------------------------------------------------------
void CUser::SendUnlimitChallengePacket()
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
LimitItemUsage* CUser::GetItemLimitInfo(int itemIdx)
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
bool CUser::IsLimitItemUsable(int itemIdx)
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
void CUser::processGrowTypeEvent(unsigned char p1, unsigned char p2,
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
int CUser::Competition_Mercenary_Charac(unsigned int slot, char modeA, char modeB)
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
void CUser::updateBingoData()
{
    if (static_cast<BingoData*>(getBingoData())->m_needSave)
    {
        DB_UpdateBingoEvent::makeRequest(
            GetUID(), get_acc_id(),
            static_cast<int>(static_cast<BingoData*>(getBingoData())->m_eventId),
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
void CUser::UpdateBloodRecord()
{
    const _Charac_info* cur = getCurCharacR();
    if (cur == 0)
        return;
    for (std::map<int, stBloodBestRecord>::const_iterator it = cur->m_bloodBestRecords.begin(),
         e = cur->m_bloodBestRecords.end(); it != e; ++it)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bbb);
        CStreamGuard guard(stream, true);
        guard << 0x140;
        guard << (int)GetUID();
        SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD* sig =
            guard.GetInBuffer<SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>();
        memset(sig, 0, 0x10);
        *reinterpret_cast<unsigned int*>(sig) = (unsigned int)getCurCharacNo();
        *reinterpret_cast<unsigned int*>((char*)sig + 4) = it->second.m_time;
        *reinterpret_cast<unsigned int*>((char*)sig + 8) = it->second.m_round;
        *reinterpret_cast<unsigned int*>((char*)sig + 12) = (unsigned int)it->first;
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
void CUser::SetEquipment(int slot, void* invenData, int size, void* outData)
{
    int* cnt = (int*)outData;
    for (int i = 0; i <= 9; ++i)
    {
        Inven_Item* items = (Inven_Item*)invenData;
        Inven_Item& it = items[i];
        bool expired = false;
        if (it.m_addInfo != 0)
        {
            // ORIG：get_add_info() 在 `<1` 判与 `<=tick` 判两处各自重调，缓存违反 P183/P48。
            if (it.get_add_info() >= 1)
            {
                int now = OS_API::GetDateTimeTick();
                if (now > it.get_add_info())
                    expired = true;
            }
        }
        if (expired)
        {
            _Charac_info& ch = m_characList[slot];
            ((int*)outData)[*cnt + 1] = ch.m_characNo;
            ((char*)outData)[*cnt + 0x5a4] = (char)i;
            it.reset();
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
void CUser::processFatigueCharac(int usedMin)
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
    if (def == 0)
        return;

    int itemIndex = def->get_index();
    def->make_item(item);
    item.set_add_info(1);

    unsigned short fatigue = getCurCharacUsedFatigue();
    char msg[256];
    OS_API::snprintf(msg, 0xff,
                     g_scriptStringManager_.findString(4, fmtKey.c_str(), 0),
                     fatigue);
    if (itemIndex != 0)
    {
        int expire = def->getExpirationDate();
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, title.c_str(), 0),
            item, 0, getCurCharacNo(), msg, (int)strlen(msg), expire,
            (ENUM_SERVER_GROUP)srvGrp, false, true);
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x086792d6 _ZN5CUser19processAwakenRewardEc：
// switch(stage) 设定邮件正文/标题脚本文案（jump table），命中后构造
// Inven_Item(0x3c0, sealCount=1) 并 ReqDBSendNewSystemMail；default 直接返回。
// ----------------------------------------------------------------------------
void CUser::processAwakenReward(char stage)
{
    int rewardItemId = 0x3c0;
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
unsigned int CUser::SaveMercenaryData()
{
    if (m_mercenaryMgr.m_characs.empty())
        return 1;
    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0xab3b);
    CStreamGuard guard(stream, true);
    guard << 0x2b3;
    guard << (int)GetUID();
    SIG_MERCENARY_DB_DATA* db = guard.GetInBuffer<SIG_MERCENARY_DB_DATA>();
    memset(db, 0, 0xbd4);

    unsigned int row = 0;
    for (unsigned int i = 0; i < m_mercenaryMgr.m_characs.size(); ++i)
    {
        CHARAC_LOAD_MERCENARY& mc = m_mercenaryMgr.m_characs[i];
        if (mc.m_field24 != 0)
        {
            CHARAC_LOAD_MERCENARY* dst =
                (CHARAC_LOAD_MERCENARY*)((char*)db + 4 + row * 0x54);
            memcpy(dst, &mc, 0x54);
            *((unsigned int*)db) = row + 1;
            mc.save_complete();
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

void CUser::updatePcRoomPlayTime()
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
bool CUser::_fnStatQuestClear(unsigned int questIdx, char* context)
{
    CUser* user = *(CUser**)context;
    std::vector<int>* elixirIds = *(std::vector<int>**)(context + 4);
    std::vector<std::pair<int, int> >* outList =
        *(std::vector<std::pair<int, int> >**)(context + 8);

    Quest* q = G_CDataManager()->find_quest((int)questIdx);
    if (q == 0)
        return 1;
    if (q->m_fieldB8 != 0 && q->m_fieldB8 != 0x19)
        return 1;

    std::vector<RewardItemInfo>* rewards = user->getUserQuestRewardItem(q, user);
    for (std::vector<RewardItemInfo>::iterator ri = rewards->begin();
         ri != rewards->end(); ++ri)
    {
        for (std::vector<int>::iterator ii = elixirIds->begin();
             ii != elixirIds->end(); ++ii)
        {
            if (ri->m_itemIdx == *ii)
            {
                user->add_matched_list(*outList, ri->m_itemIdx, ri->m_count);
            }
        }
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
void CUser::calc_quest_stat(_Additioal_info& ai)
{
    std::vector<int> elixirIds;
    for (int i = 0; i <= 7; ++i)
        elixirIds.push_back(elixir_item_id[i]);

    std::vector<std::pair<int, int> > ownCounts;
    count_specific_items(elixirIds, ownCounts);

    std::vector<std::pair<int, int> > rewards;
    WongWork::CQuestClear* qc =
        (WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4);
    qc->enumQuestClear(_fnStatQuestClear, this, &elixirIds, &rewards);

    complete_reward_list(rewards, ownCounts);

    for (size_t i = 0; i < rewards.size(); ++i)
    {
        apply_elixir_item(ai, rewards[i]);
    }
}
