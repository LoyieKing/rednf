// df_game_r WongWork::CMailBox / CMailBox::CMail 方法实现（U2 批次）。
// 依据 docs/class_func_reports/WongWork__CMailBox/ 与 WongWork__CMailBox__CMail/
// 各报告 + ORIG objdump 逐函数转写；类布局见 CGameManager.h（CMail 真实字段）。
// CMailBox 对象仍按 ORIG 偏移访问（+0x00..+0x90，与 CGameManager.cpp 中
// ctor/dtor/Init 的偏移视图一致）。

#include "CGameManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "Inven_Item.h"

#include <algorithm>
#include <cstring>
#include <list>
#include <map>
#include <vector>

namespace
{

template <typename T>
inline T& sig_at(void* base, int off)
{
    return *reinterpret_cast<T*>(static_cast<char*>(base) + off);
}

template <typename T>
inline const T& sig_at(const void* base, int off)
{
    return *reinterpret_cast<const T*>(static_cast<const char*>(base) + off);
}

typedef std::map<unsigned int, WongWork::CMailBox::CMail*> MailMap;
typedef std::map<int, bool> KeepMap;

const int OFF_LOADED      = 0x00;  // bool 加载状态（IsLoaded/SetLoadState）
const int OFF_LASTLOADIDX = 0x04;  // unsigned int（GetLastLoadIdx/SetLoadState 时间）
const int OFF_NOTLOADED   = 0x08;  // int（Get/SetNotLoadedMailCount）
const int OFF_MAILPOOL    = 0x10;  // StaticPool<CMail,20>
const int OFF_MAILMAP     = 0x40;  // map<unsigned int, CMail*>
const int OFF_KEEP        = 0x64;  // map<int,bool>（LetterKeepCount）
const int OFF_LASTLETTER  = 0x7c;  // unsigned int（GetLastLoadLetterIdx）
const int OFF_LOADCNT     = 0x80;  // int（Get/SetLoadedLetterCount）

// CAvatarItemMgr 的宝石槽/扩展信息访问器：完整类定义在 CAvatarItemMgr.h（与
// CInventory.h 镜像声明冲突，不可同时 include），此处以 asm 标签直接对齐 ORIG 符号。
extern char* CMailBox_AvatarGetJewelSocketData(const void* mgr, int key)
    asm("_ZNK8WongWork14CAvatarItemMgr18getJewelSocketDataEi");
extern char* CMailBox_AvatarGetExpansionInfo(const void* mgr, int key)
    asm("_ZNK8WongWork14CAvatarItemMgr16GetExpansionInfoEi");

}  // namespace

namespace WongWork
{

// ============================================================================
// CMailBox::CMail —— 访问器（ORIG 反汇编直译）
// ============================================================================

// ORIG 0x085574d4 (W)
unsigned int CMailBox::CMail::GetLetterId() { return m_letterId; }

// ORIG 0x085574f2 (W)
long CMailBox::CMail::GetPeriod() const { return m_period; }

// ORIG 0x085574e0 (W)：m_period <= nowSec
bool CMailBox::CMail::IsTimeOver(long nowSec) const { return m_period <= nowSec; }

// ORIG 0x08557590 (W)
bool CMailBox::CMail::isUnlimit() const { return m_unlimit != 0; }

// ORIG 0x085574fe (W)
const char* CMailBox::CMail::GetSenderName() const { return m_senderName; }

// ORIG 0x08557516 (W)
unsigned int CMailBox::CMail::GetPackageMoney() const { return m_packageMoney; }

// ORIG 0x08557522 (W)：sret 按值拷贝 Inven_Item（0x3d 字节）
Inven_Item CMailBox::CMail::GetPackageItem() const { return m_item; }

// ORIG 0x0855750a (W)
unsigned int CMailBox::CMail::GetSenderCharacNo() const { return m_senderCharacNo; }

// ORIG 0x0855759c (W)：m_packageType == 1
bool CMailBox::CMail::isAvatarPackage() const { return m_packageType == 1; }

// ORIG 0x085575ae (W)：m_packageType == 2
bool CMailBox::CMail::isCreaturePackage() const { return m_packageType == 2; }

// ORIG 0x085519fe (T)：senderName[0]=0、money=0、item.reset、type=0、unlimit=0
void CMailBox::CMail::ResetPackage()
{
    m_senderName[0] = '\0';
    m_item.reset();
    m_packageMoney = 0;
    m_packageType = 0;
    m_unlimit = 0;
}

// ORIG 0x08551a36 (T)——docs/class_func_reports/WongWork__CMailBox__CMail/MakePacket.md
void CMailBox::CMail::MakePacket(PacketGuard* packet, long nowSec,
                                 CUser const* user) const
{
    packet->put_int((int)m_mailIndex);

    int nameLen = (int)std::strlen(m_senderName);
    packet->put_int(nameLen);
    packet->put_str(m_senderName, nameLen);

    packet->put_int(m_packageMoney);
    packet->put_int(sig_at<int>(&m_item, 0x02));            // item idx（Inven_Item+0x02）
    packet->put_byte(m_item.m_field0 != 0 ? 1 : 0);         // Inven_Item+0x00

    if (!isAvatarPackage())
    {
        packet->put_int(sig_at<int>(this, 0x33));           // avatar/creature uid 域
    }
    else
    {
        CInventory* inv = const_cast<CInventory*>(user->getCurCharacInvenR());
        inv->GetAvatarItemMgrR()->GetRemainDate(sig_at<int>(this, 0x33),
                                                (int)nowSec);
    }

    packet->put_short((int)sig_at<unsigned short>(this, 0x37));

    packet->put_byte((int)(m_item.GetItemAttr() & 0xff));

    packet->put_int(sig_at<int>(this, 0x39));

    // +0x3d 即 Inven_Item 内嵌 stAmplifyOption_t（m_item +0x11）
    stAmplifyOption_t& ampRef =
        sig_at<stAmplifyOption_t>(const_cast<char*>(reinterpret_cast<const char*>(&m_item)), 0x11);
    packet->put_byte((int)(ampRef.getAbilityType() & 0xff));
    packet->put_short((int)(ampRef.getAbilityValue() & 0xffff));

    if (m_item.m_field1 == 5)                               // ITEM_TYPE creature
    {
        CInventory* inv = const_cast<CInventory*>(user->getCurCharacInvenR());
        user_creature::CCreatureMgr* cmgr =
            const_cast<user_creature::CCreatureMgr*>(inv->GetCreatureMgrR());
        packet->put_int(
            cmgr->getRemainTempPeriod(sig_at<int>(this, 0x33), (int)nowSec));
    }

    packet->put_packet(m_item);

    if (isAvatarPackage())
    {
        CInventory* inv = const_cast<CInventory*>(user->getCurCharacInvenR());
        const void* mgr = inv->GetAvatarItemMgrR();
        int uid = sig_at<int>(this, 0x33);

        packet->put_int(0x1e);
        packet->put_binary(CMailBox_AvatarGetJewelSocketData(mgr, uid), 0x1e);

        char* exp = CMailBox_AvatarGetExpansionInfo(mgr, uid);
        packet->put_int(4);
        if (exp == 0)
        {
            stAvatarExpansionInfo_t init;
            init.init();
            packet->put_binary(reinterpret_cast<char*>(&init), 4);
        }
        else
        {
            packet->put_binary(exp, 4);
        }
    }

    if (!isUnlimit())
    {
        long remain = m_period - nowSec;
        if (remain < 1)
            remain = 1;
        packet->put_int((int)remain);
    }
    else
    {
        packet->put_int(0);
    }

    packet->put_int((int)m_letterId);
}

// ============================================================================
// CMailBox —— 查询 / 计数（偏移视图与 CGameManager.cpp 一致）
// ============================================================================

// ORIG 0x08234566 (W)：map::size @+0x40
int CMailBox::GetRecvSize()
{
    return (int)sig_at<MailMap>(this, OFF_MAILMAP).size();
}

// ORIG 0x08557922 (W)：StaticPool<CMail,20>::Remain @+0x10
int CMailBox::GetRemainSize()
{
    return sig_at<StaticPool<CMail, 20> >(this, OFF_MAILPOOL).Remain();
}

// ORIG 0x0823454c (W)
int CMailBox::GetLoadedLetterCount() { return sig_at<int>(this, OFF_LOADCNT); }

// ORIG 0x0823455a (W)
int CMailBox::GetNotLoadedMailCount() { return sig_at<int>(this, OFF_NOTLOADED); }

// ORIG 0x08557944 (W)
int CMailBox::GetLastLoadIdx()
{
    return (int)sig_at<unsigned int>(this, OFF_LASTLOADIDX);
}

// ORIG 0x0855784c (W)
int CMailBox::GetLastLoadLetterIdx()
{
    return (int)sig_at<unsigned int>(this, OFF_LASTLETTER);
}

// ORIG 0x08557886 (W)：keep map::size @+0x64
int CMailBox::GetLetterKeepCount()
{
    return (int)sig_at<KeepMap>(this, OFF_KEEP).size();
}

// ORIG 0x08557866 (W)：clear keep map 并清零 lastLoadLetterIdx
void CMailBox::ClearLetterKeepCount()
{
    sig_at<KeepMap>(this, OFF_KEEP).clear();
    sig_at<unsigned int>(this, OFF_LASTLETTER) = 0;
}

// ORIG 0x084ed222 (W)：map.insert(make_pair(letterId,true))，返回 insert 的 bool
//（新插入 true / 已存在 false；ORIG movzbl insert 返回 pair.second）。
bool CMailBox::InsertLetterKeepCount(unsigned int letterId)
{
    return sig_at<KeepMap>(this, OFF_KEEP)
        .insert(std::make_pair((int)letterId, true)).second;
}

// ORIG 0x08557938 (W)
bool CMailBox::IsLoaded() { return sig_at<bool>(this, OFF_LOADED); }

// ORIG 0x08557950 (W)：写加载状态；置 true 时同时写 lastLoadIdx ← nowSec
void CMailBox::SetLoadState(bool loaded, long nowSec)
{
    *(bool*)((char*)this + OFF_LOADED) = loaded;   // ORIG mov %dl,(%eax) 直写
    if (loaded)
        *(unsigned int*)((char*)this + OFF_LASTLOADIDX) = (unsigned int)nowSec;   // ORIG mov %edx,0x4(%eax)
}

// ORIG 0x085577ac (W)：id==0 → false；否则在 loadLack 列表中查找
int CMailBox::FindPackageLoadLack(unsigned int id)
{
    if (id == 0)
        return 0;
    std::vector<unsigned int>& list = m_loadLackList;
    return std::find(list.begin(), list.end(), id) != list.end() ? 1 : 0;
}

// ORIG 0x08557608 (W)：逐项拷贝 lack 列表前 n 项到 out（越界即停）
void CMailBox::GetPackageLoadLack(unsigned int* out, unsigned int n)
{
    for (unsigned int i = 0; i < n && i < m_loadLackList.size(); ++i)
        out[i] = m_loadLackList[i];
}

// ORIG 0x085522c4 (T, _ZNK)：map find，未找到返回 0
CMailBox::CMail* CMailBox::GetMail(unsigned int mailIndex) const
{
    MailMap const& mails =
        *reinterpret_cast<MailMap const*>(reinterpret_cast<char const*>(this) +
                                          OFF_MAILMAP);
    MailMap::const_iterator it = mails.find(mailIndex);
    if (it == mails.end())
        return 0;
    return it->second;
}

// ORIG 0x08552138 (T)——docs/class_func_reports/WongWork__CMailBox/RemoveMail.md
int CMailBox::RemoveMail(unsigned int mailIndex)
{
    MailMap& mails = sig_at<MailMap>(this, OFF_MAILMAP);
    MailMap::iterator it = mails.find(mailIndex);
    if (it == mails.end())
        return 0;

    CMail* mail = it->second;
    mails.erase(it);

    if (mail->GetLetterId() != 0)
    {
        // 若剩余邮件中不再有同 letterId 者 → 已加载计数 +1
        bool stillExists = false;
        for (MailMap::iterator jt = mails.begin(); jt != mails.end(); ++jt)
        {
            if (jt->second->GetLetterId() == mail->GetLetterId())
                stillExists = true;
        }
        if (!stillExists)
            sig_at<int>(this, OFF_LOADCNT) += 1;
    }

    mail->ResetPackage();
    sig_at<StaticPool<CMail, 20> >(this, OFF_MAILPOOL).Free(mail);
    return 1;
}

// ORIG 0x08551f5a (T)——docs/class_func_reports/WongWork__CMailBox/AddNewMail.md
// stAddNewMailInput 布局（AddNewMail 装载证据，与 CMailBoxHelper 写入一致）：
//   +0x00 mailIndex   +0x04 senderCharacNo  +0x08 packageMoney
//   +0x0c letterId    +0x10 remainSec       +0x14 packageType
//   +0x18 period      +0x1c unlimit(byte)   +0x20 const char* senderName
//   +0x24 Inven_Item*（可空）
CMailBox::CMail* CMailBox::AddNewMail(const stAddNewMailInput& inputRaw)
{
    const void* in = reinterpret_cast<const void*>(&inputRaw);
    CMail* mail = sig_at<StaticPool<CMail, 20> >(this, OFF_MAILPOOL).Acquire();
    if (mail == 0)
        return 0;

    mail->m_mailIndex      = sig_at<unsigned int>(in, 0x00);
    mail->m_senderCharacNo = sig_at<unsigned int>(in, 0x04);
    mail->m_packageMoney   = sig_at<unsigned int>(in, 0x08);
    mail->m_letterId       = sig_at<unsigned int>(in, 0x0c);
    mail->m_remainSec      = sig_at<unsigned int>(in, 0x10);
    mail->m_packageType    = sig_at<int>(in, 0x14);
    mail->m_period         = (long)sig_at<unsigned int>(in, 0x18);
    mail->m_unlimit        = sig_at<unsigned char>(in, 0x1c);

    const Inven_Item* srcItem = sig_at<const Inven_Item*>(in, 0x24);
    if (srcItem == 0)
        mail->m_item = Inven_Item();
    else
        std::memcpy(&mail->m_item, srcItem, sizeof(Inven_Item));

    const char* name = sig_at<const char*>(in, 0x20);
    if (name != 0)
    {
        std::strncpy(mail->m_senderName, name, 0x1d);
        mail->m_senderName[0x1d - 1] = '\0';
    }
    else
    {
        mail->m_senderName[0] = '\0';
    }

    sig_at<MailMap>(this, OFF_MAILMAP)[mail->m_mailIndex] = mail;
    return mail;
}

}  // namespace WongWork

// ============================================================================
// user_creature::CCreatureMgr —— MakePacket 宠物期限链依赖（U2 批次补定义）
// ============================================================================

namespace user_creature
{

// ORIG 0x08339292 (_ZNK...16FindCreatureItemEi)：m_creatureItems map 查找。
CCreatureItem* CCreatureMgr::FindCreatureItem(int uid) const
{
    std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.find(uid);
    if (it == m_creatureItems.end())
        return 0;
    return it->second;
}

// ORIG 0x0833d88c (_ZNK...18getRemainTempPeriodEii)
// docs/class_func_reports/user_creature__CCreatureMgr/getRemainTempPeriod.md：
// m_field30 有 key=uid：v<1 直接返回 v；否则 r=v-nowSec，r==0 时取 1。
// 无记录时委托装备宠的 GetRemainDate（BLOCKED: CCreatureItem 未建模 → -1，推断）。
int CCreatureMgr::getRemainTempPeriod(int uid, int nowSec) const
{
    std::map<int, int>::const_iterator it = m_field30.find(uid);
    if (it != m_field30.end())
    {
        int v = it->second;
        if (v < 1)
            return v;
        int r = v - nowSec;
        return r == 0 ? 1 : r;
    }
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    // ORIG 委托 CCreatureItem::GetRemainDate(nowSec)。
    return item->GetRemainDate(nowSec);
}

}  // namespace user_creature
