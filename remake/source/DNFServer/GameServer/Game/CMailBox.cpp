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
inline __attribute__((always_inline)) T& sig_at(void* base, int off)
{
    return *reinterpret_cast<T*>(static_cast<char*>(base) + off);
}

template <typename T>
inline __attribute__((always_inline)) const T& sig_at(const void* base, int off)
{
    return *reinterpret_cast<const T*>(static_cast<const char*>(base) + off);
}
#define SIG_OFF(T, base, OFF) \
    (*reinterpret_cast<T*>(const_cast<char*>(reinterpret_cast<const char*>(base)) + (OFF)))

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

// b22 桥清零（P201，Model22）：原「CAvatarItemMgr.h 与 CInventory.h 镜像声明
// 冲突」注记过时——CInventory.h:37 已改 include 权威 CAvatarItemMgr.h（本地
// 最小镜像退役，m_avatarItemMgr 即权威类型 0x84），GetAvatarItemMgrR() 直接
// 返回 const CAvatarItemMgr*，两个 asm 标签桥转真 const 成员调用
// （getJewelSocketData W 0x.../GetExpansionInfo，CAvatarItemMgr.h:58-59 声明，
// mangled 与 ORIG 逐字符一致）。

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
unsigned char CMailBox::CMail::isUnlimit() const { return m_unlimit; }

// ORIG 0x085574fe (W)
const char* CMailBox::CMail::GetSenderName() const { return m_senderName; }

// ORIG 0x08557516 (W)
unsigned int CMailBox::CMail::GetPackageMoney() const { return m_package.m_packageMoney; }

// ORIG 0x08557522 (W)：sret 按值拷贝 Inven_Item（0x3d 字节）
Inven_Item CMailBox::CMail::GetPackageItem() const { return m_package.m_item; }

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
    m_package.m_item.reset();
    m_package.m_packageMoney = 0;
    m_unlimit = 0;
    m_packageType = 0;
}

// ORIG 0x08551a36 (T)——docs/class_func_reports/WongWork__CMailBox__CMail/MakePacket.md
void CMailBox::CMail::MakePacket(PacketGuard* packet, long nowSec,
                                 CUser const* user) const
{
    packet->put_int((int)m_mailIndex);

    packet->put_int((int)std::strlen(m_senderName));
    packet->put_str(m_senderName, (int)std::strlen(m_senderName));

    packet->put_int(m_package.m_packageMoney);
    packet->put_int(m_package.m_item.m_addInfo);            // item idx（Inven_Item+0x02）
    packet->put_byte(m_package.m_item.m_field0 != 0 ? 1 : 0);         // Inven_Item+0x00

    if (isAvatarPackage())
    {
        CInventory* inv = const_cast<CInventory*>(user->getCurCharacInvenR());
        // ORIG 8551b25/8551b34：GetRemainDate 返回即 put_int（avatar 分支也 put）
        packet->put_int(inv->GetAvatarItemMgrR()->GetRemainDate(
            m_package.m_item.m_addInfo2, (int)nowSec));
    }
    else
    {
        packet->put_int(m_package.m_item.m_addInfo2);           // avatar/creature uid 域
    }

    packet->put_short((int)m_package.m_item.m_fieldb);

    packet->put_byte((int)(m_package.m_item.GetItemAttr() & 0xff));

    packet->put_int(m_package.m_item.m_fieldd);

    // +0x3d 即 Inven_Item 内嵌 stAmplifyOption_t（m_package.m_item +0x11）
    packet->put_byte((int)(m_package.m_item.m_amp.getAbilityType() & 0xff));
    packet->put_short((int)(m_package.m_item.m_amp.getAbilityValue() & 0xffff));

    if (m_package.m_item.m_field1 == 5)                               // ITEM_TYPE creature
    {
        CInventory* inv = const_cast<CInventory*>(user->getCurCharacInvenR());
        user_creature::CCreatureMgr* cmgr =
            const_cast<user_creature::CCreatureMgr*>(inv->GetCreatureMgrR());
        packet->put_int(
            cmgr->getRemainTempPeriod(m_package.m_item.m_addInfo2, (int)nowSec));
    }

    packet->put_packet(m_package.m_item);

    if (isAvatarPackage())
    {
        int uid = m_package.m_item.m_addInfo2;

        packet->put_int(0x1e);
        // ORIG 8551c6d/8551cab：getCurCharacInvenR/GetAvatarItemMgrR 各点重取（P48 去缓存）
        packet->put_binary(const_cast<CInventory*>(user->getCurCharacInvenR())
                               ->GetAvatarItemMgrR()->getJewelSocketData(uid),
                           0x1e);

        const void* exp = const_cast<CInventory*>(user->getCurCharacInvenR())
                              ->GetAvatarItemMgrR()->GetExpansionInfo(uid);
        packet->put_int(4);
        if (exp == 0)
        {
            stAvatarExpansionInfo_t init;
            init.init();
            packet->put_binary(reinterpret_cast<const char*>(&init), 4);
        }
        else
        {
            packet->put_binary(reinterpret_cast<const char*>(exp), 4);
        }
    }

    if (!isUnlimit())
    {
        long remain = m_period - nowSec;
        packet->put_int(std::max<int>((int)remain, 1));      // ORIG 8551d65 std::max<int>
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
    return (int)m_mailMap.size();
}

// ORIG 0x08557922 (W)：StaticPool<CMail,20>::Remain @+0x10
int CMailBox::GetRemainSize()
{
    return m_mailPool.Remain();
}

// ORIG 0x0823454c (W)
int CMailBox::GetLoadedLetterCount() { return m_loadedLetterCount; }

// ORIG 0x0823455a (W)
int CMailBox::GetNotLoadedMailCount() { return m_notLoadedMailCount; }

// ORIG 0x08557944 (W)
int CMailBox::GetLastLoadIdx()
{
    return (int)m_lastLoadIdx;
}

// ORIG 0x0855784c (W)
int CMailBox::GetLastLoadLetterIdx()
{
    return (int)m_lastLoadLetterIdx;
}

// ORIG 0x08557886 (W)：keep map::size @+0x64
int CMailBox::GetLetterKeepCount()
{
    return (int)m_letterKeepCount.size();
}

// ORIG 0x08557866 (W)：clear keep map 并清零 lastLoadLetterIdx
void CMailBox::ClearLetterKeepCount()
{
    m_letterKeepCount.clear();
    m_lastLoadLetterIdx = 0;
}

// ORIG 0x084ed222 (W)：map.insert(make_pair(letterId,true))，返回 insert 的 bool
//（新插入 true / 已存在 false；ORIG movzbl insert 返回 pair.second）。
bool CMailBox::InsertLetterKeepCount(unsigned int letterId)
{
    return m_letterKeepCount
        .insert(std::make_pair(letterId, true)).second;
}

// ORIG 0x08557938 (W)
bool CMailBox::IsLoaded() { return m_bInit; }

// ORIG 0x08557950 (W)：写加载状态；置 true 时同时写 lastLoadIdx ← nowSec
void CMailBox::SetLoadState(bool loaded, long nowSec)
{
    m_bInit = loaded;                              // ORIG mov %dl,(%eax) 直写
    if (loaded)
        m_lastLoadIdx = (unsigned int)nowSec;      // ORIG mov %edx,0x4(%eax)
}

// ORIG 0x085577ac (W)：id==0 → false；否则在 loadLack 列表中查找
bool CMailBox::FindPackageLoadLack(unsigned int id)
{
    if (id == 0)
        return 0;
    return std::find(m_loadLackList.begin(), m_loadLackList.end(), id)
               != m_loadLackList.end();
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
    MailMap::const_iterator it = m_mailMap.find(mailIndex);
    if (it == m_mailMap.end())
        return 0;
    return it->second;
}

// ORIG 0x08552138 (T)——docs/class_func_reports/WongWork__CMailBox/RemoveMail.md
int CMailBox::RemoveMail(unsigned int mailIndex)
{
    CMail* mail = 0;                       // ORIG movl $0x0 提前清零
    MailMap::iterator it;                  // ORIG 先默认构造再拷贝赋值
    it = m_mailMap.find(mailIndex);
    if (it == m_mailMap.end())
        return 0;

    mail = it->second;
    m_mailMap.erase(it);

    if (mail->GetLetterId() != 0)
    {
        // 若剩余邮件中不再有同 letterId 者 → 已加载计数 +1
        bool stillExists = false;
        for (MailMap::iterator jt = m_mailMap.begin(); jt != m_mailMap.end(); ++jt)
        {
            if (jt->second->GetLetterId() == mail->GetLetterId())
                stillExists = true;
        }
        if (!stillExists)
            m_loadedLetterCount += 1;
    }

    mail->ResetPackage();
    m_mailPool.Free(mail);
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
    CMail* mail = m_mailPool.Acquire();
    if (mail == 0)
        return 0;

    mail->m_mailIndex      = SIG_OFF(unsigned int, &inputRaw, 0x00);
    mail->m_senderCharacNo = SIG_OFF(unsigned int, &inputRaw, 0x04);
    mail->m_package.m_packageMoney   = SIG_OFF(unsigned int, &inputRaw, 0x08);

    // ORIG：srcItem 分支逐字段展开拷贝；空时构造 Inven_Item() 后逐字段搬移。
    const Inven_Item* srcItem = SIG_OFF(const Inven_Item*, &inputRaw, 0x24);
    if (srcItem == 0)
        mail->m_package.m_item = Inven_Item();
    else
        mail->m_package.m_item = *srcItem;

    mail->m_period = (long)SIG_OFF(unsigned int, &inputRaw, 0x18);

    std::strncpy(mail->m_senderName, SIG_OFF(const char*, &inputRaw, 0x20), 0x1d);

    mail->m_packageType = SIG_OFF(int, &inputRaw, 0x14);
    mail->m_unlimit     = SIG_OFF(unsigned char, &inputRaw, 0x1c);
    mail->m_letterId    = SIG_OFF(unsigned int, &inputRaw, 0x0c);

    m_mailMap.insert(std::make_pair(mail->m_mailIndex, mail));

    mail->m_remainSec = SIG_OFF(unsigned int, &inputRaw, 0x10);
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
    if (it != m_creatureItems.end())
        return it->second;
    return 0;
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
        if (it->second > 0)
        {
            int r = it->second - nowSec;
            if (r == 0)
                return 1;
            return r;
        }
        return it->second;
    }
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    // ORIG 委托 CCreatureItem::GetRemainDate(nowSec)。
    return item->GetRemainDate(nowSec);
}

}  // namespace user_creature