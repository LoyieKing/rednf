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

// ===== _Z14InitTileScriptv —— ORIG 0x089dcbac =====
// mov $1 直返。
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

// ORIG T 0x0841d2ea _Z11PrintLetter15SIG_LETTER_INFO：空体。
#include "OnlyOrigBatch14/PrintLetter.inc"

// ORIG T 0x08104884 _Z11SetSockNonbim：ioctl(fd, FIONBIO=0x5421, &arg) == 0。
// 返回 int（ORIG 尾 sete %al;movzbl %al,%eax 加宽形，bool 返回 4.4 不加宽）。
#include "OnlyOrigBatch14/SetSockNonb.inc"

// ORIG T 0x089bd03b _Z20IsDamageActiveStatusi：
// status<=0xb && ((1<<status) & 0xac4)。ORIG 形：bool 局部汇聚（089bd042
// movb $0,-0x5；两写点 089bd065/089bd06b）+ **无符号边界**（089bd046
// cmpl $0xb,0x8(%ebp) 直比参数槽 + ja，P137/P2）+ 末尾 movzbl 读回。
#include "OnlyOrigBatch14/IsDamageActiveStatus.inc"

// ORIG T 0x0808c74b _Z13PI_GetType_SCcs：type==1 查表、type==2 if 链、缺省 0。
#include "OnlyOrigBatch14/PI_GetType_SC.inc"

// ORIG T 0x0808bc7e _Z6Rand_rPj：LCG rand_r（NULL 时用全局种子）。
// [b28d-solo-OnlyOrigBatch14 复核 lane24 判词·N 封顶] 唯一残差 = NULL 分支
// `staticSeed = timeGetTime() + staticSeed`：ORIG 0x0808bc8f `mov %eax,%edx;
// mov staticSeed,%eax; lea (%edx,%eax,1),%eax`（RHS-first + lea），4.4 恒
// `mov staticSeed,%edx; add %edx,%eax`（LHS-first + add）。P211 负例（static
// 存储期变量复合赋值装载序不可翻，b28c C28L24 三救均败）+ add/lea 跨版本；
// 其余 49 条与 ORIG 全等。
// [P1T1 本波复核] 残差复核 = 仅 NULL 分支 3 条（ORIG 0x0808bc8f `mov %eax,%edx;
// mov staticSeed,%eax; lea (%edx,%eax,1),%eax` vs 我方 `mov staticSeed,%edx;
// add %edx,%eax`）；维持 P211 static 装载序 + add/lea 跨版本封顶。
unsigned int Rand_r(unsigned int* seed)
{
    static unsigned int staticSeed = 0;  // ORIG _ZZ6Rand_rPjE10staticSeed 0x0936e480
    if (seed == 0)
    {
        // [b28c-lane24] ORIG 0x0808bc82：`call timeGetTime; mov %eax,%edx;
        // mov staticSeed,%eax; lea (%edx,%eax,1),%eax` = RHS 值先（P211 符号性
        // 异型 ⇒ RHS-first）；LHS/RHS 同为 unsigned 时 4.4 恒 LHS-first
        //（`mov staticSeed,%edx; add %edx,%eax`）⇒ 交换书写序探测（P211 无效：
        // staticSeed int 化反使取址多一条 eax 中转，已回退）。
        staticSeed = timeGetTime() + staticSeed;
        return Rand_r(&staticSeed);
    }
    unsigned int next = *seed;
    // 拆句 *=/+= 复现 ORIG addl $K,-off 内存加与 shll mem 内存移
    // [b28c-lane24] result 由 unsigned 改 int：ORIG xor 段 `mov %eax,%edx;
    // and $0x3ff,%edx; mov -0xc(%ebp),%eax; xor %edx,%eax; mov %eax,-0xc(%ebp)`
    // = RHS 先物化（P211 异型）；同型时 4.4 恒折 `and %eax; xor %eax,mem`。
    next *= 0x41c64e6d;
    next += 0x3039;
    int result = (next >> 16) & 0x7ff;
    next *= 0x41c64e6d;
    next += 0x3039;
    result <<= 10;
    result = result ^ ((next >> 16) & 0x3ff);
    next *= 0x41c64e6d;
    next += 0x3039;
    result <<= 10;
    result = result ^ ((next >> 16) & 0x3ff);
    *seed = next;
    return result;
}

// ============================================================================
// 3) 脚本 import/export 族
// ============================================================================

// [b14 本波·报告核对] 三处修复：
//  ①槽序：ORIG token@-0x14 / got@-0xd（4.4「后声明更深」）⇒ 声明序改 token/got；
//  ②`token = 7` 位置按 ORIG 0x88bdcb8 提到 findString 之前；
//  ③**真语义 bug**：报告 Ghidra C 0x88bdd77 段 logFormat 变参序为
//    ("ScanAnyData", streamName, lineNo, tokenData)，原写 (streamName,"ScanAnyData")
//    两串互换（日志内容反写）⇒ 已按报告修正。
// ndiff 54→4。残余 1 处（N 封顶）：`if(!ignoreUnknown)` ORIG 为 4.1
// `cmpb $0,-0x2c` 内存直测 vs 4.4 `movzbl; xor $1; test`（N122 bool 取反 lowering；
// `==0`/`==false`/`!=true` 三形实测同形）。p167 CLEAR（26=26）。语义等价。
#include "OnlyOrigBatch14/ScanAnyData.inc"

// ORIG T 0x089e6dc4 _Z20ScanChampionFeaturesiR27STAICharacterMapArrangeData：
// count 行 × 0x25；匹配名取自**二级寻址** `NAME[ VALUE[i] & 0xff ]`
// （ORIG 0x089e6df7 `mov 0x8e1c6e0(,%eax,4),%eax; and $0xff,%eax;
// mov 0x93f55a0(,%eax,4),%eax`），命中后 push 的却是 VALUE[i] **未截断原值**
// （ORIG 0x089e6e20 二次载 VALUE[i] → -0x14 → push_back(EOi)）。
// VALUE 表自 ORIG .data 0x08e1c6e0 逐项提取（低字节=名表索引）。
static const unsigned int g_championFeatureValue[0x25] = {
    0x1ff00, 0xff01, 0x2ff02, 0x2ff03, 0x30004, 0x30005, 0x30006, 0x30007,
    0x40008, 0x3fe09, 0x3fe0a, 0x3fe0b, 0x3fe0c, 0x2000d, 0x3000e, 0x3000f,
    0x20010, 0x40011, 0x10012, 0x20013, 0x2fd14, 0x2fd15, 0x2fd16, 0x2fd17,
    0x2fd18, 0x2fd19, 0x2fd1a, 0x3001b, 0x4fd1c, 0x4fd1d, 0x4fd1e, 0x4fd1f,
    0x4fd20, 0x4fd21, 0x4fd22, 0x4fd23, 0x4fd24
};
static const char* const g_championFeatureName[0x25] = {
    "[Immune-Magic]", "[Immune-Physic]", "[Enhance-Physicaldefense]",
    "[Enhance-Magicaldefense]", "[Immune-Fire]", "[Immune-Water]",
    "[Immune-Light]", "[Immune-Dark]", "[Immune-ActiveStatus]",
    "[Enchant-Fire]", "[Enchant-Water]", "[Enchant-Light]", "[Enchant-Dark]",
    "[Speedup-Move]", "[Speedup-Attack]", "[Speedup-Hitrecovery]",
    "[Speedup-Wakeup]", "[Speedup-Hpregen]", "[Etc-Neverfly]",
    "[Etc-Reserrection]", "[onAttack-stun]", "[onAttack-bleeding]",
    "[onAttack-confuse]", "[onAttack-slow]", "[onAttack-lightning]",
    "[onAttack-poison]", "[onAttack-blind]", "[high-activestatus]",
    "[Etc-divide]", "[Etc-invisible]", "[Etc-healed]", "[Etc-infected]",
    "[Etc-counted]", "[Etc-berserk]", "[Etc-sleep]", "[Etc-revive]",
    "[Etc-teleport]"
};
#include "OnlyOrigBatch14/ScanChampionFeatures.inc"

// ORIG T 0x08a65da5 _Z15ScanRewardItemsRSt6vectorI14RewardItemInfoSaIS0_EESs。
// [b14 报告核对·两条真修复]
//  ①真语义缺口（报告 Ghidra C 0x8a65e6a 段 + objdump 0x8a65e6a..0x8a65eec 双证）：
//    ORIG 为 `if (ok1) { if (s=="[job]") { ScanInt→m_job; ScanInt→m_growType; }
//    ScanInt→v; } info.m_count = v;` —— 最后一次 ScanInt 在 ok1 分支内**无条件**
//    执行（非 "[job]" 时 `je 0x12e` 亦落该段）；原实现写在 else 分支 ⇒ [job]
//    情形少消耗一个 int（真 bug）。修后 123=123、gap 0。
//  ②ORIG 全程只用一个 -0xc 变量（v 兼作 ScanInt 中转槽；Ghidra local_10 贯穿）
//    ⇒ 删除原 v2 具名中转。
// 声明序按 4.4「后声明更深」排：info@-0x20 / s@-0x24 已与 ORIG 一致。
// [b14 本波·DIFF→NEAR] `s == token` 真块位置镜像已翻正（旧判词②作废）：
//   ORIG 形 `test %al,%al; jne <真岛 +0x164>` + fall-through `mov $0x0,%ebx`
//   （false 紧随），if-return/`!=` 两形在 4.4 恒把真块前置 ⇒ 改 **goto 尾标签**
//   形（`if (s == token) goto scan_reward_true; return false;` + 函数尾
//   `scan_reward_true: return true;`），4.4 jump-threading 折成 ORIG 的
//   `jne <真岛>`；现 124=124 助记符逐条全等、全部分支偏移一致
//   （+0x76/+0x164/+0x187 三处与 ORIG 逐位相同）⇒ **NEAR**（ae 800→799、
//   diff 13→12、near 0→1）。
// 残余（NEAR，N 封顶）：标量区槽基 4 字节差（我方 ok1/unusedFlag/ok2/v =
//   -0xa/-0x9/-0xb/-0x10 对 ORIG -0xf/-0xe/-0xd/-0xc；ORIG 侧 int v 浅于
//   bool 组、4.4 侧 int 恒最深 = N82附/N126 版本分区常数；前置 dead pad 实测
//   ndiff 44→66 反劣、v 提前声明致 info/s 下移 4，均已回退）。
// p167 CLEAR（13=13）。
#include "OnlyOrigBatch14/ScanRewardItems.inc"

// ORIG T 0x08a0fff6 _Z30exportCharacterGiveSkillScriptP8_IO_FILEPSt6vectorISt4pairIiiESaIS3_EE。
#include "OnlyOrigBatch14/exportCharacterGiveSkillScript.inc"

// ORIG T 0x08a100f3 _Z29exportCharacterEtcStatsScriptP8_IO_FILEP17CharacterEtcStats。
// [b28d 判词·N 封顶（N82 槽复用 + N85 x87）] 本波修：①第一循环加具名
// ENUM_CATEGORY cat 局部（P0：ORIG 0x08a10151 槽往返 + 回读压参）。
// [H10-b14wave14 本波·槽序归零] 两处修复（均 ORIG 逐地址实证）：
//  ①`int i = 0;` → `int i;`：消我方多发的一条 `movl $0x0,-0x18(%ebp)`
//    （ORIG 0x08a101f3 段仅一条 for-init 写；旧形声明初始化 + for-init 各写一次）
//    ⇒ our_insns 118→117、ndiff 该 hunk 消失（P191/P328⑤）。
//  ②`int j;` 由函数头移入 `if (i != 4)` 块内（for-init 单点赋值保留）：4.4 按
//    内层块后声明占浅槽分配 ⇒ j@-0xc（ORIG 0x08a1024e）、i1@-0x14（ORIG
//    0x08a10134）、cat@-0x10（ORIG 0x08a10151）、i2@-0x18（ORIG 0x08a101f3）
//    四处与 ORIG 逐槽相同 ⇒ 原「N82 槽复用」两 hunk 全消。
//  p167 CLEAR（14=14）。残余唯一差异 = N85 x87 lowering（ORIG 0x08a10215 段
//  `fucompp;fnstsw %ax;sahf` vs 4.4 `fucomip %st(1),%st;fstp %st(0)`）。
// [b14 A/B 实测·路由不提] N85 x87 lowering：默认 mne_delta =
// {sahf:-1,fucompp:-1,fnstsw:-1,fucomip:+1,fstp:+1,movl:+1}（ndiff 55 / gap 0）；
// 追加 `-march=i486 -mtune=generic`（Main 判据，须同带 mtune）后 x87 差异全消，
// 但**新引入 1 条 movl**（gap 0→-1、ndiff 52），本件仍 DIFF，且整 TU 四数逐项
// 不变（396+800=1196 / near 0 / diff 13 / missing 197；30 个函数指令形变化、
// 0 件翻转）⇒ 依 Main 口径（仅整 TU 净收益才提路由）不提。
#include "OnlyOrigBatch14/exportCharacterEtcStatsScript.inc"

// ORIG T 0x08a0fb38 _Z27exportCharacterStatusScriptP8_IO_FILEP19STRDCharacterStatus：
// 非零字段输出（值 /10.0f）。
static const char* const g_activeResistName[17] = {
    "[slow resistance]", "[freeze resistance]", "[poison resistance]",
    "[stun resistance]", "[curse resistance]", "[blind resistance]",
    "[lightning resistance]", "[stone resistance]", "[sleep resistance]",
    "[burn resistance]", "[weapon break resistance]", "[bleeding resistance]",
    "[piercing resistance]", "[stuck resistance]", "[confuse resistance]",
    "[hold resistance]", "[armor break resistance]"
};
// [b14 本波·AE 归零] 三条真缺口（旧判词「N85 x87 除法 lowering 封顶」作废）：
//  ①`(double)field` → `(float)field`：4.4 对 double 常量发 `fldl <A>`，ORIG 是
//    `flds <A>`（float 常量）；且 `(float)unsigned int`（+0x0/+0x4/+0x42）走
//    `mov $0,%edx; … fildll` 零扩展形、`(float)int`（+0x3a/+0x4e）走 `fildl`
//    —— 与 ORIG 逐段同形（探针 /tmp/b14_probe2.cpp pA/pB/pC/pE）。
//  ②elemResist(+0x10..+0x16) 与 activeResist(+0x18..) 需 `(float)(short)field`：
//    ORIG 发 `movzwl; mov %ax,-0x1e; filds -0x1e`（16 位载入，共 5 处），直写
//    `(float)field` 只给 32 位 `fildl`（探针 pC 复证）。
//  ③m_hpRegen/m_mpRegen/m_hitRecovery/m_jumpPower(+0x3e/+0x40/+0x4a/+0x4c) 同为
//    filds 形，各加 `(short)`。
// 终态 orig=331 our=331 gap=0 ndiff=0。
#include "OnlyOrigBatch14/exportCharacterStatusScript.inc"

// ORIG T 0x08919127 _Z22importQuestPieceScriptPKcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE。
#include "OnlyOrigBatch14/importQuestPieceScript.inc"

// ORIG T 0x088ffaa9 _Z15importSlangListPcPSt6vectorISsSaISsEE。
#include "OnlyOrigBatch14/importSlangList.inc"

// [b14 本波·AE 归零] 内层循环局部槽序（旧判词「残余 NEAR=N98 槽序」作废）：
// ORIG s@-0x1c(深)/isInt@-0x16/isType@-0x15/v@-0x14(浅)，4.4「后声明更深」
// ⇒ 声明序改 v/isType/isInt/s（v 声明与赋值分离，P244）后 121=121、ndiff=0。
#include "OnlyOrigBatch14/importHiddenNpcListInfo.inc"

// ORIG T 0x088ffe75 _Z29importUniqueRewardForAllUsersPcPSt3mapIiSt6vectorIS1_I22STStackablePackageDataSaIS2_EESaIS4_EESt4lessIiESaISt4pairIKiS6_EEE。
// [b28d-solo-OnlyOrigBatch14；gap 2→1 本波收敛]
// loadRDARScriptFile 参数序已修为 ("", path)（真语义 bug，ORIG 0x088ffe8d
// 压参序实证）。本波两条真缺口：
//   (a) `[package selection]` 内层循环 ORIG 088fff63-69 为负极性
//       `int itemIdx; for(;;){ if(!ScanInt(&itemIdx)) break; … }`（有 xor、
//       无 for-init `movl $0`），原 `for(int itemIdx=0; ScanInt(&itemIdx); )`
//       产 for-init+尾条件正极性 jne（多 movl、少 xor）；
//   (b) group 块尾 ORIG 0890002d `jmp 08900044` → jobs dtor → 0890004f
//       `jmp 088ffeca`（循环头）= 原码 **continue**（语义等价：token=="[group]"
//       时 "[/package]" 恒 false）。
// 残余 gap 1 = ORIG 的正常路径 jobs dtor 块（08900044-4f）物理位于 EH 段之后
// 并由 `jmp 08900044` 进入，4.4 把该 dtor 内联在 continue 点（N104 EH 尾块
// 相对序）。p167 MISSING `_ZSteqIcSt11char_traitsIcESaIcEE…EPKS3_`
// （string==const char* 外链实例，4.4 libstdc++ 全内联，N89）。
// [b14 复核] 槽序修复：P212「4.4 块内先声明占**浅**槽（4.1 相反）」⇒ 把
// `int itemIdx;` 提到 `std::vector<STStackablePackageData> items;` 之前，
// 复现 ORIG items@-0x30（12B，深）/ itemIdx@-0x24（4B，浅）的槽对
// （改前我方 itemIdx@-0x30 / items@-0x2c 恰好互换）。修复后 174 vs 173，
// 唯一残差即上述 `jmp +0x1cf` 单源 EH 尾岛（P223 负例：单源 goto 的尾岛
// 4.4 恒 jump-thread ⇒ 不可造，封 N）。
// 控制流/全部插入点与 ORIG 逐段核对，语义等价。
#include "OnlyOrigBatch14/importUniqueRewardForAllUsers.inc"

// ORIG T 0x08a358d7 _Z15importPetScriptP9PetScriptPKc。
#include "OnlyOrigBatch14/importPetScript.inc"

// [b14 本波] 循环体第二条 `m_moveSpeed[i+2]=0`：ORIG 0x08a3dc28 为
// `add $0x8,%edx; movl $0,0x8(%eax,%edx,4)`（+2 折进 disp8），4.4 恒
// `lea 0x2(%eax),%edx; add $0x8,%edx; movl $0,(%eax,%edx,4)`（+2 折进索引）。
// 探针 /tmp/b14_probe3.cpp f1..f4（`[i+2]`/`[2+i]`/`(arr+2)[i]`/`*(arr+i+2)` 四形）
// 全部产出 lea 形（后两形更劣 9 条）⇒ 判 N4/P30 索引常量分配跨版本差，封顶。
// 其余 `m_moveSpeed[4]` 清零等本波已补（真语义缺口）。ndiff 5 / gap -1。
#include "OnlyOrigBatch14/clear.inc"

// ORIG T 0x0890a334 _Z27importUltimateSkillCutScenePKcRSt6vectorI23STUltimateSkillCutSceneSaIS2_EERi：
// 空体（mov $0,%eax; ret）。
#include "OnlyOrigBatch14/importUltimateSkillCutScene.inc"

// ORIG T 0x088c1e68 _Z18importActiveStatusR14STActiveStatusb
// （报告缺失，按 §3.2 以 ORIG objdump 全量转写；两侧 AE 逐条核对）。
// 结构：flag 入参 spill(-0x6c)、s(-0x5c)、16 个 ScanInt 物化临时(-0x58..-0x1c)、
// type(-0x18)、循环计数 i9(-0x14)/iD(-0x10)/i4(-0xc)。
// [b14 本波·AE 归零] 两条真缺口（旧判词「N 封顶、32 条 mov 不可消」作废）：
//  ①16 处原写 `tX = ScanInt((bool*)0); push_back(static_cast<int&&>(tX));`
//    ⇒ 4.4 对具名 int 的右值 cast 额外 materialize 中转（每处 +2 条 mov，共 +32）。
//    改为 **prvalue 直传** `push_back(ScanInt((bool*)0))`：prvalue 绑定 int&& 仍
//    选中 ORIG 的 `_ZNSt6vectorIiSaIiEE9push_backEOi`（p167 42=42 CLEAR），每处只余
//    `call; mov %eax,-0xNN` 一条 —— ORIG 的 -0x58..-0x1c 16 槽即这些匿名物化临时。
//  ②标量区声明序：4.4 按「先声明更深」分配 ⇒ 声明序改 type/i9/iD/i4（s 为类对象
//    优先占低槽 -0x5c，位置不受影响）。
// 终态 orig=261 our=261 gap=0 ndiff=0。
#include "OnlyOrigBatch14/importActiveStatus.inc"

// ============================================================================
// 4) 成员/类函数
// ============================================================================

// ORIG T 0x089e82f6 _ZN12STTileScript5ClearEv。
#include "OnlyOrigBatch14/Clear.inc"

// ORIG W 0x0844d584 _ZN21STBreakAwayMemberData5clearEv。
#include "OnlyOrigBatch14/clear_1.inc"

// ORIG T 0x085e709e _ZN15MissionLessThanclEPK8CMissionS2_：
// 先比 MissionKind 再比 MissionIndex。
#include "OnlyOrigBatch14/operator.inc"

// ORIG T 0x081b5bf2 _ZNK23NeedMaterialDataManager15getNeedMaterialEi：
// getNeedMaterial(G_CDataManager()->find_item(i))。
#include "OnlyOrigBatch14/getNeedMaterial.inc"

// ORIG T 0x0816d454 _ZN16RestrictCategory12enumToStringENS_4EnumE。
#include "OnlyOrigBatch14/enumToString.inc"

// （importKeyNameIndexTable 已按 R3 批准迁至 ScriptExtras.cpp —— 判词随行迁走。）

// ORIG T 0x0808aa37 _Z16WriteCodeSectionPKc：读取 PE 头后写回 .code.bin。
#include "OnlyOrigBatch14/WriteCodeSection.inc"

// ORIG T 0x089bbe1e _Z32getActivePropertyTypeToEnumValueRSs：
// 函数局部 static map + SpinLock 惰性填充；键 "slow resistance" 等 16 项。
#include "OnlyOrigBatch14/getActivePropertyTypeToEnumValue.inc"
// ===== _Z13G_EncryptToolv —— ORIG 0x0848e1c6 =====
// ORIG：mov 尾调 GlobalInstance<EncryptTool>::inst_ptr()（inst_ptr 0x848e1d3）。
// [b14 判据㉓·Main 已批准单点修法] ORIG nm 绑定 = **W**（弱符号，0x0848e1c6 W
// 0xd）；本 TU 原为 T 强绑定。加 weak 使绑定对齐（尺寸/mangled/函数体零变化）。
__attribute__((weak)) EncryptTool* G_EncryptTool()
{
    return GlobalInstance<EncryptTool>::inst_ptr();
}

// ===== _ZN35FAIR_PVP_SKILL_TREE_SKILL_PER_POINT5clearEv —— ORIG 0x08a5e874 =====
// 反汇编：map@+0x00 clear、map@+0x18 clear、vector<pair>@+0x30 clear、int@+0x3c=0。
#include "OnlyOrigBatch14/clear_2.inc"
// ORIG C1 0x08a5e7e6：构造成员后调 clear()。
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

// ===== _Z32ImportDifficultPenaltyInfoScriptPKcR20DifficultPenaltyInfo —— ORIG 0x0890a33e =====
// loadRDARScriptFile(dir="", path) 失败 → 返回 0x1bdb；逐行 ScanType(token, true)：
// 遇 "[COMMON TRAP EXPLOSION]" 段读取 5 个 int（ScanInt 成功才写入 out）。
#include "OnlyOrigBatch14/ImportDifficultPenaltyInfoScript.inc"

// ===== _Z16ImportTileScriptP12STTileScriptPKc —— ORIG 0x089dcbb6 =====
// loadRDARScriptFile("", path) 失败 → printf("TILE LOAD FAIL! [%s]\n", path) 返回 0；
//   【gi00 串 key 配平修正】ORIG 0x089dcbc5 `movl $"",(%esp)` ⇒ 第一参为**空串**
//   （非 "TILE"；"TILE" 只出现在后面的 printf 文案里）。旧注释误记为 "TILE"。
// 先 Clear()，随后按 token 分派：
//   [IMAGE]   → 循环 { pair<string,int>；ScanStr(&first)；second=ScanInt((bool*)0)；
//                 m_actions.push_back }
//   [tile type]→ ScanStr 比较 "[basic]"→m_flagC=0、"[extended]"→m_flagC=1
//   [img pos] → m_field1c = ScanInt((bool*)0)
//   [pass type]→ m_collisions 清空；循环 { n=ScanInt(&ok)；!ok 退出；
//                 row.clear()+push(n)；再读 13 个 ScanInt((bool*)0) 追加；push_back(row) }
// 返回 1。
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

// ===== _Z25ImportPassiveObjectScriptP21STPassiveObjectScriptPKcb —— ORIG 0x08a2c51a =====
// loadRDARScriptFile(基目录, path) 失败 → false；随后 clear 三个子对象、
// getDirectoryPath 拼 m_s90，主 token 循环分派（未知 token → false；
// bRepair 且已读 [name] 时提前结束）。EOF 统一尾处理：
// FindIndexByFullScan(path) 写 m_idx 并返回 true。
#include "OnlyOrigBatch14/ImportPassiveObjectScript.inc"

// ===== _Z17GetSlicedIntervalPlS_S_S_lll —— ORIG 0x085732d8 =====
// 切片区间工具：arg6>0 时写 [0,arg6] 到 out1/out2（count=1）；
// 若 arg7+arg6 < arg5（总长），第二段 [arg7+arg6, arg5-(arg7+arg6)] 写
// out3/out4（count==0 时写 out1/out2），count 增 1。返回段数。
#include "OnlyOrigBatch14/GetSlicedInterval.inc"

// ===== _Z24CheckDayScheduleTimeOveril —— ORIG 0x0846c370 =====
// time(&local)（ORIG 0x0846c37a `lea -0xc(%ebp),%eax; call time` 传局部地址，
// 非 time(NULL)）+ day * (-86400) > checkTime（0xfffeae80 == -86400 补码）。
#include "OnlyOrigBatch14/CheckDayScheduleTimeOver.inc"

// ===== _Z29DecreaseTradeLimitCountByUserR5CUserii —— ORIG 0x0812017d =====
// curCharac 缺失 → false；GetInvenRef(space, slot) 缺失 → false；
// 否则委托 DecreaseTradeLimitCount(Inven_Item&)。
#include "OnlyOrigBatch14/DecreaseTradeLimitCountByUser.inc"

// ===== _Z18GiveItemReturnUserP5CUser —— ORIG 0x084bfd28 =====
// find_item(0x1f15) 失败直接返回；make_item 填局部 Inven_Item、set_add_info(1)。
// 按最后游玩 dungeon 是否存在分两路拼邮件：正文
// findString(4,"return_user_msg_1",0) + snprintf("%s", dungeonName) 或
// findString(4,"return_user_msg_2",0)；发件人 findString(4,"return_user_msg_title",0)；
// ReqDBSendNewSystemMail(title, item, 0, characNo, body, strlen(body), 7,
//                        serverGroup, false, false)。
#include "OnlyOrigBatch14/GiveItemReturnUser.inc"

// ===== _Z38GetInstanceDeathTowerPlaydataStatisticv —— ORIG 0x0860f451 =====
// ORIG：mov $0x94733a8 直返（.bss 静态实例，无 guard）。重建以文件级
// static 对象承载同一地址语义。
CDeathTowerPlayDataStatistic* GetInstanceDeathTowerPlaydataStatistic()
{
    // ORIG 0x094733a8 = _ZZ38GetInstanceDeathTowerPlaydataStatisticvE8instance（b，1B）
    // ⇒ 函数内 static 真名为 `instance`（原 `s_instance` 为发明名，改回 ORIG 名）。
    static CDeathTowerPlayDataStatistic instance;
    return &instance;
}
