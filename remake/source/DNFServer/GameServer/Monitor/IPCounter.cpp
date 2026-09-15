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
void CIPCounter::setLoadTerm(unsigned char term)
{
    m_term = (int)(unsigned char)term * 0x3c;
    if ((int)m_term > 0x2a30)
    {
        m_term = 0x2a30;
    }
    if (m_term < 0x708)
    {
        m_term = 0x708;
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0002 | monitor | 与ORIG差异=DIFF | CIPCounter::setMinIPCount | 详见 function_reports/monitor/_ZN10CIPCounter13setMinIPCountEh.md
void CIPCounter::setMinIPCount(unsigned char count)
{
    // [P-DIFF] ORIG 形态 = MIN_EXPR 展开（寄存器内比较 + 条件常量装载、无 join jmp）。
    // 本形态把 ndiff 从 6（if 夹取式）降到 4（同 13/14 len），仍差 1 条 jmp。
    m_option = count <= 200 ? count : 200;
}


