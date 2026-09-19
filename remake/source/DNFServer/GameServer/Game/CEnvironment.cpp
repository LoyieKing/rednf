// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEnvironment/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CEnvironment（服务器配置解析与访问）还原（2026-08-16）。
// 函数形态/字段偏移以 ORIG 汇编为准；字符串常量取自 df_game_r .rodata。

#include "CEnvironment.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "DNFFunctionLib.h"

// ===================== STDBConnInfo =====================

__attribute__((weak)) STDBConnInfo::STDBConnInfo()
{
    memset(m_host, 0, 0x11);
    memset(m_user, 0, 0x15);
    memset(m_pass, 0, 0x15);
    memset(m_db, 0, 0x1f);
    memset(m_pad61, 0, 0x100);
    m_port = 0;
    m_tail = 0;
}

// ===================== Server_Envir =====================

__attribute__((weak)) Server_Envir::Server_Envir()
{
    memset(m_ipgIP, 0, 0x10);
    m_acceptLockDBQueueCount = 0;
    m_acceptUnlockDBQueueCount = 0;
    m_smallPoolSize = 0xbb8;
    m_defaultPoolSize = 0xbb8;
    m_bigPoolSize = 0x1f4;
    memset(m_pcroomServerIP, 0, 0x10);
    m_pcroomServerPort = 0;
    memset(m_communityServerIP, 0, 0x10);
    m_communityServerPort = 0;
    memset(m_auctionServerIP, 0, 0x10);
    m_auctionServerPort = 0;
    memset(m_ceraAuctionServerIP, 0, 0x10);
    m_ceraAuctionServerPort = 0;
#if remake_verify
    memcpy(m_systemMonitoringIP, "127.0.0.1", 0xf);
#else
    // DNF-FIX: 0xf 是从 10 字节字面量（含 NUL）往外多读 5 字节的越界读；
    // ORIG 全局字面量相邻数据碰巧无害，现代布局下会被 ASan 抓到，改为按字面量长度拷贝。
    memcpy(m_systemMonitoringIP, "127.0.0.1", sizeof("127.0.0.1"));
#endif
    m_scriptpacks = 0;
    m_channelType = 0x13;
    m_validateScript = 1;
    m_garenaPublickey = "publickey.pem";
}

__attribute__((weak)) Server_Envir::~Server_Envir()
{
}

// ===================== CEnvironment 构造/析构 =====================

__attribute__((weak)) CEnvironment::CEnvironment()
{
    m_suspendState = false;
    m_cipher.Initialize("qortmddkqortmdcksqordudwlswjdguswn", 0x21, CTEA::sm_chain0, 0, 0);
    memset(m_serverEnvir.m_exchangeServerIP, 0, 0x10);
    m_serverEnvir.m_exchangeServerPort = 0;
}

__attribute__((weak)) CEnvironment::~CEnvironment()
{
}

// ===================== 解析/加载 =====================

#include "CEnvironment/load.inc"


#include "CEnvironment/fgetline.inc"


#include "CEnvironment/check_comment.inc"


#include "CEnvironment/DecryptValue.inc"


#include "CEnvironment/IsEnableValue.inc"


#include "CEnvironment/adjust_base_script_directory.inc"


#include "CEnvironment/parse_se_event_db.inc"


#include "CEnvironment/parse.inc"


// ===================== 简单 getter/setter =====================
// [判据㉓ 符号绑定 · 2026-09-10] 本区 55 个符号 ORIG 全为 **W** 弱绑定（CEnvironment 的
// 50 个 getter/setter + lock/unlock、Server_Envir C1/C2/D1/D2、STDBConnInfo C1/C2、
// CEnvironment C1/C2/D1/D2），尺寸逐位相同；ORIG 地址分散于多个 TU 段
// （get_tcp_port 0x0857f428 / get_gc_no 0x0844dc6e / lock 0x082a6ccc / C1 0x080cc00c /
// Server_Envir C1 0x080cbca8 / STDBConnInfo C1 0x080ecd38 …）⇒ 原码头内定义。
// **为何不用头内 inline**（Main 裁决 2026-09-10）：CEnvironment.h 被 161 个 TU include，
// 改头触发全量重编 + codegen/GI/EH 连锁风险 >> 收益；.cpp 内 out-of-line 定义加
// __attribute__((weak)) 同样产出 nm 绑定 W、尺寸逐位不变、符号身份一致
// （与 G_* 单例、Cipher.cpp 同口径）。ORIG 保持 **T** 的 8 个函数
// （parse/load/fgetline/check_comment/DecryptValue/IsEnableValue/
//  adjust_base_script_directory/parse_se_event_db）不动。

__attribute__((weak)) char* CEnvironment::get_file_name()
{
    return m_serverEnvir.m_fileName;
}

__attribute__((weak)) void CEnvironment::set_file_name(const char* name)
{
    strcpy(m_serverEnvir.m_fileName, name);
}

__attribute__((weak)) char* CEnvironment::get_server_ip()
{
    return m_serverEnvir.m_serverIP;
}

__attribute__((weak)) int CEnvironment::get_tcp_port()
{
    return m_serverEnvir.m_tcpPort;
}

__attribute__((weak)) int CEnvironment::get_udp_port()
{
    return m_serverEnvir.m_udpPort;
}

__attribute__((weak)) int CEnvironment::get_channel_no()
{
    return m_serverEnvir.m_channelNo;
}

__attribute__((weak)) int CEnvironment::get_gc_no()
{
    return m_serverEnvir.m_gcNo;
}

__attribute__((weak)) int CEnvironment::get_gc_no_hardcode(ENUM_SERVER_GROUP group)
{
    return (int)group * 1000 + m_serverEnvir.m_channelNo;
}

__attribute__((weak)) char* CEnvironment::get_channel_name()
{
    return m_serverEnvir.m_channelName;
}

__attribute__((weak)) void CEnvironment::set_channel_name(char* name)
{
    strcpy(m_serverEnvir.m_channelName, name);
}

__attribute__((weak)) int CEnvironment::get_server_group()
{
    return m_serverEnvir.m_serverGroup;
}

__attribute__((weak)) bool CEnvironment::get_suspend_state()
{
    return m_suspendState;
}

__attribute__((weak)) void CEnvironment::set_suspend_state(bool state)
{
    lock();
    m_suspendState = state;
    unlock();
}

__attribute__((weak)) eStartArgumentType_t CEnvironment::get_operation_type()
{
    return m_operationType;
}

__attribute__((weak)) void CEnvironment::set_operation_type(eStartArgumentType_t type)
{
    m_operationType = type;
}

__attribute__((weak)) char* CEnvironment::get_udp_ip_monitor()
{
    return m_serverEnvir.m_udpIPMonitor;
}

__attribute__((weak)) int CEnvironment::get_udp_port_monitor()
{
    return m_serverEnvir.m_udpPortMonitor;
}

__attribute__((weak)) char* CEnvironment::get_udp_ip_guild()
{
    return m_serverEnvir.m_udpIPGuild;
}

__attribute__((weak)) int CEnvironment::get_udp_port_guild()
{
    return m_serverEnvir.m_udpPortGuild;
}

__attribute__((weak)) int CEnvironment::get_tcp_port_guild()
{
    return m_serverEnvir.m_tcpPortGuild;
}

__attribute__((weak)) int CEnvironment::get_tcp_port_monitor()
{
    return m_serverEnvir.m_tcpPortMonitor;
}

__attribute__((weak)) char* CEnvironment::get_udp_ip_hades()
{
    return m_serverEnvir.m_udpIPHades;
}

__attribute__((weak)) int CEnvironment::get_udp_port_hades()
{
    return m_serverEnvir.m_udpPortHades;
}

__attribute__((weak)) int CEnvironment::get_tcp_port_hades()
{
    return m_serverEnvir.m_tcpPortHades;
}

__attribute__((weak)) char* CEnvironment::get_udp_ip_statistic()
{
    return m_serverEnvir.m_udpIPStatistic;
}

__attribute__((weak)) int CEnvironment::get_udp_port_statistic()
{
    return m_serverEnvir.m_udpPortStatistic;
}

__attribute__((weak)) char* CEnvironment::get_udp_ip_doublecheck()
{
    return m_serverEnvir.m_udpIPDoubleCheck;
}

__attribute__((weak)) int CEnvironment::get_udp_port_doublecheck()
{
    return m_serverEnvir.m_udpPortDoubleCheck;
}

__attribute__((weak)) char* CEnvironment::get_udp_ip_channel()
{
    return m_serverEnvir.m_udpIPChannel;
}

__attribute__((weak)) int CEnvironment::get_udp_port_channel()
{
    return m_serverEnvir.m_udpPortChannel;
}

__attribute__((weak)) int CEnvironment::get_tcp_port_channel()
{
    return m_serverEnvir.m_udpPortChannel;
}

__attribute__((weak)) char* CEnvironment::get_exchange_server_ip()
{
    return m_serverEnvir.m_exchangeServerIP;
}

__attribute__((weak)) int CEnvironment::get_exchange_server_port()
{
    return m_serverEnvir.m_exchangeServerPort;
}

__attribute__((weak)) char* CEnvironment::get_school_server_ip()
{
    return m_serverEnvir.m_schoolServerIP;
}

__attribute__((weak)) int CEnvironment::get_school_server_port()
{
    return m_serverEnvir.m_schoolServerPort;
}

__attribute__((weak)) char* CEnvironment::get_pcroom_server_ip()
{
    return m_serverEnvir.m_pcroomServerIP;
}

__attribute__((weak)) int CEnvironment::get_pcroom_server_port()
{
    return m_serverEnvir.m_pcroomServerPort;
}

__attribute__((weak)) char* CEnvironment::get_community_server_ip()
{
    return m_serverEnvir.m_communityServerIP;
}

__attribute__((weak)) int CEnvironment::get_community_server_port()
{
    return m_serverEnvir.m_communityServerPort;
}

__attribute__((weak)) char* CEnvironment::get_auction_server_ip()
{
    return m_serverEnvir.m_auctionServerIP;
}

__attribute__((weak)) int CEnvironment::get_auction_server_port()
{
    return m_serverEnvir.m_auctionServerPort;
}

__attribute__((weak)) char* CEnvironment::get_cera_auction_server_ip()
{
    return m_serverEnvir.m_ceraAuctionServerIP;
}

__attribute__((weak)) int CEnvironment::get_cera_auction_server_port()
{
    return m_serverEnvir.m_ceraAuctionServerPort;
}

__attribute__((weak)) char* CEnvironment::get_system_monitoring_ip()
{
    return m_serverEnvir.m_systemMonitoringIP;
}

__attribute__((weak)) void CEnvironment::lock()
{
    m_lock.lock();
}

__attribute__((weak)) void CEnvironment::unlock()
{
    m_lock.unlock();
}

// ===================== 单例 =====================

// 判据㉓ 符号绑定：ORIG `_Z14G_CEnvironmentv` @0x080cc181 为 **W** 弱绑定（size 0xd，
// 与本节 T 版尺寸一致）⇒ 定义处加 weak（Main 已批准单点修法，只改 .cpp）。
__attribute__((weak)) CEnvironment* G_CEnvironment()
{
    return GlobalInstance<CEnvironment>::inst_ptr();
}

template class GlobalInstance<CEnvironment>;
