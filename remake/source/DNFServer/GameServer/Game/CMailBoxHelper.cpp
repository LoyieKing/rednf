// df_game_r Game/ WongWork::CMailBoxHelper 实现（独立 TU）。
// 依据 docs/class_func_reports/WongWork__CMailBoxHelper/ 各报告逐函数转写。
// SIG_* 布局与 ORIG memset/字段写入证据一致；深水区以 // 推断 标注。

#include "CMailBoxHelper.h"   // 本类权威叶子头（不再经 CGameManager.h）
#include "SigTypes.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CSystemTime.h"
#include "TimerQueue.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CCeraShop.h"
#include "GamePODTypes.h"
#include "GameWorld.h"
#include "STStackableScript.h"
#include "LogManager.h"
#include "RDARScriptStringManager.h"
#include "DB_AvatarChangeOwner.h"

#include <cstring>
#include <cstdlib>
#include <list>
#include <map>
#include <algorithm>

namespace
{
// ---- 本 TU 局部偏移读写助手（SIG 结构按 ORIG 偏移访问，见 SigTypes.h 注释） ----

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

inline void sig_copy(void* dst, int off, const void* src, int len)
{
    std::memcpy(static_cast<char*>(dst) + off, src, len);
}


// CMailBox 内部布局（ORIG 偏移，权威布局批次前以偏移访问）：
//   +0x10 StaticPool<CMail,20> / +0x40 map<uint,CMail*> / +0x58 list<Stream*>
//   +0x80 int letter keep 计数
const int CMAILBOX_OFF_STATICPOOL = 0x10;
const int CMAILBOX_OFF_MAILMAP    = 0x40;
const int CMAILBOX_OFF_STREAMLIST = 0x58;
const int CMAILBOX_OFF_KEEPCNT    = 0x80;

typedef std::map<unsigned int, WongWork::CMailBox::CMail*> MailMap;
typedef std::list<Stream*> MailStreamList;

inline MailMap& mail_map(WongWork::CMailBox* box)
{
    return sig_at<MailMap>(box, CMAILBOX_OFF_MAILMAP); // 推断（ORIG 偏移）
}

} // namespace
// ---- ORIG 全局 T 符号（本 TU 提供定义） ----

// ORIG DB_AccountHack_CheckMyAccount::makeRequest（0x843a13e，T）——全局命名空间，
// 定义由 DB_U3.cpp 提供（匿名命名空间会产生独立 mangled 符号导致 undefined）。
class DB_AccountHack_CheckMyAccount
{
public:
    static int makeRequest(unsigned int uid, unsigned int accId,
                           unsigned int a, unsigned int money,
                           char* senderName, int nameLen);
};

// ORIG 0x085538c0：item->GetItemType()（vtable 槽 3）==0x10 或 ==0x22 时可在
// 村庄使用 → false；否则 true。ORIG 无空指针检查，与汇编一致。
bool _isUnusableSpecialItemInVillage(CItem const* item)
{
    int type = item->GetItemType();
    return !(type == 0x10 || type == 0x22);
}

// ORIG 0x081201dc：find_item(item.m_addInfo)；为空或非 stackable → false；
// 否则 GetAttachType()==7。
bool IsTradeLimitAttachTypeItem(Inven_Item const& item)
{
    CItem* found = G_CDataManager()->find_item(item.m_addInfo);
    if (found == 0)
        return false;
    if (!found->is_stackable())
        return false;
    return found->GetAttachType() == 7;
}
// ORIG 0x0855254c：遍历 SIG_MAILBOX_LIST 无限信列表（count @+0x794，条目
// 步长 0x61；条目基 +i*0x61：+0x5c dword=信件 id，+0x5b byte=无限标志）。
bool isUnlimitMail(SIG_MAILBOX_LIST const* info, int letterId)
{
    for (int i = 0; i < sig_at<int>(info, 0x794); ++i)
    {
        const char* entry =
            reinterpret_cast<const char*>(info) + i * 0x61;
        if (sig_at<int>(entry, 0x5c) == letterId)
            return sig_at<unsigned char>(entry, 0x5b) != 0;
    }
    return false;
}
class DB_InsertCeraItemHistory
{
public:
    static int makeRequest(int itemIdx, unsigned int senderCharacNo,
                           unsigned int receiverCharacNo, char flag); // 0x0844511a (T)
};
namespace AvatarRouletteServer
{
bool isAvatarPottery(unsigned long v);                                // 0x0817fb0a (T)
} // namespace AvatarRouletteServer


namespace WongWork
{

inline unsigned int GetUIDOf(CUser* user) { return user->GetUID(); }



// ---- 生命周期 / 池 ----
CMailBox* CMailBoxHelper::AssignMailBox()
{
    // ORIG 0x08697838 (W)：g_MailBoxPool.Acquire → reset
    CMailBox* box = g_MailBoxPool.Acquire();
    if (box == 0)
        return 0;
    box->reset();
    return box;
}

void CMailBoxHelper::FreeMailBox(CMailBox* box)
{
    // ORIG 0x0869786a (W)
    g_MailBoxPool.Free(box);
}

bool CMailBoxHelper::IsCeraPointItem(unsigned long itemIdx)
{
    // ORIG 0x08553912：0x28eba2 / 0x28ec3d 两固定 idx
    return itemIdx == 0x28eba2 || itemIdx == 0x28ec3d;
}

// ---- 用户侧取件 ----
void CMailBoxHelper::GetPackege(CUser* user, unsigned int mailIndex)
{
    // ORIG 0x085549de
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, 0x15);
        return;
    }
    CMailBox::CMail* mail = box->GetMail(mailIndex);
    if (mail == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, 0x15);
        return;
    }

    std::vector<std::pair<int, int> > items;
    ENUM_ERROR err = ENUM_ERROR_NONE;
    if (!_DoGetPackage(user, mail, items, true, err))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, (unsigned char)(err & 0xff));
        return;
    }
    _DoGetPackage(user, mail, items, false, err);

    PacketGuard packet;
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin();
         it != items.end(); ++it)
    {
        packet.put_header(0, 0xe);
        packet.put_byte(it->first);
        packet.put_short(1);
        if (it->first == 1)
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)2, it->second, packet);
        else if (it->first == 7)
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)3, it->second, packet);
        else
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, it->second, packet);
        packet.finalize(true);
        user->Send(packet);
    }

    unsigned int money = mail->GetPackageMoney();
    if (money != (money / 100000) * 100000)
    {
        char* senderName = const_cast<char*>(mail->GetSenderName());
        DB_AccountHack_CheckMyAccount::makeRequest(
            user->GetUID(), user->get_acc_id(), 0, money,
            senderName, (int)std::strlen(senderName));
    }

    ((cUserHistoryLog*)((char*)user + 0x79700))
        ->RecvMail(mailIndex, mail->GetSenderName(), user->get_acc_name()); // 推断：+0x79700 成员偏移

    packet.clear();
    packet.put_header(1, 0x62);
    packet.put_byte(1);
    packet.put_int((int)mailIndex);
    packet.finalize(true);
    user->Send(packet);

    unsigned int removeList[2];
    removeList[0] = 1;
    removeList[1] = mailIndex;
    ReqDBRemoveMail(user, removeList, false);
    box->RemoveMail(mailIndex);
}

void CMailBoxHelper::ReqDBLoadMail(CUser* user, unsigned int mailIndex)
{
    // ORIG 0x085552ac
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x4e0), true);
    guard << 0x7f;
    guard << (int)user->GetUID();
    SIG_MAILBOX_LIST* p = guard->GetInBuffer<SIG_MAILBOX_LIST>();
    std::memset(p, 0, 0x338a);
    sig_at<unsigned int>(p, 0x00) = mailIndex;
    sig_at<unsigned int>(p, 0x04) = (unsigned int)box->GetLastLoadIdx();
    if (0x14 < box->GetLoadedLetterCount())
        box->SetLoadedLetterCount(0x14);
    sig_at<int>(p, 0x08) = box->GetRemainSize() - box->GetLoadedLetterCount();
    sig_at<unsigned int>(p, 0x0c) = (unsigned int)box->GetLastLoadLetterIdx();
    sig_at<unsigned int>(p, 0x10) = (unsigned int)box->getMailLoadCount();
    box->GetPackageLoadLack(&sig_at<unsigned int>(p, 0x14), 0x14);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CMailBoxHelper::ReqDBRemoveMail(CUser const* user, unsigned int* mailIdx, bool a)
{
    // ORIG 0x08556772
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x8ce), true);
    guard << 0x81;
    guard << (int)user->GetUID();
    SIG_MAILBOX_DEL_MAIL* p = guard->GetInBuffer<SIG_MAILBOX_DEL_MAIL>();
    sig_at<unsigned int>(p, 0x00) = mailIdx[0];
    for (unsigned int i = 1; i <= mailIdx[0]; ++i)
        sig_at<unsigned int>(p, (int)(i * 4)) = mailIdx[i];
    sig_at<unsigned int>(p, 0x54) = a ? 2u : 1u;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CMailBoxHelper::SetPeriodCheckTimer(CUser* user)
{
    // ORIG 0x085523e2
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    long nowSec = GlobalData::s_systemTime_.getCurSec();
    long deadline = nowSec + 0x151800;
    long minPeriod = deadline;
    MailMap& mails = mail_map(box);
    for (MailMap::iterator it = mails.begin(); it != mails.end(); ++it)
    {
        if (!it->second->isUnlimit())
        {
            long period = it->second->GetPeriod();
            if (period < minPeriod)
                minPeriod = period;
        }
    }
    if (minPeriod != deadline)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)8, (int)user->GetLoginTick(),
                                    (int)user->getCurCharacNo(), 0);
    }
}

unsigned int CMailBoxHelper::CheckMailTimeout(CUser* user, PacketGuard* packet)
{
    // ORIG 0x08554e90
    unsigned int removed[21];
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
    {
        removed[0] = 0;
        return removed[0];
    }
    std::memset(removed, 0, sizeof(removed));
    long nowSec = GlobalData::s_systemTime_.getCurSec();

    MailMap& mails = mail_map(box);
    for (MailMap::iterator it = mails.begin(); it != mails.end();)
    {
        CMailBox::CMail* mail = it->second;
        bool expired = (!mail->isUnlimit() && mail->IsTimeOver(nowSec));
        if (!expired)
        {
            ++it;
            continue;
        }
        packet->put_int((int)it->first);
        removed[0] = removed[0] + 1;
        removed[removed[0]] = it->first;
        MailMap::iterator eraseIt = it;
        ++it;
        mails.erase(eraseIt);

        if (mail->GetLetterId() != 0)
        {
            bool found = false;
            for (MailMap::iterator jt = mails.begin(); jt != mails.end(); ++jt)
            {
                if (jt->second->GetLetterId() == mail->GetLetterId())
                    found = true;
            }
            if (!found)
                sig_at<int>(box, CMAILBOX_OFF_KEEPCNT) =
                    sig_at<int>(box, CMAILBOX_OFF_KEEPCNT) + 1;
        }
        mail->ResetPackage();
        sig_at<StaticPool<CMailBox::CMail, 20> >(box, CMAILBOX_OFF_STATICPOOL)
            .Free(mail); // 推断：+0x10 StaticPool<CMail,20>
    }
    ReqDBRemoveMail(user, removed, true);
    return removed[0];
}

void CMailBoxHelper::MakeMailBoxListPacket(CUser* user, PacketGuard* packet)
{
    // ORIG 0x08555118
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    long nowSec = GlobalData::s_systemTime_.getCurSec();
    packet->put_header(0, 0x61);
    int count = 0;
    int idx = packet->get_index();
    packet->put_byte(box->GetRecvSize());
    packet->put_byte(0);
    MailMap& mails = mail_map(box);
    for (MailMap::iterator it = mails.begin(); it != mails.end(); ++it)
    {
        unsigned int letterId = it->second->GetLetterId();
        if (box->FindPackageLoadLack(letterId) == 0)
        {
            count = count + 1;
            it->second->MakePacket(packet, nowSec, user);
        }
    }
    packet->put_byte(idx, count);
    packet->put_short(box->GetNotLoadedMailCount());
}

void CMailBoxHelper::SetMailBoxInfo(CUser* user, SIG_MAILBOX_LIST const* info)
{
    // ORIG 0x085525a6：DB 邮箱列表落地 → 增量/全量 0x61 列表包 + 信件明细。
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    box->incMailLoadCount();
    bool bFirstLoad = !box->IsLoaded();

    PacketGuard packet;
    long nowSec = GlobalData::s_systemTime_.getCurSec();

    // +0x333a：PackageLoadLack 表（20 项）
    std::set<unsigned int> lackSet;
    box->SetPackageLoadLack(&sig_at<unsigned int>(info, 0x333a), 0x14, lackSet);

    int count = 0;
    if (!bFirstLoad)
    {
        packet.put_header(0, 0x61);
        int idx = packet.get_index();
        packet.put_byte(sig_at<int>(info, 0x794));
        packet.put_byte(1);
        MailMap& mails = mail_map(box);
        for (MailMap::iterator it = mails.begin(); it != mails.end(); ++it)
        {
            unsigned int letterId = it->second->GetLetterId();
            if (lackSet.find(letterId) == lackSet.end())
            {
                it->second->MakePacket(&packet, nowSec, user);
                count = count + 1;
            }
        }
        packet.put_byte(idx, sig_at<int>(info, 0x794)); // 占位回填（ORIG 回填 count 于尾部）
        packet.put_short(sig_at<int>(info, 0x798));
    }

    // +0x7a0 avatar 注册区，步长 0x47
    int avatarCount = sig_at<int>(info, 0x7a0);
    for (int i = 0; i < avatarCount; ++i)
    {
        int base = i * 0x47;
        CInventory* invW = user->getCurCharacInvenW();
        invW->GetAvatarItemMgrW()->RegistItem(
            sig_at<int>(info, base + 0x7a4),
            sig_at<int>(info, base + 0x7a8),
            sig_at<int>(info, base + 0x7ac),
            &sig_at<char>(info, base + 0x7b0),
            sig_at<stAvatarEmblemInfo_t>(info, base + 0x7c9),
            sig_at<stAvatarExpansionInfo_t>(info, base + 0x7e7),
            false);
    }

    // +0xd30 creature 临时期限区，步长 0x25
    int creatureCount = sig_at<int>(info, 0xd30);
    for (int i = 0; i < creatureCount; ++i)
    {
        int base = i * 0x25;
        CInventory* invW = user->getCurCharacInvenW();
        invW->GetCreatureMgrW()->RegistTempPeriod(
            sig_at<int>(info, base + 0xd34), sig_at<int>(info, base + 0xd38), 0);
    }

    box->SetLoadState(true, sig_at<long>(info, 0x79c));

    // 信件条目区：count @+0x794，步长 0x61
    int mailCount = sig_at<int>(info, 0x794);
    for (int i = 0; i < mailCount; ++i)
    {
        int base = i * 0x61;
        Inven_Item item;
        int itemIdx = sig_at<int>(info, base + 0x2a);
        if (itemIdx != 0)
        {
            if (itemIdx == 1)
            {
                item.m_addInfo = itemIdx;
                sig_at<int>(&item, 0) = sig_at<int>(info, base + 0x29);
                item.m_addInfo2 = sig_at<int>(info, base + 0x2e);
                sig_at<unsigned short>(&item, 0x0b) =
                    sig_at<unsigned short>(info, base + 0x33);
            }
            else
            {
                CItem* proto = G_CDataManager()->find_item(itemIdx);
                if (proto != 0)
                {
                    proto->make_item(item);
                    item.m_field0 = sig_at<unsigned char>(info, base + 0x29);
                    item.m_addInfo = itemIdx;
                    item.m_addInfo2 = sig_at<int>(info, base + 0x2e);
                    item.SetItemAttr(sig_at<unsigned char>(info, base + 0x32));
                    sig_at<unsigned short>(&item, 0x0b) =
                        sig_at<unsigned short>(info, base + 0x33);
                    sig_at<int>(&item, 0x0d) = sig_at<int>(info, base + 0x35);
                    sig_at<unsigned char>(&item, 0x11) =
                        sig_at<unsigned char>(info, base + 0x43);
                    sig_at<unsigned short>(&item, 0x12) =
                        sig_at<unsigned short>(info, base + 0x44);
                    sig_at<int>(&item, 0x14) = sig_at<int>(info, base + 0x46);
                    sig_at<unsigned short>(&item, 0x18) =
                        sig_at<unsigned short>(info, base + 0x52);
                    item.m_amp.assign(sig_at<unsigned char>(info, base + 0x43),
                                      sig_at<unsigned short>(info, base + 0x44));
                    sig_at<unsigned int>(&item, 0x25) =
                        sig_at<unsigned int>(info, base + 0x46);
                    sig_at<unsigned int>(&item, 0x29) =
                        sig_at<unsigned int>(info, base + 0x4a);
                    sig_at<unsigned int>(&item, 0x2d) =
                        sig_at<unsigned int>(info, base + 0x4e);
                    sig_at<unsigned short>(&item, 0x31) =
                        sig_at<unsigned short>(info, base + 0x52);
                    item.m_upgradeSep.SetUpgradeSeparateAndTradeRestriction(
                        sig_at<unsigned char>(info, base + 0x54));

                    if (sig_at<unsigned char>(info, base + 0x60) == 4 &&
                        IsTradeLimitAttachTypeItem(item))
                    {
                        unsigned char limit = item.GetTradeLimitCount();
                        item.SetTradeLimitCount(limit + 0xff);
                        ((cUserHistoryLog*)((char*)user + 0x79700))
                            ->ChangeItemLimitCount(1, -1, itemIdx, -1, 4); // 推断：+0x79700
                    }
                }
            }
        }

        // 邮件类型：+0x59 money / +0x5a coin → 1/2/0
        int pkgType = 0;
        if (sig_at<unsigned char>(info, base + 0x59) != 0)
            pkgType = 1;
        else if (sig_at<unsigned char>(info, base + 0x5a) != 0)
            pkgType = 2;

        CMailBox::stAddNewMailInput input;
        sig_at<int>(&input, 0x00) = sig_at<int>(info, base + 0x00);
        sig_at<int>(&input, 0x04) = sig_at<int>(info, base + 0x04);
        sig_at<char*>(&input, 0x08) =
            const_cast<char*>(&sig_at<char>(info, base + 0x08)); // 发件人名（推断）
        sig_at<unsigned int>(&input, 0x0c) = sig_at<unsigned int>(info, base + 0x5c);
        sig_at<unsigned char>(&input, 0x10) = sig_at<unsigned char>(info, base + 0x60);
        sig_at<int>(&input, 0x14) = pkgType;
        sig_at<unsigned int>(&input, 0x18) = sig_at<unsigned int>(info, base + 0x55);
        sig_at<unsigned int>(&input, 0x1c) = sig_at<unsigned int>(info, base + 0x5b);

        CMailBox::CMail* added = box->AddNewMail(input);
        if (added != 0 && box->FindPackageLoadLack(
                              sig_at<unsigned int>(info, base + 0x5c)) == 0)
        {
            count = count + 1;
            if (!bFirstLoad)
                added->MakePacket(&packet, nowSec, user);
        }
    }

    box->SetNotLoadedMailCount(sig_at<int>(info, 0x798));
    if (!bFirstLoad)
    {
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        MakeMailBoxListPacket(user, &packet);
        if (mailCount > 0)
            SetPeriodCheckTimer(user);
        box->ClearLetterKeepCount();
        box->SetLoadedLetterCount(0);
    }

    box->SetLoadedLetterCount(box->GetLoadedLetterCount() +
                              sig_at<int>(info, 0x332a));

    // ---- 信件明细区：count @+0x1018，SIG_LETTER_INFO 步长 299 ----
    int detailCount = sig_at<int>(info, 0x1018);
    count = 0;
    int idxPos = packet.get_index();
    packet.clear();
    packet.put_header(1, 0x61);
    packet.put_short(detailCount);

    for (int i = 0; i < detailCount; ++i)
    {
        int base = i * 299;
        unsigned int letterId = sig_at<unsigned int>(info, base + 0x101c);
        if (box->FindPackageLoadLack(letterId) != 0)
            continue; // ORIG：缺载信件入 box 内 pending map（布局待权威批次）

        packet.put_int(sig_at<int>(info, base + 0x101c));
        packet.put_int(sig_at<int>(info, base + 0x1020));
        const char* pTitle = &sig_at<char>(info, base + 0x1024);
        packet.put_int((int)std::strlen(pTitle));
        packet.put_str(pTitle, std::strlen(pTitle));
        const char* pMsg = &sig_at<char>(info, base + 0x1041);
        packet.put_int((int)std::strlen(pMsg));
        packet.put_str(pMsg, std::strlen(pMsg));
        if (!isUnlimitMail(info, letterId))
            packet.put_int(sig_at<int>(info, base + 0x1141));
        else
            packet.put_int(0);
        packet.put_short(sig_at<short>(info, base + 0x1145));
        count = count + 1;

        box->SetLastLoadLetterIdx(std::max<unsigned int>(
            (unsigned int)box->GetLastLoadLetterIdx(), letterId));
        if (sig_at<short>(info, base + 0x1145) == 3)
            box->InsertLetterKeepCount(letterId);
    }
    packet.put_short(idxPos, count);
    packet.finalize(true);
    user->Send(packet);

    if (!bFirstLoad)
    {
        packet.clear();
        packet.put_header(1, 99);
        packet.put_byte(1);
        packet.put_short(box->GetNotLoadedMailCount());
        packet.finalize(true);
        user->Send(packet);
    }
}

// ---- 取件内部实现 ----
bool CMailBoxHelper::_DoGetPackage(CUser* user, CMailBox::CMail const* mail,
                                   std::vector<std::pair<int, int> >& items,
                                   bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08554650
    if (user->GetMailBox() == 0)
        return false;

    Inven_Item const& pkg = mail->GetPackageItem();
    unsigned long itemIdx = (unsigned long)sig_at<unsigned int>(&pkg, 0x02);
    unsigned int money = mail->GetPackageMoney();

    if (money != 0)
    {
        if (bReset)
        {
            if (IsCeraPointItem(itemIdx))
            {
                if (user->GetCeraPoint() + (int)money > 1000000000u)
                {
                    err = (ENUM_ERROR)0xb;
                    return false;
                }
            }
            else
            {
                int limit = G_CDataManager()->GetMoneyLimitPerLevel(
                    user->get_charac_level(), user->get_acc_name());
                if (limit - user->getCurCharacMoney() < (int)money)
                {
                    err = (ENUM_ERROR)0xa;
                    return false;
                }
            }
        }
        else
        {
            if (!IsCeraPointItem(itemIdx))
            {
                Secu_GoldControl* gold = user->GetGoldControl();
                gold->SetMailCharName(mail->GetSenderName());
                user->getCurCharacInvenW()->gain_money((int)money,
                                                       (eMoneyAddReason)0xe, true, 0);
            }
            items.push_back(std::make_pair(0, 0));
        }
    }

    if (itemIdx == 0)
        return true;
    if (itemIdx == 1)
        return _DoGetPackageCoin(user, mail, items, bReset, err);

    if (AvatarRouletteServer::isAvatarPottery(itemIdx))
        return AvatarCoin::GetPackage(
            user, sig_at<int>(&pkg, 0x07), bReset); // 推断：+0x07 avatar uid

    CItem* proto = G_CDataManager()->find_item((int)itemIdx);
    if (proto == 0)
    {
        err = (ENUM_ERROR)1;
        return false;
    }
    if (proto->IsAvatarItem())
        return _DoGetPackageAvatar(user, mail, items, bReset, err);
    if (proto->IsCreatureItem())
        return _DoGetPackageCreature(user, mail, items, bReset, err);
    return _DoGetPackageItem(user, mail, items, bReset, err);
}

bool CMailBoxHelper::_DoGetPackageCoin(CUser* user, CMailBox::CMail const* mail,
                                       std::vector<std::pair<int, int> >& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553672
    CInventory* invW = user->getCurCharacInvenW();
    int coin = (int)invW->GetCoin() + (int)invW->GetEventCoin() +
               (int)invW->GetPayCoin();
    if (coin >= 0x186a1)
    {
        err = (ENUM_ERROR)0x4f;
        return false;
    }
    if (bReset)
        return true;

    Inven_Item const& pkg = mail->GetPackageItem();
    int amount = sig_at<int>(&pkg, 0x03);          // 推断：ORIG 栈偏移 -0x66
    short coinType = sig_at<short>(&pkg, 0x07);    // 推断：ORIG 栈偏移 -0x62

    cUserHistoryLog* hist = (cUserHistoryLog*)((char*)user + 0x79700);
    if (coinType == 1)
    {
        int cur = (int)user->getCurCharacInvenR()->GetEventCoin();
        invW->SetEventCoin(cur + amount);
        hist->EventCoinAdd((int)invW->GetEventCoin(), amount, (eCoinAddReason)2);
    }
    else if (coinType == 2)
    {
        int cur = (int)user->getCurCharacInvenR()->GetPayCoin();
        invW->SetPayCoin(cur + amount);
        hist->PayCoinAdd((int)invW->GetEventCoin(), amount, (eCoinAddReason)3);
    }
    else if (coinType == 0)
    {
        int cur = (int)user->getCurCharacInvenR()->GetCoin();
        invW->SetCoin(cur + amount);
        hist->CoinAdd((int)invW->GetEventCoin(), amount, (eCoinAddReason)0);
    }

    items.push_back(std::make_pair(1, 0));
    return true;
}

bool CMailBoxHelper::_DoGetPackageItem(CUser* user, CMailBox::CMail const* mail,
                                       std::vector<std::pair<int, int> >& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553936
    Inven_Item const& pkgRef = mail->GetPackageItem();
    Inven_Item pkg(pkgRef); // 按值副本（ORIG 栈拷贝语义）
    int itemIdx = sig_at<int>(&pkg, 0x02);

    if (0x28d287 < (unsigned int)itemIdx && (unsigned int)itemIdx < 0x28d29a)
        return user->DoGetGoldItem(itemIdx, items, bReset, err);
    if (IsCeraPointItem((unsigned long)itemIdx))
        return user->DoGetCeraPointItem(mail->GetPackageMoney(), bReset, err,
                                        (unsigned long)itemIdx);

    WongWork::CCeraShop* shop = GlobalData::s_pCeraShop;
    int specialA = 0;
    int specialB = 0;
    bool specialFlag = false;
    bool isSpecial = shop != 0 && shop->IsSpecialItem(itemIdx, specialA, specialB,
                                                      specialFlag);
    if (isSpecial && _isUnusableSpecialItemInVillage(G_CDataManager()->find_item(itemIdx)))
    {
        if (bReset)
        {
            if (specialA == 1)
            {
                if (specialB <= user->getCurCharacCargoR()->GetCapacity())
                {
                    err = (ENUM_ERROR)0xd;
                    return false;
                }
            }
            else if (specialA == 5)
            {
                // 背包镜像预演 event items 插入
                std::vector<std::pair<int, int> > evItems;
                CItem* proto = G_CDataManager()->find_item(itemIdx);
                std::vector<STStackablePackageData>& vec =
                    sig_at<std::vector<STStackablePackageData> >(proto, 0x21c); // 推断：+0x21c
                for (std::vector<STStackablePackageData>::iterator it = vec.begin();
                     it != vec.end(); ++it)
                    evItems.push_back(std::make_pair(it->m_field0, it->m_field4));

                CInventory mirror;
                InventoryMemory mem;
                mirror.SetInventoryMemory(&mem);
                mirror.setCopy(*user->getCurCharacInvenR());
                int inserted = mirror.tryInsertEventItems(evItems);
                if (inserted != (int)evItems.size())
                {
                    err = (ENUM_ERROR)4;
                    return false;
                }
            }
            return true;
        }

        int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
        long curSec = GlobalData::s_systemTime_.getCurSec();
        std::vector<Inven_Item> outItems;
        CCeraShop::stCeraShopItemParam_t param;
        int outCnt = 0;
        if (shop->ProcessSpecialItem(user, itemIdx, 0, restrictCode, curSec, true,
                                     outItems, param, '\0', '\0', false, 1))
        {
            if (outCnt != 0)
                return false;
            if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                shop->SaveFeaturedIdx(user, restrictCode, (int)curSec, 0);
                user->SendRestrictedGoods(true, restrictCode);
            }
            if (itemIdx == 0xc1 &&
                !shop->ProcessSpecialItem(user, 0x10, 0xc352, restrictCode, curSec,
                                          true, outItems, param, '\0', '\0', false, 1))
            {
                LogManager::logFormat(
                    1, "MailBox.cpp",
                    "static bool WongWork::CMailBoxHelper::_DoGetPackageItem(CUser*, "
                    "const WongWork::CMailBox::CMail*, "
                    "WongWork::CMailBoxHelper::IntPairVector&, bool, ENUM_ERROR&)",
                    0x34c, "AuctionVIP Error - Present to friend(add private-store)");
            }
        }
        return true;
    }

    if (bReset)
    {
        if (user->getCurCharacInvenR()->tryInsertItemIntoInventory(pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
        return true;
    }

    int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        pkg, (eItemAddReason)0x15, true, false);
    if (slot < 0)
        return true; // ORIG：失败亦返回 1（不置错误码）
    items.push_back(std::make_pair(0, slot));
    return true;
}

bool CMailBoxHelper::_DoGetPackageAvatar(CUser* user, CMailBox::CMail const* mail,
                                         std::vector<std::pair<int, int> >& items,
                                         bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x0855419c
    (void)items;
    Inven_Item const& pkg = mail->GetPackageItem();
    if (!mail->isAvatarPackage())
    {
        err = (ENUM_ERROR)1;
        return false;
    }
    int avatarUid = sig_at<int>(&pkg, 0x07); // 推断：avatar uid @+0x07

    if (bReset)
    {
        const CAvatarItemMgr* mgr = user->getCurCharacInvenR()->GetAvatarItemMgrR();
        if (mgr->GetExpireDate(avatarUid) == -1)
            return false;
        if (user->getCurCharacInvenW()->tryInsertAvatarIntoInventory(
                *(Inven_Item*)&pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
        return true;
    }

    int slot = user->getCurCharacInvenW()->insertAvatarIntoInventory(
        *(Inven_Item*)&pkg, (eAvatarItemAddReason)5, true);
    if (slot < 0)
        return false;
    user->getCurCharacInvenW()->SendAvatarEvent(0x36, avatarUid, 0, slot + 10, 0);

    const char* sysName =
        g_scriptStringManager_.findString(4, "game_server_msg_221", 0);
    bool fromSystem = std::strcmp(mail->GetSenderName(), sysName) == 0;
    DB_InsertCeraItemHistory::makeRequest(
        avatarUid, mail->GetSenderCharacNo(), user->getCurCharacNo(),
        fromSystem ? 1 : 0);
    return true;
}

bool CMailBoxHelper::_DoGetPackageCreature(CUser* user, CMailBox::CMail const* mail,
                                           std::vector<std::pair<int, int> >& items,
                                           bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x0855447c
    Inven_Item pkg(mail->GetPackageItem());
    if (!mail->isCreaturePackage())
    {
        err = (ENUM_ERROR)1;
        return false;
    }

    if (bReset)
    {
        if (user->getCurCharacInvenR()->tryInsertItemIntoCreature(pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
        return true;
    }

    int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        pkg, (eItemAddReason)0x15, true, true);
    if (slot < 0)
        return false;
    user->getCurCharacInvenW()->GetCreatureMgrW()->RecvMail(
        (Inven_Item*)&pkg, slot, 0x15);
    if (pkg.m_field1 != 5)
        items.push_back(std::make_pair(7, slot));
    return true;
}

// ---- 收件人校验 ----
int CMailBoxHelper::ReqDBCheckReceiver(CUser* user, const char* name,
                                       MSG_CHECK_MAIL_RECEIVER* msg)
{
    // ORIG 0x08555464：直接转发 forchina 版本
    return ReqDBCheckReceiver_forchina(user, name, msg);
}

int CMailBoxHelper::ReqDBCheckReceiver_forchina(CUser* user, const char* name,
                                                MSG_CHECK_MAIL_RECEIVER* msg)
{
    // ORIG 0x08555486：同名拒绝(7)；否则 DB_Q 0x7e 校验请求
    if (std::strcmp(user->getCurCharacName(), name) == 0)
        return 7;

    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("MailBox.cpp", 0x52e),
                       true);
    guard << 0x7e;
    guard << (int)user->GetUID();
    SIG_MAILBOX_CHECK_RECEIVER* p = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    std::strncpy((char*)p, name, 0x1d);
    std::memcpy(sig_at<char*>(p, 0x31), msg, 0x3f88);
    sig_at<unsigned int>(p, 0x2d) = user->get_charac_no(-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}
} // namespace WongWork

namespace WongWork
{
StaticPool<CMailBox, 600> CMailBoxHelper::g_MailBoxPool; // ORIG B 0x0943e000

// ---- 发信 ----
void CMailBoxHelper::_makeSendMailData(SIG_MAILBOX_SEND_NEW_MAIL* msg,
                                       const char* pSender, int nSenderCharacNo,
                                       Inven_Item const& item, unsigned int nDestCharacNo,
                                       unsigned int nAmount, const char* pTitle,
                                       int nTitleLen, unsigned int nB, int nType)
{
    // ORIG 0x08555756 逐字段转写
    std::strncpy(sig_at<char*>(msg, 0x00), pSender, 0x1d);
    sig_at<unsigned int>(msg, 0x22) = nAmount;
    sig_at<int>(msg, 0x1e) = nSenderCharacNo;
    sig_at<unsigned int>(msg, 0x26) = nDestCharacNo;

    const Inven_Item* it = &item;
    sig_at<unsigned char>(msg, 0x2a) = sig_at<unsigned char>(it, 0x00);
    sig_copy(msg, 0x2b, &it->m_addInfo, 4);
    sig_copy(msg, 0x2f, &it->m_addInfo2, 4);
    sig_copy(msg, 0x34, &it->m_fieldb, 2);
    sig_at<unsigned char>(msg, 0x33) = it->GetItemAttr();
    sig_copy(msg, 0x36, &it->m_fieldd, 4);
    sig_copy(msg, 0x3a, static_cast<const char*>(static_cast<const void*>(it)) + 0x15, 4);
    sig_copy(msg, 0x3e, static_cast<const char*>(static_cast<const void*>(it)) + 0x19, 4);
    sig_copy(msg, 0x42, static_cast<const char*>(static_cast<const void*>(it)) + 0x1d, 2);
    it->m_amp.getValues(sig_at<unsigned char>(msg, 0x44),
                        sig_at<unsigned short>(msg, 0x45));
    sig_copy(msg, 0x47, static_cast<const char*>(static_cast<const void*>(it)) + 0x25, 4);
    sig_copy(msg, 0x4b, static_cast<const char*>(static_cast<const void*>(it)) + 0x29, 4);
    sig_copy(msg, 0x4f, static_cast<const char*>(static_cast<const void*>(it)) + 0x2d, 4);
    sig_copy(msg, 0x53, static_cast<const char*>(static_cast<const void*>(it)) + 0x31, 2);
    sig_at<unsigned char>(msg, 0x55) = it->m_upgradeSep.GetUpgradeSeparate();
    sig_at<unsigned char>(msg, 0x58) = it->IsCreatureItemType() ? 1 : 0;
    sig_at<unsigned char>(msg, 0x56) = (it->m_field1 == 8) ? 1 : 0;

    sig_at<unsigned short>(msg, 0x59) = 0;
    std::memset(static_cast<char*>(static_cast<void*>(msg)) + 0x5b, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        unsigned short lens[2];
        lens[0] = (unsigned short)nTitleLen;
        lens[1] = 0xff;
        sig_at<unsigned short>(msg, 0x59) = std::min(lens[0], lens[1]);
        std::strncpy(sig_at<char*>(msg, 0x5b), pTitle, sig_at<unsigned short>(msg, 0x59));
    }
    sig_at<unsigned int>(msg, 0x15b) = nB;
    sig_at<int>(msg, 0x163) = nType;
    sig_at<unsigned char>(msg, 0x167) = 0;
}

int CMailBoxHelper::_DoSendNewMail(CUser* user, Inven_Item const& item,
                                   unsigned int nDestCharacNo, unsigned int nAmount,
                                   bool bA, const char* pTitle, int nTitleLen,
                                   int nType)
{
    // ORIG 0x08555916
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x59b), false);
    guard << 0x80;
    guard << (int)user->GetUID();
    SIG_MAILBOX_SEND_NEW_MAIL* msg = guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    std::memset(msg, 0, 0x168);
    _makeSendMailData(msg, user->getCurCharacName(), user->getCurCharacNo(), item,
                      nDestCharacNo, nAmount, pTitle, nTitleLen, 0, nType);
    sig_at<unsigned int>(msg, 0x15f) = (unsigned int)user->GetServerGroup();
    if (bA)
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else
    {
        CMailBox* box = (CMailBox*)user->GetMailBox();
        MailStreamList& pending =
            sig_at<MailStreamList>(box, CMAILBOX_OFF_STREAMLIST); // 推断：+0x58
        pending.push_back(guard.Get());
    }
    return 0;
}

int CMailBoxHelper::ReqDBSendNewMail(CUser* user, Inven_Item const& item,
                                     unsigned int nDestCharacNo, unsigned int nAmount,
                                     bool bA, const char* pTitle, int nTitleLen)
{
    // ORIG 0x08555ac8：转发 _DoSendNewMail(type=0)
    return _DoSendNewMail(user, item, nDestCharacNo, nAmount, bA, pTitle,
                          nTitleLen, 0);
}

int CMailBoxHelper::ReqDBSendNewMail(CUser* user, MSG_MAILBOX_SEND* msg,
                                     unsigned int nCharacNo, bool bA,
                                     unsigned int nB, int nC)
{
    // ORIG 0x08555b14：发信核心（金币/物品/AVATAR/CREATURE 校验、扣费、DB 请求）。
    // MSG_MAILBOX_SEND 为 opaque（PacketTypes.h m_pad[0x100]），按 ORIG 偏移访问：
    //   +0x11 收件角色名, +0x2e 金币额, +0x36 物品类型选择, +0x37 槽位,
    //   +0x39 item id, +0x3d add_info/目标角色, +0x41 标题长度, +0x45 标题。
    // 返回码：0=成功 1=物品不存在 0x10=金币不足 0x11=校验失败 0x17=类型/绑定不符。
    if (sig_at<int>(msg, 0x3d) < 0)
        return 0x11;

    int tax = 1000;
    if (sig_at<int>(msg, 0x39) == 0)
        tax = 100;
    if (sig_at<int>(msg, 0x2e) != 0)
    {
        tax = (sig_at<int>(msg, 0x2e) * 5) / 100 + tax;
        if (sig_at<int>(msg, 0x39) == 0)
            tax = std::min(0x2774, tax);
        else
            tax = std::min(0x2af8, tax);
    }
    if (user->getCurCharacMoney() < sig_at<int>(msg, 0x2e) + tax)
        return 10;

    CInventory* inven = user->getCurCharacInvenW();
    int invenType = 6;
    Inven_Item found;      // 槽内物品（GetInvenSlot 返回）
    Inven_Item sendCopy;   // 待发副本

    if (sig_at<int>(msg, 0x39) != 0)
    {
        unsigned char mType = sig_at<unsigned char>(msg, 0x36);
        if (mType == 1)       invenType = 2;
        else if (mType == 7)  invenType = 3;
        else if (mType != 0)  return 0x17;
        else                  invenType = 1;

        found = inven->GetInvenSlot((INVEN_TYPE)invenType,
                                    sig_at<unsigned short>(msg, 0x37));

        if (found.m_addInfo != sig_at<int>(msg, 0x39))
            return 0x11;
        if (found.m_field1 == 4)
            return 0x17;
        if (found.isEquipableItemType() != 1 &&
            found.m_addInfo2 < sig_at<int>(msg, 0x3d))
            return 0x11;

        CItem* proto = G_CDataManager()->find_item(found.m_addInfo);
        if (proto == 0)
            return 1;
        if (proto->isPackagable() != 1)
            return 0x17;

        switch (proto->GetAttachType())
        {
        case 1:
        case 2:
            return 0x17;
        case 3:
            if (found.m_field0 != 1)   // 需数量/标志为 1（ORIG 逐字节校验）
                return 0x17;
            break;
        case 7:
            if (!DecreaseTradeLimitCount(found))
                return 0x17;
            ((cUserHistoryLog*)((char*)user + 0x79700))
                ->ChangeItemLimitCount(invenType,
                                       sig_at<unsigned short>(msg, 0x37),
                                       found.m_addInfo, -1, 3);
        }

        if (found.m_upgradeSep.IsTradeRestriction())
            return 0x17;

        found.setCopy(sendCopy);
        if (sendCopy.isEquipableItemType() != 1)
            sendCopy.set_add_info(sig_at<int>(msg, 0x3d));

        if (proto->IsAvatarItem())
        {
            const WongWork::CAvatarItemMgr* mgr = inven->GetAvatarItemMgrR();
            if (mgr->IsTempKey(found.m_addInfo2))
                return 0x11;
            if (mgr->GetExpireDate(found.m_addInfo2) != 0)
                return 0x11;
        }
    }

    if (inven->use_money(tax, (eMoneySubReason)0x10, 1) == 1)
    {
        if (sig_at<int>(msg, 0x2e) != 0)
        {
            user->GetGoldControl()->SetMailCharName((const char*)msg + 0x11);
            if (inven->use_money(sig_at<int>(msg, 0x2e),
                                 (eMoneySubReason)0x11, 1) != 1)
                return 10;
        }

        if (sig_at<int>(msg, 0x39) != 0)
        {
            if (found.m_field1 == 8)   // AVATAR
            {
                if (inven->delete_item((INVEN_TYPE)2,
                                       sig_at<unsigned short>(msg, 0x37),
                                       1, (eItemDelReason)2, 1) != 1)
                    return 0x11;
                inven->SendAvatarEvent(0x36, found.m_addInfo2, 2,
                                       sig_at<unsigned short>(msg, 0x37) + 10, 0);
                DB_AvatarChangeOwner::makeRequest(found.m_addInfo2,
                                                  user->getCurCharacNo(),
                                                  nCharacNo, '\0');
                inven->GetAvatarItemMgrW()->UnRegistItem(found.m_addInfo2);
            }
            else
            {
                if (inven->delete_item((INVEN_TYPE)invenType,
                                       sig_at<unsigned short>(msg, 0x37),
                                       sig_at<int>(msg, 0x3d),
                                       (eItemDelReason)0xf, 1) != 1)
                    return 0x11;
            }
            if (found.IsCreatureItemType())
            {
                inven->GetCreatureMgrW()->SendMail(
                    &found, sig_at<unsigned short>(msg, 0x37), 0xf);
            }
        }

        ((cUserHistoryLog*)((char*)user + 0x79700))
            ->SendMail((::MSG_MAILBOX_SEND*)msg, nB);

        return _DoSendNewMail(user, sendCopy, sig_at<int>(msg, 0x2e), nCharacNo,
                              bA, (const char*)msg + 0x45,
                              sig_at<int>(msg, 0x41), nC);
    }
    return 10;
}

int CMailBoxHelper::ReqDBSendNewMailCashShop(CUser* user, Inven_Item const& item,
                                             unsigned int nDestCharacNo,
                                             unsigned int nAmount, bool bA,
                                             const char* pTitle, int nTitleLen)
{
    // ORIG 0x08556d5c
    if (G_GameWorld()->GetChannelType() == 7)
        return 0;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x95d), true);
    guard << 0x80;
    guard << (int)user->GetUID();
    SIG_MAILBOX_SEND_NEW_MAIL* msg = guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    _makeSendMailData(msg, user->getCurCharacName(), user->getCurCharacNo(), item,
                      nDestCharacNo, nAmount, pTitle, nTitleLen, 0, 0);
    sig_at<unsigned int>(msg, 0x15f) = 0;
    sig_at<unsigned char>(msg, 0x57) = 1;
    if (bA)
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else
    {
        CMailBox* box = (CMailBox*)user->GetMailBox();
        sig_at<MailStreamList>(box, CMAILBOX_OFF_STREAMLIST).push_back(guard.Get());
    }
    return 0;
}

int CMailBoxHelper::ReqDBSendStoredMail(CUser* user)
{
    // ORIG 0x085566ae
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return 0;
    MailStreamList& pending = sig_at<MailStreamList>(box, CMAILBOX_OFF_STREAMLIST);
    for (MailStreamList::iterator it = pending.begin(); it != pending.end(); ++it)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, *it);
    pending.clear();
    return 0;
}

int CMailBoxHelper::ReqDBSendNewSystemMail(const char* pSender, Inven_Item const& item,
                                           unsigned int nA, unsigned int nCharacNo,
                                           const char* pTitle, int nTitleLen,
                                           unsigned int nB,
                                           ENUM_SERVER_GROUP nServerGroup,
                                           bool bA, bool bB)
{
    // ORIG 0x085555e8
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x54f), true);
    guard << 0x80;
    guard << -1;
    SIG_MAILBOX_SEND_NEW_MAIL* msg = guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    std::memset(msg, 0, 0x168);
    _makeSendMailData(msg, pSender, 0, item, nA, nCharacNo, pTitle, nTitleLen, nB, 0);
    sig_at<unsigned int>(msg, 0x15f) = (unsigned int)nServerGroup;
    sig_at<unsigned char>(msg, 0x57) = (unsigned char)(bA ? 1 : 0);
    sig_at<unsigned char>(msg, 0x167) = (unsigned char)(bB ? 1 : 0);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

int CMailBoxHelper::ReqDBSendNewSystemMultiMail(const char* pSender,
                                                Inven_Item const* itemBase,
                                                unsigned int nCount,
                                                unsigned int nA,
                                                unsigned int nCharacNo,
                                                const char* pTitle, int nTitleLen,
                                                unsigned int nB,
                                                ENUM_SERVER_GROUP nServerGroup,
                                                bool bA)
{
    // ORIG 0x08556b68
    if (nCount >= 0xb || nCount == 0)
        return 0x11;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x917), true);
    guard << 0x2a4;
    guard << -1;
    SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* msg =
        guard->GetInBuffer<SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL>();
    std::memset(msg, 0, 0x3a9);
    sig_at<unsigned int>(msg, 0x122) = 0;
    std::strcpy(sig_at<char*>(msg, 0x00), pSender);
    for (unsigned int i = 0; i < nCount; ++i)
    {
        sig_at<Inven_Item>(msg, (int)(i * sizeof(Inven_Item) + 0x126))
            .setCopy(sig_at<Inven_Item>(const_cast<Inven_Item*>(itemBase),
                                        (int)(i * sizeof(Inven_Item))));
    }
    sig_at<unsigned int>(msg, 0x388) = nCount;
    sig_at<unsigned int>(msg, 0x38c) = nA;
    sig_at<unsigned int>(msg, 0x11e) = nCharacNo;
    std::strcpy(sig_at<char*>(msg, 0x1e), pTitle);
    sig_at<unsigned int>(msg, 0x399) = nTitleLen;
    sig_at<unsigned int>(msg, 0x390) = nB;
    sig_at<unsigned int>(msg, 0x395) = (unsigned int)nServerGroup;
    sig_at<unsigned char>(msg, 0x394) = (unsigned char)(bA ? 1 : 0);
    sig_at<unsigned int>(msg, 0x3a5) = 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

int CMailBoxHelper::MakeSystemMultiMailPostal(std::vector<std::pair<int, int> >& items,
                                              Inven_Item* itemBase, unsigned int nMax)
{
    // ORIG 0x08556a14
    if (items.size() >= 0xb)
        return 0;
    if (items.empty())
        return 0;
    unsigned int slot = 0;
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin();
         it != items.end() && slot < nMax; ++it, ++slot)
    {
        Inven_Item tmp;
        tmp.reset();
        sig_at<int>(&tmp, 0x00) = it->first;
        sig_at<int>(&tmp, 0x02) = it->second; // item idx
        G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, tmp,
                                                  it->second);
        (itemBase + slot)->setCopy(tmp);
    }
    return 1;
}

// ---- 系统邮件 ----
void CMailBoxHelper::ReqDBSendNewAvatarMail(CUser* user, int nCharacNo, int nItemIdx,
                                            int nExpireDate, char cA, char cB,
                                            const char* pAgency, const char* pTitle,
                                            int nTitleLen)
{
    // ORIG 0x085561b0
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x7dd), true);
    guard << 0xc5;
    guard << (int)user->GetUID();
    SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK* msg =
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>();
    sig_at<unsigned int>(msg, 0x00) = 0;
    std::strncpy(sig_at<char*>(msg, 0x04), user->getCurCharacName(), 0x1d);
    sig_at<int>(msg, 0x24) = nCharacNo;
    sig_at<unsigned int>(msg, 0x28) = user->getCurCharacNo();
    SIG_AVATAR_INSERT::set((SIG_AVATAR_INSERT*)(static_cast<char*>(static_cast<void*>(msg)) + 0x2c),
                           nCharacNo, (unsigned int)nItemIdx, 0, nExpireDate, true, cA,
                           pAgency, 2, 0);
    sig_at<unsigned short>(msg, 0xa0) = 0;
    std::memset(static_cast<char*>(static_cast<void*>(msg)) + 0xa2, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        sig_at<unsigned short>(msg, 0xa0) = (unsigned short)nTitleLen;
        std::strncpy(sig_at<char*>(msg, 0xa2), pTitle, nTitleLen);
    }
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    CItem* item = G_CDataManager()->find_item(nItemIdx);
    bool hasSocket = false;
    if (item != 0)
        hasSocket = item->IsAvatarItem();  // ORIG vtable+0x10 虚调用
    if (hasSocket)
        ((CEquipItem*)item)->getAvatarSocket((int)cB, emblem);
    SIG_AVATAR_INSERT::setJewelSocket(
        (SIG_AVATAR_INSERT*)(static_cast<char*>(static_cast<void*>(msg)) + 0x2c),
        emblem);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(CUser* user, int nCharacNo,
                                                    int nItemIdx, int nExpireDate,
                                                    char cA, char cB,
                                                    const char* pAgency,
                                                    const char* pTitle, int nTitleLen)
{
    // ORIG 0x08556f16：与 AvatarMail 相同，仅 +0x1a2 置 1 且初始化 +0x7b expansion
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x976), true);
    guard << 0xc5;
    guard << (int)user->GetUID();
    SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK* msg =
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>();
    sig_at<unsigned int>(msg, 0x00) = 0;
    std::strncpy(sig_at<char*>(msg, 0x04), user->getCurCharacName(), 0x1d);
    sig_at<int>(msg, 0x24) = nCharacNo;
    sig_at<unsigned int>(msg, 0x28) = user->getCurCharacNo();
    SIG_AVATAR_INSERT::set((SIG_AVATAR_INSERT*)(static_cast<char*>(static_cast<void*>(msg)) + 0x2c),
                           nCharacNo, (unsigned int)nItemIdx, 0, nExpireDate, true, cA,
                           pAgency, 2, 0);
    sig_at<unsigned char>(msg, 0x1a2) = 1;
    sig_at<unsigned short>(msg, 0xa0) = 0;
    std::memset(static_cast<char*>(static_cast<void*>(msg)) + 0xa2, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        sig_at<unsigned short>(msg, 0xa0) = (unsigned short)nTitleLen;
        std::strncpy(sig_at<char*>(msg, 0xa2), pTitle, nTitleLen);
    }
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    CItem* item = G_CDataManager()->find_item(nItemIdx);
    bool hasSocket = false;
    if (item != 0)
        hasSocket = item->IsAvatarItem();  // ORIG vtable+0x10 虚调用
    if (hasSocket)
        ((CEquipItem*)item)->getAvatarSocket((int)cB, emblem);
    SIG_AVATAR_INSERT::setJewelSocket(
        (SIG_AVATAR_INSERT*)(static_cast<char*>(static_cast<void*>(msg)) + 0x2c),
        emblem);
    stAvatarExpansionInfo_t expansion;
    expansion.init();
    sig_copy(msg, 0x7b, &expansion, sizeof(expansion));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CMailBoxHelper::ReqDBSendNewCreatureMail(CUser* user, int nCharacNo,
                                              Inven_Item const& item,
                                              const char* pTitle, int nTitleLen)
{
    // ORIG 0x08556422
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x83f), true);
    guard << 0xc5;
    guard << (int)user->GetUID();
    SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK* msg =
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>();
    sig_at<unsigned int>(msg, 0x00) = 1;
    std::strncpy(sig_at<char*>(msg, 0x04), user->getCurCharacName(), 0x1d);
    sig_at<int>(msg, 0x24) = nCharacNo;
    sig_at<unsigned int>(msg, 0x28) = user->getCurCharacNo();
    std::memset(static_cast<char*>(static_cast<void*>(msg)) + 0x2c, 0, 0x74);
    sig_at<Inven_Item>(msg, 0x30).setCopy(item);
    sig_at<unsigned int>(msg, 0x58) = 0;
    sig_at<int>(msg, 0x74) = nCharacNo;
    sig_at<unsigned int>(msg, 0x70) = 100;
    sig_at<unsigned char>(msg, 0x7d) = 0x20;
    sig_at<unsigned char>(msg, 0x7f) = 1;
    sig_at<unsigned int>(msg, 0x94) = 0;

    CItem* found = G_CDataManager()->find_item(item.m_addInfo);
    if (found == 0)
        return;
    if (found->is_stackable())
        return;
    sig_at<unsigned char>(msg, 0x7c) =
        (((CEquipItem*)found)->get_sub_type() == 0) ? 1 : 0;
    sig_at<unsigned int>(msg, 0x80) = (unsigned int)found->getUsablePeriod();
    sig_at<unsigned int>(msg, 0x84) = (unsigned int)found->getExpirationDate();
    sig_at<unsigned short>(msg, 0xa0) = 0;
    std::memset(static_cast<char*>(static_cast<void*>(msg)) + 0xa2, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        sig_at<unsigned short>(msg, 0xa0) = (unsigned short)nTitleLen;
        std::strncpy(sig_at<char*>(msg, 0xa2), pTitle, nTitleLen);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CMailBoxHelper::ReqDBSendNewCreatureMailCashShop(CUser* user, int nCharacNo,
                                                      Inven_Item const& item,
                                                      const char* pTitle, int nTitleLen)
{
    // ORIG 0x085571a0：同 CreatureMail，另 +0x1a2=1
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x99c), true);
    guard << 0xc5;
    guard << (int)user->GetUID();
    SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK* msg =
        guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>();
    sig_at<unsigned int>(msg, 0x00) = 1;
    std::strncpy(sig_at<char*>(msg, 0x04), user->getCurCharacName(), 0x1d);
    sig_at<int>(msg, 0x24) = nCharacNo;
    sig_at<unsigned int>(msg, 0x28) = user->getCurCharacNo();
    std::memset(static_cast<char*>(static_cast<void*>(msg)) + 0x2c, 0, 0x74);
    sig_at<Inven_Item>(msg, 0x30).setCopy(item);
    sig_at<unsigned int>(msg, 0x58) = 0;
    sig_at<int>(msg, 0x74) = nCharacNo;
    sig_at<unsigned int>(msg, 0x70) = 100;
    sig_at<unsigned char>(msg, 0x7d) = 0x20;
    sig_at<unsigned char>(msg, 0x7f) = 1;
    sig_at<unsigned int>(msg, 0x94) = 0;
    sig_at<unsigned char>(msg, 0x1a2) = 1;

    CItem* found = G_CDataManager()->find_item(item.m_addInfo);
    if (found == 0)
        return;
    if (found->is_stackable())
        return;
    sig_at<unsigned char>(msg, 0x7c) =
        (((CEquipItem*)found)->get_sub_type() == 0) ? 1 : 0;
    sig_at<unsigned int>(msg, 0x80) = (unsigned int)found->getUsablePeriod();
    sig_at<unsigned int>(msg, 0x84) = (unsigned int)found->getExpirationDate();
    sig_at<unsigned short>(msg, 0xa0) = 0;
    std::memset(static_cast<char*>(static_cast<void*>(msg)) + 0xa2, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        sig_at<unsigned short>(msg, 0xa0) = (unsigned short)nTitleLen;
        std::strncpy(sig_at<char*>(msg, 0xa2), pTitle, nTitleLen);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

int CMailBoxHelper::ReqChangeLetterStat(CUser* user, int mailIdx, int state)
{
    // ORIG 0x085568a6
    if (state != 2)
    {
        if (state == 3)
        {
            CMailBox* box = (CMailBox*)user->GetMailBox();
            if (box == 0)
                return 0x15;
            if (10 < box->GetLetterKeepCount() + 1)
                return 0x16;
        }
        else if (state != 0)
        {
            return 0x13;
        }
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x8f0), true);
    guard << 0xc2;
    guard << (int)user->GetUID();
    SIG_LETTER_STAT* p = guard->GetInBuffer<SIG_LETTER_STAT>();
    sig_at<unsigned int>(p, 0x0) = user->getCurCharacNo();
    sig_at<int>(p, 0x4) = mailIdx;
    sig_at<short>(p, 0x8) = (short)state;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}
} // namespace WongWork

// ---- SIG_AVATAR_INSERT 静态方法（ORIG 0x850d3c8 / 0x850d44c，全局作用域类型） ----

void SIG_AVATAR_INSERT::set(SIG_AVATAR_INSERT* ins, int a1, unsigned int a2,
                            int a3, int expire, bool bSet, char c,
                            const char* agency, int a7, unsigned long a8)
{
    (void)c;
    sig_at<int>(ins, 0x00) = a1;
    sig_at<unsigned int>(ins, 0x04) = a2;
    sig_at<unsigned char>(ins, 0x08) = (unsigned char)a3;
    sig_at<int>(ins, 0x0a) = expire;
    sig_at<unsigned char>(ins, 0x0e) = (unsigned char)expire;
    sig_at<unsigned char>(ins, 0x09) = (unsigned char)(bSet ? 1 : 0);
    if (agency != 0)
        std::strncpy(sig_at<char*>(ins, 0x0f), agency, 0x18);
    sig_at<int>(ins, 0x28) = a7;
    sig_at<unsigned char>(ins, 0x2c) = (unsigned char)a8;
}

void SIG_AVATAR_INSERT::setJewelSocket(SIG_AVATAR_INSERT* ins,
                                       const stAvatarEmblemInfo_t& emblem)
{
    sig_copy(ins, 0x2d, &emblem, 0x1e); // 7 dword + 1 word，同 ORIG 逐字段拷贝
}
