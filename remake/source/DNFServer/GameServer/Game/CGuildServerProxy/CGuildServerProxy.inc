// ORIG 强符号（0x0846d818 T）：头文件仅声明，此处给出非内联定义。
// [B24D判词 N] 语义正确：string ip/udp/tcp 三参 → m_ip=ip + map C1 + CNetwork C1 +
//   CUdpHandler C1。diff=EH 清理 landing pad 我方多分离一 `jmp` 跳转（4.1 vs 4.4），
//   61/60 纯 lowering 表示差异，语义等价。
CGuildServerProxy::CGuildServerProxy(std::string ip, int udpPort, int tcpPort)
    : m_ip(ip), m_udpPort(udpPort), m_guildInfoMap(), m_tcpPort(tcpPort),
      m_tcp(), m_udp()
{
}