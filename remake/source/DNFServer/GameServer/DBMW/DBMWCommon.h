#ifndef DBMW_COMMON_H_
#define DBMW_COMMON_H_

#include <stddef.h>

// 成员视图：把「基址 + 常量偏移」的原始字节访问转成带成员的结构访问，
// 使 -O0 下把偏移折叠进寻址（ORIG 为 mov 0xa(%eax) 形态，纯指针运算会 add/lea）。
// FieldView 用于自然对齐偏移（成员可绑定引用）；FieldViewP 用于未对齐偏移
// （packed，仅按值读写；若需传引用则在调用点保持原始指针运算形态）。
template<int OFF, class T>
struct FieldView
{
    char pad[OFF];
    T v;
};

template<int OFF, class T>
struct FieldViewP
{
    char pad[OFF];
    T v;
} __attribute__((packed));

#include "DBMWTypes.h"

#include "Method.h"
#include "SHA.h"
#include "TEA.h"
#include "SystemTimeHandler.h"
#include "StackBuffer.h"
#include "DNFPacket.h"
#include "DNFTableBase.h"
#include "DNFProhibitUser.h"
#include "DNFPacketBuffer.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFTerminateSig.h"
#include "DNFTcpAcceptThread.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFServerConfig.h"
#include "version.h"
#include "DNFKillUserConfig.h"
#include "ServerLoadChecker.h"
#include "DNFUdpHandler.h"
#include "DNFTcpHandler.h"
#include "EpollHandler.h"
#include "DNFTcpSocket.h"
#include "TcpNetSystem.h"
#include "DNFNetworkThread.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpServerInterface.h"
#include "DNFGameServer.h"
#include "DNFGuildServer.h"
#include "DNFMonitorServer.h"
#include "DNFStatisticsServer.h"
#include "DNFServerHandler.h"
#include "DNFMySql.h"
#include "DBManager.h"
#include "GuildManager.h"
#include "GMAccounts.h"
#include "QueryCounter.h"
#include "PacketCounter.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFUserManager.h"
#include "DNFSignal.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFSystemFailSig.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFUser1Sig.h"
#include "DNFSignalTranslator.h"
#include "ServerXml.h"
#include "DNFApplication.h"
#include "Peer.h"

#endif
