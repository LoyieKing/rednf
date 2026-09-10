// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TCPThread/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// nsl::TCPThread.cpp
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// [D组还原] point ORIG 的 TCPThread.cpp TU 视图里 PACKET_HEADER 是无 ctor 的 POD：
// loop 中 memset 后直接三个 setter（无 PACKET_HEADERC1Ev 调用），且实参先于 this 入栈
// （movl $imm,0x4(%esp) 在 lea this 之前）。共享头 PacketDesign.h 的空 ctor 会额外
// 生成一次 ctor 调用并反转入栈顺序，故本 TU 局部定义同布局 POD（mangled 调用名一致），
// 并跳过共享头中的定义（NSL_PACKETDESIGN_H_ 守卫）。
#define NSL_PACKETDESIGN_H_

namespace nsl {

#pragma pack(push, 1)
struct PACKET_HEADER
{
    char classification;
    char msg_no;
    unsigned int sLength;
    unsigned int check_sum;
    unsigned int sequence;
    unsigned int ack;

    unsigned int getPacketID()
    {
        return (unsigned int)msg_no;
    }

    void setPacketID(int n)
    {
        msg_no = (char)n;
    }

    int getSize()
    {
        return sLength;
    }

    void setSize(int n)
    {
        sLength = n;
    }

    unsigned int getCategory()
    {
        return (unsigned int)classification;
    }

    void setCategory(int n)
    {
        classification = (char)n;
    }

    void setCheckSum(unsigned int n)
    {
        check_sum = n;
    }

    unsigned int getCheckSum()
    {
        return check_sum;
    }

    void setVariableLength(int n)
    {
        sequence = n;
    }

    bool isVariableLength()
    {
        return sequence != 0;
    }

    operator unsigned int()
    {
        return sLength;
    }

    void _copyData(PACKET_HEADER* p)
    {
        memcpy(_getData(), p->_getData(), p->sLength - sizeof(PACKET_HEADER));
    }

    char* _getData()
    {
        return (char*)this + sizeof(PACKET_HEADER);
    }

    PACKET_HEADER* _getHeader()
    {
        return this;
    }
};

#pragma pack(pop)

} // namespace nsl

#include "TCPThread.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "Reactor.h"
#include "DataPool.h"
#include "TCPUser.h"
#include "TCPSocket.h"
#include "Message.h"
#include "TraceLog.h"
#include "Script.h"
#include "TActiveConnect.h"
#include "ActiveNetClose.h"
#include "TCPSendThread.h"
#include "CommonDataPool.h"
#include "PacketDesign.h"

namespace nsl {

extern char configpath[256];
extern bool is_config_changed;
extern bool is_config_reloaded;

#include "TCPThread/TCPThread.inc"
}

#include "TCPThread/dtor_TCPThread.inc"
}

#include "TCPThread/loop.inc"
}

} // namespace nsl
