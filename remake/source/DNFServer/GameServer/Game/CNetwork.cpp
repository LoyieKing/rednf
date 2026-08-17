// df_game_r 还原 —— CNetwork<T,R> 显式实例化（ORIG 符号集）。
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
template int CNetwork<4096, 450000>::GetRemainSendSize();
template char* CNetwork<4096, 450000>::GetPeerIP2(char*, int);
template char* CNetwork<4096, 450000>::get_str_ip();
template unsigned char CNetwork<4096, 450000>::get_nat_type();
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
template bool CNetwork<100000, 100000>::Parsing_ServerPacket(int);
template bool CNetwork<100000, 100000>::Parsing_AuctionServerPacket(int);

// ---- CNetwork<6144, 100000>（社区/频道服务）----
template CNetwork<6144, 100000>::CNetwork();
template int CNetwork<6144, 100000>::CreateConnectionSocket(int&, const char*, int);
template void CNetwork<6144, 100000>::disconnect();
template int CNetwork<6144, 100000>::recv_packet();
template bool CNetwork<6144, 100000>::Parsing_ServerPacket(int);
template int CNetwork<6144, 100000>::send_packet();
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
template int CNetwork<95000, 450000>::CreateListenSocket(int&, int, const char*);
template int CNetwork<95000, 450000>::CreateConnectionSocket(int&, const char*, int);
template void CNetwork<95000, 450000>::disconnect();
template void CNetwork<95000, 450000>::send_lock();
template void CNetwork<95000, 450000>::send_unlock();
template bool CNetwork<95000, 450000>::SetNonBlock(int);
