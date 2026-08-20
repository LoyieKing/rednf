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

void DB_CheckCharacName::makeRequest(int uid, unsigned int accId, const char* charName, bool flag)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4f9);
    CStreamGuard guard(stream, true);
    guard << (int)0x2a2;
    guard << uid;
    SIG_CHECK_CHARAC_NAME* pkt = guard.GetInBuffer<SIG_CHECK_CHARAC_NAME>();
    *(unsigned int*)pkt = accId;
    if (charName != NULL && charName[0] != '\0')
    {
        strncpy((char*)pkt + 4, charName, 0x1e);
    }
    *((char*)pkt + 0x22) = (char)flag;
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
    user_gold_info* total = (user_gold_info*)&m_totalGoldLow;
    memcpy(total, &info, sizeof(user_gold_info));
}

void Secu_GoldControl::SetMailCharName(const char* name)
{
    if (name)
    {
        strncpy(m_mailCharName, name, 0x1e);
    }
}

void Secu_GoldControl::resetRangeMoney(user_gold_info& dst, const user_gold_info& src)
{
    // Copy 0x3c (60) bytes from src to dst
    memcpy(&dst, &src, 0x3c);

    int* srcInts = (int*)&src;
    int* dstInts = (int*)&dst;

    // Process 6 pairs of 64-bit values
    for (int i = 0; i < 6; i++)
    {
        int lowOffset = i * 8;
        int highOffset = i * 8 + 4;
        int high = srcInts[highOffset / 4];
        unsigned int low = (unsigned int)srcInts[lowOffset / 4];

        if (high == 0 && low <= 0xfffffffe)
        {
            // dst = src - 1
            unsigned long long val = ((unsigned long long)high << 32) | low;
            val -= 1;
            dstInts[lowOffset / 4] = (int)(val & 0xffffffff);
            dstInts[highOffset / 4] = (int)(val >> 32);
        }
        else
        {
            // dst = 0, src = src + 1
            dstInts[lowOffset / 4] = 0;
            dstInts[highOffset / 4] = 0;
            unsigned long long val = ((unsigned long long)high << 32) | low;
            val += 1;
            srcInts[lowOffset / 4] = (int)(val & 0xffffffff);
            srcInts[highOffset / 4] = (int)(val >> 32);
        }
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
    // 64-bit gold at +0x40
    unsigned long long gold = ((unsigned long long)m_goldHigh << 32) | m_goldLow;
    gold += value;
    m_goldLow = (int)(gold & 0xffffffff);
    m_goldHigh = (int)(gold >> 32);
    m_counter74++;

    // 64-bit total gold at +0x04
    unsigned long long total = ((unsigned long long)m_totalGoldHigh << 32) | m_totalGoldLow;
    total += value;
    m_totalGoldLow = (int)(total & 0xffffffff);
    m_totalGoldHigh = (int)(total >> 32);
    m_counter38++;

    void* hades = m_pUser->getHades();
    typedef void (*GoldPlusFn)(void*, unsigned int, int);
    GoldPlusFn fn = (GoldPlusFn)0x084b93fa;
    fn(hades, value, 0);
}

void Secu_GoldControl::SubMoney(unsigned int value)
{
    // 64-bit sub gold at +0x50
    unsigned long long subGold = ((unsigned long long)m_subGoldHigh << 32) | m_subGoldLow;
    subGold += value;
    m_subGoldLow = (int)(subGold & 0xffffffff);
    m_subGoldHigh = (int)(subGold >> 32);
    m_counter78++;

    // 64-bit total sub gold at +0x14
    unsigned long long totalSub = ((unsigned long long)m_totalSubGoldHigh << 32) | m_totalSubGoldLow;
    totalSub += value;
    m_totalSubGoldLow = (int)(totalSub & 0xffffffff);
    m_totalSubGoldHigh = (int)(totalSub >> 32);
    m_counter3c++;

    void* hades = m_pUser->getHades();
    typedef void (*GoldMinusFn)(void*, unsigned int, int);
    GoldMinusFn fn = (GoldMinusFn)0x084b944a;
    fn(hades, value, 0);
}

void Secu_GoldControl::AddImportMoney(unsigned int value)
{
    unsigned long long importGold = ((unsigned long long)m_importGoldHigh << 32) | m_importGoldLow;
    importGold += value;
    m_importGoldLow = (int)(importGold & 0xffffffff);
    m_importGoldHigh = (int)(importGold >> 32);
    m_counter76++;

    unsigned long long totalImport = ((unsigned long long)m_totalImportGoldHigh << 32) | m_totalImportGoldLow;
    totalImport += value;
    m_totalImportGoldLow = (int)(totalImport & 0xffffffff);
    m_totalImportGoldHigh = (int)(totalImport >> 32);
    m_counter3a++;

    void* hades = m_pUser->getHades();
    typedef void (*GoldPlusFn)(void*, unsigned int, int);
    GoldPlusFn fn = (GoldPlusFn)0x084b93fa;
    fn(hades, value, 1);
}

void Secu_GoldControl::SubImportMoney(unsigned int value)
{
    unsigned long long importSub = ((unsigned long long)m_importSubHigh << 32) | m_importSubLow;
    importSub += value;
    m_importSubLow = (int)(importSub & 0xffffffff);
    m_importSubHigh = (int)(importSub >> 32);
    m_counter7a++;

    unsigned long long totalImportSub = ((unsigned long long)m_totalImportSubHigh << 32) | m_totalImportSubLow;
    totalImportSub += value;
    m_totalImportSubLow = (int)(totalImportSub & 0xffffffff);
    m_totalImportSubHigh = (int)(totalImportSub >> 32);
    m_counter3e++;

    void* hades = m_pUser->getHades();
    typedef void (*GoldMinusFn)(void*, unsigned int, int);
    GoldMinusFn fn = (GoldMinusFn)0x084b944a;
    fn(hades, value, 1);
}
void Secu_GoldControl::AddAuctionMoney(unsigned int value)
{
    if (value > 4999)
    {
        unsigned long long auctionAdd = ((unsigned long long)m_auctionAddHigh << 32) | m_auctionAddLow;
        auctionAdd += value;
        m_auctionAddLow = (int)(auctionAdd & 0xffffffff);
        m_auctionAddHigh = (int)(auctionAdd >> 32);
        m_counter70++;

        unsigned long long totalAuctionAdd = ((unsigned long long)m_totalAuctionAddHigh << 32) | m_totalAuctionAddLow;
        totalAuctionAdd += value;
        m_totalAuctionAddLow = (int)(totalAuctionAdd & 0xffffffff);
        m_totalAuctionAddHigh = (int)(totalAuctionAdd >> 32);
        m_counter34++;

        void* hades = m_pUser->getHades();
        typedef void (*GoldPlusFn)(void*, unsigned int, int);
        GoldPlusFn fn = (GoldPlusFn)0x084b93fa;
        fn(hades, value, 2);
        // GoldTracking
        char ipBuf[16];
        typedef void (*GetIPFn)(void*, char*);
        GetIPFn getIP = (GetIPFn)0x084b9774;
        getIP(hades, ipBuf);

        unsigned short fatigue = m_pUser->getCurCharacUsedFatigue();
        unsigned int accId = m_pUser->get_acc_id();

        typedef void (*GoldTrackingFn)(void*, int, const char*, unsigned int, unsigned int, unsigned short);
        GoldTrackingFn track = (GoldTrackingFn)0x084b949a;
        track(hades, 4, ipBuf, accId, value, fatigue);
    }
}

void Secu_GoldControl::MailComplete(bool flag)
{
    if (flag)
    {
        SubImportMoney(m_mailGold);
        SubMoney(m_mailGold);
    }
    else
    {
        AddMoney(m_mailGold);
    }
    m_mailGold = 0;
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
void Secu_GoldControl::SubAuctionMoney(unsigned int value)
{
    if (value > 4999)
    {
        unsigned long long auctionSub = ((unsigned long long)m_auctionSubHigh << 32) | m_auctionSubLow;
        auctionSub += value;
        m_auctionSubLow = (int)(auctionSub & 0xffffffff);
        m_auctionSubHigh = (int)(auctionSub >> 32);

        unsigned long long totalAuctionSub = ((unsigned long long)m_totalAuctionSubHigh << 32) | m_totalAuctionSubLow;
        totalAuctionSub += value;
        m_totalAuctionSubLow = (int)(totalAuctionSub & 0xffffffff);
        m_totalAuctionSubHigh = (int)(totalAuctionSub >> 32);
        m_counter36++;

        void* hades = m_pUser->getHades();
        typedef void (*GoldMinusFn)(void*, unsigned int, int);
        GoldMinusFn fn = (GoldMinusFn)0x084b944a;
        fn(hades, value, 2);
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
    if (a && (m_counter74 != 0 || m_counter78 != 0 || m_fieldb8 != 0))
    {
        char ipBuf[16];
        void* hades = m_pUser->getHades();
        typedef void (*GetIPFn)(void*, char*);
        GetIPFn getIP = (GetIPFn)0x084b9774;
        getIP(hades, ipBuf);

        char macBuf[65];
        memset(macBuf, 0, sizeof(macBuf));
        // get mac address from CNetwork at this+0xe0
        char* mac = m_pUser->m_network.get_mac_addr();
        strncpy(macBuf, mac, 0x41);

        if (c)
        {
            user_gold_info tmp;
            memset(&tmp, 0, sizeof(tmp));
            resetRangeMoney(*(user_gold_info*)&m_goldLow, tmp);
        }
        else
        {
            user_gold_info* gold = (user_gold_info*)&m_goldLow;
            memset(gold, 0, sizeof(user_gold_info));
        }
        m_fieldb8 = 0;
    }

    if (b)
    {
        if (m_counterb0 != 0 || m_counterb4 != 0 || m_fieldbc != 0)
        {
            char ipBuf[16];
            void* hades = m_pUser->getHades();
            typedef void (*GetIPFn)(void*, char*);
            GetIPFn getIP = (GetIPFn)0x084b9774;
            getIP(hades, ipBuf);

            char macBuf[65];
            memset(macBuf, 0, sizeof(macBuf));

            user_gold_info* backup = (user_gold_info*)&m_backupGoldLow;
            memset(backup, 0, sizeof(user_gold_info));
            m_fieldbc = 0;
        }
    }
}

void Secu_GoldControl::CheckMoneyRange()
{
    if ((m_goldHigh != 0 || m_goldLow == -1) || (m_subGoldHigh != 0 || m_subGoldLow == -1))
    {
        SavetoDB(true, false, true);
    }
}
void* Secu_GoldControl::GetGoldInfo()
{
    return &m_totalGoldLow;
}
void Secu_GoldControl::CheckDate()
{
    if (m_dateFlag != GlobalData::g_GoldControlDateFlag)
    {
        m_dateFlag = GlobalData::g_GoldControlDateFlag;
        // Copy current gold info (+0x40) to backup (+0x7c)
        user_gold_info* current = (user_gold_info*)&m_goldLow;
        user_gold_info* backup = (user_gold_info*)&m_backupGoldLow;
        memcpy(backup, current, sizeof(user_gold_info));
        // Reset total gold info (+0x04)
        user_gold_info* total = (user_gold_info*)&m_totalGoldLow;
        memset(total, 0, sizeof(user_gold_info));
        // Reset current gold info (+0x40)
        memset(current, 0, sizeof(user_gold_info));
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
