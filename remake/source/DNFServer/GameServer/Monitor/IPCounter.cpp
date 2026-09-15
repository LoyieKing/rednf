// df_monitor_r — IPCounter（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "IPCounter.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFServerHandler.h"

CIPCounter::CIPCounter() {}

CIPCounter::~CIPCounter() {}

void CIPCounter::Init(CServerHandler* handler)
{
    m_count = 0;
    m_min = 0;
    m_term = 0x708;
    m_option = 5;
    m_loadFlag = 1;
    m_loadParam = 1;
    m_handler = handler;
}

void CIPCounter::Proc(unsigned int tick)
{
    if (m_min >= tick)
    {
        return;
    }
    if (m_loadFlag == 0)
    {
        return;
    }
    if (m_count + m_term < tick)
    {
        m_handler->SendDBMWRequestIPCounter(m_option, m_loadParam);
        m_count = tick;
        CMyFileLog log(__FUNCTION__, 0x3c);
        log("./log/Secu", "[IP Counter] LoadStart");
    }
    m_min = tick + 0x3c;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-NEAR-0002 已消除）| CIPCounter::setOption
void CIPCounter::setOption(unsigned char type, unsigned char opt)
{
    if (type == 0)
    {
        setLoadTerm(opt);
    }
    else if (type == 1)
    {
        setMinIPCount(opt);
    }
    else if (type == 2)
    {
        m_loadFlag = 1;
        m_min = 0;
        m_count = 0;
    }
    else if (type == 3)
    {
        m_loadFlag = 0;
    }
    else if (type == 4)
    {
        m_loadParam = 0;
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0001 | monitor | 与ORIG差异=DIFF | CIPCounter::setLoadTerm | 详见 function_reports/monitor/_ZN10CIPCounter11setLoadTermEh.md
// [P-DIFF] 27/29 ndiff=13。已落地：删掉具名临时 `int v`，改为直接对成员展开 ⇒ 帧 0x14→**0x4**、
//   槽集合与 ORIG 逐项相同（T2 分诊：原为我方多槽的反向族）；第一段比较加 (int) 使 `jbe`→`jle`（与 ORIG 有符号比较一致）。
//   第二段 ORIG 是 `cmp $0x708; jae`、我方恒为 `cmp $0x707; ja`：已试 `!(m_term >= 0x708u)`、`0x708u > m_term`
//   两种写法均被 GCC 规范化回 0x707/ja（同语义不同编码）。
//   余差 = ORIG 的 MIN 展开形态（计算值在寄存器内比较 + 条件常量装载 + **单次**存储），我方为"存成员→读回→条件覆盖"。
//   已试不利：具名 v 三元（cmov）、std::min（引入模板展开）、f2 成员展开（帧 0x4 但 clamp 用内存立即数）。
// [P-DIFF-v2] 改用 MIN/MAX 语义三目（`X > HI ? HI : X`）：27/29 → **27/27**（长度与 ORIG 完全一致），
//   canon ndiff 13 → **8**；首段 clamp 载体回到寄存器（无内存读回）。残差 = GCC 对 MIN/MAX 用 `cmov`，
//   ORIG 用 `cmp+jle/jae+mov` 分支形态（已扫 25+ 变体：if/if-else/register/成员自反/switch 均不可复现无槽载体 ⇒ 判 ✗ 停投入）。
void CIPCounter::setLoadTerm(unsigned char term)
{
    m_term = (int)(unsigned char)term * 0x3c > 0x2a30 ? 0x2a30 : (int)(unsigned char)term * 0x3c;
    m_term = m_term < 0x708 ? 0x708 : m_term;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0002 | monitor | 与ORIG差异=DIFF | CIPCounter::setMinIPCount | 详见 function_reports/monitor/_ZN10CIPCounter13setMinIPCountEh.md
void CIPCounter::setMinIPCount(unsigned char count)
{
    // [P-DIFF] ORIG 形态 = MIN_EXPR 展开（寄存器内比较 + 条件常量装载、无 join jmp）。
    // 本形态把 ndiff 从 6（if 夹取式）降到 4（同 13/14 len），仍差 1 条 jmp。
    // [W11-MonitorRest] 已扫 25 变体（三元/if/if-else/先赋值后条件覆盖/register 具名/成员自反/switch/
    //   各 signedness 与 0xc8/200/201 常量形式）：ORIG 的「载体无栈槽（eax）+ cmp+jbe 条件覆盖 + 单次 store」
    //   = GCC 4.4 -O0 的 **表达式临时** 形态；任何带具名变量（含 register，落 ebx 并多 push/pop）或
    //   if 语句（先 store 后条件覆盖）的写法都 ≥7 条 ⇒ 本件与 setLoadTerm 同属「无槽载体」族 ✗ 勿再投入。
    m_option = count <= 200 ? count : 200;
}


