// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBoosterGage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN12CBoosterGage14inc_total_gageEi`（ORIG 0x80dd4b7）。
#include <iostream>

#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <utility>
#include <vector>

#include "CBoosterGage.h"
#include "CSystemTime.h"
#include "Stream.h"

#include "CMailBoxHelper.h"   // WongWork::CMailBoxHelper 权威叶子头（连带真实 Inven_Item）
#include "NpServerXml.h"  // np_server_xml::CServerXml/g_ServerString_
#include "PacketGuard.h"
// ============================================================================
// 本 TU 最小依赖声明（CUser/CInventory/CDataManager/CItem 等属后续批次，
// 只声明本 TU 调用所需的符号形态；与 CInventory.h/GameStubs 的版本待合并）。
// ============================================================================





class CItem
{
public:
    virtual ~CItem();
    virtual void make_item(Inven_Item& item) const = 0;
    virtual int GetItemType() const = 0;
    virtual bool IsAvatarItem() const = 0;
    virtual bool IsCreatureItem() const = 0;
};

namespace WongWork
{
class CAvatarItemMgr
{
public:
    int GetRemainDate(int nAvatarUid, int nTick) const;
};


}

namespace user_creature
{
class CCreatureMgr
{
public:
    void InsertCreatureItem(Inven_Item* pItem, int nSlot, int nA, int nB, int nC);
};
}

class CInventory
{
public:
    int AddAvatarItem(int nItemIdx, int nCount, bool bA, char cB, char cC,
                      const char* pAgency, eAvatarItemAddReason eReason,
                      bool bD, unsigned short nE);
    int insertItemIntoInventory(Inven_Item item, eItemAddReason eReason,
                                bool bLog, bool bParam);
    Inven_Item GetInvenSlot(int nInvenType, int nSlot) const;
    const WongWork::CAvatarItemMgr* GetAvatarItemMgrR() const;
    user_creature::CCreatureMgr* GetCreatureMgrW();
};

class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    int getCurCharacNo() const;
};

class CUser : public CUserCharacInfo
{
public:
    unsigned int GetUID() const;
    unsigned int get_acc_id() const;
    int GetAccountLastPlayTime();
    ENUM_SERVER_GROUP GetServerGroup() const;  // ORIG 无 movsbl（返回 4 字节枚举）
    CBoosterGage* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE eType) const;
    void Send(PacketGuard& packetGuard);
};

class CDataManager
{
public:
    CItem* find_item(int nItemIdx) const;

    char m_pad0[0x8770];
    BoosterGageData m_boosterGage;  // +0x8770
};

extern CDataManager* G_CDataManager();


extern CEnvironment* G_CEnvironment();
#include "CBoosterGage/CheckDailyScheduleTime.inc"

class StreamPool
{
public:
    Stream* Acquire(const char* pFile, int nLine);
};

class CStreamGuard
{
public:
    CStreamGuard(Stream* stream, bool bOwn);
    ~CStreamGuard();

    CStreamGuard* operator->();
    CStreamGuard* operator*();
    Stream& operator<<(int v);

    template <class T> T* GetInBuffer();

    Stream* m_stream;  // +0x00
    bool m_bOwn;       // +0x04
};

template <class T>
#include "CBoosterGage/GetInBuffer.inc"
}

class MsgQueueMgr
{
public:
    enum QUEUE_IDX
    {
        NETWORK_Q = 0,
        DISPATCH_Q = 1,
        DB_Q = 2,
        TIMER_Q = 3,
        LOGDB_Q = 4
    };

    void put(QUEUE_IDX queue_idx, CStreamGuard& guard);
};

namespace GlobalData
{
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
extern CSystemTime s_systemTime_;
}

// CServerXml 类声明与 g_ServerString_ extern 见 NpServerXml.h（跨 TU 唯一声明点）；
// 全局定义（C 名 g_ServerString_）在 ServerCommon/ServerXml.cpp（ORIG B 0x093fa660）。

struct SIG_BOOSTER_GAGE_DATA
{
    unsigned int m_dwAccountNo;  // +0x00
    int m_dwBoosterGage;         // +0x04
};

// ============================================================================
// CBoosterGage
// ============================================================================

#include "CBoosterGage/inc_total_gage.inc"
}

// [GI-ioinit L21] 键名符号须为本 TU 首个强符号（P327）：inc_total_gage 前移，
// D2 定义排其后（ORIG 的 D2 为头内 inline 弱符号，不参与键名）。
#include "CBoosterGage/dtor_CBoosterGage.inc"
}

#include "CBoosterGage/sub_total_gage.inc"
}

#include "CBoosterGage/get_booster_point.inc"
}

// [b28d-solo-CBoosterGage AE] 210=210 AE（check_tu_routed clean；p167 25/25 CLEAR）。
// 修法（N78 突破口）：①`char szPost[0x100] = {0}` 令 4.4 发 rep stos 内联（P214③）；
// ②前置死局部 `char cPad1[1]` 把 szPost 顶到非 4 对齐槽 -0x13d（与 ORIG 同址）⇒
// 4.4 随即发与 4.1 相同的「对齐剥离 + 主 stos + 尾 2/1 字节」慢路（P232 死局部）；
// ③strTitle/strBody 具名变量改全表达式临时链 GetServerString(...).c_str()，复现 ORIG
// 「strncpy 后立即 D1」的块作用域析构序；④pItem 空判改裸早返，复现 `je 尾岛; jmp body`。
// ORIG 0x080dc6d6。
#include "CBoosterGage/make_postar.inc"
}

#include "CBoosterGage/log_booster_gage_bonus_reward.inc"
}

// [b28d-solo-CBoosterGage N82] 118/118 助记符全等（NEAR）、p167 13/13 CLEAR；残余 = 槽序：
// ORIG nSlot@-0x64（函数级标量紧接形参 spill 区）、invenItem sret@-0x5d；
// 4.4 恒「聚合对象优先占低槽、标量分区排布」（N82附）⇒ invenItem@-0x61、nSlot@-0x24，
// 帧两侧同为 0xa0。已试尽无旋钮：声明序交换（4.4 按首次使用序，N98 无效）、
// int[1] 数组化、struct 包裹、内层块收窄 invenItem 作用域、死局部 char[1]
// （仅把 nSlot 推深 4B 且破坏帧）。ORIG 0x080dcbd6。
#include "CBoosterGage/AddAvatarItem.inc"
}

#include "CBoosterGage/InsertCreatureItem.inc"
}

#include "CBoosterGage/insertItemIntoInventory.inc"
}

#include "CBoosterGage/check_max_booster_gage.inc"
}

#include "CBoosterGage/cal_booster_gage.inc"
}

#include "CBoosterGage/send_error_state_packet.inc"
}

#include "CBoosterGage/send_data.inc"
}

#include "CBoosterGage/getData.inc"
}

#include "CBoosterGage/loadData.inc"
}

#include "CBoosterGage/ResetDaily.inc"
}

// [b28d-solo-CBoosterGage N104] 81/80（ORIG 尾含 1 条对齐 nop ⇒ 净 80=80），p167 15/15 CLEAR、
// 槽位集全等（guard@-0x14、pData@-0xc，bRet 走裸 register %ebx 与 ORIG 同）、逐条同构；
// 唯一残差 = EH 段物理位置：ORIG 正常路径 `jmp` 尾岛、landing pad 居中，4.4 恒正常路径
// fall-through 到 guard D1、pad 置尾（N104 无源码旋钮；throw() 会删 ORIG 存在的 pad）。ORIG 0x080dd340。
#include "CBoosterGage/_saveData.inc"
}

#include "CBoosterGage/_reset.inc"
}
