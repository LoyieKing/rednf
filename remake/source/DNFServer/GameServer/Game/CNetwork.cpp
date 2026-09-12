// df_game_r 还原 —— CNetwork<T,R> 显式实例化（ORIG 符号集）。
// [R11fixE4 2026-09-12] 6144 实例的 send_packet()/Parsing_ServerPacket(int) 改用
// 非 inline 显式特化（文件末尾）：其 4 个 logFormat 文件名实参在 ORIG 中为
// 'localjapan/../Network.h'，通用模板体单一字面量无法按实例分派。
#include "CNetwork.h"

// ---- CNetwork<4096, 450000>（CUser 客户端网络）----
template CNetwork<4096, 450000>::CNetwork();
template CNetwork<4096, 450000>::~CNetwork();
template int CNetwork<4096, 450000>::get_socket();
template void CNetwork<4096, 450000>::disconnect();
template int CNetwork<4096, 450000>::send_packet(char*, int);
template int CNetwork<4096, 450000>::recv_packet();
template int CNetwork<4096, 450000>::send_packet();
template bool CNetwork<4096, 450000>::SetNonBlock(int);
template void CNetwork<4096, 450000>::send_lock();
template void CNetwork<4096, 450000>::send_unlock();
template char* CNetwork<4096, 450000>::GetPeerIP2(char*, int);
template char* CNetwork<4096, 450000>::get_str_ip();
template char CNetwork<4096, 450000>::get_nat_type();
template void CNetwork<4096, 450000>::set_nat(char, unsigned int, unsigned int,
                                              unsigned short, unsigned int);
template void CNetwork<4096, 450000>::set_nat(char, unsigned int, unsigned int,
                                              unsigned short, const char*);
template char* CNetwork<4096, 450000>::get_mac_addr() const;
template unsigned int CNetwork<4096, 450000>::get_inner_ip();
template unsigned int CNetwork<4096, 450000>::get_ip();
template unsigned short CNetwork<4096, 450000>::get_port();
template int CNetwork<4096, 450000>::get_mtu();
template bool CNetwork<4096, 450000>::set_socket(int, bool);
template void CNetwork<4096, 450000>::initNetworkBuffer();

// ---- CNetwork<100000, 100000>（BaseServerProxy 内嵌）----
template CNetwork<100000, 100000>::CNetwork();
template CNetwork<100000, 100000>::~CNetwork();
template int CNetwork<100000, 100000>::get_socket();
template void CNetwork<100000, 100000>::disconnect();
template int CNetwork<100000, 100000>::send_packet(char*, int);
template int CNetwork<100000, 100000>::recv_packet();
template int CNetwork<100000, 100000>::send_packet();
template bool CNetwork<100000, 100000>::SetNonBlock(int);
template void CNetwork<100000, 100000>::send_lock();
template void CNetwork<100000, 100000>::send_unlock();
template int CNetwork<100000, 100000>::GetRemainSendSize();
template int CNetwork<100000, 100000>::CreateConnectionSocket(int&, const char*, int);
template int CNetwork<100000, 100000>::CreateConnectionBlockSocket(int&, const char*, int);
// [b28d-solo-CNetwork] 两件均 AE（定义在 CNetwork.h）：
//   Parsing_ServerPacket        ORIG 0x0846d050 155=155；真语义 bug×2（守卫负极性
//                               短路 ps<=9||ps>0x1800；dispatch 后 remain 分支归属）
//   Parsing_AuctionServerPacket ORIG 0x082f7ee2 148=148；真语义 bug×2 + header
//                               线格式缓冲视图消空 ctor call（P215）
// p167 6=6 / 8=8 CLEAR；旧「N82 封顶」判词作废。
template bool CNetwork<100000, 100000>::Parsing_ServerPacket(int);
template bool CNetwork<100000, 100000>::Parsing_AuctionServerPacket(int);

// ---- CNetwork<6144, 100000>（社区/频道服务）----
// [R11fixE4 2026-09-12] send_packet() 与 Parsing_ServerPacket(int) 的实例化移到
// 文件末尾的显式特化（同一符号名、同一强符号发射；ORIG 该实例 4 站点见
// CNetwork.h 宏说明）。此处不再对这两个成员写 `template …;` 显式实例化
// （对已特化成员写显式实例化为 ill-formed）。
template CNetwork<6144, 100000>::CNetwork();
template int CNetwork<6144, 100000>::CreateConnectionSocket(int&, const char*, int);
template void CNetwork<6144, 100000>::disconnect();
template int CNetwork<6144, 100000>::recv_packet();
template int CNetwork<6144, 100000>::GetRemainSendSize();
template bool CNetwork<6144, 100000>::SetNonBlock(int);
template void CNetwork<6144, 100000>::send_lock();
template void CNetwork<6144, 100000>::send_unlock();

// ---- CNetwork<95000, 450000>（exchange/监视服务）----
template CNetwork<95000, 450000>::CNetwork();
template CNetwork<95000, 450000>::~CNetwork();
template int CNetwork<95000, 450000>::get_socket();
template int CNetwork<95000, 450000>::recv_packet();
template int CNetwork<95000, 450000>::send_packet();
template bool CNetwork<95000, 450000>::set_socket(int, bool);
template int CNetwork<95000, 450000>::send_packet(char*, int);
template bool CNetwork<95000, 450000>::CreateListenSocket(int&, int, const char*);
template int CNetwork<95000, 450000>::CreateConnectionSocket(int&, const char*, int);
template void CNetwork<95000, 450000>::disconnect();
template void CNetwork<95000, 450000>::send_lock();
template void CNetwork<95000, 450000>::send_unlock();
template bool CNetwork<95000, 450000>::SetNonBlock(int);

// ============================================================================
// [R11fixE4 2026-09-12] CNetwork<6144, 100000> 专用显式特化 —— 仅 logFormat 的
// 文件名实参不同（ORIG 该实例 4 站点 0x818e6a2/0x818e744/0x818e365/0x818e40a
// 加载 VA 0x8b95b10，raw = 6c 6f 63 61 6c 6a 61 70 61 6e 2f 2e 2e 2f 4e 65 74
// 77 6f 72 6b 2e 68 = 'localjapan/../Network.h'；其余三实例 0x8c20bc7 /
// 0x8cf1627 / 0x8c78a68 = 'Network.h'）。
// 通用模板体（CNetwork.h）使用字面量 'Network.h'，无法按实例分派 ⇒ 在本 TU 给
// 出非 inline 显式特化：同名 mangled 强符号在此发射（GNU ld 取强符号，覆盖
// Arad_DevMgr_Server_Proxy.cpp 隐式 weak 副本，无重复定义错误）。
// 特化体与通用模板体**逐字等同**（含注释），唯一差异 = 文件名实参字面量；
// 修改通用体时必须同步本特化体。
// ============================================================================
template <>
__attribute__((weak))
int CNetwork<6144, 100000>::send_packet()
{
    int ret = 0;
    if (m_sendSize == 0)
    {
        return 0;
    }

    send_lock();
    if ((ret = write(m_sock, m_send, m_sendSize)) <= 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ||
            errno == 0 || errno == EINPROGRESS)
        {
            send_unlock();
            return 0;
        }
        // [Fn28Gap 2026-09-11] ORIG 0x0818e5dc: movl $0x8b96710,0x4(%esp) =
        // _ZZN8CNetworkILi6144ELi100000EE11send_packetEvE12__FUNCTION__
        // （r,12B="send_packet\0"）⇒ 原码 __FUNCTION__（旧手写 "send_packet()"
        // 串内容亦与 ORIG 不同）。
        // [R11fixE4 2026-09-12] fmt 按 ORIG raw：0x8c20c7c 'SEND ERROR
        //   DISCONNNECT NOW FD[%d] : %d(%s)'（ORIG 0x82fe459 装载 三 slot 一致）。
        cMyTrace(__FUNCTION__, 0x2af, 0)("SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
            m_sock, errno, strerror(errno));
        send_unlock();
        return -1;
    }
    if (ret > 0)
    {
        if ((int)m_sendSize > ret)
        {
            m_sendBuf = m_send + ret;
            m_sendSize -= ret;
            if ((unsigned int)m_sendSize > (unsigned int)sizeof(m_send))
            {
                // [Fn28Gap 2026-09-11] ORIG 0x0818e69a: 0x8(%esp)=__FUNCTION__。
                // [R11fixE4 2026-09-12] 0x2d1 fmt = 0x8c20ca8 'm_remain_sendlen <
                //   MAX_SEND_BUF :  m_remain_sendlen:%d]'（" : " 后**两个空格**，
                //   raw 逐字节核；单 vararg = m_sendSize，ORIG 0x82fe4e8）。
                LogManager::logFormat(1, "localjapan/../Network.h", __FUNCTION__, 0x2d1,
                    "m_remain_sendlen < MAX_SEND_BUF :  m_remain_sendlen:%d]", m_sendSize);
                return -1;
            }
            memmove(m_send, m_sendBuf, m_sendSize);
            m_sendBuf = m_send + m_sendSize;
        }
        else if ((int)m_sendSize < ret)
        {
            // [Fn28Gap 2026-09-11] ORIG 0x0818e73c: 0x8(%esp)=__FUNCTION__。
            // [R11fixE4 2026-09-12] 0x2db fmt = 0x8c20ce0 'offset error[Remain_Data:
            //   %d Send:%d]'；实参 (m_sendSize, ret)（ORIG 0x82fe58a 两 slot 一致）。
            LogManager::logFormat(1, "localjapan/../Network.h", __FUNCTION__, 0x2db,
                "offset error[Remain_Data: %d Send:%d]", m_sendSize, ret);
            send_unlock();
            return -1;
        }
        else
        {
            m_sendBuf = m_send;
            m_sendSize = 0;
        }
    }
    send_unlock();
    return ret;
}

template <>
__attribute__((weak))
bool CNetwork<6144, 100000>::Parsing_ServerPacket(int recvLen)
{
    // [b28d-solo-CNetwork] AE（ORIG 0x0846d050，155=155 条；p167 6/6 CLEAR）。
    // 修两处真语义 bug：①packetSize 守卫为负极性短路 `ps<=9 || ps>0x1800` 走 log
    // （ORIG 0x0846d153 `jbe <log>` / 0x0846d159 `jbe <dispatch>`；旧嵌套
    // if(ps>9){if(ps>0x1800)} 在 4.4 下把 ps<=9 误送 dispatch）；
    // ②dispatch 后 `>9 continue` 属 if 块内部、0<remain<=9 落块外 break
    // （ORIG 0x0846d204 `jne` / 0x0846d214 `cmpl $0x9;jg`）；旧版块内无条件
    // continue ⇒ 半包回循环。残余仅 .o 未解析重定位（call 目标/rodata 地址）
    // ⇒ AE 口径（P264/N124）。
    PacketHeader header(0, 0);
    int remain = m_recvOffset + recvLen;
    unsigned int packetSize;
    int headerSize = 0xa;
    if (remain <= 9)
    {
        m_recvOffset += recvLen;
        m_recvBuf += recvLen;
        // [Fn28Gap 2026-09-11] ORIG 0x0818e35d: 0x8(%esp) =
        // _ZZN8CNetworkILi6144ELi100000EE20Parsing_ServerPacketEiE12__FUNCTION__
        // （r,21B="Parsing_ServerPacket\0"）⇒ 原码 __FUNCTION__。
        // [R11fixE4 2026-09-12] 0x31d fmt = 0x8c65524 '(offset:%x - buf:%x) =
        //   remainlen:%d, Recv Size[%d] '（尾随一个空格，raw 逐字节核）；四 vararg
        //   序 (m_recvBuf, m_recv, m_recvOffset, recvLen)（ORIG 0x846d0c2..0x846d0d1
        //   逐槽核对：0x14/0x18/0x1c/0x20）。
        LogManager::logFormat(1, "localjapan/../Network.h", __FUNCTION__, 0x31d,
            "(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d] ", m_recvBuf,
            m_recv, m_recvOffset, recvLen);
        return 1;
    }

    while (true)
    {
        if (m_recvOffset != 0)
        {
            m_recvBuf -= m_recvOffset;
        }
        // [SemB12 2026-09-12 真语义差异修复] ORIG 0x0846d11c 的 `je +0xdc` 落点 =
        //   memcpy 段起点（idx61）⇒ offset==0 时**仍执行** memcpy（否则 header 未
        //   初始化即被读）；旧写法把 memcpy 放块内，我方 `je` 落 idx68 跳过它。
        memcpy(&header, m_recvBuf, 0xa);
        packetSize = header.packetSize;
        // [b28d-solo-CNetwork] ORIG 0x0846d153 段实证：`cmpl $0x9; jbe <log>; cmpl
        // $0x1800; jbe <dispatch>` ⇒ 原码是负极性短路 `ps<=9 || ps>0x1800` 走 log；
        // 旧嵌套 if(ps>9){if(ps>0x1800)} 在 4.4 下把 ps<=9 误送 dispatch（真语义 bug）。
        if (packetSize <= 9 || packetSize > 0x1800)
        {
            // [Fn28Gap 2026-09-11] ORIG 0x0818e402: 0x8(%esp)=__FUNCTION__。
            // [R11fixE4 2026-09-12] 0x32d fmt = 0x8c65558 'Recv Size[%d], Parsing
            //   Packet Size[%d], recvbuf_offset[%d] is Too Large'；三 vararg 序
            //   (recvLen, packetSize, m_recvBuf)（ORIG 0x846d162..0x846d176 一致）。
            LogManager::logFormat(1, "localjapan/../Network.h", __FUNCTION__,
                0x32d, "Recv Size[%d], Parsing Packet Size[%d], recvbuf_offset[%d] is Too Large",
                recvLen, packetSize, m_recvBuf);
            m_recvBuf = m_recv;
            m_recvOffset = 0;
            return 0;
        }
        if (remain >= packetSize)
        {
            CDispatchServerPacket::dispatchPacket(m_recvBuf);
            remain -= packetSize;
            m_recvBuf += packetSize;
            m_recvOffset = 0;
            if (remain == 0)
            {
                m_recvBuf = m_recv;
                break;
            }
            // [b28d-solo-CNetwork] ORIG 0x0846d204/0x0846d214 段形：
            //   jne +0x10; <赋值>; jmp exit; +0x10: cmpl $0x9,remain; jg <loop>;
            //   jmp exit; nop; exit:  ⇒ 该 `>9 continue` 属 dispatch 块内部；
            // remain<=9 落到块外 break。旧版把它放在 if 块外且块内无条件
            // continue ⇒ dispatch 后 0<remain<=9 也回循环（真语义 bug）。
            if (remain > 9)
            {
                continue;
            }
        }
        break;
    }

    if (remain > 0)
    {
        if ((unsigned int)remain > (unsigned int)100000)
        {
            return 0;
        }
        memmove(m_recv, m_recvBuf, remain);
        m_recvBuf = m_recv + remain;
        m_recvOffset = remain;
    }
    return 1;
}
