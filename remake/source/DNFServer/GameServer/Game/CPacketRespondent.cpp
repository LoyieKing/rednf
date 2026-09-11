// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPacketRespondent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <string.h>

#include "CPacketRespondent.h"

#include "CUser.h"
#include "CUserCharacInfo.h"   // getCurCharacVill
#include "GameWorld.h"         // G_GameWorld/send_all/send_to_area/getUserVillage
#include "Village.h"           // Village::get_gate_area
#include "LogManager.h"        // cMyTrace
#include "CGameManager.h"      // user_creature::CCreature 访问器
#include "CInventory.h"        // user_creature::CCreatureItem

namespace user_creature
{

#include "CPacketRespondent/CPacketRespondent.inc"


#include "CPacketRespondent/dtor_CPacketRespondent.inc"


#include "CPacketRespondent/finalize.inc"


#include "CPacketRespondent/Send.inc"


#include "CPacketRespondent/Broadcast.inc"


#include "CPacketRespondent/SendAll.inc"


#include "CPacketRespondent/IsGateArea.inc"


#include "CPacketRespondent/SendAccordingToPlace.inc"


#include "CPacketRespondent/SetError.inc"


#include "CPacketRespondent/SetOk.inc"


#include "CPacketRespondent/MakeCmdpacketRenameCreature.inc"


#include "CPacketRespondent/MakeNotipacketRenameCreature.inc"


#include "CPacketRespondent/MakeNotipacketDiedCreature.inc"


#include "CPacketRespondent/MakeNotipacketGainExpCreature.inc"
                                                      bool growFlag)
{
    clear();
    put_header(0, 0x66);
    put_byte(level);
    if (growFlag)
    {
        put_byte(1);
        // ORIG 0x83407bd：int 全宽落槽 -0xc 后，movzwl -0xa(%ebp)（同址 u.w[1]）
        // + cwtl + 写回参数槽 0xc ⇒ 推断源为 union 拆半惯用形（P0：shr/独立 16 位槽均形不符）。
        union { int all; unsigned short w[2]; unsigned char b[4]; } u;
        u.all = exp;
        exp = (int)(short)u.w[1];
        put_int(exp);
        put_byte(0xa);
        put_byte((int)u.b[0]);
    }
    else
    {
        put_byte(0);
        put_int(exp);
    }
    finalize(true);
}

#include "CPacketRespondent/MakeNotipacketEvoluteCreature.inc"


#include "CPacketRespondent/MakeNotipacketRevivalCreature.inc"


#include "CPacketRespondent/MakeNotipacketCreatureState.inc"


#include "CPacketRespondent/MakeNotipacketResponseCreature.inc"


#include "CPacketRespondent/MakeNotipacketCreatureItemList.inc"


}  // namespace user_creature
