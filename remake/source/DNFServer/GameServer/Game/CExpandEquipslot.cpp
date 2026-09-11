// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CExpandEquipslot/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r 还原 —— CExpandEquipslot（G1-2 扩展装备槽，26 符号）
// 参照 docs/class_func_reports/CExpandEquipslot.md + ORIG 反汇编逐函数核对。
// 布局：charac_expand::CData 基类（pack(1)，vptr+0x0 / m_bIsSave+0x4）+
//   Inven_Item m_equipSlot[12]@+0x05 / m_expandEquipSlot[12]@+0x2e1 /
//   m_expandAvatarSlot[12]@+0x5bd，sizeof = 0x899。
// CUser/CInventory/CItem/CParty 等暂无权威头，仅声明本 TU 调用形态。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN16CExpandEquipslotC2Ev`（ORIG 0x849aa88）。
// [GI 判据 · Main 裁决 2026-09-11，含更正] `_Z41__static_initialization_and_destruction_0ii`
// 的逐 TU DIFF 一律为工具假缺口：ORIG 该符号有 493 个实例，工具按名错配取首个
// （0x0807ee76，属 PacketTypes TU）⇒ 读数不可比，跳过、勿改；本 TU 实例为
// 16 条标准 ioinit 形（GI 键 `_GLOBAL__I__ZN16CExpandEquipslotC2Ev` 体间接目标），本就正确。
#include <iostream>


#include <cstring>
#include <vector>

#include "CExpandEquipslot.h"
#include "CItem.h"
#include "CItemAmplifier.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "Stream.h"
#include "CTitleBook.h"   // item_lock::stItemLockRef 权威定义
#include "PacketGuard.h"



// ---- CUserCharacInfo / CUser（最小声明；CUser 继承于 +0x00）----
class CParty;

class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    char getCurChannelEquipslotSwitch();
    char getCurExpandEquipslotSwitch();
    void setCurChannelEquipslotSwitch(char value);
};

class CUser : public CUserCharacInfo
{
public:
    int get_state();
    unsigned int GetUID() const;
    unsigned short get_unique_id() const;
    int get_charac_no(int type);
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const;
    bool CheckItemLock(int a, int b) const;
    CParty* GetParty();
    void send_equip(int slot);
    void make_basic_info(char* pBuf, char flag);
    void Send(PacketGuard& packet);
};

// ---- CInventory（最小声明；m_pEquipSlot 经字节算术访问 +0x650）----
class CInventory
{
public:
    bool CheckValidSlot(int invenType, int slot) const;
    const Inven_Item* GetInvenRef(int invenType, int slot) const;
    Inven_Item GetInvenSlot(int invenType, int slot) const;
    void GetInvenData(int invenType, void* out, int size) const;
    void SetInvenData(int invenType, void* in, int size);
    void MakeEquipList(void* packet, bool flag, ENUM_USERINFO info,
                       ENUM_EQUIPSLOT slot) const;

    char m_pad1c[0x1c];
    Inven_Item m_invenItem[26];   // +0x1c
    char m_pad64e[0x650 - 0x64e];
    Inven_Item* m_pEquipSlot;  // +0x650
};

#include "CExpandEquipslot/CheckEquipable.inc"
#include "CExpandEquipslot/IsEquipable.inc"

// ---- item_lock（EquipslotSwitch 迭代用，权威定义在 CTitleBook.h）----
class CParty
{
public:
    void Reset_party_overlapped_drop_ratio();
};

class CCharacterView
{
public:
    void enableSaveCharacView();
};

class CDataManager
{
public:
    CItem* find_item(int itemIdx) const;
};

extern CDataManager* G_CDataManager();

class CSecu_ProtectionField
{
public:
    // ORIG _ZN21CSecu_ProtectionField5CheckEP5CUser18SECURITY_PROTCTION（0x08288a02）
    // 为 2 参 thiscall 成员函数（定义于 CSecu_ProtectionField.cpp）。
    int Check(CUser* user, SECURITY_PROTCTION type);
};

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
#include "CExpandEquipslot/GetInBuffer.inc"
{
    return m_stream->GetInBuffer<T>();
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
extern CSecu_ProtectionField* s_pSecuProtectionField;
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
extern CSystemTime s_systemTime_;
}

// ---- 存档结构（0x89c 字节：characNo + 0x24 个 Inven_Item + 2 个 switch）----
#pragma pack(push, 1)
struct SIG_SAVE_EXPAND_CHARAC
{
    // b28d-65 [P211档③]：ORIG 0x0849xxxx store 为 RHS-first+`mov %eax,%edx` 中转
    // ⇒ 字段与 RHS 异型。m_characNo 改 unsigned（RHS get_charac_no 返 int）；
    // m_switch1/2 改 char（与 getCur*EquipslotSwitch 返 char 同型，去 cast 后 LHS-first）。
    unsigned int m_characNo;     // +0x00
    Inven_Item m_items[0x24];    // +0x04
    char m_switch1;              // +0x898
    char m_switch2;              // +0x899
    char m_pad[0x89c - 0x89a];   // +0x89a
};
#pragma pack(pop)

// ============================================================================
// 构造 / 析构
// ============================================================================

#include "CExpandEquipslot/CExpandEquipslot.inc"


#include "CExpandEquipslot/dtor_CExpandEquipslot.inc"


// ============================================================================
// 简单访问器
// ============================================================================

#include "CExpandEquipslot/getExpandEquipslotR.inc"


#include "CExpandEquipslot/getExpandEquipslotW.inc"


#include "CExpandEquipslot/GetInvenRefR.inc"


#include "CExpandEquipslot/GetInvenRefW.inc"


#include "CExpandEquipslot/GetInvenSlot.inc"


#include "CExpandEquipslot/FindItemLock.inc"


// ============================================================================
// 数据 / 存档
// ============================================================================

#include "CExpandEquipslot/_reset.inc"


#include "CExpandEquipslot/getData.inc"


#include "CExpandEquipslot/loadData.inc"


#include "CExpandEquipslot/_saveData.inc"


#include "CExpandEquipslot/SetExpandEquipslot.inc"


// ============================================================================
// 发包
// ============================================================================

#include "CExpandEquipslot/Send_Equip_Info.inc"


#include "CExpandEquipslot/Send_Expand_Equip_Info.inc"


// ============================================================================
// 装备切换 / 移动
// ============================================================================

#include "CExpandEquipslot/EquipslotSwitch.inc"


#include "CExpandEquipslot/Move_Item.inc"


#include "CExpandEquipslot/Change_Equip.inc"

