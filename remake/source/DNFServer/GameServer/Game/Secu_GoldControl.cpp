// df_game_r Game/ Secu_GoldControl 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/Secu_GoldControl.md
#include "CUser.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include <string.h>

class DB_CheckCharacName
{
public:
    static void makeRequest(int uid, unsigned int accId, const char* charName, bool flag);
};
// ORIG 金币统计区组件视图（P1：成员视图折叠 lea/add 与 ORIG 同形）
struct SecuGoldView
{
    char m_pad0[0x04];          // m_pUser @ +0x00
    user_gold_info m_total;     // +0x04
    user_gold_info m_cur;       // +0x40
    user_gold_info m_backup;    // +0x7c
};
// 协议视图（W8LT_DBTask.cpp 同型）：+0x00=accId、+0x04=char_name 串、
// +0x22=flag 字节（§4.3 具名字段直写，ORIG `mov %dl,0x22(%eax)` 直接寻址）
#pragma pack(push, 1)
struct DB_CheckCharacNameReqView
{
    unsigned int m_accId;       // +0x00
    char m_charName[0x1e];      // +0x04
    unsigned char m_flag;       // +0x22
    unsigned char m_pad23;      // +0x23
};
#pragma pack(pop)

void DB_CheckCharacName::makeRequest(int uid, unsigned int accId, const char* charName, bool flag)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4f9),
                       true);
    guard.operator*()->operator<<((int)0x2a2);   // op*(guard)→op<<(int)
    guard.operator*()->operator<<(uid);          // op*(guard)→op<<(int)
    SIG_CHECK_CHARAC_NAME* pkt =
        guard.operator->()->GetInBuffer<SIG_CHECK_CHARAC_NAME>();
    reinterpret_cast<DB_CheckCharacNameReqView*>(pkt)->m_accId = accId;
    if (charName != NULL && charName[0] != '\0')
    {
        strncpy(reinterpret_cast<DB_CheckCharacNameReqView*>(pkt)->m_charName,
                charName, 0x1e);
    }
    reinterpret_cast<DB_CheckCharacNameReqView*>(pkt)->m_flag =
        (unsigned char)flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
void Secu_GoldControl::Init(CUser* user)
{
    m_pUser = user;
    m_dateFlag = GlobalData::g_GoldControlDateFlag;
    m_fieldb8 = 0;
    m_fieldbc = 0;
    m_mailGold = 0;
    m_fielde6 = 0;
    memset(m_mailCharName, 0, 0x1e);
}

void Secu_GoldControl::SetInfo(user_gold_info& info)
{
    info.toCopy(m_total);
}

void Secu_GoldControl::SetMailCharName(const char* name)
{
    if (name)
    {
        strncpy(m_mailCharName, name, 0x1e);
    }
}

void Secu_GoldControl::resetRangeMoney(user_gold_info& cur, user_gold_info& copy)
{
    // ORIG 0x08287bdc 逐条：memcpy(copy, cur, 0x3c)；6 组 64 位
    // if (cur > 0x00000000fffffffe) { copy = -1; cur -= 0xffffffff; }
    // else { cur = 0; }（ORIG add $1/adc $-1 = 64 位减 0xffffffff，手动展开）
    memcpy(&copy, &cur, 0x3c);
    if (cur.m_gold > 0x00000000fffffffeULL)
    {
        copy.m_gold = 0x00000000ffffffffULL;
        cur.m_gold -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_gold = 0;
    }
    if (cur.m_import > 0x00000000fffffffeULL)
    {
        copy.m_import = 0x00000000ffffffffULL;
        cur.m_import -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_import = 0;
    }
    if (cur.m_sub > 0x00000000fffffffeULL)
    {
        copy.m_sub = 0x00000000ffffffffULL;
        cur.m_sub -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_sub = 0;
    }
    if (cur.m_importSub > 0x00000000fffffffeULL)
    {
        copy.m_importSub = 0x00000000ffffffffULL;
        cur.m_importSub -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_importSub = 0;
    }
    if (cur.m_auctionAdd > 0x00000000fffffffeULL)
    {
        copy.m_auctionAdd = 0x00000000ffffffffULL;
        cur.m_auctionAdd -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_auctionAdd = 0;
    }
    if (cur.m_auctionSub > 0x00000000fffffffeULL)
    {
        copy.m_auctionSub = 0x00000000ffffffffULL;
        cur.m_auctionSub -= 0x00000000ffffffffULL;
    }
    else
    {
        cur.m_auctionSub = 0;
    }
}
void Secu_GoldControl::AddGold(unsigned int value, eMoneyAddReason reason)
{
    CheckDate();
    switch (reason)
    {
    case 0:
    case 3:
    case 4:
    case 5:
    case 10:
    case 15:
    case 0x1a:
        AddImportMoney(value);
        break;
    case 1:
    case 2:
        return;
    case 0xe:
        MailGold(value, false);
        {
            unsigned int accId = m_pUser->get_acc_id();
            int uid = m_pUser->GetUID();
            DB_CheckCharacName::makeRequest(uid, accId, m_mailCharName, false);
        }
        return;
    case 0x12:
        AddAuctionMoney(value);
        break;
    default:
        break;
    }
    AddMoney(value);
    CheckMoneyRange();
}
void Secu_GoldControl::SubGold(unsigned int value, eMoneySubReason reason)
{
    CheckDate();
    if (reason == 0xe)
    {
        SubImportMoney(value);
    }
    else if (reason < 0xf)
    {
        if (reason == 2)
        {
            return;
        }
    }
    else
    {
        if (reason == 0x11)
        {
            MailGold(value, true);
            unsigned int accId = m_pUser->get_acc_id();
            int uid = m_pUser->GetUID();
            DB_CheckCharacName::makeRequest(uid, accId, m_mailCharName, true);
            return;
        }
        if (reason == 0x1a)
        {
            SubAuctionMoney(value);
        }
    }
    SubMoney(value);
    CheckMoneyRange();
}

void Secu_GoldControl::AddMoney(unsigned int value)
{
    // ORIG：m_cur(+0x40).m_gold += value；m_cur.m_count[2]++；
    // m_total(+0x04).m_gold += value；m_total.m_count[2]++；hades GoldPlus。
    m_cur.m_gold += value;
    m_cur.m_count[2]++;
    m_total.m_gold += value;
    m_total.m_count[2]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldPlus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)0);
}

void Secu_GoldControl::SubMoney(unsigned int value)
{
    m_cur.m_sub += value;
    m_cur.m_count[4]++;
    m_total.m_sub += value;
    m_total.m_count[4]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldMinus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)0);
}

void Secu_GoldControl::AddImportMoney(unsigned int value)
{
    m_cur.m_import += value;
    m_cur.m_count[3]++;
    m_total.m_import += value;
    m_total.m_count[3]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldPlus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)1);
}

void Secu_GoldControl::SubImportMoney(unsigned int value)
{
    m_cur.m_importSub += value;
    m_cur.m_count[5]++;
    m_total.m_importSub += value;
    m_total.m_count[5]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldMinus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)1);
}
void Secu_GoldControl::AddAuctionMoney(unsigned int value)
{
    if (value > 4999)
    {
        m_cur.m_auctionAdd += value;
        m_cur.m_count[0]++;
        m_total.m_auctionAdd += value;
        m_total.m_count[0]++;

        // ORIG 0x828793f/0x828794a：getHades() 三次 fresh call（P0 不缓存）。
        ((XNuclear::CHades*)m_pUser->getHades())->
            GoldPlus(value, (XNuclear::ENUM_GOLD_CONTROL_REASON)2);
        // GoldTracking
        char ipBuf[16];
        ((XNuclear::CHades*)m_pUser->getHades())->GetIP(ipBuf);

        // [b24ab] P197②：ORIG fatigue(→%esi)/accId(→%ebx) callee-saved 缓存跨 getHades。
        // fatigue 用 unsigned int 承接 unsigned short 返回值：ORIG 为单条
        // movzwl %ax,%esi（零扩展直存），声明成 unsigned short 会拆成 mov+movzwl 两步。
        register unsigned int fatigue = m_pUser->getCurCharacUsedFatigue();
        register unsigned int accId = m_pUser->get_acc_id();

        // ORIG 0x828793f..0x828794a：GoldTracking(type=4, money=accId, ip=ipBuf,
        // param4=value, fatigue=fatigue)
        ((XNuclear::CHades*)m_pUser->getHades())->
            GoldTracking(4, accId, ipBuf, value, fatigue);
    }
}

// ORIG 0x0828795a：value>4999 时 m_cur/m_total.m_auctionSub += value、
// m_total.m_count[1]++，再 GoldMinus(value, 2)（无 GoldTracking）。
void Secu_GoldControl::SubAuctionMoney(unsigned int value)
{
    if (value > 4999)
    {
        m_cur.m_auctionSub += value;
        m_total.m_auctionSub += value;
        m_total.m_count[1]++;

        // ORIG 无中间槽：getHades() 返回值直接作 this（P0 去缓存，具名局部
        // 会诱发 -0xc 槽往返 + 栈帧 0x24、GoldMinus 参数走 eax 而非 edx）。
        ((XNuclear::CHades*)m_pUser->getHades())->
            GoldMinus(value, (XNuclear::ENUM_GOLD_CONTROL_REASON)2);
    }
}

void Secu_GoldControl::MailComplete(bool flag)
{
    // ORIG 尾复制：m_mailGold=0 在两分支内各出现一次（jmp/nop 共享出口仅指 ret），
    // 共享尾写法会丢 if 分支的清零序，需按分支重复。
    if (flag)
    {
        SubImportMoney(m_mailGold);
        SubMoney(m_mailGold);
        m_mailGold = 0;
    }
    else
    {
        AddMoney(m_mailGold);
        m_mailGold = 0;
    }
}

void Secu_GoldControl::TradeComplete(int subValue, int addValue)
{
    if (subValue > 0)
    {
        SubImportMoney(subValue);
        SubMoney(subValue);
    }
    if (addValue > 0)
    {
        AddMoney(addValue);
    }
}

void Secu_GoldControl::MailGold(unsigned int value, bool flag)
{
    m_mailGold = (int)value;
}

void Secu_GoldControl::MailSameMid()
{
    m_mailGold = 0;
}



void Secu_GoldControl::SavetoDB(bool a, bool b, bool c)
{
    if (a && (m_cur.m_count[2] != 0 || m_cur.m_count[4] != 0 || m_fieldb8 != 0))
    {
        char ipBuf[16] = {0};
        ((XNuclear::CHades*)m_pUser->getHades())->GetIP(ipBuf);

        char macBuf[65] = {0};
        // get mac address from CNetwork at this+0xe0
        strncpy(macBuf, m_pUser->m_network.get_mac_addr(), 0x41);

        if (c)
        {
            user_gold_info tmp;
            resetRangeMoney(m_cur, tmp);
        }
        else
        {
            m_cur.reset();
        }
        m_fieldb8 = 0;
    }

    if (b)
    {
        if (m_backup.m_count[2] != 0 || m_backup.m_count[4] != 0 || m_fieldbc != 0)
        {
            char ipBuf[16] = {0};
            ((XNuclear::CHades*)m_pUser->getHades())->GetIP(ipBuf);

            char macBuf[65] = {0};
            strncpy(macBuf, m_pUser->m_network.get_mac_addr(), 0x41);

            m_backup.reset();
            m_fieldbc = 0;
        }
    }
}

void Secu_GoldControl::CheckMoneyRange()
{
    // ORIG：(high!=0 || low==-1) 逐项比较，勿折叠成 64 位单比较。
    unsigned int* gold = (unsigned int*)&m_cur.m_gold;
    unsigned int* sub = (unsigned int*)&m_cur.m_sub;
    if ((gold[1] != 0 || gold[0] == 0xffffffffu) ||
        (sub[1] != 0 || sub[0] == 0xffffffffu))
    {
        SavetoDB(true, false, true);
    }
}
void* Secu_GoldControl::GetGoldInfo()
{
    return &m_total;
}

void Secu_GoldControl::CheckDate()
{
    if (m_dateFlag != GlobalData::g_GoldControlDateFlag)
    {
        m_dateFlag = GlobalData::g_GoldControlDateFlag;
        // Copy current gold info (+0x40) to backup (+0x7c)
        m_cur.toCopy(m_backup);
        // Reset total gold info (+0x04)
        m_total.reset();
        // Reset current gold info (+0x40)
        m_cur.reset();
        // Copy m_fieldb8 to m_fieldbc, reset m_fieldb8
        m_fieldbc = m_fieldb8;
        m_fieldb8 = 0;
    }
}


void Secu_GoldControl::UseFatigue(int count)
{
    CheckDate();
    m_fieldb8 += count;
}
