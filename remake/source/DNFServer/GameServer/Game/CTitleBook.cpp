// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CTitleBook/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 称号簿实现（G3-3 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CTitleBook.md（Ghidra C）+ ORIG 反汇编。
//
// b28d-solo-CTitleBook 泳道终判（2026-09-09）：清单 5 件全部 identical_ae
//（check_tu_routed.sh clean 口径，两侧指令数逐一相等；p167 逐符号 CLEAR）：
//   _checkFitSlot 111=111、_checkMovetoInven 59=59、getTitle 389=389、
//   putTitle 335=335、deleteTimeoutItem 175=175。
// TU 三数：identical=85 ae=163 (合计 248) near=0 diff=0（基线 248 + diff1）。
// 本波实质改动两处：
//   ① deleteTimeoutItem：正向包裹 `if (item->m_addInfo != 0) { 体 }` 消 continue
//      ——4.4 对 continue/goto 到循环 latch 恒在 `addl` 前插 1B 对齐 nop，致体尾
//      残留 `jmp .+2; nop`（旧「N121 封顶」判词作废；探针 /tmp/ctb_probe.cpp
//      p1(continue)=177 vs p3(正向包裹)=175）；
//   ② _checkFitSlot/_checkMovetoInven：`&(*it).second` → `&it->second`（ORIG 调
//      ptEv，旧写调 deEv ⇒ p167 MISSING ptEv / EXTRA deEv）。
// 无类型改动（P321 T1–T5 未触发）、无 register asm 绑定（.h 改动见下第二波）。
//
// b28d-solo-CTitleBook 第二波（判据㉓ 符号绑定 + P338，2026-09-09/10）：
//   ① 符号绑定 T→W（ORIG nm -S 实测全 W，我方原 T）：GetInvenTypeFromItemSpace
//      （ORIG 0x080f7845，26 条逐条一致）定义加 inline；stTitleBook::C1/C2
//      （0x0848f62a，64 条）、clear（0x0848f6e4，28 条）、CTitleBook::D0
//      （0x0864342e，11 条）/D1/D2（0x086433fe，16 条）改头内定义（CTitleBook.h
//      类内 inline + <cstring>）。代价：4.4 新增 local 别名 _ZN10CTitleBookD5Ev /
//      _ZN11stTitleBookC5Ev（n 类型，结构性噪音，NXCash 先例已录）。
//   ② P338：补 #include <iostream>——ORIG 本 TU 有 _ZStL8__ioinit 动态初始化 ⇒
//      _GLOBAL__I__ZN10CTitleBookC2Ev @0x0864312f（体 8 条）+ SIAD @0x086430ef
//      （体 16 条）与 ORIG 逐条一致；补前我方无 GI（缺 ioinit，4.4 会塌成 sub $0x8）。
//   ③ GI 键名前后：改前无 _GLOBAL__I_；改后键名 == ORIG 键名（P327：inline/weak
//      定义不参与键名 ⇒ GetInvenTypeFromItemSpace 必须 W 才能让 CTitleBook::C2Ev
//      当选为 TU 首个强符号外部链接实体）。
// 四数：85/163/0/0/39 → 86/163/0/1(N123 SIAD 撞名豁免)/39；vtable 9 槽逐槽、
// jcc 序列、call*/call 目标差集（仅 C1/C2·D1/D2 同址别名）、全局数据引用、
// TITLE_BOOK_MAX 常量 (70,200,120,60) 逐项核对 CLEAN。
//
// b28d-solo-CTitleBook 第三波（Main「jmp/jcc 目标下标」盲区排查，2026-09-10）：
//   真 bug 2 处（AE 归一化把 jcc 目标折成 <T> 掩盖，mnemonic 序列核对查不出）：
//   getTitle/putTitle 的 `if ((itemIdx != 0) && (x.m_addInfo != itemIdx)) return 2;`
//   在 itemIdx==0 时**继续**执行；ORIG（getTitle 0x086426d1 / putTitle 0x086420d9）
//   为 `cmpl $0,itemIdx; je <mov $0x2,%..; jmp 返回>` + `cmp m_addInfo,itemIdx;
//   je <继续>` ⇒ **itemIdx==0 也 return 2**（语义相反）。修为
//   `if ((itemIdx == 0) || (x.m_addInfo != itemIdx)) return 2;`。
//   验证：四数 86/163/0/1(N123)/39 不回退；getTitle 389=389、putTitle 335=335；
//   自写 jumpcheck（比较 (助记符, 函数内目标偏移) 序列）由 2 处 mismatch → 47/49
//   一致（余 2 = D2Ev/C2Ev 同址别名，objdump --disassemble 空输出，P93）。
#include <algorithm>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <iostream>   // P338：ORIG 本 TU 有 __ioinit ⇒ _GLOBAL__I__ZN10CTitleBookC2Ev
                      // @0x0864312f（sub $0x18 体）+ SIAD @0x086430ef；缺则塌成 sub $0x8

#include "CTitleBook.h"
#include "CAchievement.h"
#include "CItemAmplifier.h"
#include "CSystemTime.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号（ORIG 真实符号）
// ============================================================================

// ORIG 0x80f7845（W，mangled _Z25GetInvenTypeFromItemSpace14ENUM_ITEMSPACE）：
// switch 跳表（0x8b3c7b0），返回 INVEN_TYPE：0→1, 1→2, 2→4, 3→0, 7→3, 18→5，
// 其余/越界(>0x12)→6。原 GameStubs.cpp 定义迁移。
// b28d-solo-CTitleBook（判据㉓ 符号绑定）：ORIG 该符号为 **W 弱**（nm -S 实测
// @0x080f7845），我方原为 T 强 ⇒ 定义加 inline 发射 W（NXCash 先例：inline 定义 ⇒
// W 且不参与 GI 键名；__attribute__((weak)) 会使 callee 失帧继承）。本 TU
// getTitle/putTitle 有 ODR 使用点 ⇒ 必发射 out-of-line 副本供各消费 TU 的 U 引用。
inline int GetInvenTypeFromItemSpace(ENUM_ITEMSPACE space)
{
    int result = 6;
    switch ((int)space) {
    case (ENUM_ITEMSPACE)3:   result = 0; break;
    case (ENUM_ITEMSPACE)1:   result = 2; break;
    case (ENUM_ITEMSPACE)0:   result = 1; break;
    case (ENUM_ITEMSPACE)7:   result = 3; break;
    case (ENUM_ITEMSPACE)2:   result = 4; break;
    case (ENUM_ITEMSPACE)0x12: result = 5; break;
    // b28d-163：ORIG 为 0..0x12 全覆盖跳表（未列项与 default 共块）；
    // 补全 case 使 4.4 密度判定选跳表而非 if 链（P163 反向应用）。
    case (ENUM_ITEMSPACE)4:  case (ENUM_ITEMSPACE)5:  case (ENUM_ITEMSPACE)6:
    case (ENUM_ITEMSPACE)8:  case (ENUM_ITEMSPACE)9:  case (ENUM_ITEMSPACE)0xa:
    case (ENUM_ITEMSPACE)0xb: case (ENUM_ITEMSPACE)0xc: case (ENUM_ITEMSPACE)0xd:
    case (ENUM_ITEMSPACE)0xe: case (ENUM_ITEMSPACE)0xf: case (ENUM_ITEMSPACE)0x10:
    case (ENUM_ITEMSPACE)0x11:
    default:  result = 6; break;
    }
    return result;
}
// ---- ORIG 真实符号（自由函数 / 跨类方法，普通 C++ 声明，无 extern "C" 桥） ----
// _checkTimeoutItem（ORIG 0x867cf8d T）定义于 CDataManagerScripts.cpp。
// b28d-solo-CTitleBook（P248 档①）：ORIG 调用点 0x086412ba 为 `call; test %al,%al;
// jne`（bool callee 裸形）；原 int 声明使 4.4 生 `test %eax,%eax`。
#include "CTitleBook/_checkTimeoutItem.inc"
class DeleteInvalidItemScript
{
public:
    bool IsInvalidItem2Delete(int idx);
};

// GameWorld（本地最小视图；IsEnchantRevisionChannel 定义于 GameWorld.cpp:3048，
// 符号 _ZN9GameWorld24IsEnchantRevisionChannelEv）
class GameWorld
{
public:
    bool IsEnchantRevisionChannel();
};
GameWorld* G_GameWorld();
class CExpandEquipslot;

// ---- 最小依赖声明（完整 CUser/CDataManager/CInventory 属其它 TU） ----
class CItem
{
public:
    virtual ~CItem();
    virtual void make_item(Inven_Item& item) const = 0;
    bool is_stackable() const;
};

class CDataManager
{
public:
    CItem* find_item(int idx) const;
};

CDataManager* G_CDataManager();

#include "CTitleBook/IsEquipable.inc"
class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);
    Inven_Item GetInvenSlot(int invenType, int slot) const;   // ORIG _ZNK10CInventory12GetInvenSlotEii
    int tryInsertItemIntoInventory(Inven_Item item) const;
    int tryInsertItemIntoEquipment(Inven_Item item) const;
    int insertItemIntoInventory(Inven_Item item, eItemAddReason reason,
                                bool bLog, bool b);
    int insertItemIntoEquipment(Inven_Item item, eItemAddReason reason,
                                bool bLog, bool b);
    // b28d-solo-CTitleBook（P248 档①）：ORIG putTitle 调用点 0x0864227d 为
    // `call; xor $0x1,%eax; test %al,%al; je`（bool callee 裸形）⇒ 返回 bool。
    bool delete_item(INVEN_TYPE invenType, int slot, int count,
                     eItemDelReason reason, bool bLog);
};



// CUserCharacInfo::getCurCharacInvenW/R 实际返回 CInventory*（头文件占位为
// Inven_Item*，按 ORIG ABI 使用）
class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    int getCurCharacNo() const;
    int get_charac_level() const;
};

class CUser : public CUserCharacInfo
{
public:
    unsigned int GetUID() const;
    unsigned int get_acc_id() const;
    int get_charac_no(int type);
    int get_unique_id() const;
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE eType) const;
    void Send(PacketGuard& packet);
    void send_equip(int slot);
    // RewardItem2DeleteInvalidItem（ORIG 0x086931c4，定义于 CUser.cpp:4080）
    void RewardItem2DeleteInvalidItem(
        const std::string& title,
        const std::vector<std::pair<int, int> >& list);
};


// ORIG _ZL14TITLE_BOOK_MAX @0x8e2e484：TU-local static（P204③）
static const int TITLE_BOOK_MAX[4] = { 0x46, 0xc8, 0x78, 0x3c };

// ---- CDataManager 称号表（+0xa788 stTitleBookInfo，0x58 字节） ----
struct CTitleBookInfoRaw
{
    char m_pad0[0xc];                                            // +0x00
    std::vector<stTitleSection> m_sections;                      // +0x0c
    std::map<int, std::vector<stTitleElement> > m_elements;      // +0x18
    char m_pad34[0x8];                                           // +0x34
    std::vector<stAdjust> m_adjustList;                          // +0x3c
};

// CDataManager 局部代理：m_titleBook@+0xa788（成员链让 GCC 折叠为单 lea）
struct CTitleBookMgrRaw
{
    char m_pad0[0xa788];                                         // +0x00
    CTitleBookInfoRaw m_titleBook;                               // +0xa788
};

// 称号簿 DB 存档（stTitleBook_Save，0x6b3e 字节；ORIG 未对齐写 +0x6b3a → pack(1)）
#pragma pack(push, 1)
struct stTitleBook_Save
{
    char m_data[0x6b3a];   // +0x00
    int m_characNo;        // +0x6b3a
};
#pragma pack(pop)
// ============================================================================
// stTitleBook
// ============================================================================

// stTitleBook::stTitleBook()/clear() 与 CTitleBook::~CTitleBook() 已改头内定义
// （CTitleBook.h，判据㉓ 符号绑定 W；ORIG C1/C2 0x0848f62a、clear 0x0848f6e4、
// D1/D2 0x086433fe、D0 0x0864342e —— nm -S 实测全 W）。

#include "CTitleBook/CTitleBook.inc"
}

#include "CTitleBook/loadData.inc"
}

#include "CTitleBook/getData.inc"
}
#include "CTitleBook/_saveData.inc"
}
#include "CTitleBook/_reset.inc"
}

#include "CTitleBook/_gm_clear.inc"
}

#include "CTitleBook/getTitleSlot.inc"
}

#include "CTitleBook/getTitleSlotR.inc"
}

#include "CTitleBook/findItemLock.inc"
}

#include "CTitleBook/getItemLockList.inc"
}

#include "CTitleBook/sendList.inc"
}

#include "CTitleBook/sendListOtherUser.inc"
}

#include "CTitleBook/isAbleItemLock.inc"
}

#include "CTitleBook/_checkFitSlot.inc"
}

#include "CTitleBook/_getFitSlot.inc"
}

#include "CTitleBook/deleteTimeoutItem.inc"
}

#include "CTitleBook/_isMatchedItem.inc"
}

#include "CTitleBook/checkError.inc"
}

#include "CTitleBook/getTitle.inc"
}

#include "CTitleBook/putTitle.inc"
}

#include "CTitleBook/putTitleOnly.inc"
}

#include "CTitleBook/putTitleOnly_1.inc"
}

// ============================================================================
// 内部辅助
// ============================================================================

#include "CTitleBook/_getTitle.inc"
}

#include "CTitleBook/_getTitleR.inc"
}

#include "CTitleBook/_insertTitle.inc"
}

#include "CTitleBook/_deleteTitle.inc"
}

#include "CTitleBook/_checkInsertTitle.inc"
}

#include "CTitleBook/_checkMovetoInven.inc"
}

#include "CTitleBook/_getOwnerTitleInfo.inc"
}

#include "CTitleBook/_getCategoryItemCount.inc"
}

#include "CTitleBook/_empty.inc"
}

#include "CTitleBook/_sendCategoryData.inc"
}

#include "CTitleBook/_putItemData.inc"
}
