// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Secu_GoldControl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "Secu_GoldControl/Init.inc"
{
    m_pUser = user;
    m_dateFlag = GlobalData::g_GoldControlDateFlag;
    m_fieldb8 = 0;
    m_fieldbc = 0;
    m_mailGold = 0;
    m_fielde6 = 0;
    memset(m_mailCharName, 0, 0x1e);
}

#include "Secu_GoldControl/makeRequest.inc"
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
#include "Secu_GoldControl/SetInfo.inc"
{
    info.toCopy(m_total);
}

#include "Secu_GoldControl/SetMailCharName.inc"
{
    if (name)
    {
        strncpy(m_mailCharName, name, 0x1e);
    }
}

#include "Secu_GoldControl/resetRangeMoney.inc"
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
// ORIG 0x0828757c。**判词：AE（61/61，diff_lines=0）**。
// 手法：①case 0xe 置 switch 首（块物理序对齐，见内注 P324）；②makeRequest 三实参
// 直写进调用（删具名 accId/uid），令 4.4 自然升格 callee-saved `push %esi/%ebx`
// —— 与 ORIG 0x082875c1 `lea 0xc8(%eax),%esi` / 0x082875d4 `mov %eax,%ebx` 同形
// （旧形两局部各占 4B 栈槽、帧 0x28 vs ORIG 0x10）。
#include "Secu_GoldControl/AddGold.inc"
{
    CheckDate();
    switch (reason)
    {
    // [P324 叶块书写序] ORIG 0x0828757c 块物理序 = MailGold(0x28) → Import(0x81)
    // → Auction(0x95) → Money(0xa7)，即 case 0xe 在源码中排最前（4.1 按 case
    // 声明序 emit 块）；旧形把 Import 组写最前 ⇒ 三块整体位移（ndiff=0 但 diff）。
    case 0xe:
        MailGold(value, false);
        {
            // [P52/P134 实测] ORIG 0x0828757c 以 `push %esi/%ebx` 保活实参跨调用
            // （无 -0x10/-0xc 栈槽）⇒ 删具名 accId/uid 内联进调用，让 4.4 自然升格
            // callee-saved。旧形两局部各占 4B 槽且帧 0x28 vs ORIG 0x10。
            DB_CheckCharacName::makeRequest(m_pUser->GetUID(), m_pUser->get_acc_id(),
                                            m_mailCharName, false);
        }
        return;
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
    case 0x12:
        AddAuctionMoney(value);
        break;
    default:
        break;
    }
    AddMoney(value);
    CheckMoneyRange();
}
// ORIG 0x0828764a。**判词：AE（69/69，diff_lines=0）**。
// 手法：①if-else 链 → 真 switch（case 声明序 0x11,0xe,0x1a,2，见内注）；②同
// AddGold 的实参内联（ORIG 0x082876a1 `lea 0xc8(%eax),%esi` / 0x082876b4
// `mov %eax,%ebx` 跨 MailGold→makeRequest 保活，无 -0x10/-0xc 栈槽）。
#include "Secu_GoldControl/SubGold.inc"
{
    CheckDate();
    // [P339/switch 轴] ORIG 0x0828764a 是真 switch 展开 ——
    //   `mov 0x10(%ebp),%eax; cmp $0xe,%eax; je <import>; cmp $0xe,%eax; jg <L2>;
    //    cmp $2,%eax; je <case2>; jmp <money>; L2: cmp $0x11 ...; cmp $0x1a ...`，
    // 块物理序 MailGold(0x3a) → SubImport(0x93) → SubAuction(0xa7) → SubMoney(0xb9)
    // → case2 空块(0xd8: `nop`) ⇒ 源码为 switch 且 case 声明序 = 0x11, 0xe, 0x1a, 2。
    // 旧 if-else 链形：`cmpl $0xe,0x10(%ebp); jne`（4.4 内存直比）、case2 无空块，
    // 且 makeRequest 实参经具名 accId/uid 落 -0x10/-0xc 栈槽（ORIG 无槽）。
    switch (reason)
    {
    case 0x11:
        MailGold(value, true);
        // [P52/P134] 同 AddGold：实参直接内联进调用，让 4.4 自然升格 callee-saved
        // （ORIG 以 esi/ebx 保活 charName/get_acc_id 结果跨 call，无栈槽）。
        DB_CheckCharacName::makeRequest(m_pUser->GetUID(), m_pUser->get_acc_id(),
                                        m_mailCharName, true);
        return;
    case 0xe:
        SubImportMoney(value);
        break;
    case 0x1a:
        SubAuctionMoney(value);
        break;
    case 2:
        return;
    default:
        break;
    }
    SubMoney(value);
    CheckMoneyRange();
}

#include "Secu_GoldControl/AddMoney.inc"
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

#include "Secu_GoldControl/SubMoney.inc"
{
    m_cur.m_sub += value;
    m_cur.m_count[4]++;
    m_total.m_sub += value;
    m_total.m_count[4]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldMinus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)0);
}

#include "Secu_GoldControl/AddImportMoney.inc"
{
    m_cur.m_import += value;
    m_cur.m_count[3]++;
    m_total.m_import += value;
    m_total.m_count[3]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldPlus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)1);
}

#include "Secu_GoldControl/SubImportMoney.inc"
{
    m_cur.m_importSub += value;
    m_cur.m_count[5]++;
    m_total.m_importSub += value;
    m_total.m_count[5]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldMinus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)1);
}
#include "Secu_GoldControl/AddAuctionMoney.inc"
{
    // P63：ORIG if 块出口统一 → 尾部 jmp;nop 中继；early-return 形使 4.4
    // 同产 jbe→中继（现形 jbe 直落 epilogue 少中继）。
    if (value <= 4999)
        return;
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
#include "Secu_GoldControl/SubAuctionMoney.inc"
{
    // P63 同 AddAuctionMoney。
    if (value <= 4999)
        return;
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

#include "Secu_GoldControl/MailComplete.inc"
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

#include "Secu_GoldControl/TradeComplete.inc"
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

#include "Secu_GoldControl/MailGold.inc"
{
    m_mailGold = (int)value;
}

#include "Secu_GoldControl/MailSameMid.inc"
{
    m_mailGold = 0;
}



#include "Secu_GoldControl/SavetoDB.inc"
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

#include "Secu_GoldControl/CheckMoneyRange.inc"
{
    // b28d-31: P183 去指针缓存 —— ORIG 0x08287b80 直接
    // `mov 0x44(%eax),%edx; mov 0x40(%eax),%eax; cmp $0,hi; ja/jb;
    //  cmp $0xfffffffe,lo; ja` 即 64 位无符号 `> 0xfffffffeULL` 展开
    // （逐半指针比较四形不产生该 cmp 链）。语义等价，且消去 -0x10/-0xc 槽。
    if (m_cur.m_gold > 0xfffffffeULL || m_cur.m_sub > 0xfffffffeULL)
    {
        SavetoDB(true, false, true);
    }
}
#include "Secu_GoldControl/GetGoldInfo.inc"
{
    return &m_total;
}

#include "Secu_GoldControl/CheckDate.inc"
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


#include "Secu_GoldControl/UseFatigue.inc"
{
    CheckDate();
    m_fieldb8 += count;
}
