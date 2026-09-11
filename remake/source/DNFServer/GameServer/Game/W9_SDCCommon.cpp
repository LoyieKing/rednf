// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W9_SDCCommon/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <vector>
#include "GlobalData.h"           // CDataManager 前置（头要求其后包含）
#include "STStackableScript.h"    // ENUM_MODULE_TYPE
#include "sync_script.h"          // STEquipmentScript 完整类型（头注释要求）
#include "DNFLexWrapper.h"
#include "PvPTypes.h"
#include "DisJoint.h"
#include "StrikerScript.h"
#include "CharacManageScript.h"
#include "CItemUpgrade_Separate.h"
#include "DataManagerInitDepends.h"
#include "STSDCActiveStatusSyncDataCLONG.h"  // CLONG 完整定义（双向 operator=）

extern float ACTIVESTATUS_ETC_PARAMETERS[18][8];  // B 0x093f5a40
extern int ACTIVESTATUS_DURATION[18];             // B 0x093f58a0
extern float ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE;        // D 0x0936e48c
extern float ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER;  // D 0x0936e490
extern int ACTIVESTATUS_MAXLEVEL;                          // D 0x0936e484
// [b28c-L7] P82/P205 具名 8 字节步长视图：ORIG 4.1 对 `POWER[idx*2]` 发单条
// scale-8 disp（`mov POWER(,%eax,8),%eax`），4.4 对 `idx*2` 恒 add-自增+scale-4；
// 以本视图取偶槽即折叠为 scale-8。仅寻址视图，不参与业务写入。
struct SDCPair
{
    float m_lo;   // 偶槽 idx*2
    float m_hi;   // 奇槽 idx*2+1
};

// [b28d-solo-W9_SDCCommon] P291/P235 具名 union 双视角（仅本 TU 视图，全局符号
// 名 ACTIVESTATUS_POWER1/2 不参与 mangling，owner ScriptDepsA.cpp 仍按 float[36]
// 定义/写入，布局逐字节相同）。ORIG 0x0808d3af/0x0808d414 对 POWER[idx*2] 发单条
// scale-8 `mov ACTIVESTATUS_POWER1(,%eax,8),%eax`，而 4.4 对 float[36] 下标 idx*2
// 恒折 `add %eax,%eax; mov (,%eax,4)`（+1 条）。m_pair 视图按 8 字节元素下标即
// 折叠为单条 scale-8；奇槽继续走 m_flat[idx*2+1]，保持 ORIG 的
// `add %eax,%eax; add $1; mov (,%eax,4)` 形（探针 /tmp/sdc_p7.cpp 29/29 全等）。
union SDCActiveStatusPowerView
{
    float m_flat[36];
    SDCPair m_pair[18];
};
extern SDCActiveStatusPowerView ACTIVESTATUS_POWER1;  // B 0x093f5900
extern SDCActiveStatusPowerView ACTIVESTATUS_POWER2;  // B 0x093f59a0

// ORIG 各函数实证为 fistpl 截断（fnstcw/fldcw 设 RC=truncate），非四舍五入；
// 故一律用 static_cast<int>（GCC O0 生成相同截断序列）。

// ORIG T 0x0808d0a6：Send（packed 0x35）→ CLONG（0x44 展开）逐字段；
// +0/+4/+0x14/+0x18 符号扩展（movsbl），+0x8 零扩展（movzbl），余 dword。
STSDCActiveStatusSyncDataCLONG& STSDCActiveStatusSyncDataCLONG::operator=(
    const STSDCActiveStatusSyncDataSend& o)
{
    m_field00 = (int)(char)o.m_field00;
    m_field04 = (int)(char)o.m_field01;
    m_field08 = o.m_field02;
    m_field0c = o.m_baseDamage;
    m_field10 = o.m_durationTick;
    m_field14 = (int)(char)o.m_attackerType;
    m_field18 = (int)(char)o.m_isPvp;
    m_field1c = o.m_field0d;
    m_field28 = o.m_field19;    // ORIG 先拷 dest[0x28]（src[0x19]）再 dest[0x20]/0x24
    m_field20 = o.m_field11;
    m_field24 = o.m_field15;
    m_field2c = o.m_hpPercent;
    m_field30 = o.m_field21;
    m_field34 = o.m_field25;
    m_field38 = o.m_field29;
    m_field3c = o.m_field2d;
    m_field40 = o.m_field31;
    return *this;
}

// ORIG T 0x0808d18c：CLONG → Send 截断回填（packed 字节对齐）。
STSDCActiveStatusSyncDataSend& STSDCActiveStatusSyncDataSend::operator=(
    const STSDCActiveStatusSyncDataCLONG& o)
{
    m_field00 = (unsigned char)o.m_field00;
    m_field01 = (unsigned char)o.m_field04;
    m_field02 = (unsigned char)o.m_field08;
    m_baseDamage = o.m_field0c;
    m_durationTick = o.m_field10;
    m_attackerType = (unsigned char)o.m_field14;
    m_isPvp = (unsigned char)o.m_field18;
    m_field0d = o.m_field1c;
    m_field19 = o.m_field28;
    m_field11 = o.m_field20;
    m_field15 = o.m_field24;
    m_hpPercent = o.m_field2c;
    m_field21 = o.m_field30;
    m_field25 = o.m_field34;
    m_field29 = o.m_field38;
    m_field2d = o.m_field3c;
    m_field31 = o.m_field40;
    return *this;
}

// [b28c-L7] P210 帧继承三分律：本 callee 的**定义**原置于所有 caller 之前，
// 使 4.4 按其帧精确继承 outgoing（caller 帧 0x28），而 ORIG 4.1 恒保守撑帧
// （0x38）⇒ 整组 x87 槽号平移 8（dmg -0x4 vs -0xc、中转 -0x18 vs -0x20、
// cw -0x12/-0x14 vs -0x1a/-0x1c）。探针 /tmp/xprobe4.cpp n1（callee 仅声明）
// 逐条复现 ORIG 全组槽位。故定义下移至本 TU 所有 caller 之后（见文件尾）。
// 声明由 DataManagerInitDepends.h:450 提供。

// [b28d-lane15 R3 迁出，Main 批准] _SDC::isObjectType T 0x080950e8 的**定义**已迁至
// SDCLink.cpp（P210②：定义先于 caller 使 4.4 精确继承 outgoing 帧 sub $0xc）。
// 本 TU 及全仓调用点改持 U 引用，声明见 DataManagerInitDepends.h:447。
namespace _SDC
{

#include "W9_SDCCommon/isSyncActiveStatus.inc"

}  // namespace _SDC

// ============================ SDCActiveStatusSyncCommon ============================

#include "W9_SDCCommon/getActiveStatusPower1.inc"


#include "W9_SDCCommon/getActiveStatusPower2.inc"


#include "W9_SDCCommon/getActiveStatusEtcParameter.inc"


#include "W9_SDCCommon/getUserToUserDamageRate.inc"


#include "W9_SDCCommon/getActiveStatusDurationClient.inc"


#include "W9_SDCCommon/getDuration.inc"


#include "W9_SDCCommon/getActiveStatusDurationServer.inc"


#include "W9_SDCCommon/getBurnDamage.inc"


#include "W9_SDCCommon/getPoisonDamage.inc"


#include "W9_SDCCommon/getDiseaseDamage.inc"


#include "W9_SDCCommon/getBleedingDamageServer.inc"


#include "W9_SDCCommon/getBleedingDamageClient.inc"


#include "W9_SDCCommon/makeEtcParameters.inc"


#include "W9_SDCCommon/makeEtcParameters_1.inc"


#include "W9_SDCCommon/getActiveStatusDamageDurationForSDC.inc"


#include "W9_SDCCommon/SDCActiveStatusSyncServer.inc"


#include "W9_SDCCommon/dtor_SDCActiveStatusSyncServer.inc"


#include "W9_SDCCommon/GetUniformVelocityFloatSDC.inc"

