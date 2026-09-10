// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GmCmds_B/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// GmCmds_B.cpp — game_master 命令体系还原·批次 B（11 类）。
//
// 对照 docs/class_func_reports/game_master__{CChangeCharacName,CChangeExpertJobCmd,
//   CChangeExpertJobExpCmd,CChangeNPCFavorCmd,CChangeNPCMaxDayCmd,CCharacStatCmd,
//   CChoiceItem,CCirculationStatisticCmd,CClearAvatarInvenCmd,CClearInventoryCmd,
//   CClearRewardTest}/{parse,execute,printClient}.md。
//
// 占位字段说明（本批次禁止编辑 GameMasterCmd.h）：
//   头文件对这些类统一给出占位成员 std::string m_str0（+0x08）。ORIG 布局中
//   CChangeExpertJobCmd/CChangeExpertJobExpCmd/CChangeNPCFavorCmd/
//   CChangeNPCMaxDayCmd/CChoiceItem 的 +0x08（及 NPC 双参数类的 +0x0c）实为
//   int 槽（ctor 不构造、parse 按 atoi 写入），故此处按存储位置 reinterpret
//   访问；对象大小与 ORIG 一致（32 位 COW std::string 为 4 字节）。
//   CChangeCharacName 的 +0x08 是真 std::string（ORIG ctor 调 _ZNSsC1Ev）。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "CDataManager.h"
#include "CItem.h"
#include "SkillSlot.h"
#include "STExpertJobScript.h"
#include "CExpertJob.h"
#include "Inven_Item.h"
#include "CInventory.h"
#include "LogManager.h"
#include "CBattle_Field.h"
#include "CDungeon.h"
#include "CParty.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------
// ORIG T 0x08426590 _ZN22DB_ReqChangeCharacName11makeRequestEiiPKci
// （静态成员；仓库尚无该类声明，按 objdump 符号补 extern 声明）
// ---------------------------------------------------------------------------
class DB_ReqChangeCharacName
{
public:
    static void makeRequest(int uid, int characNo, char const* name, int flag);
};

// ---------------------------------------------------------------------------
// ORIG 0x084a905e：getCurCharacAddInfoR() 的附加信息缓冲在 +0x42/+0x46/+0x48
// 处存放 move/attack/cast speed，ORIG 对三处一律单条 disp 访问
//（`mov 0x42(%eax),%eax` / `movzwl 0x46(%eax),%eax` / `movzwl 0x48(%eax),%eax`）。
// 用 `unsigned char*` + 常量偏移在 4.4 会退化成 `add $0xNN,%eax; mov (%eax),%eax`
// 两步（N107 族）⇒ 按 P328② 用具名 pack(1) 视图恢复 disp 折叠。视图仅表达布局，
// 不发符号、不改任何权威头。
// ---------------------------------------------------------------------------
namespace
{
struct CharacAddInfoStatView
{
    unsigned char m_pad[0x42];
    int           m_moveSpeed;    // +0x42
    unsigned short m_attackSpeed; // +0x46
    unsigned short m_castSpeed;   // +0x48
} __attribute__((packed));
}

namespace game_master
{

// ===========================================================================
// CChangeCharacName — ORIG W ctor 0x082a8418：基类 vptr → 自身 vptr(0x8c81930)，
// string@+0x08 默认构造（_ZNSsC1Ev）。
// ===========================================================================

// ORIG parse 0x084ab524：trim(parser->args)；m_str0 = parser->args（新角色名）。
#include "GmCmds_B/parse.inc"
}

// ORIG execute 0x084ab558。
// b28d-21: NEAR 封顶 — 承 b26 判词：ORIG 0x084ab558 首结果→%ebx/次→%esi，
// 4.4 恒反（N7 callee-saved 角色互换）；实参求值序 R2L 由编译器固定，无源码自由度。
#include "GmCmds_B/execute.inc"
}

// ===========================================================================
// CChangeExpertJobCmd — ORIG W ctor 0x082a816a：仅基类 vptr + 自身 vptr
// (0x8c81b70)；+0x08 为 int 槽【专家职业类型】，不构造。
// ===========================================================================

// ORIG parse 0x084a98c8：trim(args)；*(int*)(this+8) = atoi(args.c_str())。
#include "GmCmds_B/parse_1.inc"
}

// ORIG execute 0x084a9900。
// b28d-21: 已修 P157/P183 — 两次 growtype_skill 调用点各自重取 skillW/curJob
// 并重发 operator[]（删 skillIdx/skillLevel/curJob/skillW 四个具名局部）⇒
// p167 调用点差集 CLEAR（39=39，原 MISSING 6）；ndiff 108→54。余 N82 帧/槽
// （178 vs 182）。ORIG 0x084a9900。
#include "GmCmds_B/execute_1.inc"
}

// ===========================================================================
// CChangeExpertJobExpCmd — ORIG W ctor 0x082a8186：仅 vptr 设置；
// +0x08 为 int 槽【专家职业经验增量】。
// ===========================================================================

// ORIG parse 0x084a9b98：trim(args)；*(int*)(this+8) = atoi(args.c_str())。
#include "GmCmds_B/parse_2.inc"
}

// ORIG execute 0x084a9bd0。
#include "GmCmds_B/execute_2.inc"
}

// ===========================================================================
// CChangeNPCFavorCmd — ORIG W ctor 0x082a831c：仅 vptr；
// +0x08/+0x0c 为双 int 槽【npcId / favor】。
// ===========================================================================

// ORIG parse 0x084aadd8：trim(args)；pos=args.find(" ")；
// *(int*)(this+8)=atoi(substr(0,pos))；*(int*)(this+0xc)=atoi(substr(pos+1))。
#include "GmCmds_B/parse_3.inc"
}

// ORIG execute 0x084aaef2：空实现（ret）。
#include "GmCmds_B/execute_3.inc"
}

// ===========================================================================
// CChangeNPCMaxDayCmd — ORIG W ctor 0x082a8300：仅 vptr；
// +0x08/+0x0c 为双 int 槽【npcId / maxDay】。
// ===========================================================================

// ORIG parse 0x084aacb8：与 CChangeNPCFavorCmd::parse 同构。
#include "GmCmds_B/parse_4.inc"
}

// ORIG execute 0x084aadd2：空实现（ret）。
#include "GmCmds_B/execute_4.inc"
}

// ===========================================================================
// CCharacStatCmd — ORIG W ctor 0x082a8098：仅 vptr（无成员）。
// ===========================================================================

// ORIG execute 0x084a905e：读 AdditionalInfo 移速/攻速/施速，组串回发本人。
// b28d-21: 已修 P214③（255 字节缓冲 `memset` → `char buf[255] = {0}` 复现 ORIG
// 内联「对齐剥离 + rep stos」39 条，ndiff 92→9）；余 N1/槽（ORIG -0x1c add 形 vs
// 重建 disp 折叠）。ORIG 0x084a905e。
#include "GmCmds_B/execute_5.inc"
}

// ===========================================================================
// CChoiceItem — ORIG W ctor 0x082a87c2：仅 vptr；+0x08 为 int 槽【物品索引】。
// ===========================================================================

// ORIG parse 0x084b0a34：trim(args)；*(int*)(this+8) = atoi(args.c_str())。
#include "GmCmds_B/parse_5.inc"
}

// ORIG execute 0x084b0a6e。
#include "GmCmds_B/execute_6.inc"
}

// ===========================================================================
// CCirculationStatisticCmd — ORIG W ctor 0x082a8044：仅 vptr（无成员）。
// ===========================================================================

// ORIG execute 0x084a8ff6：空实现（ret）。
#include "GmCmds_B/execute_7.inc"
}

// ===========================================================================
// CClearInventoryCmd — ORIG W ctor 0x082a8338：仅 vptr（无成员）。
// ===========================================================================

// ORIG execute 0x084aaef8：清空背包槽 [3,0x137] 后 send_itemspace(INVENTORY)。
#include "GmCmds_B/execute_8.inc"
}

// ===========================================================================
// CClearAvatarInvenCmd — ORIG W ctor 0x082a8354：仅 vptr（无成员）。
// ===========================================================================

// ORIG execute 0x084aafb0：逐 avatar 槽 [0,0x68] 删除非空物品并 SendAvatarEvent，
// 末尾 send_itemspace(AVATAR)。
#include "GmCmds_B/execute_9.inc"
}

// ===========================================================================
// CClearRewardTest — ORIG W ctor 0x082a8902：仅 vptr（无成员）。
// ===========================================================================

// ORIG parse 0x084b2a02：空实现（ret）。
#include "GmCmds_B/parse_6.inc"
}


// ORIG execute 0x084b2a08。
// b28d-21: DIFF/N 封顶（语义缺口已定位）— p167 EXTRA 5 项（重建多 cMyTrace/
// 临时对象构造）；残差 N82 帧/槽（597 vs 608）+ N89。ORIG 0x084b2a08。
#include "GmCmds_B/execute_10.inc"
}

// ORIG printClient 0x084b3398
// _ZN11game_master16CClearRewardTest11printClientEP5CUserPKcS4_：0x13e 包封装两段
// 字符串（文件名 + 内容）发本人。
#include "GmCmds_B/printClient.inc"
}

} // namespace game_master