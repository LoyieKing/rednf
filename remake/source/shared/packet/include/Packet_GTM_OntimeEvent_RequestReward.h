//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H
#define PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H

#include "PacketHeader.h"

class Packet_GTM_OntimeEvent_RequestReward : public PacketHeader {
public:
    int m_fieldA;  // +0xa（SendRequestOnTimeEventReward 的 param_1；无名称证据，保留占位）
    unsigned int m_fieldB;  // +0xe（param_2；ctor 默认 0xffffffff）
    int m_fieldC;  // +0x12（param_3）

    Packet_GTM_OntimeEvent_RequestReward() : PacketHeader(0x17c3, 0x16),
        m_fieldA(0), m_fieldB(0xffffffffu), m_fieldC(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_GTM_OntimeEvent_RequestReward, 0x16);

// 缺口说明（第三轮穷举 2026-08-16）：发送方 CMonitorServerProxy::SendRequestOnTimeEventReward
// @84711fe 仅把 3 个 uint 依次写入 +a/+e/+12；语料内无任何调用点（grep
// SendRequestOnTimeEventReward|0x17c3|OntimeEvent 均无新发现）；接收端
// Dispatcher_RequestOnTimeEventReward::dispatch_sig@822458c / process@82340c8 均不读 PacketBuf
// （纯触发型，奖励逻辑只依赖 CUser 自身 DB 数据）；monitor OnTimeEventManager 还原源码仅用
// Packet_MTG_OntimeEvent_RewardStart/End 等其它包 ID，无 0x17c3 处理器；guild/statics/community
// 还原源码均无引用，故 3 字段语义无法确定。


#endif //PACKET_GTM_ONTIMEEVENT_REQUESTREWARD_H
