// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCommunityServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CCommunityServerProxy（独占 OrigCommPx 批）。
// 依据 docs/class_func_reports/CCommunityServerProxy/*.md 与 objdump 转写
// （OnTcpRecv/SendTcpPacket 报告缺失，按 §3.2 nm 定址后 objdump 区间转写）。
#include <errno.h>
#include <string.h>   // memcpy / strerror

#include "GameTypes.h"   // cMyTrace
#include "CCommunityServerProxy.h"
#include "CEnvironment.h"   // G_CEnvironment / get_channel_no
#include "CUser.h"          // CUser::get_acc_id/GetUID/GetServerGroup/getSex
#include "Packet_Community_Login.h"
#include "Packet_Community_Logout.h"
#include "Packet_Request_Add_PvP_Buddy.h"
#include "Packet_Response_Add_PvP_Buddy.h"
#include "Packet_Request_Remove_PvP_Buddy.h"

// ORIG T 0x0846c5c8：基类构造 + string(ip)；m_tcpPort/m_udpPort 在函数体赋值
//（报告汇编：string 构造后先写 +0x30e00 再清 +0x30df8）。
#include "CCommunityServerProxy/CCommunityServerProxy.inc"
    : CNetwork<100000, 100000>(), m_ip(ip)
{
    m_tcpPort = port;
    m_udpPort = (ENUM_SOCKET_STATE)0;   // +0x30df8 实为 socket 状态（GDataC6 enum 化），初值 disconnected
}

// ORIG T 0x0846c660：纯转发 InitTcp。
#include "CCommunityServerProxy/Init.inc"
{
    InitTcp();
}

// ORIG T 0x0846c674：ip/port 有效且 socket 未连时 CreateConnectionSocket；
// 失败 cMyTrace 报错返 0（P15 赋值即条件 + P37 临时链式 trace）。
#include "CCommunityServerProxy/InitTcp.inc"
{
    // b28d-40: N80 封顶——P226 语句表达式已试，strerror/errno/ret 升格 callee-saved
    // 的求值序与保活寄存器仍差 2 条；帧 0x4c vs 0x40。ORIG 0x0846c674。
    // [N5C_EventChar 2026-09-11 复核] 维持封顶。新证据（镜像探针
    // /tmp/r5/n5/N5C_EventChar/probe6.cpp 逐条复现本函数：帧 0x40/push esi,ebx/寄存器
    // 分配与本 .o 全等）：
    //   ① `({ ret; })` 裸语句表达式作第 3 实参（probeB）与基线**同形**（P226 对槽变量无效）；
    //   ② `*__errno_location()` 显式形（probeC，BaseServerProxy 惯用法）只交换 %esi/%ebx
    //      角色，助记符序列与条数不变；P290 int→unsigned（probe5）两形完全相同；
    //   ③ ORIG 独有 2 条 = `push %edi` + `mov -0x1c(%ebp),%esi`（ret 在 ctor 前升格跨调用
    //      保活）⇒ 4.1 独有寄存器保活族（P332：callee-saved push 数源级不可诱导）；
    //   ④ 帧 0x4c 非 4.4 帧量化形（≡4 mod 16，P190 附注）且槽差 = N126 首槽常数（4.1 -0x4
    //      / 4.4 -0xc）⇒ N115+N126+N18a 封顶，勿再试。
    // prett/返回类型轴已核并修正（N5C_EventChar 2026-09-11）：ORIG 串
    // `_ZZN21CCommunityServerProxy7InitTcpEvE19__PRETTY_FUNCTION__` @0x08c662e0 =
    // "bool CCommunityServerProxy::InitTcp()"（P174 串定返回类型）⇒ 声明/定义已由 int 改
    // bool（返回类型不入 mangled，零 ABI 风险；调用点 Init 转发与 GlobalData 重连均丢弃返回值）。
    // 修正后 strall 串多重集差异 1→0（仅余下方 SendTcpPacket 一处真串错）。
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return 0;
    }
    if (m_sock < 0)
    {
        // ORIG: call 后 mov %eax,-0x1c(%ebp) 落槽、cmpl 槽比零（P0 具名局部，
        // 错误块 mov -0x1c(%ebp),%esi 槽重读）——勿用 register 寄存器直判。
        int ret;
        if ((ret = CreateConnectionSocket(m_sock, m_ip.c_str(), m_tcpPort)) != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbf, 0)
                ("CCommunityServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, ({ strerror(errno); }));
            return 0;
        }
        // 残差 N80（patterns 定论负例，实证即本函数）：ORIG 在 cMyTrace ctor call
        // 之前右起求 varargs 并跨 call 保活 callee-saved（__errno_location→strerror
        // 先于 ctor）；4.4 恒 ctor 后求。具名/临时/register 三探针不可翻。
        // 另残差 N82：ORIG 帧 0x4c/ret@-0x1c vs 4.4 帧 0x40/ret@-0xc（槽号平移）。
    }
    return 1;
}

// ORIG T 0x0846c770：连接有效（ip 非空且端口非 0）时 disconnect + 清 udpPort。
#include "CCommunityServerProxy/Disconnect.inc"
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return;
    }
    else
    {
        disconnect();
        m_udpPort = (ENUM_SOCKET_STATE)0;   // 断开置状态 0
    }
}

// ORIG T 0x0846c7c4：连接态(+0x30df8)==2 时经 CNetwork::send_packet 下发；
// 否则 cMyTrace 报 disconnected（字符串 .rodata 直读转写）。
#include "CCommunityServerProxy/SendTcpPacket.inc"
{
    if (m_udpPort != 2)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xd4, 0);
        // [N5C_EventChar 2026-09-11] §6.5 串盲区真错（strall 审计发现）：ORIG .rodata
        // 串无句号，我方多一个 '.' ⇒ 运行期日志文本与 ORIG 不一致且 AE 口径不可见（此处修正）。
        trace("cannot send packet because Community Server is disconnected");
        return;
    }
    send_packet(data, len);
}

// ORIG T 0x0846c82c（报告缺失，objdump 转写）：
// recv_packet()>0 → Parsing 失败则 Disconnect 返 0；<0 → logFormat 后 Disconnect 返 0。
#include "CCommunityServerProxy/OnTcpRecv.inc"
{
    int recvLen = recv_packet();
    if (recvLen > 0)
    {
        if (!Parsing(recvLen))
        {
            Disconnect();
            return 0;
        }
        return 1;
    }
    if (recvLen < 0)
    {
        LogManager::logFormat(1, "DF_CommunityServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0xed,
                              "Community Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
}

// ORIG T 0x0846c8c4：send_packet()<0 → 报错+Disconnect 返 -1；否则返 m_recvOffset。
#include "CCommunityServerProxy/OnTcpSend.inc"
{
    if (send_packet() < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x101, 0)
            ("CommunityServer OnSend() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        Disconnect();
        return -1;
    }
    return m_recvOffset;
}

// ORIG T 0x0846c962：errno!=0 → 报错返 0；否则返 1。
#include "CCommunityServerProxy/OnTcpError.inc"
{
    if (errno != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x10f, 0)
            ("CommunityServer OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}

// ORIG T 0x0846c9ec：转发 Parsing_ServerPacket（返回 bool，OnTcpRecv 取反检验）。
#include "CCommunityServerProxy/Parsing.inc"
{
    return Parsing_ServerPacket(recvLen);
}

// ORIG T 0x0846ca06：Packet_Community_Login 填用户信息 + PvP 好友载荷后下发。
// 字段/调用序逐条对齐报告汇编（m_id←acc_id, server_id←GetServerGroup,
// channel_no←G_CEnvironment()->get_channel_no, m_uid←GetUID,
// charac_no←getCurCharacNo, growType/level/job/sex, name memcpy 0x1d,
// buddyCount←sig[0], buddies memcpy 0x560）。
#include "CCommunityServerProxy/SendLogin.inc"
{
    Packet_Community_Login pkt;
    pkt.m_id = user->get_acc_id();
    pkt.gameUserInfo.user_m_id = user->get_acc_id();
    pkt.gameUserInfo.server_id = user->GetServerGroup();
    pkt.gameUserInfo.channel_no = G_CEnvironment()->get_channel_no();
    pkt.gameUserInfo.m_uid = user->GetUID();
    pkt.gameUserInfo.charac_no = user->getCurCharacNo();
    pkt.gameUserInfo.m_growType = user->getCurCharacGrowType();
    pkt.gameUserInfo.m_level = user->get_charac_level();
    pkt.gameUserInfo.m_job = user->get_charac_job();
    pkt.gameUserInfo.m_sex = user->getSex();
    memcpy(pkt.gameUserInfo.m_name, user->getCurCharacName(), 0x1d);
    pkt.buddyCount = sig->m_buddyCount;
    memcpy(pkt.buddies, sig->m_buddies, 0x560);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}

// ORIG T 0x0846cb44：Packet_Community_Logout(m_id=accId, m_characNo) 下发。
#include "CCommunityServerProxy/SendLogout.inc"
{
    Packet_Community_Logout pkt;
    pkt.m_id = accId;
    pkt.m_characNo = characNo;
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}

// ---- StubS3A_deps 批：Send*PvPBuddy（ORIG 逐指令转写）----
// 通用形态：栈上构造对应包（ctor 预填 header+len+清零），填
// +0x0a accId / +0x0e characNo / +0x12 flag，memcpy(name, nameLen) 到 +0x13，
// SendTcpPacket(pkt, *(ushort*)(pkt+2)=packetSize)。

// ORIG T 0x0846cbbc _ZN21CCommunityServerProxy18SendReqAddPvPBuddyEjjcPci
#include "CCommunityServerProxy/SendReqAddPvPBuddy.inc"
    unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen)
{
    Packet_Request_Add_PvP_Buddy pkt;   // ORIG W ctor 0x0846cf34
    pkt.m_accId = accId;
    pkt.m_characNo = characNo;
    pkt.m_flag = flag;
    memcpy(pkt.m_name, name, nameLen);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}

// ORIG T 0x0846cc22 _ZN21CCommunityServerProxy18SendResAddPvPBuddyEjjcPci
#include "CCommunityServerProxy/SendResAddPvPBuddy.inc"
    unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen)
{
    Packet_Response_Add_PvP_Buddy pkt;   // ORIG W ctor 0x0846cf90
    pkt.m_accId = accId;
    pkt.m_characNo = characNo;
    pkt.m_flag = flag;
    memcpy(pkt.m_name, name, nameLen);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}

// ORIG T 0x0846cc88 _ZN21CCommunityServerProxy18SendRemovePvPBuddyEjjcPci
#include "CCommunityServerProxy/SendRemovePvPBuddy.inc"
    unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen)
{
    Packet_Request_Remove_PvP_Buddy pkt;   // ORIG W ctor 0x0846cfec
    pkt.m_accId = accId;
    pkt.m_characNo = characNo;
    pkt.m_flag = flag;
    memcpy(pkt.m_name, name, nameLen);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}
