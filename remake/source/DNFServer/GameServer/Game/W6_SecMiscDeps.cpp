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
#include "TaiwanCash.h"             // WongWork::IPG::CIPGHelper 权威类（b22 IPGInput 真调用）
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
// b22 桥清零（P201 + 真修，Model22）：原「handle-of-handle」§4.5 例外经 ORIG
// 0x086c1383..13f2 逐指令复核**推翻**——`mov eax,ds:0x941f734; mov eax,[eax]`
// 取 [s_pIPGHelper] 所指对象 vptr 的槽 0，`mov edx,ds:0x941f734; mov [esp],edx`
// 首参传 **s_pIPGHelper 值（对象 this）**，非旧注所称 &变量地址；旧桥
// `fn(&s_pIPGHelper, ...)` 系首参错位真 bug（IPGInput 以 this 解引用对象字段，
// 传 handle 槽地址=越界语义）。正解：TaiwanCash.h:436 权威虚槽 0
// IPGInput(CUser*, const char*×5, void*)（T 0x080ffb0a，定义 W6_IPGDeps.cpp:515）
// 真成员调用，与 GlobalData.cpp:3465 IPGOutputConfirm 真调用先例同族。
// ----------------------------------------------------------------------------

// ============================ WongWork::CSecurityCardCenter =================

WongWork::CSecurityCardCenter::CSecurityCardCenter()
{
    // [b28d-solo-W6_SecMiscDeps 判词 · IDENTICAL_AE] ORIG 0x082a6654 W：构造
    // CRijndael@0x0（0x47c 子对象，成员而非基类——见 GlobalData.h 判词）与
    // list<uchar>@0x47c；无派生类 vptr 写。改成员子对象后逐条全等（原旧判词
    // 「4.4 多发 vptr 写、N18a 封顶」已作废：真因是建模成派生类导致发射
    // _ZTV/_ZTI + 派生 vptr 写）。时间计数不在此写入（init() 置 1）。
}

void WongWork::CSecurityCardCenter::init()
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（P209附）] ORIG 0x085fe05e：残余仅
    // unsigned 局部 for 尾测
    // （ORIG `mov -0xc,%eax; cmp $0x29,%eax; setbe` vs 4.4 `cmpl $0x29,-0xc; setbe`，
    // 4.4 恒折内存直比；P209附 已实证 do-while/while 变体同折 ⇒ 封顶）；
    // 初始化/字符集/计数写入逐条等价（其余 30/32 条全等，ndiff 7→唯 1 条 mov+cmp）。
    // ORIG T 0x085fe05e：固定密钥初始化头部 CRijndael；42 字符 characterSet
    // 逐个 push 进 m_characterSet；+0x484 置 1。（字符集数据段 0x93ac1e0）
    static const unsigned char characterSet[] =
        "+<>?@#$&ABCDEFGHIJKLMNPQRSTUVWXYZ123456789";
    m_cipher.Initialize("n6;fw;vjfs0fuguf", 0x10, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    // ORIG `cmp $0x29,%eax; setbe` = 无符号 `i<=41`（P2 setbe=unsigned）。
    // 4.4 对 int 出 setle → 循环计数器改 unsigned 对齐 setbe。
    for (unsigned int i = 0; i < 42; ++i)
    {
        m_characterSet.push_back(characterSet[i]);
    }
    m_timeCount = 1;
}

char* WongWork::CSecurityCardCenter::encryptString(char const* src, char* out)
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（P75 实参装载序 + P30）；105→19]
    // ORIG 0x085fe2e2：帧 0x220。
    // **真语义 bug 修正**：旧 `memset(plain,0,0x40)` 少清 0xc0 字节——ORIG 两处
    // `mov $0x40,%edx; …; rep stos` 的 0x40 是 **dword 计数**（0x40×4=0x100 字节，
    // P116/P253）⇒ 必须 `char plain[0x100] = {0}`（P214③ 复现 rep stos）。
    // 另：paddedLength 预清零（P198①）、length 槽复用为 hexLength、异常路径直接
    // `return 0`（ORIG ebx=0，无 result 局部）。
    // ORIG T 0x085fe2e2 (size 0x161)。两块 256 字节栈缓冲清零 → strncpy 0x100
    // → strlen → IMethod::GetBlockSize 取块大小并向上取整 → Pad → CRijndael::
    // Encrypt（asm 为非虚派发直呼）→ Binary2Hex 转十六进制并以 len*2+1 下标补
    // '\0'。异常路径返回 NULL（ORIG catch 吞掉后 ebx=0）。
    char plain[0x100] = {0};
    unsigned char cipher[0x100] = {0};
    strncpy(plain, src, 0x100);
    size_t length = strlen(plain);
    int paddedLength = 0;
    try
    {
        const int blockSize = m_cipher.GetBlockSize((unsigned int*)0);
        // 块序照 ORIG：`js L_neg` 远跳 + else 块落函数尾（内部 rem==0 的 je 落中部）
        if (blockSize >= 0)
        {
            if ((int)length % blockSize != 0)
            {
                paddedLength = ((int)length / blockSize + 1) * blockSize;
            }
            else
            {
                paddedLength = (int)length;
            }
        }
        else
        {
            paddedLength = (int)length;
        }
        if (blockSize >= 0)
        {
            m_cipher.Pad(plain, (int)length, (unsigned int*)0);
        }
        m_cipher.Encrypt(plain, reinterpret_cast<char*>(cipher),
                         (size_t)paddedLength);
        length = (size_t)(paddedLength * 2 + 1);
        DNFFLib::Binary2Hex(cipher, paddedLength, out);
        out[length] = '\0';
    }
    catch (...)
    {
        return 0;   // ORIG __cxa_begin_catch/__cxa_end_catch → ebx=0
    }
    return out;
}

char* WongWork::CSecurityCardCenter::generateCertKey(char* out)
{
    // ORIG T 0x085fe17e (size 0x3b)：rand∈[10000,99999] 格式化为 5 位数字串。
    // P0/P183：ORIG 无 `value` 栈槽（call 结果经 %eax 直传 snprintf 出参槽），
    // 具名局部导致 spill/reload 2 条 + 帧 +0x10 → 内联为表达式右值。
    OS_API::snprintf(out, 6, "%d", get_rand_int(89999) + 0x2710);
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
    // [b28d-solo-W6_SecMiscDeps 判词 · IDENTICAL_AE] ORIG 0x085fe0ec：random5/timeCount/
    // channelNo/serverGroup 全程 callee-saved（%ebx/%edi/%esi/%eax）无栈槽往返 ⇒
    // 原码无这些具名局部，必须内联进 snprintf 实参位（P0）；仅 curSec 占 -0x1c
    // 具名槽。旧具名局部版每值各多 store+load 且无 push edi/esi/ebx。
    // ORIG T 0x085fe0ec (size 0x91)：
    // "%02d%02d%010d%05d%05d"(env+0x378, env+0x1b0, curSec, timeCount, rand5)
    _updateTimeCount();
    const int curSec = s_systemTime_.getCurSec();
    // CEnvironment 服务器标识字段尚未具名化（域外批次负责），按既有口径偏移读。
    OS_API::snprintf(out, 0x21, "%02d%02d%010d%05d%05d",
                     G_CEnvironment()->m_serverEnvir.m_serverGroup,  // +0x378
                     G_CEnvironment()->m_serverEnvir.m_channelNo,    // +0x1b0
                     curSec, m_timeCount, get_rand_int(99999));
    return out;
}

bool WongWork::CSecurityCardCenter::checkPhoneNumber(char const* phone) const
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（N4/P30 + P209附）] ORIG 0x085fe444：
    // 残余为 `add 0xc(%ebp),%eax` 两步
    // 寻址 vs 4.4 `mov 0xc(%ebp),%edx; lea (%edx,%eax,1),%eax`（P30/N4）+ 循环尾测
    // mem 直比（P209附）；数字判定/越界语义逐条等价。原 b28c-L7 已实证 [] / +i /
    // &arr[i] 三形同产 lea，本轮 fresh 复核判定维持 N 封顶。
    // ORIG T 0x085fe444 (size 0x52)：i<=0xb 为循环条件；'\0' 提前 break 共享
    // return-1 出口（je→尾块+loop 尾 jmp 对）；数字判定单式
    // 9U < (unsigned)(schar)c - 0x30U（jbe 即无符号序，涵盖 c<'0'）。
    // [b28c-L7] N4/N18b 封顶（语义已逐条对齐，两处 4.1 固有寻址/装载形不可造）：
    // ①ORIG 0x085fe456 指针+下标折叠为 `mov -0x4,%eax; add 0xc(%ebp),%eax`
    //   （mem 作 add 源），4.4 恒 `mov 0xc(%ebp),%edx; lea (%edx,%eax,1),%eax`
    //   （N4：数组 add-形 与 lea-形固有差，穷举 [] / +i / &arr[i] 三形同产 lea）；
    // ②ORIG 循环界 `mov -0x4(%ebp),%eax; cmp $0xb,%eax; setbe`（值先载寄存器），
    //   4.4 对 unsigned 局部比较恒 `cmpl $0xb,-0x4(%ebp)` 直读内存（P209 反向，
    //   改 signed 则 setbe→setle 助记符先失配）。其余 24 条逐条全等。
    for (unsigned int i = 0; i <= 0xb; ++i)
    {
        if (phone[i] != '\0')
        {
            if (9U < (unsigned)((int)(signed char)phone[i] - 0x30U))
                return false;
        }
        else
            break;
    }
    return true;
}

char* WongWork::CSecurityCardCenter::generateCardData(char* out) const
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（P30 add-vs-lea + P209附 尾测）；42→7]
    // ORIG 0x085fe1ba：帧 0x40。
    // 源码形态按 ORIG 反汇编还原：①`movl $0x26,-0x18` 死局部（写后从未读，
    // P171/P232 照抄）；②`for (pos=0; pos<=0x25; ++pos)`（4.1 尾测 setbe 物化 +
    // jmp 到条件块）；③begin/end 各求值一次后缓存（P13，`it != end` 用缓存槽
    // -0x28/-0x2c）；④seen 声明先于 pick（槽 -0x10/-0xc）。
    // ORIG T 0x085fe1ba (size 0x128)：拷贝 m_characterSet 至临时 list；循环
    // （pos<=0x25 继续）：以 list.size() 为上界随机取槽，顺序迭代到第 pick 个
    // 元素写出并 erase（每字符只使用一次）。
    std::list<unsigned char> pool(m_characterSet);
    int unusedLimit = 0x26;   // ORIG -0x18：写后从未读（死局部，照抄）
    (void)unusedLimit;
    for (unsigned int pos = 0; pos <= 0x25; ++pos)
    {
        int seen = 0;
        const int pick = get_rand_int((int)pool.size());
        std::list<unsigned char>::iterator it = pool.begin();
        std::list<unsigned char>::iterator end = pool.end();
        while (it != end)
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
    }
    return out;
}

// ============================ WongWork::CSecurityCard =======================

namespace
{
// CSecurityCard 答案表布局视图（ORIG 表基址 this+0x08，`movzbl 0x8(%eax,%edx,1)`
// 实证；仅 TU 局部 POD 布局视图，无成员函数/无符号发射）。
struct CSecurityCardAnswerView
{
    char m_pad08[8];
    unsigned char m_table[0x64];
};
}

bool WongWork::CSecurityCard::checkAnswer(unsigned char ansA, unsigned char ansB) const
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（P30/N4 add-vs-lea + P235 视图 base 先载）]
    // ORIG 0x085fdf6a：帧 0x8，
    // 形参 spill -0x4/-0x8；rowA/colA/rowB/colB 与 table 指针**无槽往返**
    // （ORIG `movzbl 0x3d(%eax); movsbl %al,%edx; …; movzbl 0x8(%eax,%edx,1)`）
    // ⇒ 原码全部内联表达式 + 表按 this+8 具名数组视角访问（disp 折叠）；
    // 比较用 P222 正向包裹（ORIG `jne <共享 false 尾块>`）。
    // ORIG W 0x085fdf6a (size 0xbb)。问题四元组 @0x3c..0x3f；答案表基址 this+8，
    // 第一问索引 = question[1]*10 + question[0]，第二问 = question[3]*10 +
    // question[2]；表值与答案均经 movsbl/zbl 后整数值比较。
    if (m_question3c[0] == 0 && m_question3c[1] == 0 &&
        m_question3c[2] == 0 && m_question3c[3] == 0)
    {
        return false;
    }
    if (static_cast<signed char>(
            reinterpret_cast<const CSecurityCardAnswerView*>(this)->m_table[
                static_cast<signed char>(m_question3c[1]) * 10 +
                static_cast<signed char>(m_question3c[0])]) ==
        static_cast<int>(ansA))
    {
        if (static_cast<signed char>(
                reinterpret_cast<const CSecurityCardAnswerView*>(this)->m_table[
                    static_cast<signed char>(m_question3c[3]) * 10 +
                    static_cast<signed char>(m_question3c[2])]) ==
            static_cast<int>(ansB))
        {
            return true;
        }
    }
    return false;
}

// ============================ WongWork::CGMAccounts =========================

WongWork::CGMAccounts::stGMInfo_t WongWork::CGMAccounts::getGMInfo(unsigned int mid) const
{
    // ORIG T 0x0810940a (size 0xdd)：std::find(m_gmList.begin(), end(), key)
    // key={mid,3}；命中取该元素，未命中返回 {0,3}（局部 -0x1c/-0x18 初值）。
    // [b28c-L7] P212 全头部声明 + P0 槽序：ORIG 槽阶梯为 fallback(-0x1c/-0x18,
    // 浅) → key(-0x24/-0x20) → find 结果(-0x28, 最深)；4.4 块内"先声明浅"，
    // 故声明序必须 fallback → key → it（原形 key 先 ⇒ 整组阶梯反向）。
    // ORIG key 段四写 `0,0 → type=3 → mid=mid` ⇒ key 具 value-init 预清。
    stGMInfo_t fallback;
    fallback.mid = 0;
    fallback.type = 3;
    stGMInfo_t key = stGMInfo_t();
    key.type = 3;
    key.mid = mid;
    std::list<stGMInfo_t>::const_iterator it =
        std::find(m_gmList.begin(), m_gmList.end(), key);
    if (it != m_gmList.end())
    {
        return *it;
    }
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
    // [b28d-solo-W6_SecMiscDeps 判词 · IDENTICAL_AE（P157/P183 去缓存 + P223 双岛）]
    // ORIG 0x086ade9c：sret=0x8(%ebp)、
    // this=0xc(%ebp)（m_entries@+0x00）；ORIG 每处字段读都重发完整 `i*0x14 + this`
    // 链（shl/add/shl + `mov (%eax,%ecx,1)`），**无** entries 指针槽与元素引用槽 ⇒
    // 源码必须逐字段下标直写（禁 `entries`/`entry` 缓存，旧版多 9 条槽往返）。
    // ORIG T 0x086ade9c (size 0xb6)：遍历 entries[1..0x69]（stride 0x14），
    // m_type != 0 且 filter==0x270f 或等于 m_type 时 push_back。
    std::vector<SUserPremiumInfo> list;
    for (int i = 1; i <= 0x69; ++i)
    {
        // P223 多岛形：ORIG 两失败出口落**不同** nop 岛（0x86adf15 / 0x86adf18，
        // 各带/共享 jmp 到循环尾 0x86adf19）⇒ 两条独立否定 goto + 标签串，
        // 标签序 = 守卫序（合并 && 会被 4.4 jump-thread 成 0 岛）。
        if (m_entries[i].m_type == 0)
            goto fail1;
        if (type != 0x270f && m_entries[i].m_type != type)
            goto fail2;
        list.push_back(m_entries[i]);
fail1:;
fail2:;
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
    // EVENT_PLAIN)。ORIG 尾 `test %al,%al; je L; mov $1; jmp; L: mov $0` = 分支
    // 物化返 bool（P56）；直 `return _IsEventing(...)` 被 4.4 直穿 %al → 改
    // if(cond)return true; return false; 复现分支段。
    if (_IsEventing(0x7d6, 1, 0x18, 0, 0x7d6, 2, 7, 6, EVENT_PLAIN))
        return true;
    return false;
}

// W7_BuildHarmonize 去重：IsChildrensDayEventing 唯一 owner 为 CEventMgr.cpp。

// ============================ WongWork::CWebEvent ===========================

// 数据表内容取自 df_game_r D 0x093ace60（size 0xb4）/ 0x093acf20（size 0x6e），
// 槽长 0x12/0xb，非零槽逐一转抄，其余为零串。
char WongWork::CWebEvent::m_strEventName[10][0x12] = {
    "", "", "Yegam", "ATI Coupon", "DNF Guide book",
    "Everland", "Muz", "King of Alttle", "Free Charge Combo", "" };

// 一维化（Main 批准）：槽 3/4 非空。33 个 '\0' 前缀 = 槽 0..2 空置，
// "IP00000001"+"\0" 占槽 3（偏移 33..43），"IP00000002" 占槽 4（44..54），余自动补 0。
char WongWork::CWebEvent::m_strEventIPGCode[10 * 0xb] =
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "IP00000001\0IP00000002";

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
    // stride 0xb。ORIG 形：p 预置槽（idx>9 分支 mov -0x4,%eax 读槽），
    // body 两处独立完整地址计算（movzbl 直用内联地址；return 处重算），
    // 条件 jle→body（fall-through = idx>9）。
    char const* p = 0;
    if (idx > 9)
    {
        return p;
    }
    if (m_strEventIPGCode[idx * 0xb] == '\0')
    {
        return 0;
    }
    return m_strEventIPGCode + idx * 0xb;
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
    // ORIG 实参内联（Acquire 结果直传构造，无 stream 局部槽）；bool 直写
    // 字节（movzbl+store，P136）。
    CStreamGuard guard(s_stream_pool->Acquire("WebEvent.cpp", 0xa0), true);
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
    sig->m_field18 = last;
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
    // [b28d-solo-W6_SecMiscDeps 判词 · 语义完成 / byte 层 N 封顶（N82/N89/N93附）]
    // ORIG 0x086c121e：§6.2 三件套 —— ①gap 6（431/425）；②p167 调用点差集
    // **CLEAR 52=52**（修前 MISSING operator*×2/operator<<(int)×2/G_CDataManager×1，
    // EXTRA operator<<(uint)×2/CGenUniqueNo::C1×1/~CStreamGuard×1/getItemList×1）；
    // ③残差为帧/槽号整体平移 0x20（N82）+ 返回值常量岛物化（N93附）+ STL 内联差
    // （N89）。本轮修 4 处真 bug（见各分支注释）。
    // ORIG T 0x086c121e (size 0x6f1)：
    if (itemId == 1)
    {
        // ---- 活动币入账分支（asm +0x16..+0x97）----
        // [b28d-solo-W6_SecMiscDeps] P0/P52：ORIG prev/now 无槽（%ebx/%eax 直传
        // SetEventCoin/EventCoinAdd），旧具名局部多 6 条槽往返。
        user->getCurCharacInvenW()->SetEventCoin(
            user->getCurCharacInvenR()->GetEventCoin() + count);
        user->m_historyLog.EventCoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), (int)count,
            (eCoinAddReason)2);
        user->SaveMoneyCoin();
        user->SendCoin();
        return 1;
    }
    if (itemId == 999)
    {
        // ---- IPG 计费分支（asm +0xa9..+0x207）----
        char countStr[0xc];
        // 真 bug 修正：ORIG 086c12c7 读 0x10(%ebp)=**count**（非 extra）作 %d。
        OS_API::snprintf(countStr, sizeof(countStr), "%d", (int)count);
        // P259：Acquire 结果内联进构造实参（ORIG 无 stream 局部槽）。
        CStreamGuard guard(s_stream_pool->Acquire("WebEvent.cpp", 0x47), false);
        // P103/P132：ORIG 调 operator*()->operator<<(int) 链（非 CStreamGuard::operator<<
        // 转发；p167 差集 MISSING operator*×2/operator<<(int)×2，EXTRA operator<<(uint)×2）。
        guard.operator*()->operator<<((int)eventIdx);
        guard.operator*()->operator<<((int)extra);
        char uniqueNoBuf[0x20];
        const char* codeStr = _GetEventIPGCode((int)eventIdx);
        // ORIG 单析构出口（086c141a 被两路径汇入）；GenUniqueNo 为 static（p167
        // EXTRA CGenUniqueNo::C1 证实旧 `CGenUniqueNo generator;` 多余）。
        if (codeStr == 0)
        {
            s_stream_pool->Free(guard);   // asm +0x156
        }
        else
        {
            s_pIPGHelper->IPGInput(user, codeStr, countStr,
                                   CGenUniqueNo::GenUniqueNo(uniqueNoBuf),
                                   _GetEventString((int)eventIdx),
                                   user->getCurCharacName(), guard.Get());
        }
        return codeStr == 0;              // asm +0x169/+0x1fc：ebx=1/0
    }

    // ---- 物品发放分支（asm +0x20c..+0x6ef）----
    // ORIG 无 mgr 具名槽：两次独立 `call G_CDataManager`（086c142d / 086c1647）。
    CItem* item = G_CDataManager()->find_item((int)itemId);
    if (item == 0)
    {
        return 0;
    }
    Inven_Item slot;
    item->make_item(slot);
    // 块序照 ORIG：`cmp $1,%al; je <equip 块>` —— 非 equip 直落（P222 正向包裹）。
    if (slot.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
    {
        slot.m_addInfo2 = (int)count;     // ←item+0x07（asm -0xde 槽）
    }
    else
    {
        // asm +0x273..+0x29b：equip 类且 count>1 才回写数量到 item+0x07。
        if ((int)count > 1)
        {
            slot.m_addInfo2 = (int)count;
        }
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
        // 真 bug 修正：ORIG AddAvatarItem 实参为 (itemId,0,false,ability,-1,agency,2,false,0)
        //（086c158e `mov %esi,0x10(%esp)`=char c、086c1586 `movl $0xffffffff,0x14(%esp)`=char d）。
        const int ability = _GetATIAvatarAbility((int)slot.m_addInfo);
        char agencyName[0x14] = {0};   // ORIG 5×movl $0 清零（P214③）
        OS_API::snprintf(agencyName, 0x14, "E_%s",
                         _GetEventString((int)eventIdx));
        CInventory* invW = user->getCurCharacInvenW();
        int avatarSlot = -1;   // ORIG `movl $0xffffffff,-0x28`（P198① 预清两句式）
        avatarSlot = invW->AddAvatarItem((int)slot.m_addInfo, 0, false,
                                         (char)(signed char)ability, (char)-1,
                                         agencyName,
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
    if (s_pCeraShop->IsSpecialItem((int)slot.m_addInfo, outType, outValue, flag))
    {
        // ---- 商城特殊物品分支（asm +0x417..+0x5d1）----
        // 真 bug 修正：ORIG 第 3 参（count 形参）= GetRestrictCode 结果(-0x24)，
        // 第 4 参（int& restrictCode）= 预清 0 的独立输出局部(-0x84)；旧码把
        // GetRestrictCode 结果直接当 restrictCode& 传、count 当第 3 参，两处错位。
        int outRestrict = 0;   // ORIG 086c1635 `movl $0x0,-0x84` 预清
        const int restrictCode =
            G_CDataManager()->m_itemList->GetRestrictCode((int)slot.m_addInfo);
        const long curSec = (long)s_systemTime_.getCurSec();
        std::vector<Inven_Item> attached;
        CCeraShop::stCeraShopItemParam_t paramObj;
        const bool ok = s_pCeraShop->ProcessSpecialItem(
            user, (int)slot.m_addInfo, restrictCode, outRestrict, curSec, true,
            attached, paramObj, '\0', '\0', false, 1);
        if (ok)
        {
            if (outRestrict != 0)
                return 0;   // ORIG 086c170b：ebx=0 经共享析构出口返回 0
            if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                s_pCeraShop->SaveFeaturedIdx(user, restrictCode,
                                             (int)curSec, 0);
            }
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
