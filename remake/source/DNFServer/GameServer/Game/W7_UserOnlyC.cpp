// W7_UserOnlyC.cpp —— W7C_UserOnly 链接收敛独占 TU（2026-08-27）
//
// 承载此前无 owner 的 ORIG 真实符号定义；函数体依 docs/class_func_reports
// 对应报告或 objdump 逐指令忠实转写，偏移/常量来自汇编。符号清单：
//   CPacketTimeChecker::{reset,check} + validCount_E/validSecond_E
//   CPacketVerifier::CPacketVerifier/Init/UpdatePacket/GetSequence + 4 statics
//   _NS_PI_2ND_GetHackTypeEnum / _NS_PI_2ND_GetHackType（本 TU 内部 static）
//   PIReceiverManager::CheckHashReceived                    T 0x0808bf7e
//   _NS_PI_2ND_RecvHash_CS                                  T 0x08274f73
//   CUser::Parsing(int)                                     T 0x08647890
//   CUser::SetSkillLevel(int,int)                           T 0x0867b51c
//   CUser::getReturnUserLevelKey(int,int)                   T 0x0869230a
//   CUser::GiveItemsReturnUser()                            T 0x08692000
//   CUser::GiveEquipItemsReturnUser()                       T 0x08692140
//   CUser::setCharacLinkMessageFlag(bool)                   W 0x084ece20
//   CUser::SendLimitItemUsageNotiInfo(int,int,int)          T 0x08688f02
//   CUser::SendCeraShopGiftStatisticLog(uint,uint,ulong,uint) T 0x0868a87a
//   LimitItemUsage::{C1,D1,isAnotherDay,isUsable,useIt}     W 0x8695ef0..
//   UserMercenaryInfoMgr::UpdateCharacName(uint,string const&) T 0x081b462e
//   CUserEventUtil::updateCharacterDayEvent(CUser*,int)     T 0x08688230
//   CDungeon::VerifyMaze() const                            T 0x0834c9f0
//   CDungeon::VerifyMaze(MazeScript const&) const           T 0x0834cb7c
//   CStackableItem::GetMonsterCardId()                      W 0x0849f530

#include "CUser.h"
#include "CDungeon.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "CSkill.h"
#include "Cipher.h"
#include "Encryption.h"
#include "PacketSecurity.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"
#include "CHackAnalyzer.h"
#include "CEnvironment.h"
#include "CMailBoxHelper.h"
#include "SkillSlot.h"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <utility>

extern CDataManager* G_CDataManager();
extern char* NumberToString(unsigned int value, int radix);
extern char* GetPacketName(ENUM_PACKETCLASS packetClass, unsigned short packetId);
extern bool CheckDailyScheduleTime(int scheduleTime, long lastPlayTime,
                                   long curTime);
unsigned char MakeHash(unsigned char* data, unsigned int len,
                       unsigned char seed);          // 定义 PIHash.cpp
short _NS_PI_2ND_GetPacketIndex(char* packet);       // 定义 PIHash.cpp

// ============================================================================
// ---- CPacketTimeChecker（ORIG W 0x8696d5c reset / 0x8696d74 check）----
// ============================================================================

int CPacketTimeChecker::validCount_ = 100;     // D 0x093ace48
int CPacketTimeChecker::validSecond_ = 1;      // D 0x093ace4c

void CPacketTimeChecker::reset()
{
    m_lastTime = 0;
    m_count = 0;
}

bool CPacketTimeChecker::check()
{
    ++m_count;
    long now = time(0);
    if (now - m_lastTime >= validSecond_)
    {
        m_lastTime = now;
        m_count = 0;
    }
    else if (m_count > validCount_)
    {
        return false;
    }
    return true;
}

// ============================================================================
// ---- CPacketVerifier（ORIG 0x8696fe0 C1 / 0x8696de4 Init / 0x8696e58
//      UpdatePacket / 0x8696e48 GetSequence）----
// ============================================================================

int CPacketVerifier::m_nVaridCount = 5;         // D 0x093ace38
int CPacketVerifier::m_nVaridSecond = 1;        // D 0x093ace3c
int CPacketVerifier::m_nAuctionVaridCount = 6;  // D 0x093ace40
int CPacketVerifier::m_nAuctionVaridSecond = 1; // D 0x093ace44

CPacketVerifier::CPacketVerifier()
{
    Init();                    // ORIG C1: checker ctor(空体) + Init
}

void CPacketVerifier::Init()
{
    m_lastClass = 0;
    m_lastId = 0;
    m_lastChecksum = 0;
    m_sameCount = 0;
    m_sameTime = 0;
    m_sequence = 0;
    m_auctionCount = 0;
    m_auctionTime = 0;
    m_checker.reset();
}

unsigned short CPacketVerifier::GetSequence()
{
    return (unsigned short)m_sequence;
}

// 返回错误码：0=正常；1=同包洪水；3=timechecker 节流；4=拍卖包洪水。
int CPacketVerifier::UpdatePacket(const _header& hdr)
{
    if (!m_checker.check())
        return 3;

    // 拍卖类包（id 0xb9..0xc2）独立计数窗口。
    unsigned int id = hdr.m_id;
    if ((unsigned int)(id - 0xb9) <= 8)
    {
        ++m_auctionCount;
        if (m_auctionCount > m_nAuctionVaridCount)
        {
            long now = time(0);
            if (now - m_auctionTime < m_nAuctionVaridSecond)
                return 4;
            m_auctionCount = 0;
            m_auctionTime = now;
        }
    }

    // ORIG 比较按 int 直读 class/id/checksum。
    if (static_cast<int>(hdr.m_class) == m_lastClass &&
        static_cast<int>(hdr.m_id) == m_lastId &&
        *reinterpret_cast<const int*>(&hdr.m_checksum) == m_lastChecksum)
    {
        ++m_sameCount;
        if (m_sameCount > m_nVaridCount)
        {
            long now = time(0);
            if (now - m_sameTime < m_nVaridSecond)
                return 1;
            m_sameCount = 0;
            m_sameTime = now;
        }
    }
    else
    {
        m_lastClass = hdr.m_class;
        m_lastId = hdr.m_id;
        m_lastChecksum = *reinterpret_cast<const int*>(&hdr.m_checksum);
        m_sameCount = 0;
        m_sameTime = time(0);
    }
    return 0;
}

// ============================================================================
// ---- PI_2ND 反作弊 hash 族 ----
// ============================================================================

// class==1：包 id → hack type 映射表（ORIG 跳转表 0x08bff190 共 607 项，
// 每项取对应 mov $imm,%eax 桩的返回值，0 = 表内默认分支）。class==0：
// 第二张表 0x08bffb0c 共 563 项；其余 class 返回 0（ORIG 0x82767bc jne →
// 0x82772a2 ret 0）。
#include "W7_PiHackTables.inc"

static int _NS_PI_2ND_GetHackTypeEnum(unsigned char cls, short id)
{
    if (cls == 1)
    {
        if (id > 0x25e)
            return 0;
        return s_pi2HackTypeClass1[id];
    }
    if (cls == 0)
    {
        if (id > 0x232)
            return 0;
        return s_pi2HackTypeClass0[id];
    }
    return 0;
}

// ORIG W 0x08277359：(hackEnum + 1000)*2 + packId，封顶 0xe0f。
static unsigned int _NS_PI_2ND_GetHackType(unsigned int packId,
                                           unsigned char cls,
                                           unsigned short id)
{
    unsigned int v =
        static_cast<unsigned int>(_NS_PI_2ND_GetHackTypeEnum(cls, id) + 0x3e8) *
            2 +
        packId;
    return (v <= 0xe0f) ? v : 0xe0f;
}

// ORIG T 0x0808bf7e PIReceiverManager::CheckHashReceived(uchar*,uint,uint,bool)
// this+0x04 为接收 hash key 字节状态。
bool PIReceiverManager::CheckHashReceived(unsigned char* data,
                                          unsigned int len, unsigned int key,
                                          bool bNewKey)
{
    unsigned char* pThis = reinterpret_cast<unsigned char*>(this);
    if (bNewKey)
        pThis[4] = 0x27;

    const unsigned short packed = static_cast<unsigned short>(key >> 8);
    const unsigned char oldKey = pThis[4];

    // 新 key 低字节重组：位段 12..13→0..1、4..5→2..3、8..9→4..5、整体左移 6 取低 8 位。
    const unsigned char cur = static_cast<unsigned char>(
        ((packed >> 12) & 3) | ((packed >> 2) & 0xc) |
        ((packed & 0x300) >> 4) | (packed << 6));

    // 校验字节：位段 6..7→0..1、14..15→2..3、2..3→4..5、10..11→6..7。
    bool ok;
    if (len == 0)
    {
        ok = true;
    }
    else
    {
        const unsigned char expect = static_cast<unsigned char>(
            ((packed & 0xc0) >> 6) | ((packed & 0xc000) >> 12) |
            ((packed & 0xc) << 2) | ((packed & 0xc00) >> 4));
        ok = (MakeHash(data, len, oldKey) == expect);
    }

    pThis[4] = cur;      // ORIG 无条件写回新 key
    return ok;
}

// ORIG T 0x08274f73：接收 hash 登记 / 异常路径报 hack。
void _NS_PI_2ND_RecvHash_CS(char* packet, CUser* user, unsigned int recvSize)
{
    const unsigned char cls = static_cast<unsigned char>(packet[0]);
    const short index = _NS_PI_2ND_GetPacketIndex(packet);

    std::set<std::pair<unsigned char, unsigned short> >* seen =
        static_cast<std::set<std::pair<unsigned char, unsigned short> >*>(
            user->GetCSHashSet());

    const std::pair<unsigned char, unsigned short> entry(cls, index);
    const bool firstSeen = (seen->find(entry) == seen->end());

    const unsigned int hashDword =
        *reinterpret_cast<const unsigned int*>(packet + 7);

    bool isSpecial = false;
    if (cls == 1 && index == 1)
        isSpecial = true;

    const unsigned int hackPackId = hashDword >> 8;
    const unsigned int bodyLen = recvSize - 13;
    unsigned char* body = reinterpret_cast<unsigned char*>(packet + 13);

    // ORIG：check 通过，或首见包未通过 ⇒ 直接返回；非首见未通过 ⇒ 记 hack。
    bool record;
    if (static_cast<PIReceiverManager*>(user->GetPIReceiverManager())
            ->CheckHashReceived(body, bodyLen, hashDword, isSpecial))
    {
        record = false;
    }
    else
    {
        record = !firstSeen;
    }
    if (!record)
        return;

    const unsigned int hackType =
        _NS_PI_2ND_GetHackType(hackPackId, cls,
                               static_cast<unsigned short>(index));
    WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
    analyzer->addServerHackCnt(user,
                               static_cast<WongWork::ENUM_HACKTYPE>(hackType),
                               1, 0, 0);
    user->lock();
    seen->insert(entry);
    user->unlock();
}

// ============================================================================
// ---- CUser::Parsing（ORIG T 0x08647890，报告 Parsing.md 转写）----
// ============================================================================

bool CUser::Parsing(int received)
{
    _header hdr;
    CNetwork<4096, 450000>& net = m_network;

    int remain = static_cast<int>(net.m_recvOffset) + received;
    const long long nowTick = GlobalData::s_systemTime_.getCurTickCount();

    net.m_lastRecvTick = (unsigned int)nowTick;   // CUser+0xe4（ORIG 64 位整写拆两 int，tick 在低 4 字节）
    net.m_recvGapTick = 0;                        // CUser+0xe8

    if (remain <= 0xc)
    {
        net.m_recvOffset += received;                 // +0xf8
        net.m_recvBuf += received;                    // +0x10c
        cMyTrace t(__PRETTY_FUNCTION__, 0x69d, 0);
        t("(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d]",
          reinterpret_cast<unsigned int>(net.m_recvBuf),
          reinterpret_cast<unsigned int>(net.m_recv), net.m_recvOffset,
          received);
        net.m_bBufOffsetTrace = 1;                    // bufoffset 标记 +0x109
        return true;
    }

    do
    {
        if (net.m_recvOffset != 0)
            net.m_recvBuf -= net.m_recvOffset;

        if (net.m_bBufOffsetTrace != 0)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x6b2, 0);
            t("[BUFOFFSET TRACE](offset:%x - buf:%x) = remainlen:%d, Recv Size[%d]",
              reinterpret_cast<unsigned int>(net.m_recvBuf),
              reinterpret_cast<unsigned int>(net.m_recv), net.m_recvOffset,
              received);
        }

        std::memcpy(&hdr, net.m_recvBuf, 13);

        if (hdr.m_size < 13 || hdr.m_size > 0x578)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x6ce,
                "Recv Size[%d], Parsing Packet Size[%d], recvbuf_offset[%d] is Too Large",
                received, static_cast<int>(hdr.m_size),
                reinterpret_cast<int>(net.m_recvBuf));
            return false;
        }

        if (remain < hdr.m_size)
            break;

        int verdict = m_packetVerifier.UpdatePacket(hdr);
        if (verdict != 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x6f4,
                "[Packet Varifier Error %d][classification %d][packet id %d][ServerSequence: %d][ClientSequence: %d](m_id: %s)",
                verdict, static_cast<int>(hdr.m_class),
                static_cast<int>(hdr.m_id),
                static_cast<int>(m_packetVerifier.GetSequence()),
                static_cast<int>(hdr.m_seq), NumberToString(get_acc_id(), 0));
            if (verdict != 4)
                return false;
        }

        if (verdict == 0)
        {
            _NS_PI_2ND_RecvHash_CS(net.m_recvBuf, this,
                                   static_cast<unsigned int>(hdr.m_size));

            if (hdr.m_id != 0x25c)
            {
                unsigned char decrypted[0x578];
                int outLen = 0x578;
                G_Cipher()->Decrypt(
                    static_cast<int>(hdr.m_id),
                    reinterpret_cast<const unsigned char*>(net.m_recvBuf + 13),
                    hdr.m_size - 13, decrypted, &outLen);
                if (outLen > 0)
                    std::memcpy(net.m_recvBuf + 13, decrypted, outLen);

                unsigned int crc = Encryption::crc32N(
                    reinterpret_cast<void*>(net.m_recvBuf + 11),
                    hdr.m_size - 11);
                _NS_PI_2ND_MakeChecksumTo1Byte2(crc, hdr.m_checksum);
                if (hdr.m_checksum != crc)
                {
                    cMyTrace t(__PRETTY_FUNCTION__, 0x73a, 5);
                    t("[CHECK SUM] check_sum error PACKET : %s",
                      GetPacketName((ENUM_PACKETCLASS)hdr.m_class, hdr.m_id));
                    return false;
                }
            }

            const int streamLen = hdr.m_size + 8;
            CStreamGuard guard(true);
            Stream* acquired = 0;
            if (streamLen <= 0xfff)
                acquired = GlobalData::s_small_stream_pool->Acquire("user.cpp", 0x758);
            else if (streamLen <= 0x3fff)
                acquired = GlobalData::s_stream_pool->Acquire("user.cpp", 0x75c);
            else
                acquired = GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x760);
            guard.Set(acquired);

            guard << 0;
            guard << static_cast<int>(GetUID());
            guard.put_binary(reinterpret_cast<void*>(net.m_recvBuf), hdr.m_size);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            net.m_bBufOffsetTrace = 0;
        }

        remain -= hdr.m_size;
        net.m_recvBuf += hdr.m_size;
        net.m_recvOffset = 0;

        if (remain == 0)
        {
            net.m_recvBuf = net.m_recv;
            break;
        }
    } while (remain > 0xc);

    net.m_bBufOffsetTrace = 0;

    if (remain > 0)
    {
        if (remain > 0x1000)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x793,
                "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                remain);
            return false;
        }
        memmove(net.m_recv, net.m_recvBuf, remain);
        net.m_recvOffset = remain;
        net.m_recvBuf = net.m_recv + remain;
    }
    return true;
}

// ============================================================================
// ---- CUser::SetSkillLevel（ORIG T 0x0867b51c，报告 SetSkillLevel.md）----
// ============================================================================

void CUser::SetSkillLevel(int skillIdx, int level)
{
    bool runnable = true;
    if (*reinterpret_cast<const unsigned char*>(
            reinterpret_cast<const char*>(this) + 0x711d8) == 1)
    {
        runnable = (getCurCharacR() == 0);
    }
    if (!runnable)
        return;

    const int job = get_charac_job();
    CSkill* skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
        return;

    const char secondGrow = getCurCharSecondGrowType();
    if (secondGrow < 1)
    {
        level = std::max(
            0, std::min(skill->get_type_max_level(getCurCharFirstGrowType()),
                        level));
    }
    else
    {
        int maxLevel = skill->get_second_growtype_max_level(
            getCurCharFirstGrowType(), secondGrow);
        if (maxLevel == 0)
            maxLevel = skill->get_type_max_level(getCurCharFirstGrowType());
        level = std::max(0, std::min(maxLevel, level));
    }

    const int treeIdx = GetCurCharacSkillTreeIndex();
    reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->growtype_skill(job, skillIdx, level,
                                         (ENUM_SKILL_TREE_KIND)treeIdx);
}

// ============================================================================
// ---- CUser 回馈用户物品族 ----
// ============================================================================

// ORIG T 0x0869230a：等级段映射。
int CUser::getReturnUserLevelKey(int level, int mode)
{
    if (mode != 0)
    {
        if (level <= 0x13) return 0xf;
    }
    else
    {
        if (level <= 9) return 5;
        if (level <= 0xe) return 10;
        if (level <= 0x13) return 0xf;
    }
    if (level <= 0x1d) return 0x14;
    if (level <= 0x27) return 0x1e;
    if (level <= 0x31) return 0x28;
    if (level <= 0x3b) return 0x32;
    if (level <= 0x45) return 0x3c;
    return 0x46;
}

// ORIG T 0x08692000，报告 GiveItemsReturnUser.md。
void CUser::GiveItemsReturnUser()
{
    // ORIG 读 G_CDataManager()+0xb548 = m_returnUserReward.m_field0。
    const int itemIdx = G_CDataManager()->m_returnUserReward.m_field0;
    Inven_Item item;                                 // ctor 清零
    CItem* base = G_CDataManager()->find_item(itemIdx);
    if (base != 0)
    {
        item.m_addInfo = itemIdx;                    // +0x02 item idx
        base->make_item(item);                       // vtable+8 虚调用
        item.set_add_info(1);

        const char* msgBody =
            g_scriptStringManager_.findString(4, "return_user_msg_1", 0);
        const ENUM_SERVER_GROUP group = GetServerGroup();
        const unsigned int msgLen = strlen(msgBody);
        const unsigned int characNo =
            static_cast<unsigned int>(getCurCharacNo());
        const char* sender =
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item, 100000u, characNo, msgBody,
            static_cast<int>(msgLen), 7u, group, false, false);
    }
}

// ORIG T 0x08692140（objdump 直转）：装备回馈走 +0xb550 =
// m_returnUserReward.m_levelUp 表。
void CUser::GiveEquipItemsReturnUser()
{
    const int growType = getCurCharFirstGrowType();
    const int level = get_charac_level();
    const int key = getReturnUserLevelKey(level, growType);

    const int job = get_charac_job();
    LevelUpRewardItemTableKey tableKey(key, job, growType);
    LevelUpRewardItemData data;
    if (!G_CDataManager()->m_returnUserReward.m_levelUp.get_data(tableKey,
                                                                 data))
        return;

    Inven_Item item;
    CItem* base = G_CDataManager()->find_item(data.m_field0);
    if (base == 0)
        return;

    item.m_addInfo = data.m_field0;
    base->make_item(item);
    item.set_add_info(data.m_field4);

    const char* msgBody =
        g_scriptStringManager_.findString(4, "return_user_msg_2", 0);
    const ENUM_SERVER_GROUP group = GetServerGroup();
    const unsigned int msgLen = strlen(msgBody);
    const unsigned int characNo =
        static_cast<unsigned int>(getCurCharacNo());
    const char* sender =
        g_scriptStringManager_.findString(4, "return_user_msg_title", 0);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(sender, item, 0u, characNo,
                                                     msgBody,
                                                     static_cast<int>(msgLen),
                                                     7u, group, false, false);
}

// ORIG W 0x084ece20：单字节写 this+0x703d8（m_breakAway 尾 pad 字节）。
void CUser::setCharacLinkMessageFlag(bool flag)
{
    m_breakAway.m_pad14[0] = flag ? 1 : 0;
}

// ORIG T 0x08688f02，报告 SendLimitItemUsageNotiInfo.md。
void CUser::SendLimitItemUsageNotiInfo(int a, int b, int c)
{
    PacketGuard guard;
    guard.put_header(0, 0x11f);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_int(c);
    guard.finalize(true);
    Send(guard);
}

// ORIG T 0x0868a87a，报告 SendCeraShopGiftStatisticLog.md。
void CUser::SendCeraShopGiftStatisticLog(unsigned int arg1, unsigned int arg2,
                                         unsigned long val64,
                                         unsigned int arg4)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0xa2c0);
    CStreamGuard guard(stream, true);
    guard << 0x29c;
    guard << static_cast<int>(GetUID());

    SIG_STATISTIC_CERASHOP_GIFT_LOG* sig =
        guard.GetInBuffer<SIG_STATISTIC_CERASHOP_GIFT_LOG>();
    unsigned char* raw = reinterpret_cast<unsigned char*>(sig);
    *reinterpret_cast<unsigned int*>(raw + 0x00) = m_accId;
    *reinterpret_cast<unsigned int*>(raw + 0x04) =
        static_cast<unsigned int>(getCurCharacNo());
    *reinterpret_cast<unsigned int*>(raw + 0x08) = arg1;
    *reinterpret_cast<unsigned int*>(raw + 0x0c) = arg2;
    *reinterpret_cast<unsigned long*>(raw + 0x10) = val64;
    *reinterpret_cast<unsigned int*>(raw + 0x14) = arg4;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ============================================================================
// ---- LimitItemUsage（ORIG W；字段按汇编偏移访问，头内布局保留 pad 形态）----
// ============================================================================

// 说明：LimitItemUsage 为 packed 布局（+0x15/+0x19 非对齐 int），
// 具名成员直写（gcc packed 自动非对齐访问）即等价 ORIG 行为。

// ORIG C1/C2 0x08695ef0：CommonTime 子对象刷新当前时间，其余字段清零。
LimitItemUsage::LimitItemUsage()
{
    CommonTime* t = reinterpret_cast<CommonTime*>(this);
    t->SetCurTime();
    m_field10 = 0;
    m_pad11[0] = 0;
    m_pad11[1] = 0;
    m_pad11[2] = 0;
    m_pad11[3] = 0;
    m_field15 = 0;
    m_field19 = 0;
}

// ORIG D1/D2/D0 0x086a6480：空析构（ORIG 仅 ret）。
LimitItemUsage::~LimitItemUsage()
{
}

// ORIG W 0x08688fba，报告 LimitItemUsage/isAnotherDay.md。
bool LimitItemUsage::isAnotherDay()
{
    const long serverSchedule = static_cast<long>(
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(
                                          G_CEnvironment()) + 0x37c));
    const CommonTime* ct = reinterpret_cast<const CommonTime*>(this);
    const long createTime = static_cast<long>(*ct);   // operator long()
    const long curSec =
        static_cast<long>(GlobalData::s_systemTime_.getCurSec());
    return CheckDailyScheduleTime(serverSchedule, createTime, curSec);
}

// ORIG W 0x08695f68。
bool LimitItemUsage::isUsable()
{
    const unsigned char state = m_field10;
    if (state == 2)
    {
        if (m_field19 >= *reinterpret_cast<const int*>(m_pad11))
            return false;
        return true;
    }
    if (state == 0 || state == 1)
    {
        if (isAnotherDay())
            m_field19 = 0;
        if (m_field19 >= *reinterpret_cast<const int*>(m_pad11))
            return false;
    }
    return true;
}

// ORIG W 0x08695f28。
bool LimitItemUsage::useIt()
{
    if (!isUsable())
        return false;
    m_field19 += 1;
    reinterpret_cast<CommonTime*>(this)->SetCurTime();
    return true;
}

// ============================================================================
// ---- UserMercenaryInfoMgr::UpdateCharacName（ORIG T 0x081b462e）----
// ============================================================================

void UserMercenaryInfoMgr::UpdateCharacName(unsigned int characNo,
                                            std::string const& name)
{
    const unsigned int idx = FindIndexByCharacNo(characNo);
    if (!CheckCharacIndex(static_cast<unsigned char>(idx)))
        return;

    CHARAC_LOAD_MERCENARY& entry = m_characs[idx];
    std::memset(entry.m_name, 0, 0x1e);
    std::strncpy(entry.m_name, name.c_str(), 0x1d);
}

// ============================================================================
// ---- CUserEventUtil::updateCharacterDayEvent（ORIG T 0x08688230）----
// ============================================================================

class CUserEventUtil
{
public:
    static void updateCharacterDayEvent(CUser*, int);
};

void CUserEventUtil::updateCharacterDayEvent(CUser* user, int day)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("user.cpp", 0x9af5);
    CStreamGuard guard(stream, true);
    guard << 0x16e;
    guard << static_cast<int>(user->GetUID());

    SIG_UPDATE_CHARACTER_DAY_EVENT* sig =
        guard.GetInBuffer<SIG_UPDATE_CHARACTER_DAY_EVENT>();
    unsigned char* raw = reinterpret_cast<unsigned char*>(sig);
    *reinterpret_cast<unsigned int*>(raw + 0x00) = user->get_acc_id();
    *reinterpret_cast<short*>(raw + 0x04) = static_cast<short>(day);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// ---- CDungeon::VerifyMaze 两重载（报告 VerifyMaze*.md 转写）----
// ============================================================================

// ORIG T 0x0834cb7c。
bool CDungeon::VerifyMaze(const MazeScript& maze) const
{
    if (maze.m_width == 1 && maze.m_height == 1)
        return true;

    if (maze.m_vec14.empty())
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x83e,
            "dungeon index(%d) maze(%d) has error. (Start positions were not declared!)",
            m_index, maze.m_64);
        return false;
    }
    for (unsigned int i = 0; i < maze.m_vec14.size(); ++i)
    {
        const GridScript& grid =
            maze.getGridR(maze.m_vec14[i].first, maze.m_vec14[i].second);
        if (m_maplist[grid.m_gridValue].size() == 0)
        {
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x847,
                "dungeon index(%d) maze(%d) has error. (Can't found start map(grid: %d,%d)!)",
                m_index, maze.m_64, maze.m_vec14[i].first,
                maze.m_vec14[i].second);
        }
    }

    if (maze.m_vec20.empty())
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x84e,
            "dungeon index(%d) maze(%d) has error. (End positions were not declared!)",
            m_index, maze.m_64);
        return false;
    }
    for (unsigned int i = 0; i < maze.m_vec20.size(); ++i)
    {
        const GridScript& grid =
            maze.getGridR(maze.m_vec20[i].first, maze.m_vec20[i].second);
        if (m_dummyMap324[grid.m_gridValue].size() == 0)
        {
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x857,
                "dungeon index(%d) maze(%d) has error. (Can't found boss map(grid: %d,%d)!",
                m_index, maze.m_64, maze.m_vec20[i].first,
                maze.m_vec20[i].second);
        }
    }

    for (int y = 0; y < maze.m_width; ++y)
    {
        for (int x = 0; x < maze.m_height; ++x)
        {
            const GridScript& grid = maze.getGridR(y, x);
            if (grid.m_gridValue != 0)
            {
                const std::pair<int, int> pos(y, x);
                const bool isStart =
                    std::find(maze.m_vec14.begin(), maze.m_vec14.end(),
                              pos) != maze.m_vec14.end();
                const bool isEnd =
                    !isStart &&
                    std::find(maze.m_vec20.begin(), maze.m_vec20.end(),
                              pos) != maze.m_vec20.end();
                if (!isStart && !isEnd)
                {
                    if (m_map1a4[grid.m_gridValue].size() == 0)
                    {
                        LogManager::logFormat(
                            1, "data_manager.cpp",
                            __PRETTY_FUNCTION__,
                            0x86b,
                            "dungeon index(%d) maze(%d) has error. (Can't found grid map(grid: %d,%d)!)",
                            m_index, maze.m_64, x, y);
                    }
                }
            }
        }
    }
    return true;
}

// ORIG T 0x0834c9f0。
bool CDungeon::VerifyMaze() const
{
    if (!(m_index <= 0x7cf))
        return true;

    for (unsigned int i = 0; i < m_mazeList.size(); ++i)
    {
        if (!VerifyMaze(m_mazeList[i]))
            return false;
    }
    for (int g = 0; g < 2; ++g)
    {
        for (unsigned int i = 0; i < m_mazeList0[g].size(); ++i)
        {
            MazeScript copy(m_mazeList0[g][i]);
            if (!VerifyMaze(copy))
                return false;
        }
    }
    return true;
}

// ============================================================================
// ---- CStackableItem::GetMonsterCardId（ORIG W 0x0849f530）----
// ============================================================================

int CStackableItem::GetMonsterCardId()
{
    return *reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(this) + 0x2b8);
}
