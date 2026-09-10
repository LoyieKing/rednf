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

// [b28c-34] ORIG T 0x0808d074：type<0xc 且 (1<<type & 0xac4)!=0（位 2/6/7/9/11）。
// 推断 ORIG 原码为 switch 位掩码族：4.4 对 if+短路形把 (1u<<type)&0xac4 前端折叠成
// (0xac4>>type)&1（N6 位折叠：shr/and $0x1），且无符号比较先载寄存器（P209），
// 共 21 条；改写 switch(type){case 2,6,7,9,11:return 1;} 后 4.4 对 switch 生成
// 范围检查+位测试，逐条复现 ORIG 的 `cmpl $0xb,0x8(%ebp); ja` +
// `mov $0x1,%edx; mov %edx,%ebx; mov %eax,%ecx; shl %cl,%ebx; and $0xac4`
// 全形（探针 /tmp/sdc_probe1.cpp d1_switch，20 条全等）。
#include "W9_SDCCommon/isSyncActiveStatus.inc"
{
    switch (type)
    {
    case 2:
    case 6:
    case 7:
    case 9:
    case 11:
        return 1;
    }
    return 0;
}
}  // namespace _SDC

// ============================ SDCActiveStatusSyncCommon ============================

// [b28d-solo-W9_SDCCommon] ORIG 0x0808d37a：P291/P235 union 双视角后偶槽单条
// scale-8、奇槽 scale-4、死槽 -0x14/-0x10、返回两段物化（fstps -0xc → 返回值
// 中转 -0x1c → flds）全等（探针 /tmp/sdc_p8.cpp p1 29/29）。
#include "W9_SDCCommon/getActiveStatusPower1.inc"
{
    // P31/P184：ORIG entry 双死 init 槽 -0x14=0/-0x10=1（照抄）；
    // float 具名 v -0xc + return v 的返回值中转槽 -0x1c（P0 双槽）。
    int deadA = 0;
    int deadB = 1;
    (void)deadA; (void)deadB;
    float v = GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER1.m_pair[idx].m_lo,
                                         ACTIVESTATUS_POWER1.m_flat[idx * 2 + 1],
                                         level - 1, ACTIVESTATUS_MAXLEVEL - 1);
    return v;
}

// [b28d-solo-W9_SDCCommon] ORIG 0x0808d3dc：union 双视角 + 双死 init 槽
// -0x10/-0xc + **直接 return 调用表达式**（无具名 v）⇒ 4.4 返回值中转槽 -0x1c
// 自拷贝形（fstps -0x1c; mov -0x1c,%eax; mov %eax,-0x1c; flds -0x1c），与 ORIG
// 30/30 全等（探针 /tmp/sdc_p8.cpp p6；注意 Power1 是具名 v 的 p1 形，两函数
// 原码结构不同）。
#include "W9_SDCCommon/getActiveStatusPower2.inc"
{
    int deadA = 0;
    int deadB = 1;
    (void)deadA; (void)deadB;
    return GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER2.m_pair[idx].m_lo,
                                      ACTIVESTATUS_POWER2.m_flat[idx * 2 + 1],
                                      level - 1, ACTIVESTATUS_MAXLEVEL - 1);
}

#include "W9_SDCCommon/getActiveStatusEtcParameter.inc"
                                                             int j)
{
    // ORIG 0x0808d446/0x0808d44d：entry 双 dead init 槽 -0x14=0/-0x10=1（P31）；
    // 返回两段物化（fstps -0xc → mov %eax,-0x1c → flds -0x1c，P0 双具名局部）。
    // [N82 封顶] 槽/指令已全对齐；唯一残差帧 0x30(ORIG 16B 对齐进位) vs 0x24(4.4
    // 紧凑)，P190 double pad 实测超调 0x38，封顶。
    int lo = 0;
    int hi = 1;
    (void)lo; (void)hi;
    // ORIG 0x0808d43e：结果先落具名 r 槽 -0xc，再经 eax 拷独立返回槽 -0x1c（P0 双局部）。
    float r = GetUniformVelocityFloatSDC(
        ACTIVESTATUS_ETC_PARAMETERS[idx][j * 2],
        ACTIVESTATUS_ETC_PARAMETERS[idx][j * 2 + 1],
        level - 1, ACTIVESTATUS_MAXLEVEL - 1);
    return r;
}

#include "W9_SDCCommon/getUserToUserDamageRate.inc"
{
    if (deathTower)
        return ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER;
    return ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE;
}

#include "W9_SDCCommon/getActiveStatusDurationClient.inc"
    int, int, IRDActiveObject*)
{
    return 0;
}

#include "W9_SDCCommon/getDuration.inc"
                                           int level, void* param)
// 0x0808dc7a：param（DataSend 视图）尾部三 int 作 objectType/a4/hpPenalty；
// ORIG 透传 getActiveStatusDurationServer 返回值。
{
    // [b28c-L7] P58/P82：ORIG 0x0808dc7a 三实参为 `mov 0x31(%eax),%ebx /
    // 0x2d(%eax),%ecx / 0x29(%eax),%edx`——基址 eax 跨三次读保持、单条 disp
    // 折叠 ⇒ 原码是**类型化指针的成员访问**；`p + 0xNN` 指针算术形使 4.4
    // 每次 `add $NN,%eax; mov (%eax)`（+1 条/处）。改用具名 int 字段直访。
    const STSDCActiveStatusSyncDataSend* d =
        static_cast<const STSDCActiveStatusSyncDataSend*>(param);
    return getActiveStatusDurationServer(
        status, level, d->m_field29, d->m_field2d, d->m_field31);
}

// [b28d-solo-W9_SDCCommon] ORIG 0x0808d272：status 的 18 个取值按掩码
// 0x385aa/0x3a55/0x4000 三组**全划分** ⇒ 原码是 switch(status) 的位测试展开
// （P17/P163：范围检查 `cmpl $0x11,0xc(%ebp); ja` + 单次 shl + 掩码链
// `mov %eax,%edx; and $0x385aa`，mask 全程留 %eax/%edx 无槽往返，P0）。
// if-else 链会把 mask 落槽 -0x14 且内存直比退化为先载寄存器（P209）。
#include "W9_SDCCommon/getActiveStatusDurationServer.inc"
    int status, int level, int objectType, int a4, int hpPenalty)
{
    int duration = 0;
    switch (status)
    {
    // P46：叶块物理顺序 = case 书写顺序。ORIG 叶块序为 duration(0x0808d2bd)
    // → power1(0x0808d2d2) → zero(0x0808d30b)，故 duration 组必须写在前面。
    case 0:
    case 2:
    case 4:
    case 6:
    case 9:
    case 11:
    case 12:
    case 13:
        duration = ACTIVESTATUS_DURATION[status] * 1000;
        break;
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 15:
    case 16:
    case 17:
        duration = static_cast<int>(1000.0f * getActiveStatusPower1(status, level));
        break;
    case 14:
        duration = 0;
        break;
    default:
        break;
    }

    if (_SDC::isObjectType(objectType, 0x211))
    {
        float f1 = static_cast<float>(hpPenalty) / 3.0f;
        float x = f1 / static_cast<float>(hpPenalty);
        // ORIG 0x0808d358：fistpl 落具名 int dec 槽 -0xc（非 duration 槽），
        // `mov -0xc,%eax; sub %eax,-0x18` 两段扣减；该具名局部同时把 duration
        // 定到 -0x18、f1/x 到 -0x14/-0x10（探针 /tmp/sdc_p10.cpp c 全等）。
        int dec = static_cast<int>(static_cast<float>(duration) * x);
        duration -= dec;
    }
    if (duration < 0)
        duration = 0;
    return duration;
}

#include "W9_SDCCommon/getBurnDamage.inc"
                                             STSDCActiveStatusSyncDataSend* d,
                                             void*, int t)
{
    float dmg =
        GetUniformVelocityFloatSDC(0.0f, static_cast<float>(d->m_baseDamage),
                                   t, d->m_durationTick);
    // [N82 封顶] fistpl 已复用 int 中转槽（与 ORIG 同构）；残差为整组槽平移 8
    // （dmg -0x4 vs ORIG -0xc、中转 -0x18 vs -0x20、cw -0x12/-0x14 vs -0x1a/-0x1c）
    // + 帧 0x28 vs 0x38；P190 double pad 实测 dmg 落 outgoing 区反劣化，封顶。
    if (0 < d->m_baseDamage)
        flag = false;
    if (d->m_isPvp != 0)
        dmg *= getUserToUserDamageRate(d->m_attackerType == 10);
    return static_cast<int>(dmg);
}

#include "W9_SDCCommon/getPoisonDamage.inc"
                                               void*, int t)
{
    if (!(d->m_hpPercent < 100))
        return 0;

    float dmg = GetUniformVelocityFloatSDC(0.0f,
                                           static_cast<float>(d->m_baseDamage),
                                           t, d->m_durationTick);
    // [N82 封顶] 同 getBurnDamage：槽组整体平移 8 + 帧 0x28 vs 0x38，封顶。
    if (d->m_isPvp != 0)
        dmg *= getUserToUserDamageRate(d->m_attackerType == 10);
    // ORIG 0x0808dcb0：fistpl 截断（fnstcw/fldcw 设 RC=truncate），非四舍五入。
    return static_cast<int>(dmg);
}

// [b28d-28] DIFF | ORIG 0x0808dd5c | N82：4.1 不复用槽（fistpl -0xc vs 4.4
// -0x10；fstps/fmuls 槽 -0x24 vs -0xc），语义等价。
#include "W9_SDCCommon/getDiseaseDamage.inc"
    STSDCActiveStatusSyncDataSend* d, int t)
{
    if (!(d->m_hpPercent < 100))
        return 0;

    // [b28d-solo-W9_SDCCommon] ORIG 0x0808dd5c 槽位复现（探针 /tmp/sdc_p4.cpp
    // vc 55/55 逐条全等）：dmg 具名 int -0xc；(float)(int) 双 cast 把
    // baseDamage 物化到 -0x20；PvP 分支 (float)dmg 作乘法**左**操作数先物化
    // 到临时 -0x24（fildl/fstps 先于 callee call），fistpl 复用 dmg 槽。
    int dmg = static_cast<int>(GetUniformVelocityFloatSDC(
        0.0f, static_cast<float>((int)d->m_baseDamage), t, d->m_durationTick));
    if (d->m_isPvp != 0)
    {
        dmg = static_cast<int>(static_cast<float>(dmg) *
            getUserToUserDamageRate(d->m_attackerType == 10));
    }
    return dmg;
}

// [b28d-28] NEAR | ORIG 0x0808df1a | N82：x87 槽组平移 0xc（raw -0x8 vs ORIG
// -0x14、cw -0x12/-0x14 vs -0x16/-0x18）；P232 撑槽探针实测反劣（帧 0x24）。
#include "W9_SDCCommon/getBleedingDamageServer.inc"
    STSDCActiveStatusSyncDataSend* d)
{
    // [b28d-solo-W9_SDCCommon] ORIG 0x0808df1a：冗余 (int) cast 强制 4.4 把
    // int 中间值物化到 -0x14 槽（mov 0x3(%eax),%eax; mov %eax,-0x14; fildl
    // -0x14），fistpl 复用同槽返回；float 具名 dmg -0x4、x87 控制词
    // -0x16/-0x18、帧 0x18 与 ORIG 全等（探针 /tmp/sdc_p3.cpp v12 23/23）。
    float dmg = static_cast<float>((int)d->m_baseDamage);
    dmg = dmg * 1.5f;
    return static_cast<int>(dmg);
}

#include "W9_SDCCommon/getBleedingDamageClient.inc"
    STSDCActiveStatusSyncDataSend* d, bool, void*)
{
    int tickB;
    int tickA = d->m_durationTick / 500;
    // ORIG 执行序：tickA 计算/存储先行，tickB 常量写入在后；槽 tickA=-0x18 深、
    // tickB=-0x14 ⇒ 4.4 先声明占高地址，tickB 仅声明、赋值延后至 tickA 后。
    tickB = 1;
    int tick = std::max(tickB, tickA);          // ORIG (%esp)=tickB、0x4(%esp)=tickA
    // P0：ORIG float 具名局部槽 -0xc（fdivrp → fstps → flds 两段物化）。
    float dmg = static_cast<float>(d->m_baseDamage) / static_cast<float>(tick);
    return static_cast<int>(dmg);
}

#include "W9_SDCCommon/makeEtcParameters.inc"
    ENUM_ACTIVESTATUS status, int level, void* param, std::vector<int>* out,
    int* outPtr)
// 0x0808d4b4：先回填 outPtr，再转发三参版本（param 作 DataSend 视图）。
{
    // ORIG 0x0808d4b4 entry：out 形参死 spill 命名槽 -0xc（P0：mov 0x18,%eax;
    // mov %eax,-0xc 先于 outPtr 检查）。
    std::vector<int>* pOut = out;
    if (outPtr != 0)
    {
        *outPtr = getDuration(status, level, param);
    }
    // ORIG 0x0808d4b4 无条件转发三参版本（无 out != 0 检查）。
    makeEtcParameters(status, level, *pOut);
}

#include "W9_SDCCommon/makeEtcParameters_1.inc"
                                                  int level,
                                                  std::vector<int>& out)
// 0x0808d50e：jump table（cmp $0x11; ja; mov table(,%eax,4); jmp *%eax）。
// [b28d-solo-W9_SDCCommon] ORIG 每个 push_back 是「(int) 右值实参」形：
// call → 4.4 内建 RC=truncate 序列（fnstcw/fldcw/fistpl，共享 -0x7a/-0x7c/-0x80）
// → 临时对象槽（-0x6c..-0xc 共 25 个，首次使用最深）→ lea + push_back(int&&)。
// p167 差集：MISSING push_back(int&&)×25 / EXTRA push_back(int const&)×25 ⇒ 必须
// 选右值重载（P270/P234）。具名 tv + 左值实参既选错重载，又使 4.4 对 dead float
// 返回值追加 22 条 `fstp %st(0)`（N85 伪残差，非真差）。探针 /tmp/sdc_p11.cpp b
// 与 /tmp/sdc_p12.cpp m 逐条全等（含槽分配方向）。
{
    switch (status)
    {
    case ACTIVE_STATUS_SLOW:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        break;
    case 2:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 4:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 2)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 3)));
        break;
    case 5:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        break;
    case 6:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 7:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 9:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        out.push_back(1);
        out.push_back(3000);
        break;
    case 10:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 11:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    case 12:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        break;
    case 13:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 1)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 2)));
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 3)));
        break;
    case 15:
        out.push_back(1);
        break;
    case ACTIVE_STATUS_17:
        out.push_back(static_cast<int>(getActiveStatusEtcParameter(status, level, 0)));
        break;
    default:
        break;
    }
}

// [b28d-solo-W9_SDCCommon] ORIG 0x0808df82：守卫顺序 outPtr(0x14)→outB(0x18)
// →d(0x1c)（P133 独立守卫，禁合并短路）；params.size()!=0 产
// `test %eax,%eax; setne %al; test %al,%al; je`（P197④：empty() 恒产 xor 反向）；
// 赋值块读 **outPtr**（0x14）而非 outB（原重建误写 *outB，真语义 bug，已修）；
// switch 叶块物理序 = case 书写序（P46）：burn(0x107)→poison(0x13a)→
// bleeding(0x162)→disease(0x17b)。
#include "W9_SDCCommon/getActiveStatusDamageDurationForSDC.inc"
    char attackType, int statusLevel, int* outPtr, int* outB,
    STSDCActiveStatusSyncDataSend* d, void*)
// 0x0808df82：栈上备份 d、经 makeEtcParameters 取 duration / 参数，
// 按 attackType 分派伤害计算并写入 *outB。
{
    if (outPtr == 0 || outB == 0 || d == 0)
        return;

    STSDCActiveStatusSyncDataSend local = *d;      // ORIG 帧 -0x4a..(-0x16) 全拷
    std::vector<int> params;

    makeEtcParameters(static_cast<ENUM_ACTIVESTATUS>(attackType), statusLevel,
                      &local, &params, outPtr);

    if (params.size() != 0)
    {
        // ORIG 0x0808e06b：local.durationTick = *outPtr（0x14），非 *outB。
        local.m_baseDamage = params[0];
        local.m_durationTick = *outPtr;
    }

    switch (attackType)
    {
    case 9:                                        // burn
    {
        bool flag = false;
        *outB = getBurnDamage(flag, &local, 0, local.m_durationTick);
        break;
    }
    case 2:                                        // poison
        *outB = getPoisonDamage(&local, 0, local.m_durationTick);
        break;
    case 11:                                       // bleeding(server)
        *outB = getBleedingDamageServer(&local);
        break;
    case 6:                                        // disease
    case 7:
        *outB = local.m_baseDamage;
        break;
    default:
        break;
    }
}

// ---- SDCActiveStatusSyncServer C1/D1（ORIG W 0x0823487c/0x08234890，仅委托基类）----
// 定义置于本 TU（基类 C1/D1 定义在 CtorProducers_2.cpp）：caller TU 不含 callee
// 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing 帧，保持 ORIG 的 sub $0x18（NL4 实证）。
#include "W9_SDCCommon/SDCActiveStatusSyncServer.inc"
{
    // 仅委托基类 ctor（隐式基类构造，asm call _ZN25SDCActiveStatusSyncCommonC1Ev）。
}

#include "W9_SDCCommon/dtor_SDCActiveStatusSyncServer.inc"
{
    // 仅委托基类 D1（隐式，asm call _ZN25SDCActiveStatusSyncCommonD1Ev）。
}

// _Z26GetUniformVelocityFloatSDCffii @0x088924c1
// [b28c-L7] 定义必须位于所有 caller 之后（P210，见文件头侧注释判词）。
#include "W9_SDCCommon/GetUniformVelocityFloatSDC.inc"
{
    if (cur >= max)
        return v1;
    if (cur <= 0)
        return v0;
    // ORIG 0x088924c1 x87 序：fildl cur 先于 flds v1 —— 乘法交换序复刻
    // （IEEE 乘法 bit-exact 同值）。
    return v0 + (float)cur * (v1 - v0) / (float)max;
}
