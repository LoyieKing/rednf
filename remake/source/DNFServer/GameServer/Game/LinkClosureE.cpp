// LinkClosureE.cpp — 全量链接收敛批 E 散件 producer（LinkClosureE 批次）
//
// 覆盖符号（ORIG nm -S --defined-only 核对；报告路径见各段注释）：
//   CMaxFatigueFactorEvent::GetMaxFatigueFactor()            W 0x0812ce54
//   CBuffGroup::CBuffGroup(CUser*) / IsRight(CUser*)         W 0x08697c3e / 0x08697c64
//   SendLV70Buff(CUser*, unsigned long)                      T 0x0865defc
//   CItemMakingSkill::Send_ItemMakingSkillList(CUser*) const T 0x08545a62
//   Arad_GiftItem_Set::send_mail(CUser*, int, int, int, int) T 0x081a329e
//
// 注：CMaxFatigueFactorEvent / CBuffGroup / CBelong / CItemMakingSkill 在
// CUserOnlyOrig.cpp 等消费 TU 内为最小声明视图；本 TU 提供权威完整视图 + 定义，
// mangled 与 ORIG 一致。CBelong 各 TU（Area.cpp 等）已有同型局部视图。

#include "CBoosterGage.h"         // charac_expand::CData 权威基类（pack(1) 0x5）
#include "PvPTypes.h"             // CMTRand（CItemMakingSkill +0x1c 字段）
#include "PacketGuard.h"
#include "GameWorld.h"            // G_GameWorld / send_to_area 两重载
#include "CUser.h"                // CUser::Send/get_area/get_unique_id/GetServerGroup
#include "CUserCharacInfo.h"      // getCurCharacVill / get_charac_level / getCurCharacNo
#include "CDataManager.h"         // G_CDataManager / find_item
#include "CItemList.h"            // Arad_GiftItem_Set（find / send_mail 声明）
#include "Arad_ItemBonus.h"       // ARAD::send_mail / ARAD_DATAS::AradMailInfo
#include "Inven_Item.h"
#include "CStackableItem.h"      // CStackableItem::m_vec168（兔盒列表 +0x168）
#include "CInventory.h"          // insertItemIntoInventory（Inven_Item 按值入包）
#include "CCharacList.h"         // CCharacList::m_characs[11]（CDataManager+0x14）
#include "CItem.h"                // CItem::make_item 虚槽（vtable slot +0x8）
#include "CMailBoxHelper.h"       // ReqDBSendNewMail
#include "LogManager.h"
#include "GameTypes.h"            // cMyTrace

class CUser;
class CStackableItem;
class CBelong;

// ---------------------------------------------------------------------------
// CMaxFatigueFactorEvent::GetMaxFatigueFactor()
// ORIG W _ZN22CMaxFatigueFactorEvent19GetMaxFatigueFactorEv @0x0812ce54 sz0xb：
//   push/mov; mov 0x8(%ebp),%eax; mov 0xc(%eax),%eax; pop; ret —— 返回 +0xc int。
// 报告缺失（objdump 转写）。消费方 CUserOnlyOrig.cpp（A-2 域，禁碰）。
// ---------------------------------------------------------------------------
class CMaxFatigueFactorEvent
{
public:
    int GetMaxFatigueFactor();   // ORIG W 0x0812ce54

    char m_pad0c[0xc];           // +0x00..+0x0b（布局未还原区）
    int m_fatigueFactor;         // +0x0c（GetMaxFatigueFactor 返回值）
};

int CMaxFatigueFactorEvent::GetMaxFatigueFactor()
{
    return m_fatigueFactor;
}

// ---------------------------------------------------------------------------
// CBuffGroup（CBelong 派生，ORIG vtable @0x08cf3998 slot0=IsRight）
// CBuffGroup::CBuffGroup(CUser*) W @0x08697c3e：call CBelongC1 → 置 vptr
//   → this+0x04 = user。IsRight W @0x08697c64（报告缺失，objdump 转写）：
//   user 等级 >0x45 → 0；|dx|>0x12c 或 |dy|>0x12c → 0；否则 1。
// ---------------------------------------------------------------------------
class CBelong
{
public:
    virtual char IsRight(CUser* user) = 0;   // 抽象虚槽（ORIG 基类槽 __cxa_pure_virtual）
};

class CBuffGroup : public CBelong
{
public:
    CBuffGroup(CUser* user);                 // ORIG W 0x08697c3e
    char IsRight(CUser* user);               // ORIG W 0x08697c64（虚槽 slot0）

    CUser* m_user;                           // +0x04
};

CBuffGroup::CBuffGroup(CUser* user)
{
    m_user = user;
}

char CBuffGroup::IsRight(CUser* user)
{
    if (user->get_charac_level() > 0x45)
        return 0;
    int dx = (int)(unsigned short)user->get_posX() -
             (int)(unsigned short)m_user->get_posX();
    int dy = (int)(unsigned short)user->get_posY() -
             (int)(unsigned short)m_user->get_posY();
    char out;
    if (__builtin_abs(dx) > 0x12c)
        out = 1;
    else if (__builtin_abs(dy) > 0x12c)
        out = 1;
    else
        out = 0;
    return out ? 0 : 1;
}

// ---------------------------------------------------------------------------
// SendLV70Buff(CUser*, unsigned long)
// ORIG T _Z12SendLV70BuffP5CUserm @0x0865defc sz0x1b2。
// 报告 docs/class_func_reports/_lt_global_gt_/SendLV70Buff.md（## 汇编+## 反编译 C）。
// 包 0x100（param→int, 0xe10）直发 + send_to_area（带 CBuffGroup）；
// 包 0x103（get_unique_id 低 16 位, param）send_to_area（无 belong）。
// ---------------------------------------------------------------------------
void SendLV70Buff(CUser* user, unsigned long buff)
{
    CBuffGroup group(user);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x100);
    packet.put_byte(1);
    packet.put_int((int)buff);
    packet.put_int(0xe10);
    packet.finalize(true);
    int area = user->get_area(false);
    int vill = user->getCurCharacVill();
    G_GameWorld()->send_to_area(vill, area, packet, &group);
    packet.clear();
    packet.put_header(0, 0x103);
    packet.put_short((int)(unsigned short)user->get_unique_id());
    packet.put_int((int)buff);
    packet.finalize(true);
    area = user->get_area(false);
    vill = user->getCurCharacVill();
    G_GameWorld()->send_to_area(vill, area, packet);
}

// ---------------------------------------------------------------------------
// CItemMakingSkill（charac_expand::CData 派生）
// ORIG vtable @0x08c9c340：slot1=loadData 0x08545202、slot2=getData 0x08545b42、
//   slot4/5=D1/D0、slot6=_saveData 0x08545276、slot7=_reset 0x085453ba ——
//   与 charac_expand::CData 虚序一致（ResetDailyMidnight/ResetDaily 走基类空实现）。
// ctor 0x08545108（objdump）：CData ctor → vptr → +0x1c = new CMTRand(time(0))；
// dtor 0x08545192：delete +0x1c 后置 0 → CData dtor。字段 +0x06..+0x1b 为
// Send_ItemMakingSkillList 逐槽读取的 11 项技能等级表（movzwl 0x6(%eax,%edx,2)）。
// 其它方法（UpSkillLevel/GainItemMakingSkillPoint 等）实现由后续批次分派。
// ---------------------------------------------------------------------------
class CMTRand;

class CItemMakingSkill : public charac_expand::CData
{
public:
    int Send_ItemMakingSkillList(CUser* user) const;   // ORIG T 0x08545a62
    int GetItemMakingSkillLevel(const CStackableItem* item, CUser* user);   // ORIG T 0x08545cf4
    int GetSkillLevel(ENUM_ITEM_MAKING_TYPE type, CUser* user) const;       // ORIG T 0x08545c0c
    void UpSkillLevel(ENUM_ITEM_MAKING_TYPE type, int point, CUser* user);  // ORIG T 0x0854571c
    void GainItemMakingSkillPoint(const CStackableItem* item, CUser* user); // ORIG T 0x085453e0
    void SetItemMakingSkillPoint(int skillIdx, int point, CUser* user);     // ORIG T 0x08545d24
    char m_pad5;                // +0x05（1 字节；语义未还原 [推断]）
    unsigned short m_skillLevels[11];   // +0x06..+0x1b（11 项等级表）
    CMTRand* m_rand;            // +0x1c（ctor new CMTRand(time(0))，dtor delete）
};

// ORIG T _ZNK16CItemMakingSkill24Send_ItemMakingSkillListEP5CUser @0x08545a62。
// 报告 docs/class_func_reports/CItemMakingSkill/Send_ItemMakingSkillList.md。
// put_header(0,0x15e) → put_byte(0xb) → i=0..0xa {put_byte(i); put_short(表[i])}
// → finalize(true) → Send → 返回 1。
int CItemMakingSkill::Send_ItemMakingSkillList(CUser* user) const
{
    PacketGuard packet;
    packet.put_header(0, 0x15e);
    packet.put_byte(0xb);
    for (int i = 0; i <= 0xa; ++i)
    {
        packet.put_byte(i);
        packet.put_short((short)m_skillLevels[i]);
    }
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

// ---------------------------------------------------------------------------
// CItemMakingSkill 技能成长链（报告 docs/class_func_reports/CItemMakingSkill/）。
// ---------------------------------------------------------------------------

namespace
{

stItemMakingSkill* itemMakingSkillTable()
{
    return reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
}

cUserHistoryLog* userHistoryLogOf(CUser* user)
{
    return &user->m_historyLog;   // CUser::m_historyLog（+0x79700）
}

}  // namespace

// ORIG T _ZNK16CItemMakingSkill13GetSkillLevelE21ENUM_ITEM_MAKING_TYPEP5CUser
// @0x08545c0c。报告 GetSkillLevel.md：idxByExp = 第一个 vec[i].first > 当前点数
// 的下标；idxByLevel = 第一个 user 等级 < vec[i].second 的下标；取 min。
int CItemMakingSkill::GetSkillLevel(ENUM_ITEM_MAKING_TYPE type, CUser* user) const
{
    const stItemMakingSkill* table = itemMakingSkillTable();
    const short cur = static_cast<short>(m_skillLevels[type]);
    int idxByExp = 0;
    while (static_cast<int>(idxByExp) < static_cast<int>(table->m_vec18.size())
           && table->m_vec18[idxByExp].first <= static_cast<int>(cur))
    {
        ++idxByExp;
    }
    int idxByLevel;
    for (idxByLevel = 0;
         idxByLevel < static_cast<int>(table->m_vec18.size()); ++idxByLevel)
    {
        if (user->get_charac_level() < table->m_vec18[idxByLevel].second)
            break;
    }
    if (idxByExp <= idxByLevel)
        idxByLevel = idxByExp;
    return idxByLevel;
}

// ORIG T _ZN16CItemMakingSkill23GetItemMakingSkillLevelEPK14CStackableItemP5CUser
// @0x08545cf4：GetSkillLevel(GetUpSkillType(item), user)。
int CItemMakingSkill::GetItemMakingSkillLevel(const CStackableItem* item, CUser* user)
{
    return GetSkillLevel(static_cast<ENUM_ITEM_MAKING_TYPE>(item->GetUpSkillType()),
                         user);
}

// ORIG T _ZN16CItemMakingSkill12UpSkillLevelE21ENUM_ITEM_MAKING_TYPEiP5CUser
// @0x0854571c。报告 UpSkillLevel.md：type>0xa 或表空守卫 → 直接返回；
// 仅当 point∈(0,m_bonusExp] 且 point<=vec.back().first 时加值；随后按等级
// （0x46 级钳 vec.back().first / 否则逐档钳制）+ 1000 上限 clamp；alter() 通知。
void CItemMakingSkill::UpSkillLevel(ENUM_ITEM_MAKING_TYPE type, int point, CUser* user)
{
    const stItemMakingSkill* table = itemMakingSkillTable();
    if (10 < type)
        return;
    if (table->m_vec18.empty())      // ORIG rbegin==rend 空判定
        return;
    bool allowed = false;
    if (0 < point && point <= table->m_bonusExp
        && point <= table->m_vec18.back().first)
    {
        allowed = true;
    }
    if (!allowed)
        return;

    m_skillLevels[type] =
        static_cast<unsigned short>(static_cast<short>(m_skillLevels[type])
                                    + static_cast<short>(point));
    if (user->get_charac_level() == 0x46)
    {
        if (static_cast<int>(table->m_vec18.back().first)
            < static_cast<short>(m_skillLevels[type]))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xb5, 0);
            trace("CHARACTER_LEVEL_CURRENT_MAX :item_making_skill_[%d] = %d",
                  static_cast<int>(type),
                  static_cast<int>(static_cast<short>(m_skillLevels[type])));
            m_skillLevels[type] =
                static_cast<unsigned short>(table->m_vec18.back().first);
        }
    }
    else
    {
        for (std::vector<std::pair<int, int> >::size_type i = 0;
             i < table->m_vec18.size(); ++i)
        {
            if (user->get_charac_level() < table->m_vec18[i].second
                && static_cast<int>(table->m_vec18[i].first)
                       < static_cast<short>(m_skillLevels[type]))
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xc5, 0);
                trace("NO CHARACTER_LEVEL_CURRENT_MAX : item_making_skill_[%d] = %d",
                      static_cast<int>(type),
                      static_cast<int>(static_cast<short>(m_skillLevels[type])));
                m_skillLevels[type] =
                    static_cast<unsigned short>(table->m_vec18[i].first);
            }
        }
    }
    if (1000 < static_cast<short>(m_skillLevels[type]))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xce, 0);
        trace("ITEM_MAKING_SKILL_LEVEL_LIMIT : item_making_skill_[%d] = %d",
              static_cast<int>(type),
              static_cast<int>(static_cast<short>(m_skillLevels[type])));
        m_skillLevels[type] = 1000;
    }
    alter();
}

// ORIG T _ZN16CItemMakingSkill24GainItemMakingSkillPointEPK14CStackableItemP5CUser
// @0x085453e0。报告 GainItemMakingSkillPoint.md：随机 gained = rand % m_bonusExp
// + m_exp（0x28/0x24）；等级/点数上限违规时 errCode/gained 清零（0xd/0x16/0x5f）；
// UpSkillLevel 后 gained=新增量；包 0x15f 应答 + ItemMakingSkillAdd 日志。
void CItemMakingSkill::GainItemMakingSkillPoint(const CStackableItem* item, CUser* user)
{
    const ENUM_ITEM_MAKING_TYPE type =
        static_cast<ENUM_ITEM_MAKING_TYPE>(item->GetUpSkillType());
    const int needLevel = item->GetNeedLevel();
    const stItemMakingSkill* table = itemMakingSkillTable();
    const int skillLevel = GetItemMakingSkillLevel(item, user);
    int gained = static_cast<unsigned int>(m_rand->randInt())
                     % static_cast<unsigned int>(table->m_bonusExp)
                 + table->m_exp;
    int errCode = 0;
    if (table->m_levelLimit <= skillLevel - needLevel)
    {
        errCode = 0xd;
        gained = 0;
    }
    if (table->m_vec18.empty())      // ORIG rbegin==rend 空判定（+0xe4 跳出）
        return;
    if (user->get_charac_level() == 0x46)
    {
        if (static_cast<int>(table->m_vec18.back().first)
            <= static_cast<short>(m_skillLevels[type]))
        {
            errCode = 0x16;
            gained = 0;
        }
    }
    else
    {
        for (std::vector<std::pair<int, int> >::size_type i = 0;
             i < table->m_vec18.size(); ++i)
        {
            if (user->get_charac_level() < table->m_vec18[i].second
                && static_cast<int>(table->m_vec18[i].first)
                       <= static_cast<short>(m_skillLevels[type]))
            {
                errCode = 0x16;
                gained = 0;
                break;
            }
        }
    }
    if (0x3e7 < static_cast<short>(m_skillLevels[type]))
    {
        errCode = 0x5f;
        gained = 0;
    }
    const short before = static_cast<short>(m_skillLevels[type]);
    UpSkillLevel(type, gained, user);
    gained = static_cast<short>(m_skillLevels[type]) - before;
    PacketGuard packet;
    packet.put_header(0, 0x15f);
    packet.put_byte(errCode);
    packet.put_byte(static_cast<int>(type));
    packet.put_short(gained);
    packet.finalize(true);
    user->Send(packet);
    userHistoryLogOf(user)->ItemMakingSkillAdd(type, errCode, gained, before);
}


// ---------------------------------------------------------------------------
// Arad_GiftItem_Set::send_mail(CUser*, int, int, int, int)
// ORIG T _ZN17Arad_GiftItem_Set9send_mailEP5CUseriiii @0x081a329e sz0x177。
// 报告 docs/class_func_reports/Arad_GiftItem_Set/send_mail.md（汇编+Ghidra C）。
// find(第5,4,3参) → 未命中 logFormat+0 → AradMailInfo 组装：
//   +0x0c=第3参、+0x00=GetServerGroup(字节)、+0x1c=Data.m_str4.c_str、
//   +0x18=Data.m_str8.c_str、+0x14=Data.m_str8.length、
//   m_item{ +0x0c=Data.m_item, +0x04=1, +0x08=2, +0x0a=Inven_Item+0xb(endurance) }
// → find_item 失败 logFormat+0；否则 item.make_item(inven)（CItem vtable slot
//   +0x8）→ 读 endurance → ARAD::send_mail(info)。
// ---------------------------------------------------------------------------
int Arad_GiftItem_Set::send_mail(CUser* user, int p2, int p3, int p4, int giftIdx)
{
    Data* data = find(giftIdx, p4, p3);
    if (data == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x15d, "[ERROR] send mail - Data None ");
        return 0;
    }

    ARAD_DATAS::AradMailInfo info;
    info.m_fieldC = p3;
    info.m_isSystemMail = (unsigned char)user->GetServerGroup();
    info.m_name = const_cast<char*>(data->m_str4.c_str());
    info.m_content = const_cast<char*>(data->m_str8.c_str());
    info.m_field14 = (int)data->m_str8.length();

    Inven_Item inven;
    info.m_item.m_fieldC = data->m_item;
    info.m_item.m_field4 = 1;
    info.m_item.m_field8 = 2;

    CItem* item = G_CDataManager()->find_item(data->m_item);
    if (item == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x175, "[ARAD_RYOSIKA] Item Null. (index:%d)", data->m_item);
        return 0;
    }

    item->make_item(inven);
    info.m_item.m_fieldA = inven.m_fieldb;
    return ARAD::send_mail(info);
}

// ---------------------------------------------------------------------------
// global::tools::InitializeCharacStat::initCharacStat(_Additioal_info&,
//     unsigned int, int, int, int, int)
// ORIG T _ZN6global5tools20InitializeCharacStat14initCharacStatER15_Additioal_infojiiii
// @0x08145e64 sz0x43f。
// 报告 docs/class_func_reports/global__tools__InitializeCharacStat/initCharacStat.md。
// info 全零守卫 → clear → job/level 校验（0x1f 行 trace）→ memcpy 职业基表
// （CDataManager::m_characList + job*0x7dc，0x52 字节）→ 三个成长态
// get_growtype_stat 校验（0x2d 行 trace）→ info = 基表 → level-1 次分段 add
// （<=0xe 态1 / <=0x31 态2 / 其余态3）→ 0x41 行结果 trace。
// ---------------------------------------------------------------------------
namespace global
{
namespace tools
{

class InitializeCharacStat
{
public:
    static void initCharacStat(_Additioal_info& info, unsigned int characNo,
                               int job, int level, int grow1, int grow2);
};

void InitializeCharacStat::initCharacStat(_Additioal_info& info,
                                          unsigned int characNo, int job,
                                          int level, int grow1, int grow2)
{
    if (info.m_field08 != 0 || info.m_field0c != 0 || info.m_field0a != 0 ||
        info.m_field0e != 0 || info.m_field00 != 0 || info.m_field04 != 0 ||
        info.m_field42 != 0)
        return;

    info.clear();
    if (job < 0 || job > 10 || level > 0x46 || level < 1)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x1f, 5);
        trace("[ARAD,STAT,RESET] Invalid character job. charac %u, job %d, level %d",
              characNo, job, level);
        return;
    }

    char base[0x52];
    memcpy(base,
           reinterpret_cast<const char*>(G_CDataManager()->m_characList) +
               job * 0x7dc,
           0x52);

    const CCharacter* charac =
        &G_CDataManager()->m_characList->m_characs[job];
    const void* stat1 = charac->get_growtype_stat(0, 0);
    const void* stat2 = charac->get_growtype_stat(grow1, 0);
    const void* stat3 = charac->get_growtype_stat(grow1, grow2);
    if (stat1 == 0 || stat2 == 0 || stat3 == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x2d, 5);
        trace("[ARAD,STAT,RESET] Invalid character grow type. charac %u, first %d, second %d",
              characNo, grow1, grow2);
        return;
    }

    memcpy(&info, base, 0x52);
    for (int i = 1; i < level; ++i)
    {
        if (i <= 0xe)
            info.add(*reinterpret_cast<const _Additioal_info*>(stat1));
        else if (i <= 0x31)
            info.add(*reinterpret_cast<const _Additioal_info*>(stat2));
        else
            info.add(*reinterpret_cast<const _Additioal_info*>(stat3));
    }

    int v16 = (int)(short)info.m_field10[3];
    int v14 = (int)(short)info.m_field10[2];
    int v12 = (int)(short)info.m_field10[1];
    int v10 = (int)(short)info.m_field10[0];
    int v42 = info.m_field42;
    int v0e = (int)info.m_field0e;
    int v0c = (int)info.m_field0c;
    int v0a = (int)info.m_field0a;
    int v08 = (int)info.m_field08;
    int v04 = info.m_field04;
    int v00 = info.m_field00;
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x41, 0);
    trace("[ARAD][RESET STAT] Init Character : %d, stat : hp %d, mp %d, pyatt %d, pydef %d, mgatt %d, mgdef %d, speed %d, element %d, element %d, element %d, element %d)",
          characNo, v00, v04, v08, v0a, v0c, v0e, v42, v10, v12, v14, v16);
}

}  // namespace tools
}  // namespace global

// ============================================================================
// LinkFinal2 核销 —— CItemMakingSkill::SetItemMakingSkillPoint(int,int,CUser*)
// ORIG T @0x08545d24 (0x24b)，objdump 逐指令转写（GM 设置技能点）：
//   skillIdx>0xa 或等级表空 → 直接返回；
//   point∈(0,0x3e8] 且 point<表尾 first（=表最大经验门槛）才放行（否则返回）；
//   写 m_skillLevels[skillIdx]=(short)point；
//   等级钳制：charac_level==0x46（70）→ 钳到表尾 first；否则从头扫表，
//     user 等级 < vec[i].second 且当前值 > vec[i].first 时钳到 vec[i].first 并停；
//   clamp：值 >0x3e8 → 0x3e8；alter() 通知；Send_ItemMakingSkillList(user)。
// ============================================================================
void CItemMakingSkill::SetItemMakingSkillPoint(int skillIdx, int point,
                                               CUser* user)
{
    const stItemMakingSkill* table = itemMakingSkillTable();
    if (skillIdx > 0xa)                                    // ORIG 0x8545d3f
        return;
    const std::vector<std::pair<int, int> >& vec = table->m_vec18;
    if (vec.rbegin() == vec.rend())                        // ORIG 空表 → 返回
        return;

    bool gate;
    if (point > 0 && point <= 0x3e8)                       // ORIG 0x8545d95
        gate = (point < vec.rbegin()->first);              // ORIG cmp/jge：first>point → 放行
    else
        gate = true;
    if (!gate)
        return;

    m_skillLevels[skillIdx] = static_cast<unsigned short>(point);  // ORIG mov %cx,0x6(,idx,2)

    if (user->get_charac_level() == 0x46)                  // ORIG 0x8545df7
    {
        const short cur = static_cast<short>(m_skillLevels[skillIdx]);
        if (cur > vec.rbegin()->first)                     // ORIG 0x8545e34 setg
            m_skillLevels[skillIdx] =
                static_cast<unsigned short>(vec.rbegin()->first);
    }
    else
    {
        for (unsigned int i = 0; i < vec.size(); ++i)      // ORIG 0x8545e84 循环
        {
            if (user->get_charac_level() < vec[i].second)  // ORIG vec[i]+4 等级档
            {
                const short cur = static_cast<short>(m_skillLevels[skillIdx]);
                if (cur > vec[i].first)                    // ORIG 0x8545ed9 setg
                {
                    m_skillLevels[skillIdx] =
                        static_cast<unsigned short>(vec[i].first);
                    break;                                 // ORIG 0x8545f04
                }
            }
        }
    }

    if (m_skillLevels[skillIdx] > 0x3e8)                   // ORIG 0x8545f31 clamp
        m_skillLevels[skillIdx] = 0x3e8;

    alter();                                               // ORIG 0x8545f4a
    Send_ItemMakingSkillList(user);                        // ORIG 0x8545f5c
}
