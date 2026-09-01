// ============================================================================
// W6_SecMiscDeps.cpp —— Wave6 杂项安全 + 小类 ORIG-only 符号收口 TU。
//
// 覆盖符号清单（ORIG 地址注释随每个定义给出）：
//   WongWork::CSecurityCardCenter  encryptString / generateCertKey /
//     _updateTimeCount / generateServerKey / checkPhoneNumber(const) /
//     generateCardData(const) / init（自 GlobalData.cpp 迁入）
//   WongWork::CSecurityCard::checkAnswer(hh) const
//   WongWork::CGMAccounts::getGMInfo(unsigned int) const
//   WongWork::CMCAPManager::getExposedCount() const
//   WongWork::CUserPremium::{getCeraShopDiscountRate() const,
//     GetPremiumInfoList(int) const(sret 版)}
//   WongWork::CEventMgr::{GetCoinPerDay, GetMoneyCharacCreate,
//     IsPCRoomEventing, IsChildrensDayEventing}
//   WongWork::CWebEvent::{m_strEventName, m_strEventIPGCode, ReqSaveEventItem,
//     RecvInfoFromWeb, _GetATIAvatarAbility, _GetEventString, _GetEventIPGCode,
//     RecvInfoFromDB}
// ============================================================================
#include "GlobalData.h"
#include "CInventory.h"
#include "CUser.h"
#include "CEventMgr.h"
#include "CGenUniqueNo.h"
#include "DNFFunctionLib.h"         // DNFFLib::Binary2Hex
#include "CGuildServerProxy.h"      // CGMAccounts / stGMInfo_t
#include "CMCAPManager.h"
#include "CCeraShop.h"
#include "CItemList.h"              // get_rand_int
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "CSystemTime.h"
#include "CRijndael.h"
#include "CEnvironment.h"           // IMethod::GetBlockSize / Pad、G_CEnvironment
#include "Inven_Item.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <list>

// ORIG 0x0858c81c _ZN6OS_API8snprintfEPcjPKcz（printf 家族封装）。
namespace OS_API
{
    int snprintf(char* buf, unsigned int size, char const* fmt, ...);
}

using namespace GlobalData;

// ----------------------------------------------------------------------------
// IPG helper 低层分发表桥。
// ORIG RecvInfoFromDB 0x086c1383..13f2：fn = **(void***)&s_pIPGHelper（对象首槽
// 零偏移），首参传 &GlobalData::s_pIPGHelper（handle-of-handle）。与
// GlobalData.cpp 既有的 +0x10/+0x14/+0x18 槽位取法同族；真实函数名归属
// WongWork__IPG 批次，此处仅按汇编指令序转写调用形态。
// ----------------------------------------------------------------------------
typedef void (*IPGBridgeFn)(void* handle, class CUser* user, char const* codeStr,
                            char const* countStr, char const* uniqueNoStr,
                            char const* eventName, char const* characName,
                            void* stream);

// ============================ WongWork::CSecurityCardCenter =================

WongWork::CSecurityCardCenter::CSecurityCardCenter()
{
    // ORIG 0x082a6654 W：构造 CRijndael@0x0（0x47c 子块）与 list<uchar>@0x47c；
    // 时间计数不在此写入（init() 置 1）。基类与 m_characterSet 为具名成员，
    // 由 C++ 默认构造完成对应子对象初始化。
}

void WongWork::CSecurityCardCenter::init()
{
    // ORIG T 0x085fe05e：固定密钥初始化头部 CRijndael；42 字符 characterSet
    // 逐个 push 进 m_characterSet；+0x484 置 1。（字符集数据段 0x93ac1e0）
    static const unsigned char characterSet[] =
        "+<>?@#$&ABCDEFGHIJKLMNPQRSTUVWXYZ123456789";
    Initialize("n6;fw;vjfs0fuguf", 0x10, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    for (int i = 0; i < 42; ++i)
    {
        m_characterSet.push_back(characterSet[i]);
    }
    m_timeCount = 1;
}

char* WongWork::CSecurityCardCenter::encryptString(char const* src, char* out)
{
    // ORIG T 0x085fe2e2 (size 0x161)。两块 256 字节栈缓冲清零 → strncpy 0x100
    // → strlen → IMethod::GetBlockSize 取块大小并向上取整 → Pad → CRijndael::
    // Encrypt（asm 为非虚派发直呼）→ Binary2Hex 转十六进制并以 len*2+1 下标补
    // '\0'。异常路径返回 NULL（ORIG catch 吞掉后 ebx=0）。
    char plain[0x100];
    unsigned char cipher[0x100];
    memset(plain, 0, 0x40);      // ORIG：rep stos 仅清 0x40 字节（两块）
    memset(cipher, 0, 0x40);
    strncpy(plain, src, 0x100);
    size_t length = strlen(plain);
    char* result = out;
    try
    {
        const int blockSize = IMethod::GetBlockSize((unsigned int*)0);
        int paddedLength;
        if (blockSize < 0)
        {
            paddedLength = (int)length;
        }
        else if ((int)length % blockSize == 0)
        {
            paddedLength = (int)length;
        }
        else
        {
            paddedLength = ((int)length / blockSize + 1) * blockSize;
        }
        if (blockSize >= 0)
        {
            IMethod::Pad(plain, (int)length, (unsigned int*)0);
        }
        CRijndael::Encrypt(plain, reinterpret_cast<char*>(cipher),
                           (size_t)paddedLength);
        const int hexLength = paddedLength * 2 + 1;
        DNFFLib::Binary2Hex(cipher, paddedLength, out);
        out[hexLength] = '\0';
        result = out;
    }
    catch (...)
    {
        result = 0;   // ORIG __cxa_begin_catch/__cxa_end_catch → 返回 0
    }
    return result;
}

char* WongWork::CSecurityCardCenter::generateCertKey(char* out)
{
    // ORIG T 0x085fe17e (size 0x3b)：rand∈[10000,99999] 格式化为 5 位数字串。
    const int value = get_rand_int(89999) + 0x2710;
    OS_API::snprintf(out, 6, "%d", value);
    return out;
}

void WongWork::CSecurityCardCenter::_updateTimeCount()
{
    // ORIG T 0x085fe026 (size 0x38)：半字计数自增，越过 0xc34f 回绕为 1。
    ++m_timeCount;
    if (m_timeCount > 0xc34f)
    {
        m_timeCount = 1;
    }
}

char* WongWork::CSecurityCardCenter::generateServerKey(char* out)
{
    // ORIG T 0x085fe0ec (size 0x91)：
    // "%02d%02d%010d%05d%05d"(env+0x378, env+0x1b0, curSec, timeCount, rand5)
    _updateTimeCount();
    const int curSec = s_systemTime_.getCurSec();
    const int random5 = get_rand_int(99999);
    const unsigned short timeCount = m_timeCount;
    // CEnvironment 服务器标识字段尚未具名化（域外批次负责），按既有口径偏移读。
    const int serverIdB =
                G_CEnvironment()->m_serverEnvir.m_serverGroup;   // +0x378（%02d 第 1 槽）
    const int serverIdA =
                G_CEnvironment()->m_serverEnvir.m_channelNo;     // +0x1b0（%02d 第 2 槽）
    OS_API::snprintf(out, 0x21, "%02d%02d%010d%05d%05d",
                     serverIdB, serverIdA, curSec, timeCount, random5);
    return out;
}

bool WongWork::CSecurityCardCenter::checkPhoneNumber(char const* phone) const
{
    // ORIG T 0x085fe444 (size 0x52)：至多扫 12 位（下标 <=0xb），遇 '\0' 提前
    // 合格返回；出现非 '0'..'9' 字符即 false；扫满 12 位亦 true。
    for (unsigned int i = 0; ; ++i)
    {
        if (i > 0xb)
        {
            break;
        }
        if (phone[i] == '\0')
        {
            break;
        }
        if ((int)(signed char)phone[i] - 0x30 > 9 || phone[i] < '0')
        {
            return false;
        }
    }
    return true;
}

char* WongWork::CSecurityCardCenter::generateCardData(char* out) const
{
    // ORIG T 0x085fe1ba (size 0x128)：拷贝 m_characterSet 至临时 list；循环
    // （pos<=0x25 继续）：以 list.size() 为上界随机取槽，顺序迭代到第 pick 个
    // 元素写出并 erase（每字符只使用一次）。
    std::list<unsigned char> pool(m_characterSet);
    unsigned int pos = 0;
    do
    {
        if (pos > 0x25)
        {
            break;
        }
        const int pick = get_rand_int((int)pool.size());
        int seen = 0;
        for (std::list<unsigned char>::iterator it = pool.begin();
             it != pool.end(); /* 分支内手动推进 */)
        {
            if (seen == pick)
            {
                out[pos] = static_cast<char>(*it);
                pool.erase(it);
                break;
            }
            ++seen;
            ++it;
        }
        ++pos;
    } while (true);
    return out;
}

// ============================ WongWork::CSecurityCard =======================

bool WongWork::CSecurityCard::checkAnswer(unsigned char ansA, unsigned char ansB) const
{
    // ORIG W 0x085fdf6a (size 0xbb)。问题四元组 @0x3c..0x3f；答案表基址 this+8，
    // 第一问索引 = question[1]*10 + question[0]，第二问 = question[3]*10 +
    // question[2]；表值与答案均经 movsbl/zbl 后整数值比较。
    if (m_question3c[0] == 0 && m_question3c[1] == 0 &&
        m_question3c[2] == 0 && m_question3c[3] == 0)
    {
        return false;
    }
    const unsigned char* table =
        reinterpret_cast<const unsigned char*>(this) + 8;
    const signed char rowA = static_cast<signed char>(m_question3c[1]);
    const signed char colA = static_cast<signed char>(m_question3c[0]);
    if (static_cast<signed char>(table[rowA * 10 + colA]) !=
        static_cast<int>(ansA))
    {
        return false;
    }
    const signed char rowB = static_cast<signed char>(m_question3c[3]);
    const signed char colB = static_cast<signed char>(m_question3c[2]);
    if (static_cast<signed char>(table[rowB * 10 + colB]) !=
        static_cast<int>(ansB))
    {
        return false;
    }
    return true;
}

// ============================ WongWork::CGMAccounts =========================

WongWork::CGMAccounts::stGMInfo_t WongWork::CGMAccounts::getGMInfo(unsigned int mid) const
{
    // ORIG T 0x0810940a (size 0xdd)：std::find(m_gmList.begin(), end(), key)
    // key={mid,3}；命中取该元素，未命中返回 {0,3}（局部 -0x1c/-0x18 初值）。
    stGMInfo_t key;
    key.mid = mid;
    key.type = 3;
    std::list<stGMInfo_t>::const_iterator it =
        std::find(m_gmList.begin(), m_gmList.end(), key);
    if (it != m_gmList.end())
    {
        return *it;
    }
    stGMInfo_t fallback;
    fallback.mid = 0;
    fallback.type = 3;
    return fallback;
}

// ============================ WongWork::CMCAPManager ========================

unsigned int WongWork::CMCAPManager::getExposedCount() const
{
    // ORIG W 0x0869504c (size 0xb)：return *(unsigned int*)(this + 0x10)。
    return m_field10;
}

// ============================ WongWork::CUserPremium ========================

unsigned short WongWork::CUserPremium::getCeraShopDiscountRate() const
{
        // ORIG W 0x08328100 (size 0xf)：movzwl +0x860（折扣率半字）。
    return m_ceraDiscountRate;
}

std::vector<WongWork::SUserPremiumInfo> WongWork::CUserPremium::GetPremiumInfoList(
    int type) const
{
    // ORIG T 0x086ade9c (size 0xb6)：sret 按值返回 vector。遍历 entries[1..0x69]
    //（stride 0x14），m_type != 0 且 filter==0x270f 或等于 m_type 时 push_back。
    std::vector<SUserPremiumInfo> list;
    const SUserPremiumInfo* entries =
        reinterpret_cast<const SUserPremiumInfo*>(m_pad);
    for (int i = 1; i <= 0x69; ++i)
    {
        const SUserPremiumInfo& entry = entries[i];
        if (entry.m_type != 0 &&
            (type == 0x270f || entry.m_type == type))
        {
            list.push_back(entry);
        }
    }
    return list;
}

// ============================ WongWork::CEventMgr ===========================

int WongWork::CEventMgr::GetCoinPerDay()
{
    // ORIG T 0x0848eff8 (size 0x9)：mov $0x1,%eax; ret —— ORIG 即恒返 1。
    return 1;
}

int WongWork::CEventMgr::GetMoneyCharacCreate()
{
    // ORIG T 0x0848f06e (size 0x9)：mov $0x0,%eax; ret —— ORIG 即恒返 0。
    return 0;
}

bool WongWork::CEventMgr::IsPCRoomEventing()
{
    // ORIG T 0x0848f0dc (size 0x64)：_IsEventing(2006-01-24 00 .. 2006-02-07 06,
    // EVENT_PLAIN)。
    return _IsEventing(0x7d6, 1, 0x18, 0, 0x7d6, 2, 7, 6, EVENT_PLAIN);
}

// W7_BuildHarmonize 去重：IsChildrensDayEventing 唯一 owner 为 CEventMgr.cpp。

// ============================ WongWork::CWebEvent ===========================

// 数据表内容取自 df_game_r D 0x093ace60（size 0xb4）/ 0x093acf20（size 0x6e），
// 槽长 0x12/0xb，非零槽逐一转抄，其余为零串。
char WongWork::CWebEvent::m_strEventName[10][0x12] = {
    "", "", "Yegam", "ATI Coupon", "DNF Guide book",
    "Everland", "Muz", "King of Alttle", "Free Charge Combo", "" };

char WongWork::CWebEvent::m_strEventIPGCode[10][0xb] = {
    "", "", "", "IP00000001", "IP00000002", "", "", "", "", "" };

char const* WongWork::CWebEvent::_GetEventString(int idx)
{
    // ORIG T 0x086c1ba8 (size 0x2b)：idx≤0xa 允许（含第 11 槽，照抄 asm jle
    // 边界）；stride 0x12。ret 栈变量形态按 ORIG 字节序（movl $0,-0x4(%ebp)
    // 初始化；if (idx>0xa) return ret → cmpl+jle 跳计算块）。
    char const* ret = 0;
    if (idx > 0xa)
    {
        return ret;
    }
    return &m_strEventName[0][0] + idx * 0x12;
}

char const* WongWork::CWebEvent::_GetEventIPGCode(int idx)
{
    // ORIG T 0x086c1bd4 (size 0x4d)：idx>9 或槽首字节为 0 时返回 NULL；
    // stride 0xb。
    if (idx <= 9)
    {
        const char* slot = &m_strEventIPGCode[0][0] + idx * 0xb;
        if (*slot != '\0')
        {
            return slot;
        }
    }
    return 0;
}

int WongWork::CWebEvent::_GetATIAvatarAbility(int itemIdx)
{
    // ORIG T 0x086c1a2e (size 0x8c)：离散映射 0x9ed0→1、0xae68→6、0xbe0c→4，
    // 其余（含 0xa060/0xaff8/0xbf9c/0xcd81/0xcf11 槽）→0。ret 栈变量形态按
    // ORIG 字节序（各 case 写 -0x4(%ebp)，尾部 mov -0x4(%ebp),%eax；无 default，
    // 0xcd81 独立 body、其余 0 值 case fall-through 共享 body——见 ORIG 布局）。
    int ret = 0;
    switch (itemIdx)
    {
        case 0x9ed0:
            ret = 1;
            break;
        case 0xae68:
            ret = 6;
            break;
        case 0xbe0c:
            ret = 4;
            break;
        case 0xcd81:
            ret = 0;
            break;
        case 0xa060:
        case 0xaff8:
        case 0xbf9c:
        case 0xcf11:
            ret = 0;
            break;
    }
    return ret;
}
 
bool WongWork::CWebEvent::ReqSaveEventItem(unsigned int p1, unsigned int p2,
                                           unsigned int p3, unsigned int p4,
                                           unsigned int p5, unsigned int p6,
                                           bool last)
{
    // ORIG T 0x086c1910 (size 0x11d)：Acquire 流 → <<0x4c(SIG) → <<-1 →
    // GetInBuffer<SIG_SAVE_EVENTITEM> 按 asm 偏移填参 → MsgQueueMgr put(DB_Q)
    // → guard 析构 → return 1。
    Stream* stream = s_stream_pool->Acquire("WebEvent.cpp", 0xa0);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x4c);
    guard.operator*()->operator<<(-1);
    SIG_SAVE_EVENTITEM* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_EVENTITEM>();
    sig->m_field00 = p1;
    sig->m_field04 = p2;
    sig->m_field0c = p3;
    sig->m_field10 = p4;
    sig->m_field08 = p5;
    sig->m_field14 = p6;
    sig->m_field18 = last ? 1 : 0;
    s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

bool WongWork::CWebEvent::RecvInfoFromWeb(unsigned int p1, unsigned int p2,
                                          unsigned int p3, unsigned int p4,
                                          unsigned int p5, unsigned int p6)
{
    // ORIG T 0x086c11d0 (size 0x4e)：p3==999 直接返回 0；否则转发
    // ReqSaveEventItem(...,false) 并透传其结果（ORIG 无布尔规范化）。
    if (p3 == 999)
    {
        return false;
    }
    return ReqSaveEventItem(p1, p2, p3, p4, p5, p6, false);
}

unsigned int WongWork::CWebEvent::RecvInfoFromDB(CUser* user, unsigned int itemId,
                                                 unsigned int count, unsigned int eventIdx,
                                                 unsigned int extra)
{
    // ORIG T 0x086c121e (size 0x6f1)。
    if (itemId == 1)
    {
        // ---- 活动币入账分支（asm +0x16..+0x97）----
        const CInventory* invR = user->getCurCharacInvenR();
        const unsigned int prev = invR->GetEventCoin();
        user->getCurCharacInvenW()->SetEventCoin(prev + count);
        const unsigned int now = user->getCurCharacInvenR()->GetEventCoin();
        user->m_historyLog.EventCoinAdd((int)now, (int)count,
                                        (eCoinAddReason)2);
        user->SaveMoneyCoin();
        user->SendCoin();
        return 1;
    }
    if (itemId == 999)
    {
        // ---- IPG 计费分支（asm +0xa9..+0x207）----
        char countStr[0xc];
        OS_API::snprintf(countStr, sizeof(countStr), "%d", (int)extra);
        Stream* stream = s_stream_pool->Acquire("WebEvent.cpp", 0x47);
        CStreamGuard guard(stream, false);
        guard << eventIdx;
        guard << extra;
        char uniqueNoBuf[0x20];
        CGenUniqueNo generator;
        char* uniqueNoOut = generator.GenUniqueNo(uniqueNoBuf);
        const char* codeStr = _GetEventIPGCode((int)eventIdx);
        if (codeStr != 0)
        {
            IPGBridgeFn fn = (IPGBridgeFn)(*(void**)(*(char**)(&s_pIPGHelper)));
            fn(&s_pIPGHelper, user, codeStr, countStr, uniqueNoOut,
               _GetEventString((int)eventIdx), user->getCurCharacName(),
               guard.Get());
        }
        else
        {
            s_stream_pool->Free(guard);   // asm +0x156：回收后走析构出口
            return 1;                     // asm +0x169：ebx=1
        }
        guard.~CStreamGuard();            // asm +0x1fc 出口析构
        return 0;                         // 发送成功路径 ORIG ebx=0
    }

    // ---- 物品发放分支（asm +0x20c..+0x6ef）----
    CDataManager* mgr = G_CDataManager();
    CItem* item = mgr->find_item((int)itemId);
    if (item == 0)
    {
        return 0;
    }
    Inven_Item slot;
    item->make_item(slot);
    if (slot.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
    {
        // asm +0x273..+0x29b：equip 类且 count>1 才回写数量到 item+0x07。
        if ((int)count > 1)
        {
            slot.m_addInfo2 = (int)count;
        }
    }
    else
    {
        slot.m_addInfo2 = (int)count;     // ←item+0x07（asm -0xde 槽）
    }
    slot.m_addInfo = (int)itemId;         // ←item+0x02（asm -0xe3 槽）

    int spaceCode = 0xd;
    if (eventIdx == 0x186a1)
    {
        spaceCode = 0xe;
    }
    else if (eventIdx == 0x30d46)
    {
        spaceCode = 0x14;
    }

    if (item->IsAvatarItem())
    {
        // ---- Avatar 分支（asm +0x2ce..+0x3cf）----
        const int ability = _GetATIAvatarAbility((int)itemId);
        char agencyName[0x14];
        memset(agencyName, 0, sizeof(agencyName));
        OS_API::snprintf(agencyName, 0x14, "E_%s",
                         _GetEventString((int)eventIdx));
        CInventory* invW = user->getCurCharacInvenW();
        const int avatarSlot =
            invW->AddAvatarItem((int)itemId, 0, false, 0,
                                (char)(signed char)ability, agencyName,
                                (eAvatarItemAddReason)2, false, 0);
        if (avatarSlot == -1)
        {
            return 0;
        }
        user->SendUpdateItemList(CUser::eSendTarget(1), ENUM_ITEMSPACE(1),
                                 avatarSlot);
        return 1;
    }

    int outType = 0;
    int outValue = 0;
    bool flag = false;
    if (s_pCeraShop->IsSpecialItem((int)itemId, outType, outValue, flag))
    {
        // ---- 商城特殊物品分支（asm +0x417..+0x5d1）----
        int restrictCode = mgr->getItemList()->GetRestrictCode((int)itemId);
        const long curSec = (long)s_systemTime_.getCurSec();
        std::vector<Inven_Item> attached;
        CCeraShop::stCeraShopItemParam_t paramObj;
        const bool ok = s_pCeraShop->ProcessSpecialItem(
            user, (int)itemId, (int)count, restrictCode, curSec, true,
            attached, paramObj, '\0', '\0', false, 1);
        if (ok)
        {
            if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                s_pCeraShop->SaveFeaturedIdx(user, restrictCode,
                                             (int)curSec, 0);
            }
            return 1;
        }
        return 1;   // asm +0x55e：ProcessSpecialItem 失败仍 esi=1 → 成功出口
    }

    // ---- 普通背包插入分支（asm +0x5d6..+0x6ea）----
    CInventory* invW = user->getCurCharacInvenW();
    const int newSlot = invW->insertItemIntoInventory(
        slot, (eItemAddReason)spaceCode, true, false);
    if (newSlot == -1)
    {
        return 0;
    }
    user->SendUpdateItemList(CUser::eSendTarget(1), ENUM_ITEMSPACE(0),
                             newSlot);
    return 1;
}
