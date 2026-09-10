// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSharedServerMessageManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r CSharedServerMessageManager（G2-5 批次，共享服务器公告管理）
// 逐函数对照 docs/class_func_reports/CSharedServerMessageManager.md 与
// CSharedServerMessageManager__CSharedServerMessageAdapter.md + ORIG 反汇编实现。
// 布局（ORIG ctor 0x0860079c / dtor 0x08600892 推导）：
//   +0x00 CSharedServerMessageAdapter m_adapter（4：stSharedServerMessageInfo*）
//   +0x04 bool m_isSet（ORIG 读取形 `movzbl; xor $0x1,%eax; test %al,%al` 证实按 bool 物化）
//   +0x08 EVENT_SERVER_MESSAGE m_event（unsigned char + vector，0x10）
//   sizeof = 0x18
// stSharedServerMessageInfo（Adapter 解引用对象）：
//   +0x00 vector<int> m_vecIndex | +0x0c vector<pair<int,int>> m_vecMapped
//   +0x18 vector<pair<int,int>> m_vecShareKind
// stSERVER_MESSAGE = 0x58（kind/index/len/msg[52]/len2/msg2[20]/endTime）。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN27CSharedServerMessageManagerC2Ev`（ORIG 0x86019b4）。
#include <iostream>

#include "CSharedServerMessageManager.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"

#include <cstring>
#include <string.h>

#include <utility>
#include <vector>

// ---- 本地消息结构（布局按 ORIG 反汇编） ----



struct stSharedServerMessageInfo
{
    std::vector<int> m_vecIndex;                     // +0x00
    std::vector<std::pair<int, int> > m_vecMapped;   // +0x0c
    std::vector<std::pair<int, int> > m_vecShareKind; // +0x18
};

struct SIG_SELECT_SERVER_MESSAGE  // 4 字节
{
    short m_server;    // +0x00
    short m_channel;   // +0x02
};

struct SIG_UPDATE_SERVER_MESSAGE  // 0x60
{
    int m_server;          // +0x00
    int m_channel;         // +0x04
    unsigned char m_kind;  // +0x08
    unsigned char m_index; // +0x09
    // [b28d-S112] ORIG 0x0860136f/0x0860138e 对 `pOut->m_lenMsg = pCur->m_lenMsg` 发单段
    // 直存 `mov 0x4(%eax),%edx; mov %edx,0xc(%eax)`（无 eax→edx 中转）⇒ 与 stSERVER_MESSAGE
    // 的 size_t 同型（P265/P254）；偏移/尺寸不变。
    unsigned int m_lenMsg; // +0x0c
    char m_msg[0x34];      // +0x10
    unsigned int m_lenMsg2; // +0x44
    char m_msg2[0x14];     // +0x48
    int m_endTime;         // +0x5c
};

// ---- Stream / CStreamGuard（S1 组；本 TU 只声明 + 实例化模板） ----
class Stream
{
public:
    int size();
    int resize(int n);
    bool enable_in(int n);
    void in_ptr(int n);

    template <class T> T* GetInBuffer();

    void* m_vptr;    // +0x00
    int m_size;      // +0x04
    char* m_buffer;  // +0x08
    int m_inPtr;     // +0x0c
    int m_outPtr;    // +0x10
};

template <class T>
#include "CSharedServerMessageManager/GetInBuffer.inc"
}

class CStreamGuard
{
public:
    CStreamGuard(Stream* pStream, bool bOwn);
    ~CStreamGuard();
    CStreamGuard* operator->();
    CStreamGuard* operator*();
    Stream& operator<<(int v);

    template <class T> T* GetInBuffer();

    Stream* m_stream;  // +0x00
    bool m_bOwn;       // +0x04
};

template <class T>
#include "CSharedServerMessageManager/GetInBuffer_1.inc"
}

class StreamPool
{
public:
    Stream* Acquire(const char* file, int line);
};

class MsgQueueMgr
{
public:
    enum QUEUE_IDX
    {
        NETWORK_Q = 0,
        DISPATCH_Q = 1,
        DB_Q = 2
    };

    void put(QUEUE_IDX queue_idx, CStreamGuard& guard);
};

namespace GlobalData
{
// b28c-L20: 权威头 GlobalData.h 声明为指针（StreamPool* @0940bd6c / MsgQueueMgr* @0940bd68）；
// ORIG 汇编 LoadServerMessageFromDB +0x08 `mov _ZN11GlobalData13s_stream_poolE,%eax`、
// +0xb1 `mov _ZN11GlobalData10s_msgq_mgrE,%eax` 均为 32 位**值装载**作 this（非取址立即数）
// ⇒ 本 TU 原 object 形声明与 ORIG 矛盾，按权威头改为指针（§4.4 统一声明点）。
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
}

// ---- CUser 族最小声明（实现属其它 TU；基类偏移 0 保证 this 直通） ----
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1
};

enum eItemDelReason
{
    eItemDelReason_3 = 3,
    eItemDelReason_0x1d = 0x1d
};

enum ENUM_ITEMSPACE
{
    ENUM_ITEMSPACE_INVEN = 0
};

class PacketGuard;
class InterfacePacketBuf;

class CInventory
{
public:
    int check_item_exist(int itemId) const;
    bool delete_item(INVEN_TYPE type, int slot, int count,
                     eItemDelReason reason, bool bFlag);
};

class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenR() const;
    CInventory* getCurCharacInvenW();
};

class CUser : public CUserCharacInfo
{
public:
    enum eSendTarget
    {
        eSendTarget_1 = 1
    };

    void Send(PacketGuard& pg);
    unsigned int GetUID() const;
    bool SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot);
};

class CEnvironment
{
public:
    int get_channel_no();

    char m_pad[0x378];
    int m_field378;   // +0x378
};

CEnvironment* G_CEnvironment();

// 权威定义见对应头文件。

// 权威定义见对应头文件。

// PacketGuard 权威定义见 PacketGuard.h。

// CSharedServerMessageManager 类声明唯一化至 CSharedServerMessageManager.h。

// ----------------------------------------------------------------------------
// 构造（[GI 调序 P327] 无参 ctor 定义前移为本 TU 首个强符号 ⇒
// `_GLOBAL__I__ZN27CSharedServerMessageManagerC2Ev`，ORIG 0x086019b4；
// 只移函数定义，不动任何带动态初始化的对象）
// ----------------------------------------------------------------------------
#include "CSharedServerMessageManager/CSharedServerMessageManager.inc"
}

// ----------------------------------------------------------------------------
// Adapter：索引/映射表查询
// ----------------------------------------------------------------------------
#include "CSharedServerMessageManager/GetMappedEventID.inc"
}

#include "CSharedServerMessageManager/IsOnIndex.inc"
}

#include "CSharedServerMessageManager/GetShareKindWithIndex.inc"
}

// ----------------------------------------------------------------------------
// 构造 / 析构
// ----------------------------------------------------------------------------
#include "CSharedServerMessageManager/CSharedServerMessageManager_1.inc"
}

#include "CSharedServerMessageManager/dtor_CSharedServerMessageManager.inc"
}

// ----------------------------------------------------------------------------
// 公告收发
// ----------------------------------------------------------------------------
#include "CSharedServerMessageManager/SendServerMessageInfo.inc"
}

#include "CSharedServerMessageManager/SetServerMessage.inc"
}

#include "CSharedServerMessageManager/getCurMessage.inc"
}

#include "CSharedServerMessageManager/IsChangableMessage.inc"
}

#include "CSharedServerMessageManager/DeleteSpecificMessage.inc"
}

#include "CSharedServerMessageManager/CheckUserCondition.inc"
}

// ----------------------------------------------------------------------------
// 公告更新（含物品条件校验 + 广播到 DB 队列）
// ----------------------------------------------------------------------------
#include "CSharedServerMessageManager/UpdateServerMessage.inc"
}

#include "CSharedServerMessageManager/UpdateServerMessageByBroadcast.inc"
}

#include "CSharedServerMessageManager/LoadServerMessageFromDB.inc"
}
