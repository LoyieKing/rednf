#ifndef GAME_AUCTIONPROXY_H_
#define GAME_AUCTIONPROXY_H_

// ============================================================================
// df_game_r 还原 —— 拍卖域代理（CAuction/CCeraAuction）。
// 布局与 CAuction.cpp 内的权威类定义完全一致（BaseServerProxy 派生，
// m_running @ +0x30e0c）。本头供跨 TU（如 CUser.cpp 的 AuctionPreCheck）
// 在不触碰 CAuction.cpp 的前提下调用 IsRunning()/SetRunning()。
// 实现（ctor/vtable/Dispatch）全部定义于 CAuction.cpp。
// ============================================================================

#include "BaseServerProxy.h"
#include "CNetwork.h"
#include "PacketDesign.h"

class CAuctionServerProxy : public BaseServerProxy
{
public:
    CAuctionServerProxy(char* ip, int port, char* name);
    virtual ~CAuctionServerProxy();

    virtual void OnDisconnect();
    virtual bool Parsing(int recvLen);

    void Init();
    void SendPacket(nsl::PACKET_HEADER* packet);
    void SendRegistPacket();
    void SendCheckAuctionReadyPacket();
    bool IsRunning();
    void SetRunning(bool flag);

    char m_running;   // +0x30e0c
};

class CCeraAuctionServerProxy : public BaseServerProxy
{
public:
    CCeraAuctionServerProxy(char* ip, int port, char* name);
    virtual ~CCeraAuctionServerProxy();

    virtual void OnDisconnect();
    virtual bool Parsing(int recvLen);

    void Init();
    void SendPacket(nsl::PACKET_HEADER* packet);
    void SendRegistPacket();
    void SendCheckCeraAuctionReadyPacket();
    bool IsRunning();
    void SetRunning(bool flag);

    char m_running;   // +0x30e0c
};

#endif  // GAME_AUCTIONPROXY_H_
