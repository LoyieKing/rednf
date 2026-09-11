// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch14/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// OnlyOrigBatch14.cpp —— Batch14 自由函数/杂散函数落地（ORIG objdump 逐符号转写）。
// 单 TU 验证：source/toolchain/check_tu_game_orig.sh DNFServer/GameServer/Game/OnlyOrigBatch14.cpp
// 符号来源：/tmp/df_batches/OnlyOrigBatch14.txt（65 个仅 ORIG 符号）。
// 本 TU 承载：crypto 族（ROR/ROL/pi*/decrypt/encrypt/compareSortedKeyArray）、
// Scan*/export*/import* 脚本族、PetScript、杂散小函数。
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include "DNFLexWrapper.h"        // g_dnfLex / ScanType / ScanInt / ScanStr / ScanFloat
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "LogManager.h"
#include "CEnvironment.h"         // SpinLock
#include "NpcScript.h"            // getCharacterJob / STScriptFileList
#include "CMailBoxHelper.h"       // SIG_LETTER_INFO（PrintLetter 形参）
#include "CMap.h"                 // STAICharacterMapArrangeData
#include "CDataManager.h"         // G_CDataManager()（getNeedMaterial(int)）
#include "Quest.h"                // RewardItemInfo
#include "STCharacterScript.h"    // STRDCharacterStatus / CharacterEtcStats
#include "STTileScript.h"
#include "STBreakAwayMemberData.h"
#include "NeedMaterialDataManager.h"
#include "RestrictCategory.h"
#include "CItem.h"
#include "CMission.h"             // CMission（MissionLessThan::operator()）
#include "MissionLessThan.h"
#include "STStackableScript.h"    // STStackablePackageData
#include "SyncScriptDeps.h"       // toTString
#include "CreatureScript.h"       // getActiveStatusType / getDrawLayer
#include "PetScript.h"
#include "DNFFunctionLib.h"       // timeGetTime
#include "EncryptTool.h"          // EncryptTool（G_EncryptTool）
#include "CUser.h"                // CUser（GiveItemReturnUser / DecreaseTradeLimitCountByUser）
#include "CUserCharacInfo.h"      // getCurCharacNo / getCurCharacLastPlayDungeonIndex /
                                  //   getCurCharacR / getCurCharacInvenW
#include "CInventory.h"           // CInventory::GetInvenRef
#include "CItem.h"                // CItem::make_item（已在上面 include，重复无害）
#include "Inven_Item.h"           // Inven_Item / DecreaseTradeLimitCount
#include "CDungeon.h"             // CDungeon::GetDungeonName
#include "GameEnums.h"            // ENUM_PACKETCLASS / ENUM_NOTIPACKET / ENUM_CMDPACKET
#include "W8LT_PassiveObjectScript.h"      // STPassiveObjectScript 及子对象
#include "STPassiveObjectAdditionalParticleInfo.h"
#include "CDeathTowerPlayDataStatistic.h"
#include "CacheCharacterType.h"   // ENUM_INTERNALPACKET（全局枚举，权威定义）

// ---- 跨 TU 依赖声明（定义见各宿主）----
int ScanStrOrInt(bool& isStr, std::string& s, bool& isInt, bool consume);  // DNFLexWrapperHelpers.cpp
std::string getObjectCategoryString(ENUM_CATEGORY cat);  // ScriptDepsA.cpp（ORIG T 0x08a3ca81）
extern char g_petScriptBaseDirectory[0x100];   // OnlyOrigBatch12.cpp（ORIG B 0x094ff3c0）
extern STScriptFileList g_petScriptFileList;   // OnlyOrigBatch12.cpp（ORIG B 0x094ff4c0）
extern std::map<std::string, int> g_mapKeyNameIndexTable;  // STEquipmentScript.cpp（ORIG B 0x0949b348）
extern char g_szPassiveObjectScriptBaseDirectory[0x100];   // InitScriptProducers_4.cpp（ORIG B 0x94ff2a0）
// P304：权威型 STScriptFileList（NpcScript.h:61，+0x00 即 map<int,string>）——
// 旧 map 视图 + reinterpret_cast 令 4.4 物化 `mov $imm,%eax; mov %eax,(%esp)`
// 并挤占 %eax（ORIG 0x8a357a0 为直压 `movl $imm,(%esp)`）。
extern STScriptFileList g_stPassiveObjectScriptFileList;  // InitScriptProducers_4.cpp（ORIG B 0x94ff3a0）
extern int getObjectCategory(std::string name);            // ScriptDepsA.cpp（ORIG T 0x08a3ca1b）
extern bool isValidExtension(const std::string& str, const std::string& ext);  // OnlyOrigBatch12.cpp（ORIG T 0x08a3c67d）

// ---- 本批新增小结构（仅本文件使用）----
struct STUltimateSkillCutScene  // ORIG 无导出成员符号，仅 importUltimateSkillCutScene 空体引用
{
    int m_field0;
};

// ORIG importActiveStatus 0x088c1e68 逐偏移：+0 int、+4 float、+8 int、+0xc byte、
// +0x10 int、+0x14 vector<int>。
struct STActiveStatus
{
    int m_type;             // +0x00
    float m_value4;         // +0x04
    int m_value8;           // +0x08
    bool m_flagC;           // +0x0c
    int m_value10;          // +0x10
    std::vector<int> m_list14;  // +0x14
};

// ============================================================================
// 1) crypto 族（ORIG 全部为 _ZL 局部符号；静态函数自动匹配 _ZL 名）
// ============================================================================

// ORIG _ZL3ROLji @0x80c3dc0 / _ZL3RORji @0x809c42d：rol/ror %cl。
// [b14 复核·N 封顶（rotate lowering 跨版本）] ORIG 为 4.1 rotate lowering：
// `mov 0x8(%ebp),%eax`（value）→
// `mov 0xc(%ebp),%edx; mov %edx,%ecx`（count）→ `ror %cl,%eax` → 写回参数槽
// `mov %eax,0x8(%ebp)` → 重读返回（10 insn）；4.4 对**形参赋值+rotate** 恒把读改写折成
// 内存形 `rorl %cl,0x8(%ebp)`（8 insn，gap 2），对**单表达式 return** 恒发 callee-saved 形
// `push %ebx; ... ror %cl,%ebx`（12 insn，ndiff 10）。探针 /tmp/d66_probe2.cpp r1..r6 +
// /tmp/d66_probe3.cpp r7..r20 共 18 形穷举 + /tmp/b14_ror.cpp|ror2.cpp 再 12 形。
// **b14 改进**：`register` 形参（P231 纯 register，非 asm 绑定）使 4.4 产
// `mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,%ecx; ror %cl,%eax` —— 与 ORIG
// 前 6 条逐条一致；唯一残差 = 4.1 的「写回参数槽 + 重读」2 条（4.4 对 register 形参
// 恒不写回，探针 ROR2/ROR12 复证）。mangled/ABI 零变化（register 不入符号）。
// 属 4.1/4.4 rotate lowering 跨版本差异 ⇒ 采用最接近 ORIG 的 register 形封顶。
// [P1T1 本波复核] 另测引用形（/tmp/p1/probe_small.cpp ROL_ref：`unsigned int& r =
// value;` ⇒ 4.4 产 `lea 0x8(%ebp),%eax` + 引用槽往返 + `rol %edx,%cl`，13 insn，
// 比 register 形更劣）⇒ 无新源形可复现 ORIG 的「写回参数槽 + 重读」，维持封顶。
// [P1T1 本波穷举·续] /tmp/p1/probe_rol2.cpp+probe_rol3.cpp 九形：volatile 重读
// （`*(volatile u32*)&value`）⇒ 13 insn（多 push %ebx，rol 落 %ebx）；`asm("" : "+m")`
// ⇒ 12 insn（同上）；`asm("" : "+a")`（register/非 register）⇒ 7~8 insn（无写回）
// 或非 register 时 `rol [ebp+0x8],%cl` 内存形 + 冗余 `mov [ebp+0x8],%eax`（8 insn，
// 与 ORIG 的寄存器 rol+写回+重读**都**不同）。ORIG 0x080c3dc0 的 10 条形
// （`rol %eax,%cl; mov %eax,0x8(%ebp); mov 0x8(%ebp),%eax`）为 4.1 独有 lowering。
static unsigned int ROL(register unsigned int value, int count)
{
    value = (value << count) | (value >> (32 - count));
    return value;
}

// [b14 复核·N 封顶（rotate lowering 跨版本）] ORIG 0x0809c42d
// 10 insn = `mov value,%eax; mov count,%edx; mov %edx,%ecx; ror %cl,%eax;
// mov %eax,0x8(%ebp); mov 0x8(%ebp),%eax`（寄存器 rotate + 写回参数槽）；
// 4.4.6 恒折内存目标 `rorl %cl,0x8(%ebp)`（8 insn）。探针
// /tmp/b28d_solo14_probe1.cpp r1..r10 十形穷举（赋值形/单表达式 return/
// `return value=expr`/局部中转/掩码/交换操作数）无一复现寄存器形；b14 增测
// register 形参（/tmp/b14_ror.cpp ROR2）⇒ 寄存器 ror 复现，仍缺写回+重读 2 条。
// 与 ROL 同判词。
// [P1T1 本波复核] 同 ROL：引用形亦劣（13 insn）；ORIG 0x0809c42d 的
// `mov %eax,0x8(%ebp); mov 0x8(%ebp),%eax`（写回+重读）在 4.4 恒被折掉 ⇒ 维持封顶。
static unsigned int ROR(register unsigned int value, int count)
{
    value = (value >> count) | (value << (32 - count));
    return value;
}

// ORIG _ZL3pi1Pm @0x80b46c9：buf[1] ^= buf[0]。
static void pi1(unsigned long* buf)
{
    buf[1] ^= buf[0];
}

// ORIG _ZL3pi2PmS_ @0x80b46e5：
// t = buf[1]+key[0]; t = ROL(t,1)+t-1; t = ROL(t,4)^t; buf[0] ^= t。
static void pi2(unsigned long* buf, unsigned long* key)
{
    unsigned int t = (unsigned int)buf[1] + (unsigned int)key[0];
    t = ROL(t, 1) + t - 1;
    t = ROL(t, 4) ^ t;
    buf[0] = (unsigned long)((unsigned int)buf[0] ^ t);
}

// ORIG _ZL3pi3PmS_ @0x80b4741：
// t = buf[0]+key[1]; t = ROL(t,2)+t+1; t = ROL(t,8)^t; t += key[2];
// t = ROL(t,1); t = ROL(t,16) ^ (buf[0] | t); buf[1] ^= t。
static void pi3(unsigned long* buf, unsigned long* key)
{
    unsigned int t = (unsigned int)buf[0] + (unsigned int)key[1];
    t = ROL(t, 2) + t + 1;
    t = ROL(t, 8) ^ t;
    t = t + (unsigned int)key[2];
    // [b28d-solo-OnlyOrigBatch14] 真语义修正：ORIG 0x080b47aa `sub -0x4(%ebp),%eax`
    // ⇒ 原码是 `t = ROL(t,1) - t`（此前漏写 `- t`）。
    t = ROL(t, 1) - t;
    t = ROL(t, 16) ^ ((unsigned int)buf[0] | t);
    buf[1] = (unsigned long)((unsigned int)buf[1] ^ t);
}

// ORIG _ZL3pi4PmS_ @0x80b47e5：
// t = buf[1]+key[3]; t = ROL(t,2)+t+1; buf[0] ^= t。
static void pi4(unsigned long* buf, unsigned long* key)
{
    unsigned int t = (unsigned int)buf[1] + (unsigned int)key[3];
    t = ROL(t, 2) + t + 1;
    buf[0] = (unsigned long)((unsigned int)buf[0] ^ t);
}

// ORIG _ZL7encryptPmiS_ @0x80b4986：i=j=0；pi1/pi2/pi3/pi4 依次执行，
// 每步后 j==n 即返回；一轮后 i^=4。
static void encrypt(unsigned long* buf, int n, unsigned long* key)
{
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x80b498c 形 `movl $0,-0x8(j);
    // mov -0x8,%eax; mov %eax,-0x4(i)` = j 先声明（4.4 先声明占深槽 ⇒
    // j@-0x8/i@-0x4 与 ORIG 一致）+ i 由 j 初始化（同一 mov-mov 拷贝形）。
    int j = 0;
    int i = j;
    for (;;)
    {
        pi1(buf);
        ++j;
        // [b28d-solo-OnlyOrigBatch14] P225：ORIG `cmp n,%eax; sete %al;
        // test %al,%al; jne <return岛>`（4.1 bool 物化）——`if` 直写 4.4 恒
        // 折 `je`；while+return 形复现物化。
        while (j == n)
            return;
        pi2(buf, key + i);
        ++j;
        while (j == n)
            return;
        pi3(buf, key + i);
        ++j;
        while (j == n)
            return;
        pi4(buf, key + i);
        ++j;
        while (j == n)
            return;
        i ^= 4;
    }
}

// ORIG _ZL7decryptPmiS_ @0x80b4a49：i=(n&1)?0:4；j=n；循环：j>3 → 依次
// pi4/pi3/pi2/pi1（各 --j）→ i^=4 → 回条件；j<=3 → 返回。
// [b28d-D66] P17：ORIG 为**跳表 switch**（`cmpl $0x3,-0x8; jle; mov $0x4,%eax;
// jmp; mov $0x0,%eax; cmp $0x4,%eax; ja Lend; mov table(,%eax,4),%eax; jmp *%eax`）
// ⇒ 原码是 `for(;;) switch (j > 3 ? 4 : 0) { case 0: return; case 4..1 降序 fall-through }`，
// 不是 while(j>3)（后者 4.4 产 setg/test/jne 物化，ndiff 37→1 实证）。
// 终态 55 vs 56（gap 1，ndiff 37→1）：跳表形/槽序（j@-0x8 先声明、i@-0x4 后声明，
// P212 全头部「先声明深」）与 ORIG 逐条一致；唯一残差 = 首句三目 `i = (n&1)?0:4`
// 的 4.1 形 `test %eax,%eax; jne L0; mov $4,%eax; jmp; L0: mov $0`（then 后置）vs
// 4.4 `test %al,%al; je L4; mov $0,%eax; jmp; L4: mov $4`（then 前置 + 低字节测试；
// 探针 /tmp/d66_probe8.cpp t1..t6 六形 —— 三目/if-else/!=0/==1/%2 全部同形）⇒
// P47/P78 跨版本块排布 + 测试宽度，封顶。
static void decrypt(unsigned long* buf, int n, unsigned long* key)
{
    // P212：全头部声明 4.1/4.4 同为「先声明深」⇒ ORIG i@-0x4/j@-0x8 要求 j 先声明；
    // i 的初始化先于 j 的赋值（ORIG `mov %eax,-0x4; mov 0xc(%ebp),%eax; mov %eax,-0x8`）。
    int j;
    // [b28d-solo-OnlyOrigBatch14] 首句三目必须写 `((n & 1) == 0) ? 4 : 0`（非 `(n&1)?0:4`）：
    // ORIG 0x80b4a52 形 `and $1,%eax; test %eax,%eax; jne L0; mov $4; jmp; L0: mov $0`
    // 要求 32 位 test + 「4 前置、0 后置」块序；`==0` 逆序写法在 4.4.6 逐条复现
    // （探针 /tmp/b28d_solo14_probe1.cpp d1 vs d6），语义等价（i∈{0,4}）。
    int i = ((n & 1) == 0) ? 4 : 0;
    j = n;
    for (;;)
    {
        // ORIG 跳表（.rodata 0x08b1f414 dump）：case0→0x80b4afb(return)、
        // case1→0x80b4ae2(pi1)、case2→0x80b4ac6(pi2)、case3→0x80b4aaa(pi3)、
        // case4→0x80b4a8e(pi4) ⇒ 原码 `switch (j > 3 ? 4 : 0)` + **降序 case 4→1
        // 逐级 fall-through**（执行序 pi4,pi3,pi2,pi1），case 0 独立 return。
        switch (j > 3 ? 4 : 0)
        {
        case 0:
            return;
        case 4:
            pi4(buf, key + i);
            --j;
        case 3:
            pi3(buf, key + i);
            --j;
        case 2:
            pi2(buf, key + i);
            --j;
        case 1:
            pi1(buf);
            --j;
        }
        i ^= 4;
    }
}

// ORIG _ZL21compareSortedKeyArrayPmS_i @0x8504cc5：逐 4 字节比较。
static int compareSortedKeyArray(unsigned long* a, unsigned long* b, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

// [b14 删除·归属修正] 原此处 SHA512_Transform / SHA384_Transform（+g_sha512K[80]）
// 已移除：ORIG 侧 `_ZL16SHA384_TransformPmS_`@0x080be32c 与
// `_ZL16SHA512_TransformPmS_`@0x080c077c **各仅一份**，调用者 SHA384_Update/
// SHA512_Update（同 TU，ORIG 0x080c0590/0x080c29e0 等直调）与其地址相邻 ⇒
// 属 ChecksumCrypto TU；本 TU 内 0 调用点（objdump -dr 无 R_386_PC32 引用）
// ⇒ 多余副本。删除后 TU 四数 diff 17→15（missing_in_game 189 不变），
// 属主由 SweepChecksumCrypto 在 ChecksumCrypto.cpp 唯一实现。

// ============================================================================
// 2) 杂散小函数
// ============================================================================

#include "OnlyOrigBatch14/InitTileScript.inc"

// ===== g_stMapScriptFileList —— ORIG B 0x094fe3a0 =====
// [b14 归属迁移·Main 批准] ORIG 本 TU static_init（0x089e81d2，经
// _GLOBAL__I__Z14InitTileScriptv 0x089e823b 调用）体在 ioinit C1+atexit 之后
// 紧接 `movl $0x94fe3a0,(%esp); call STScriptFileList::C1(0x088bacf8)` +
// `mov $0x80f1352(D1),%eax; … __cxa_atexit`（本 TU 原体 31 条）⇒ 该全局的
// ORIG 归属 TU 即本文件（原定义在 InitScriptProducers_4.cpp，属 P186/P128
// 归属错位）。定义位置保持在 InitTileScript 之后 ⇒ GI 键名仍为本 TU 首个强符号
// `_Z14InitTileScriptv`（P327），与 ORIG 键名一致。
// 迁移后本 TU static_init（.o 内 0xcb20 GI 调 0xcbxx static_init）= 23 条，与
// ORIG 本 TU 副本（0x089e81d2..0x089e823a，23 条）逐条同形（仅 .bss/.rodata
// 立即数属未解析重定位）。compare_tu 对该 local 符号按**名**索引会取到他 TU 的
// 47 条同名副本（N123 撞名），故仍记 DIFF —— 非实现缺口。
// 验收读数：本 TU identical+ae 1178→1195（+17，STScriptFileList 成员/模板实例
// 随定义落入本 TU）；InitScriptProducers_4 588→596（+8）、diff 12→6、missing
// 154→143（迁移前基线用 HEAD 版复编实测）；符号 nm 核对：g_stMapScriptFileList
// B 0x18（ORIG B 0x18）、_ZN16STScriptFileListD2Ev W 0x13（ORIG W 0x13）等全等。
STScriptFileList g_stMapScriptFileList;

#include "OnlyOrigBatch14/PrintLetter.inc"

#include "OnlyOrigBatch14/SetSockNonb.inc"

#include "OnlyOrigBatch14/IsDamageActiveStatus.inc"

#include "OnlyOrigBatch14/PI_GetType_SC.inc"


// ============================================================================
// 3) 脚本 import/export 族
// ============================================================================

#include "OnlyOrigBatch14/ScanAnyData.inc"

// ORIG T 0x089e6dc4 _Z20ScanChampionFeaturesiR27STAICharacterMapArrangeData：
// count 行 × 0x25；匹配名取自**二级寻址** `NAME[ VALUE[i] & 0xff ]`
// （ORIG 0x089e6df7 `mov 0x8e1c6e0(,%eax,4),%eax; and $0xff,%eax;
// mov 0x93f55a0(,%eax,4),%eax`），命中后 push 的却是 VALUE[i] **未截断原值**
// （ORIG 0x089e6e20 二次载 VALUE[i] → -0x14 → push_back(EOi)）。
// VALUE 表自 ORIG .data 0x08e1c6e0 逐项提取（低字节=名表索引）。
// [PoolB b28f 符号名对齐] 该表在 ORIG 即 TU-local `_ZL21CHAMPION_FEATURE_LIST`
// （@0x08e1c6e0，r 段 0x94；各 TU 各一份同内容副本）⇒ 本地 static 改名对齐
// （原我方发明名 `_ZL22g_championFeatureValue`）。类型/内容/使用点形不变。
static const unsigned int CHAMPION_FEATURE_LIST[0x25] = {
    0x1ff00, 0xff01, 0x2ff02, 0x2ff03, 0x30004, 0x30005, 0x30006, 0x30007,
    0x40008, 0x3fe09, 0x3fe0a, 0x3fe0b, 0x3fe0c, 0x2000d, 0x3000e, 0x3000f,
    0x20010, 0x40011, 0x10012, 0x20013, 0x2fd14, 0x2fd15, 0x2fd16, 0x2fd17,
    0x2fd18, 0x2fd19, 0x2fd1a, 0x3001b, 0x4fd1c, 0x4fd1d, 0x4fd1e, 0x4fd1f,
    0x4fd20, 0x4fd21, 0x4fd22, 0x4fd23, 0x4fd24
};
// [PoolB b28f 跨 TU 合并] 名表 ORIG 为全局 `CHAMPION_FEATURE_NAME` @0x093f55a0
// （D 段、外部链接、元素非 const、0x94 = 37 指针；ORIG 引用点 0x089e6df7 段
// `mov 0x8e1c6e0(,%eax,4),%eax; and $0xff,%eax; mov 0x93f55a0(,%eax,4),%eax`）。
// 唯一定义已存在（STMapScript.cpp `const char* CHAMPION_FEATURE_NAME[37]`）⇒
// 删本 TU 同内容拷贝（原 `_ZL21g_championFeatureName`），改 extern 引用 ⇒ OUR-only −1。
extern const char* CHAMPION_FEATURE_NAME[37];
#include "OnlyOrigBatch14/ScanChampionFeatures.inc"

#include "OnlyOrigBatch14/ScanRewardItems.inc"

#include "OnlyOrigBatch14/exportCharacterGiveSkillScript.inc"

#include "OnlyOrigBatch14/exportCharacterEtcStatsScript.inc"

// ORIG T 0x08a0fb38 _Z27exportCharacterStatusScriptP8_IO_FILEP19STRDCharacterStatus：
// 非零字段输出（值 /10.0f）。
// [PoolB b28f 跨 TU 合并] 原 `static const char* const g_activeResistName[17]`
// （我方发明拷贝表 `_ZL18g_activeResistName`）改为引用 ORIG 唯一表
// `_ZL16ACTIVESTATUS_TAG` @0x093f5480（d 段 0x44 = 17 指针；ORIG 引用点
// 0x08a0fdc2 `mov 0x93f5480(,%eax,4),%eax`）。唯一定义已落 STMonsterScript.cpp
// （外部链接 `const char* ACTIVESTATUS_TAG[17]`，.data 0x44），此处只留声明 ⇒
// OUR-only −1，本 TU 该件（AE 331/331 gap=0）读数不变。
extern const char* ACTIVESTATUS_TAG[17];
#include "OnlyOrigBatch14/exportCharacterStatusScript.inc"

#include "OnlyOrigBatch14/importQuestPieceScript.inc"

#include "OnlyOrigBatch14/importSlangList.inc"

#include "OnlyOrigBatch14/importHiddenNpcListInfo.inc"

#include "OnlyOrigBatch14/importUniqueRewardForAllUsers.inc"

#include "OnlyOrigBatch14/importPetScript.inc"

#include "OnlyOrigBatch14/clear.inc"

#include "OnlyOrigBatch14/importUltimateSkillCutScene.inc"

#include "OnlyOrigBatch14/importActiveStatus.inc"

// ============================================================================
// 4) 成员/类函数
// ============================================================================

#include "OnlyOrigBatch14/Clear.inc"

#include "OnlyOrigBatch14/clear_1.inc"

#include "OnlyOrigBatch14/operator.inc"

#include "OnlyOrigBatch14/getNeedMaterial.inc"

// [SplitC 2026-09-11 batch3 · TU 迁移] enumToString 已迁出至 ServiceRestrictManagerOrig.cpp（ORIG GI 键对齐）。


// （importKeyNameIndexTable 已按 R3 批准迁至 ScriptExtras.cpp —— 判词随行迁走。）

#include "OnlyOrigBatch14/WriteCodeSection.inc"

#include "OnlyOrigBatch14/getActivePropertyTypeToEnumValue.inc"


#include "OnlyOrigBatch14/clear_2.inc"
#include "OnlyOrigBatch14/FAIR_PVP_SKILL_TREE_SKILL_PER_POINT.inc"

// ============================================================================
// 脚本导入族（STPassiveObjectScript / STTileScript / DifficultPenaltyInfo）
// ============================================================================

// ---- DifficultPenaltyInfo（ORIG mangled R20DifficultPenaltyInfo；脚本
//      "[COMMON TRAP EXPLOSION]" 段读 5 个 int，反汇编 0x890a3d6 逐 4 字节写）----
struct DifficultPenaltyInfo
{
    int m_values[5];
};

#include "OnlyOrigBatch14/ImportDifficultPenaltyInfoScript.inc"

#include "OnlyOrigBatch14/ImportTileScript.inc"



// ============================================================================
// ImportPassiveObjectScript —— ORIG 0x08a2c51a（0x92f4 字节巨型 token 分派）
// 反汇编全量转写（df_game_r）。静态 token 映射为 18 处函数局部
// static map（ORIG __lookup/__init_lock/__inited 三件套，各带
// __cxa_guard），查询点内联展开：!__inited 时锁内登记全部键值后
// 以当前 token 重查重派发。
// 各 case 语义见 switch 内注释（[token] → ORIG case 值 → 写哪个字段）。
// ============================================================================

namespace
{

typedef std::map<const char*, int, less_TCHAR> LookupMap;

}  // namespace

#include "OnlyOrigBatch14/ImportPassiveObjectScript.inc"

#include "OnlyOrigBatch14/GetSlicedInterval.inc"

#include "OnlyOrigBatch14/CheckDayScheduleTimeOver.inc"

#include "OnlyOrigBatch14/DecreaseTradeLimitCountByUser.inc"

#include "OnlyOrigBatch14/GiveItemReturnUser.inc"

