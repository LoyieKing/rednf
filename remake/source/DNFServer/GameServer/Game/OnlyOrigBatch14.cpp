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
int InitTileScript()
{
    return 1;
}

// ORIG T 0x0841d2ea _Z11PrintLetter15SIG_LETTER_INFO：空体。
void PrintLetter(SIG_LETTER_INFO)
{
}

// ORIG T 0x08104884 _Z11SetSockNonbim：ioctl(fd, FIONBIO=0x5421, &arg) == 0。
// 返回 int（ORIG 尾 sete %al;movzbl %al,%eax 加宽形，bool 返回 4.4 不加宽）。
int SetSockNonb(int fd, unsigned long arg)
{
    unsigned long on = arg;
    return ioctl(fd, 0x5421, &on) == 0;
}

// ORIG T 0x089bd03b _Z20IsDamageActiveStatusi：
// status<=0xb && ((1<<status) & 0xac4)。ORIG 形：bool 局部汇聚（089bd042
// movb $0,-0x5；两写点 089bd065/089bd06b）+ **无符号边界**（089bd046
// cmpl $0xb,0x8(%ebp) 直比参数槽 + ja，P137/P2）+ 末尾 movzbl 读回。
bool IsDamageActiveStatus(int status)
// [b28d-solo-OnlyOrigBatch14 判词·N 封顶（P209），本波翻正位测试]
// 本波：位测试改 P231 纯 register（`register int bit = 1 << status;`，非 asm
// 绑定、P0 合规）⇒ 4.4 免槽并复现 ORIG 0x089bd04c 段
// `mov $0x1,%edx; mov %edx,%ebx; shl %cl,%ebx; mov %ebx,%eax; and $0xac4,%eax`
// 的寄存器形（27→25 insn，旧判词「无源形可同时满足 shl 无槽」作废）。
// 唯一残差 = 边界比较：ORIG 0x089bd046 `cmpl $0xb,0x8(%ebp); ja`（无符号内存
// 直比），4.4 对 unsigned 参与比较恒先载寄存器（`mov 0x8(%ebp),%eax;
// cmp $0xb,%eax`，P209）；写 signed 会翻 `jg` 并改变负数语义（真 bug 风险）
// ⇒ N 封顶。p167 CLEAR（0 calls 双侧）。
{
    bool ret = false;
    if ((unsigned int)status <= 0xb)
    {
        // bit 具名局部阻断 4.4 位折叠（N6：直写恒折 (0xac4>>s)&1 的
        // sar+and$1 反演形）；P231 纯 register（非 asm 绑定，P0 合规）
        // 令 4.4 免槽、复现 ORIG `shl %cl,%ebx; mov %ebx,%eax; and $0xac4`
        // 的寄存器形。残差：边界比较 mov+cmp vs ORIG cmpl 槽直比（P209）。
        register int bit = 1 << status;
        if ((bit & 0xac4) != 0)
            ret = true;
        else
            ret = false;
    }
    return ret;
}

// ORIG T 0x0808c74b _Z13PI_GetType_SCcs：type==1 查表、type==2 if 链、缺省 0。
int PI_GetType_SC(char type, short value)
{
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x0808c75e `movsbl -0x4(%ebp),%eax;
    // cmp $0x1,%eax; je; cmp $0x2,%eax; je` = 二路 switch/if 链的整字提升形；
    // type==1 为跳表 switch（sub $0x13/cmp $0x40/ja/jmp *table），type==2 为
    // 稀疏 switch 二分决策树（P46：叶块排布 = case 书写顺序）。两侧 case
    // 顺序按 ORIG 叶块地址序重排（0x13..0x53 / 0x28..0x85）。
    // 判词·AE 归零（100=100）：内层 switch 不写 default、改 switch 后
    // `break;` + 函数尾单一 `return 0`，使所有失败路径共享 ORIG 0x0808c8ae
    // 的 `mov $0x0,%eax` 尾块（P17/P46）。
    switch (type)
    {
    case 1:
        switch (value)
        {
        case 0x1f: return 9;
        case 0x1d: return 11;
        case 0x2f: return 12;
        case 0x2e: return 18;
        case 0x48: return 22;
        case 0x49: return 23;
        case 0x4a: return 24;
        case 0x20: return 25;
        case 0x13: return 26;
        case 0x43: return 27;
        case 0x53: return 44;
        }
        break;
    case 2:
        switch (value)
        {
        case 0x28: return 4;
        case 0x8e: return 5;
        case 0x8f: return 6;
        case 0x1c: return 7;
        case 0x76: return 13;
        case 0x25: return 16;
        case 0x27: return 17;
        case 0x73: return 19;
        case 0x22: return 20;
        case 0x23: return 21;
        case 0x85: return 39;
        }
        break;
    }
    return 0;
}

// ORIG T 0x0808bc7e _Z6Rand_rPj：LCG rand_r（NULL 时用全局种子）。
// [b28d-solo-OnlyOrigBatch14 复核 lane24 判词·N 封顶] 唯一残差 = NULL 分支
// `staticSeed = timeGetTime() + staticSeed`：ORIG 0x0808bc8f `mov %eax,%edx;
// mov staticSeed,%eax; lea (%edx,%eax,1),%eax`（RHS-first + lea），4.4 恒
// `mov staticSeed,%edx; add %edx,%eax`（LHS-first + add）。P211 负例（static
// 存储期变量复合赋值装载序不可翻，b28c C28L24 三救均败）+ add/lea 跨版本；
// 其余 49 条与 ORIG 全等。
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

// ORIG T 0x088bdb56 _Z11ScanAnyDataRiRSsS_b：任意类型值扫描。
// [b28d-solo-OnlyOrigBatch14 判词·N 封顶（N98/N82 槽序 + P75）] p167 CLEAR
// （26=26 calls）。残差：①ORIG bool 槽 -0xd / 结果槽 -0x14 与 4.4 -0x11 /
// -0x10 互换（4.4 栈槽按首次使用序分配，声明序翻转/提函数头实测无效，
// N98/N82）；②LogManager::logFormat 变参装载序（ORIG 先值后串 vs 4.4 相反，
// P75 跨版本实参装载时刻）；③bool 局部取反物化 xor（N122）。语义等价。
bool ScanAnyData(int& type, std::string& str, int& value, bool ignoreUnknown)
{
    type = 3;
    bool got = false;
    // [b28d-solo-OnlyOrigBatch14] P309：4.4 对函数头声明按「先声明浅」分配
    // ⇒ token 提至函数头（声明序 got 先/token 后）复现 ORIG got@-0xd、
    // token@-0x14 的槽向；循环内仅赋值。
    int token;
    for (;;)
    {
        token = g_dnfLex.getToken();
        // ORIG if 链布局（非 switch 表）；case 9 成功时 ORIG 将 token 置 7，
        // 使后续循环 break 检查（2/3/4/7/5/0）命中，避免继续循环。
        if (token == 2)   // decimal
        {
            value = g_dnfLex.getDecimal();
            type = 1;
            got = true;
        }
        else if (token == 3)   // hex
        {
            value = g_dnfLex.getHexDecimal();
            type = 1;
            got = true;
        }
        else if (token == 4)   // float
        {
            value = (int)g_dnfLex.getFloat();
            type = 1;
            got = true;
        }
        else if (token == 7)   // string
        {
            str = toTString(g_dnfLex.getString());
            got = true;
            type = 0;
        }
        else if (token == 5)   // type
        {
            str = toTString(g_dnfLex.getType());
            got = true;
            type = 2;
        }
        else if (token == 9)   // pair int
        {
            int key = g_dnfLex.getPairInt();
            if (g_dnfLex.getToken() == 0xa)
            {
                str = g_scriptStringManager_.findString(key,
                                                        g_dnfLex.getPairStr(),
                                                        0);
                type = 0;
                got = true;
                token = 7;   // ORIG 0x88bdcb8 movl $0x7,-0x14(%ebp)
            }
        }
        else
        {
            got = false;
            type = 3;
        }
        if (token == 2 || token == 3 || token == 4 || token == 7 ||
            token == 5 || token == 0)
        {
            break;
        }
        if (!ignoreUnknown)
        {
            LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x746,
                                  "ERROR! %s - TYPE mismatch. %s:%d, token = '%s'",
                                  g_dnfLex.getStreamName().c_str(),
                                  "ScanAnyData",
                                  g_dnfLex.getLastLineNo(),
                                  g_dnfLex.getLastTokenDataAsString().c_str());
        }
    }
    return got;
}

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
void ScanChampionFeatures(int count, STAICharacterMapArrangeData& data)
{
    std::string s;
    for (int line = 0; line < count; ++line)
    {
        ScanStr(&s);
        for (int i = 0; i <= 0x24; ++i)
        {
            if (s == g_championFeatureName[g_championFeatureValue[i] & 0xff])
            {
                data.m_championFeatures.push_back(
                    (int)g_championFeatureValue[i]);
                break;
            }
        }
    }
}

// ORIG T 0x08a65da5 _Z15ScanRewardItemsRSt6vectorI14RewardItemInfoSaIS0_EESs。
// [b28d-solo-OnlyOrigBatch14 判词·N 封顶（N98 槽序 + P292）] 本波修：三个
// bool 声明序 ok2/dead/ok1 + ok2 无初始化式（P309 先声明浅 + P171 dead 预清）
// 使预清序 -0xe/-0xf 与 ORIG 一致；ScanInt 改经具名 v2 中转（P0：ORIG
// 0x08a65e80 `lea -0xc; call ScanInt; mov -0xc,%eax; mov %eax,-0x18`）。
// p167 CLEAR（13=13）。残差：ok1/ok2 槽 -0xf/-0xd vs 4.4 -0xf/-0xe（N98 首次
// 使用序，声明序翻转实测不闭合）+ bool 物化岛极性 je/jne 随槽号错位（P292）。
bool ScanRewardItems(std::vector<RewardItemInfo>& out, std::string token)
{
    // [b28d-solo-OnlyOrigBatch14] P309：4.4 函数头声明「先声明浅」⇒
    // 声明序 ok2(浅 -0xd)/dead(-0xe)/ok1(深 -0xf) 对齐 ORIG；ORIG 预清仅
    // 两条（`movb $0,-0xe; movb $0,-0xf`）⇒ ok2 无初始化式（ScanTypeOrInt
    // 写入后即读）。
    bool ok2;
    bool unusedFlag = false;
    bool ok1 = false;
    RewardItemInfo info;
    std::string s;
    int v2;
    for (;;)
    {
        int v = ScanTypeOrInt(ok1, s, ok2, 1);
        if (ok1)
        {
            if (s == token)
                return true;
            return false;
        }
        if (!ok2)
            return false;
        info.m_itemIdx = v;
        info.m_job = 0xb;
        v = ScanStrOrInt(ok2, s, ok1, 1);
        if (!ok2)
            return false;
        if (ok1)
        {
            if (s == "[job]")
            {
                // [b28d-solo-OnlyOrigBatch14] P0：ORIG 0x08a65e80 段
                // `lea -0xc(%ebp),%eax; call ScanInt; mov -0xc(%ebp),%eax;
                // mov %eax,-0x18(%ebp)`（具名 int 中转槽 -0xc，再存 info.m_job）
                // ⇒ 原码经具名局部而非直接 ScanInt(&info.m_job)。
                if (!ScanInt(&v2))
                    return false;
                info.m_job = v2;
                if (!ScanInt(&v2))
                    return false;
                info.m_growType = v2;
            }
            else
            {
                if (!ScanInt(&v2))
                    return false;
                info.m_count = v2;
            }
        }
        else
        {
            info.m_count = v;
        }
        out.push_back(info);
    }
}

// ORIG T 0x08a0fff6 _Z30exportCharacterGiveSkillScriptP8_IO_FILEPSt6vectorISt4pairIiiESaIS3_EE。
void exportCharacterGiveSkillScript(FILE* fp, std::vector<std::pair<int, int> >* vec)
{
    if (!vec->empty())
    {
        fwrite("[skill]\n", 1, 8, fp);
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x08a10083 增量段调 `ppEi`（后置
        // `operator++(int)`，3 参数含 sret 临时槽 -0xc）⇒ 必须写 `it++`（P28
        // 增量侧推论）；`++it` 走 ppEv 免临时槽、后续槽整体上移。
        for (std::vector<std::pair<int, int> >::iterator it = vec->begin();
             it != vec->end(); it++)
        {
            fprintf(fp, "%d %d\n", it->first, it->second);
        }
        fwrite("[/skill]\n", 1, 9, fp);
    }
}

// ORIG T 0x08a100f3 _Z29exportCharacterEtcStatsScriptP8_IO_FILEP17CharacterEtcStats。
// [b28d-solo-OnlyOrigBatch14 判词·N 封顶（N82 槽复用 + N85 x87）] 本波修：
// ①第一循环加具名 ENUM_CATEGORY cat 局部（P0：ORIG 0x08a10151
// `mov %eax,-0x10(%ebp)` 槽往返 + 回读压参）；②j 提函数头 + for-init 单点
// 赋值，消 4.4 对块内 `for(int j=0;…)` 多发的一条 `movl $0,-0x18`
// （P191/P309）。p167 CLEAR（14=14）。残差：ORIG 三循环变量分占
// -0x14/-0x18/-0xc 而 4.4 复用同槽（N82：4.1 不按 lifetime 复用栈槽）+ x87
// 比较 `fucompp/fnstsw/sahf` vs 4.4 `fucomip/fstp`（N85；P230 注：-march=i486
// TU 旗标可消，属路由变更需 Main 统一处理）。
void exportCharacterEtcStatsScript(FILE* fp, CharacterEtcStats* stats)
{
    if (!stats->m_category.empty())
    {
        fwrite("[category]\n", 1, 0xb, fp);
        for (unsigned int i = 0; i < stats->m_category.size(); ++i)
        {
            // [b28d-solo-OnlyOrigBatch14] P0：ORIG 0x08a10136 段
            // `mov (%eax),%eax; mov %eax,-0x10(%ebp); lea -0x1c(%ebp),%eax;
            // mov -0x10(%ebp),%edx; mov %edx,0x4(%esp)` ⇒ 原码有具名
            // ENUM_CATEGORY 局部（槽往返），非内联 cast 直传。
            ENUM_CATEGORY cat = (ENUM_CATEGORY)stats->m_category[i];
            std::string s = getObjectCategoryString(cat);
            fprintf(fp, "`%s`\n", s.c_str());
        }
        fwrite("[/category]\n", 1, 0xc, fp);
    }
    int i = 0;
    int j;
    for (i = 0; i <= 3; ++i)
    {
        if (stats->m_moduleDamageRate[i] != 0.0f)
            break;
    }
    if (i != 4)
    {
        fwrite("[module damage rate]", 1, 0x14, fp);
        // [b28d-solo-OnlyOrigBatch14] j 已在函数头声明；for-init 单点赋值
        // （4.4 对 `for (int j = 0;…)` 多发一条 `movl $0,-0x18`，ORIG 仅一条）。
        for (j = 0; j <= 3; ++j)
        {
            fprintf(fp, " %.3f", (double)stats->m_moduleDamageRate[j]);
        }
        fputc(0xa, fp);
    }
}

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
// [b28d-solo-OnlyOrigBatch14] N 封顶（N85/P176 x87 除法 lowering）：ORIG 对
// `(double)field / 10.0f` 发 `mov $0,%edx; mov %eax,-0x28; mov %edx,-0x24;
// fildll -0x28; flds 常量; fdivrp %st,%st(1); fstpl 0x8(%esp)`（int→ll 中转 +
// fdivrp），4.4 恒 `filds/fdivs` 直算；x87 两向跨版本差异，IEEE 语义等价。
void exportCharacterStatusScript(FILE* fp, STRDCharacterStatus* st)
{
    if (st->m_hpMax != 0)
        fprintf(fp, "[HP MAX] %.1f\n", (double)st->m_hpMax / 10.0f);
    if (st->m_mpMax != 0)
        fprintf(fp, "[MP MAX] %.1f\n", (double)st->m_mpMax / 10.0f);
    if (st->m_physAttack != 0)
        fprintf(fp, "[physical attack] %.1f\n", (double)st->m_physAttack / 10.0f);
    if (st->m_physDefense != 0)
        fprintf(fp, "[physical defense] %.1f\n", (double)st->m_physDefense / 10.0f);
    if (st->m_magAttack != 0)
        fprintf(fp, "[magical attack] %.1f\n", (double)st->m_magAttack / 10.0f);
    if (st->m_magDefense != 0)
        fprintf(fp, "[magical defense] %.1f\n", (double)st->m_magDefense / 10.0f);
    if (st->m_elemResist[0] != 0)
        fprintf(fp, "[fire resistance] %.1f\n", (double)st->m_elemResist[0] / 10.0f);
    if (st->m_elemResist[1] != 0)
        fprintf(fp, "[water resistance] %.1f\n", (double)st->m_elemResist[1] / 10.0f);
    if (st->m_elemResist[2] != 0)
        fprintf(fp, "[dark resistance] %.1f\n", (double)st->m_elemResist[2] / 10.0f);
    if (st->m_elemResist[3] != 0)
        fprintf(fp, "[light resistance] %.1f\n", (double)st->m_elemResist[3] / 10.0f);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (st->m_activeResist[i] != 0)
        {
            fprintf(fp, "%s %.1f\n", g_activeResistName[i],
                    (double)st->m_activeResist[i] / 10.0f);
        }
    }
    if (st->m_inventoryLimit != 0)
        fprintf(fp, "[inventory limit] %.1f\n", (double)st->m_inventoryLimit / 10.0f);
    if (st->m_hpRegen != 0)
        fprintf(fp, "[HP regen speed] %.1f\n", (double)st->m_hpRegen / 10.0f);
    if (st->m_mpRegen != 0)
        fprintf(fp, "[MP regen speed] %.1f\n", (double)st->m_mpRegen / 10.0f);
    if (st->m_moveSpeed != 0)
        fprintf(fp, "[move speed] %.1f\n", (double)st->m_moveSpeed / 10.0f);
    if (st->m_attackSpeed != 0)
        fprintf(fp, "[attack speed] %.1f\n", (double)st->m_attackSpeed / 10.0f);
    if (st->m_castSpeed != 0)
        fprintf(fp, "[cast speed] %.1f\n", (double)st->m_castSpeed / 10.0f);
    if (st->m_hitRecovery != 0)
        fprintf(fp, "[hit recovery] %.1f\n", (double)st->m_hitRecovery / 10.0f);
    if (st->m_jumpPower != 0)
        fprintf(fp, "[jump power] %.1f\n", (double)st->m_jumpPower / 10.0f);
    if (st->m_weight != 0)
        fprintf(fp, "[weight] %.1f\n", (double)st->m_weight / 10.0f);
}

// ORIG T 0x08919127 _Z22importQuestPieceScriptPKcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE。
bool importQuestPieceScript(const char* path, std::map<int, int>& out)
{
    // [b28d-solo-OnlyOrigBatch14] 真语义 bug：ORIG 0x0891912f 压参序为
    // `mov 0x8(%ebp),%eax; mov %eax,0x4(%esp); movl $"",(%esp)` ⇒ 调用
    // loadRDARScriptFile(dir="", path)（与 importDifficultPenaltyInfoScript
    // 同形）；原写 (path,"") 使目录/文件参数互换。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x0891915e：`movb $0,-0x25(ok)` 在**外层
    // 循环之前**初始化一次，内层循环回跳 0x0891919a（ScanInt），不重置 ok。
    bool ok = false;
    for (;;)
    {
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x08919178 `xor $1; test; jne
        // <函数尾 return-false 块 08919224（nop 开头）>`（P59/P223）；试
        // goto 尾标签形复现失败块落点。
        if (!ScanType(token, true))
            goto script_fail;
        if (token == "[piece quest list]")
        {
            for (;;)
            {
                int left = ScanInt(&ok);
                if (!ok)
                    break;
                int right = ScanInt(&ok);
                if (!ok)
                    break;
                out.insert(std::make_pair(left, right));
            }
        }
    }
script_fail:
    return false;
}

// ORIG T 0x088ffaa9 _Z15importSlangListPcPSt6vectorISsSaISsEE。
bool importSlangList(char* path, std::vector<std::string>* out)
{
    // [b28d-solo-OnlyOrigBatch14] 真语义 bug（同 importQuestPieceScript）：
    // ORIG 0x088ffab1 压参序 = loadRDARScriptFile(dir="", path)。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;
    std::string a;
    std::string b;
    for (;;)
    {
        if (!ScanType(token, true))
            goto slang_done;
        if (token == "[SLANG]")
        {
            // [b28d-solo-OnlyOrigBatch14] ORIG 0x088ffb4a 段布局：body 在前、
            // 条件块在尾（`jmp <cond>` 后 body，cond 为
            // `call ScanStr; test %al,%al; je`×2 —— bool 返回**正向**判断，
            // 无 xor 取反物化）⇒ 源形为 `while (ScanStr(&a) && ScanStr(&b))`
            // （P283 底部条件 + P292 bool 返回正极性）。
            // 判词·AE 归零（107=107）。
            while (ScanStr(&a) && ScanStr(&b))
            {
                out->push_back(a);
                out->push_back(b);
            }
        }
    }
slang_done:
    return true;
}

// ORIG T 0x08917848 _Z23importHiddenNpcListInfoPKcRSt6vectorIiSaIiEE。
// [b28d-solo-OnlyOrigBatch14；gap 3→0 / DIFF→NEAR 本波收敛]
// 已修：①done 改 unsigned char + `for(;;){ if(done) break; … }` ⇒ 条件块
// 复现 ORIG 0x08917900 `cmpb $0,-0x9(%ebp); jne`（消 sete/xor 取反物化，
// N122）；②补 dead bool 预清（ORIG 0x089178f8 `movb $0,-0xa`，P171）使帧
// 0x30 对齐；③loadRDARScriptFile 参数序修为 ("", path)（真语义 bug）。
// 本波 gap-3 修复（三条真缺口，实测 123=123 助记符序列全等）：
//   (a) `s == "[/hidden npc]"` 路径 ORIG 0891794b/4f 为 `movb $1,done;
//       mov $1,%esi; jmp 08917990`（统一 dtor 点后 `cmp $1,%esi; je 外层头`）
//       ⇒ 原码是 **break 内层循环**（回外层由 done 检查退出），不是 return true；
//   (b) `token != "[hidden npc]"` 的 fail 块 ORIG 物理在函数尾（089179ad）⇒
//       正向 `if(token=="[hidden npc]"){…}else{return false;}` 复现块序
//       （guard 形 `if(!=)return` 被 4.4 内联在跳转点后，实测 DIFF）；
//   (c) 同上 (b) 同时补回 ORIG 089178d8 `je 089179ad` 的尾部落点布局。
// 残余 NEAR（N98 槽序）：ORIG v@-0x14 / isType@-0x15 / isInt@-0x16 / s@-0x1c
// vs 4.4 v@-0x1c / isType@-0x11 / isInt@-0x12 / s@-0x18（4.4 按首次使用序分配、
// 4.1 按声明序；P212 全头部声明不适用于每轮构造/析构的 string s）。
// p167 MISSING `_ZSteqIcSt11char_traitsIcESaIcEE…EPKS3_`（string==const char*
// 外链实例，4.4 libstdc++ 全内联，N89）。
bool importHiddenNpcListInfo(const char* path, std::vector<int>& out)
{
    // ORIG 0x089178de：`mov 0xc(%ebp),%eax; mov %eax,0x4(%esp);
    // movl $<rodata 串>,(%esp); call loadRDARScriptFile` —— **第一参数为
    // rodata 基串、第二才是 path**（我们原写反，真语义 bug）。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x08917900 条件块 `cmpb $0x0,-0x9(%ebp);
    // jne <exit>`（内存直测、无 movzbl/xor 取反物化）⇒ 原局部为**字节标量**
    // 且条件写正向 `done == 0`（N122：bool 局部取反恒 xor，不可翻）。
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x089178f8 `movb $0x0,-0xa(%ebp)` 为
    // 第二个 dead 字节局部预清（P171 照抄），补以对齐槽集。
    bool unusedFlag = false;
    unsigned char done = 0;
    out.clear();
    for (;;)
    {
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x08917900 `cmpb $0x0,-0x9(%ebp);
        // jne <exit>`（字节局部直测、无 sete 物化）⇒ 条件写 `if (done) break`。
        if (done)
            break;
        if (!ScanType(token, true))
            return false;
        // [DIFF→AE] ORIG 089178d8 `je 089179ad`：fail 块（mov $0,%ebx;jmp exit）
        // 物理位于函数尾部（089179ad，紧邻成功返回块 089179b4）⇒ 4.4 对 guard 形
        // `if(!=) return false;` 把 fail 块内联在跳转点之后（前部，实测 DIFF）；
        // 改正向 `if(==){…} else {return false;}` 使 4.4 后置 else 块，复现块序。
        if (token == "[hidden npc]")
        {
        for (;;)
        {
            // ORIG 0x089179ad：isType/isInt 为**循环内每轮**置零
            // （movb $0,-0x15/-0x16，P171 dead-init 照抄）。
            bool isType = false;
            bool isInt = false;
            std::string s;
            // [b28d-solo-OnlyOrigBatch14] ORIG 槽 isType@-0x15/isInt@-0x16/
            // s@-0x1c/v@-0x14 ⇒ 声明序 isType/isInt 在前、s 次之、v 声明与
            // 赋值分离（4.4 按首次使用分配槽）。
            int v;
            v = ScanTypeOrInt(isType, s, isInt, 1);
            if (!isInt)
                return false;
            if (isType)
            {
                if (s == "[/hidden npc]")
                {
                    // [gap-3 修复] ORIG 0891794b/4f/54：`movb $0x1,-0x9(done);
                    // mov $0x1,%esi; jmp 08917990`（统一 dtor 点后
                    // `cmp $0x1,%esi; je 外层循环头`）⇒ 原码是 **break 内层
                    // 循环**（done=1 后回外层，由外层 done 检查退出），不是
                    // 直接 return true（后者只产 `mov $1,%ebx; mov $0,%esi`，
                    // 缺 cmp/je 两条 + 外层 nop 岛）。
                    done = 1;
                    break;
                }
                return false;
            }
            out.push_back(v);
        }
        }
        else
        {
            return false;
        }
    }
    return true;
}

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
bool importUniqueRewardForAllUsers(
    char* path,
    std::map<int, std::vector<std::vector<STStackablePackageData> > >* out)
{
    if (out == 0)
        return false;
    // [b28d-solo-OnlyOrigBatch14] 真语义 bug（同 importQuestPieceScript）：
    // ORIG 0x088ffe8d 压参序 = loadRDARScriptFile(dir="", path)。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;
    std::string name;
    // [gi00 四套配平补漏] 字段串 key 差集实证 ORIG 在本函数引用第 3 个 tag
    // `[/package]`（0x08900054 压参 + 0x08900062 比较）：其结构是 do-while 尾条件
    // `bVar2 = (token == "[/package]"); } while (!bVar2);` ⇒ 命中闭合标签即跳
    // LAB_08900072（与 ScanType 失败共用的 `return 1` 出口）。原实现缺该终止条件，
    // 会越过 `[/package]` 继续吞掉其后所有 `[group]` 块 ⇒ 可观察行为差（out 多填）。
    for (;;)
    {
        if (!ScanType(token, true))
            goto unique_reward_ok;
        if (token == "[group]")
        {
            if (!ScanStr(&name))
                return false;
            int job = getCharacterJob(name.c_str());
            std::vector<std::vector<STStackablePackageData> > jobs;
            ScanType(token, true);
            while (token == "[package selection]")
            {
                // [gap-2 修复] ORIG 088fff63-69：`lea -0x24; call ScanInt;
                // xor $1,%eax; test %al,%al; jne <跳出>`（负极性、无 for-init
                // `movl $0`）⇒ 原码是 `int itemIdx; for(;;){ if(!ScanInt(&itemIdx))
                // break; … }`，不是 `for(int itemIdx=0; ScanInt(&itemIdx); )`
                // （后者 4.4 产 for-init + 尾条件正极性 jne，少 xor、多 movl）。
                // [b14 槽序修复] P212：4.4 块内先声明占**浅**槽（4.1 相反）
                // ⇒ 把 itemIdx 声明提到 items 之前，使 items 占深槽 -0x30、
                // itemIdx 占浅槽 -0x24（与 ORIG 0x088fff58 段逐条一致）。
                int itemIdx;
                std::vector<STStackablePackageData> items;
                for (;;)
                {
                    if (!ScanInt(&itemIdx))
                    {
                        break;
                    }
                    STStackablePackageData p;
                    p.m_field0 = itemIdx;
                    p.m_field4 = ScanInt((bool*)0);
                    items.push_back(p);
                }
                jobs.push_back(items);
                ScanType(token, true);
            }
            (*out)[job] = jobs;
            // [gap-1 修复] ORIG 0890002d `jmp 08900044` → jobs dtor →
            // 0890004f `jmp 088ffeca`（循环头）⇒ 原码 group 块尾是 **continue**
            // （跳过 "[/package]" 检查直接回循环头；语义等价：token=="[group]"
            // 时 "[/package]" 恒 false）。4.4 对无 continue 的形让 dtor 块
            // 直落 "[/package]" 比较块，少这条 jmp。
            continue;
        }
        if (token == "[/package]")
            break;
    }
unique_reward_ok:
    return true;
}

// ORIG T 0x08a358d7 _Z15importPetScriptP9PetScriptPKc。
bool importPetScript(PetScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_petScriptBaseDirectory, path))
        return false;
    script->clear();
    getDirectoryPath(script->m_directoryPath, "%s%s",
                     g_petScriptBaseDirectory, path);
    std::string token;
    std::string tmp;
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x08a359 段在 token/tmp 构造后立即
    // `movl $0,-0x14(v); movb $0,-0x15(ok)`（P212 全头部声明）⇒ v/ok 提到函数头。
    // [b28d-solo-OnlyOrigBatch14 判词·AE 归零（300=300）] 本波两修：
    // ①P223/P59：ScanType 失败块改 `goto pet_script_done` 尾标签（ORIG
    // 0x08a359ab `xor $1; test; jne <函数尾共享块>`；4.4 对 if-body 恒内联
    // 为 je 直落）；②P167：[int data] 循环改单 call 点
    // `for(;;){ v=ScanInt(&ok); if(!ok) break; … }`（ORIG 仅 1 条 ScanInt
    // call 指令，我方 for-init+increment 有 2 条）。v/ok 提升 + 声明序按
    // ORIG 0x08a35 段对齐。
    int v = 0;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
            // [b28d-solo-OnlyOrigBatch14] P223/P59：ORIG 0x08a359ab 段
            // `xor $1; test; jne <函数尾共享块>` ⇒ 失败块在尾部，用 goto
            // 尾标签复现（4.4 对 if-body 恒内联为 je 直落）。
            goto pet_script_done;
        if (token == "[name]")
        {
            ScanStr(&script->m_name);
        }
        else if (token == "[width]")
        {
            script->m_width1 = ScanInt((bool*)0);
            script->m_width2 = ScanInt((bool*)0);
        }
        else if (token == "[floating height]")
        {
            script->m_floatingHeight = ScanInt((bool*)0);
        }
        else if (token == "[layer]")
        {
            ScanStr(&tmp);
            script->m_layer = getDrawLayer(tmp.c_str());
        }
        else if (token == "[recovery time]")
        {
            script->m_recoveryTime = ScanInt((bool*)0);
        }
        else if (token == "[direction correct]")
        {
            script->m_directionCorrect = ScanInt((bool*)0) != 0;
        }
        else if (token == "[move speed]")
        {
            script->m_moveSpeed[0] = ScanInt((bool*)0);
            script->m_moveSpeed[1] = ScanInt((bool*)0);
            script->m_moveSpeed[2] = ScanInt((bool*)0);
            script->m_moveSpeed[3] = ScanInt((bool*)0);
            script->m_moveSpeed[4] = ScanInt((bool*)0);
        }
        else if (token == "[gravity]")
        {
            script->m_gravity = ScanInt((bool*)0);
        }
        else if (token == "[basic motion]")
        {
            ScanStr(&script->m_basicMotion);
        }
        else if (token == "[etc motion]")
        {
            script->m_etcMotion.clear();
            while (ScanStr(&tmp))
            {
                script->m_etcMotion.push_back(tmp);
            }
        }
        else if (token == "[attack info]")
        {
            script->m_attackInfo.clear();
            while (ScanStr(&tmp))
            {
                script->m_attackInfo.push_back(tmp);
            }
        }
        else if (token == "[int data]")
        {
            script->m_intData.clear();
            // [b28d-solo-OnlyOrigBatch14] P167：ORIG 该段仅 **1 条** ScanInt
            // call 指令（`call; mov %eax,-0x14; movzbl -0x15; xor $1; test;
            // je <exit>`），我方 for 形有 2 条（init+increment）⇒ 改
            // 单点 `for(;;){ v=ScanInt(&ok); if(!ok) break; … }`。
            for (;;)
            {
                v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_intData.push_back(v);
            }
        }
        else if (token == "[string data]")
        {
            script->m_stringData.clear();
            while (ScanStr(&tmp))
            {
                script->m_stringData.push_back(tmp);
            }
        }
    }
pet_script_done:
    script->m_idx0 = g_petScriptFileList.FindIndexByFullScan(path);
    return true;
}

// ORIG T 0x08a3dbaa _ZN9PetScript5clearEv。
void PetScript::clear()
{
    m_idx0 = 0;
    m_name = "";
    m_width1 = 0;
    m_width2 = 0;
    m_floatingHeight = 0;
    m_layer = 1;
    m_recoveryTime = 0;
    m_directionCorrect = true;
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x08a3dc18 循环体为 `m_moveSpeed[i]` +
    // `m_moveSpeed[i+2]`（无 *2 索引 ⇒ 无 lea 2i），且循环变量双初始化
    // （0x08a3dc08/0x08a3dc0f 两条 movl $0,-0xc，P191 配套）；ORIG 随后清
    // +0x30 = m_moveSpeed[4]（我方原缺失，真语义缺口），末尾才清 +0x34 m_gravity。
    int i = 0;
    for (i = 0; i <= 1; ++i)
    {
        m_moveSpeed[i] = 0;
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x08a3dc28 形 `mov i,%edx; ...
        // add $0x8,%edx; movl $0,0x8(%eax,%edx,4)`（+2 折进 disp）；4.4 恒把
        // +2 折进索引 `lea 0x2(%eax),%edx`（P30/N4 索引常量分配差），
        // `(m_moveSpeed+2)[i]` 形更劣（73）。⇒ N 封顶。
        m_moveSpeed[i + 2] = 0;
    }
    m_moveSpeed[4] = 0;
    m_basicMotion = "";
    m_etcMotion.clear();
    m_attackInfo.clear();
    m_intData.clear();
    m_stringData.clear();
    m_directoryPath = "";
    m_gravity = 0;
}

// ORIG T 0x0890a334 _Z27importUltimateSkillCutScenePKcRSt6vectorI23STUltimateSkillCutSceneSaIS2_EERi：
// 空体（mov $0,%eax; ret）。
int importUltimateSkillCutScene(const char*,
                                std::vector<STUltimateSkillCutScene>&, int&)
{
    return 0;
}

// ORIG T 0x088c1e68 _Z18importActiveStatusR14STActiveStatusb
// （报告缺失，按 §3.2 以 ORIG objdump 全量转写；两侧 AE 文本已逐段核对）。
// 结构：flag 入参 spill、type 与 16 个 push 结果各占独立槽（ORIG -0x6c/-0x18/
// -0x58..-0x1c，循环计数 -0x14/-0x10/-0xc）。4.4 槽按声明序自 -0xc 加深 ⇒
// **逆序声明** 对齐 ORIG 深度。
void importActiveStatus(STActiveStatus& st, bool flag)
{
    // [b28d-solo-OnlyOrigBatch14 判词·N 封顶（N82/N93 槽序）；gap 34→32 本波]
    // 16 处 push_back 用 `static_cast<int&&>(tX)` ⇒ 选中 ORIG 的
    // `_ZNSt6vectorIiSaIiEE9push_backEOi`（右值重载，P315），p167 CLEAR 42=42。
    // 本波修：删 `bool f = flag;`（ORIG 0x088c1e96 的 `mov %al,-0x6c` 是 4.1
    // 对 bool 形参的一次性截断 spill，P244 配套；写 f 令 4.4 多发
    // `movzbl -0xac; mov %al,-0x9` 两条并推深帧），ours 295→293。
    // 残差（N 封顶）：
    //  ①32 条 mov = 16 个 push_back × 2：ORIG `call ScanInt; mov %eax,-0xNN(tX);
    //    lea -0xNN; call push_backEOi`（ScanInt 结果直落 tX 槽、实参取 tX 址）；
    //    4.4 对 `static_cast<int&&>(具名 int)` 额外 materialize 临时
    //    （`mov %eax,-0xNN2; mov -0xNN2,%eax; mov %eax,-0xNN` + `lea 临时`），
    //    每处 +2 条；探针：改块内声明/去 cast 均改符号或槽序，不闭合；
    //  ②槽深序：ORIG s(-0x5c)/t5A..t4(-0x58..-0x1c)/type(-0x18)/i9(-0x14)/
    //    iD(-0x10)/i4(-0xc)，4.4 兄弟块内同类临时槽方向相反（b28c C28L8）
    //    且帧 sub 0xc0 vs ORIG `add 负立即数`(0x80)。
    // 控制流/全部 ScanInt 落点与 ORIG 逐段一致。
    int i4;
    int iD;
    int i9;
    int t4, tD, t9, tB, t6, t2, tA, t11, tF, t7, tCB, tCA, t0B, t0A, t5B, t5A;
    int type;
    std::string s;
    // [gap 复核] 删除原 `bool f = flag;`：ORIG 0x088c1e96 `mov 0xc(%ebp),%eax;
    // mov %al,-0x6c(%ebp)` 是 4.1 对 **bool 形参**的一次性截断 spill（P244
    // 配套），不是第二个具名局部；写 f 会让 4.4 多发 `movzbl -0xac; mov %al,-0x9`
    // 两条（形参 spill + f 初始化）并把帧推大 0x40。

    ScanStr(&s);
    type = getActiveStatusType(s);
    st.m_type = type;
    st.m_value4 = ScanFloat((bool*)0);
    st.m_value8 = ScanInt((bool*)0);
    if (flag)
    {
        st.m_flagC = ScanInt((bool*)0) != 0;
    }
    st.m_value10 = ScanInt((bool*)0);
    if (type == 5)
    {
        t5A = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(t5A));
        t5B = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(t5B));
    }
    else if (type == 0)
    {
        t0A = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(t0A));
        t0B = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(t0B));
    }
    else if (type == 0xc)
    {
        tCA = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(tCA));
        tCB = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(tCB));
    }
    else if (type == 7)
    {
        t7 = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(t7));
    }
    else if (type == 0xf)
    {
        tF = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(tF));
    }
    else if (type == 0x11)
    {
        t11 = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(t11));
    }
    else if (type == 0xa)
    {
        tA = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(tA));
    }
    else if (type == 2)
    {
        t2 = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(t2));
    }
    else if (type == 6)
    {
        t6 = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(t6));
    }
    else if (type == 0xb)
    {
        tB = ScanInt((bool*)0);
        st.m_list14.push_back(static_cast<int&&>(tB));
    }
    else if (type == 9)
    {
        for (i9 = 0; i9 <= 3; ++i9)
        {
            t9 = ScanInt((bool*)0);
            st.m_list14.push_back(static_cast<int&&>(t9));
        }
    }
    else if (type == 0xd)
    {
        for (iD = 0; iD <= 3; ++iD)
        {
            tD = ScanInt((bool*)0);
            st.m_list14.push_back(static_cast<int&&>(tD));
        }
    }
    else if (type == 4)
    {
        for (i4 = 0; i4 <= 3; ++i4)
        {
            t4 = ScanInt((bool*)0);
            st.m_list14.push_back(static_cast<int&&>(t4));
        }
    }
}

// ============================================================================
// 4) 成员/类函数
// ============================================================================

// ORIG T 0x089e82f6 _ZN12STTileScript5ClearEv。
void STTileScript::Clear()
{
    m_actions.clear();
    m_flagC = 0;
    m_collisions.clear();
    m_field1c = 0;
}

// ORIG W 0x0844d584 _ZN21STBreakAwayMemberData5clearEv。
void STBreakAwayMemberData::clear()
{
    m_field00 = 0;
    memset(m_name, 0, 0x1f);
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0;
    m_field34 = 0;
    m_field38 = 0;
    m_field3c = 0;
    m_field40 = 0;
}

// ORIG T 0x085e709e _ZN15MissionLessThanclEPK8CMissionS2_：
// 先比 MissionKind 再比 MissionIndex。
bool MissionLessThan::operator()(const CMission* a, const CMission* b)
{
    if (a->GetMissionKind() != b->GetMissionKind())
    {
        return a->GetMissionKind() < b->GetMissionKind();
    }
    if (a->GetMissionIndex() != b->GetMissionIndex())
    {
        return a->GetMissionIndex() < b->GetMissionIndex();
    }
    return false;
}

// ORIG T 0x081b5bf2 _ZNK23NeedMaterialDataManager15getNeedMaterialEi：
// getNeedMaterial(G_CDataManager()->find_item(i))。
CItem* NeedMaterialDataManager::getNeedMaterial(int index) const
{
    return getNeedMaterial(G_CDataManager()->find_item(index));
}

// ORIG T 0x0816d454 _ZN16RestrictCategory12enumToStringENS_4EnumE。
const char* RestrictCategory::enumToString(Enum e)
{
    static const char* const enumStringRestrict[4] = {
        "NullPolicy",
        "AuthMobilePolicy",
        "CreateCharacterPolicy",
        "SecurityPolicy"
    };
    return enumStringRestrict[e];
}

// （importKeyNameIndexTable 已按 R3 批准迁至 ScriptExtras.cpp —— 判词随行迁走。）

// ORIG T 0x0808aa37 _Z16WriteCodeSectionPKc：读取 PE 头后写回 .code.bin。
void WriteCodeSection(const char* path)
{
    // [b28d-solo-OnlyOrigBatch14] 槽序/零初始化按 ORIG：e_lfanew 无初始化（ORIG
    // 0x0808aa41 只清零 -0x18/-0x1c/-0x20 三槽），其余三变量声明序 = 槽浅→深
    // = sizeOfCode(+0x2c)/sizeOfHeaders(+0x1c)/baseOfCode(+0x34)。
    int e_lfanew;
    int sizeOfCode = 0;
    // [b28d-solo-OnlyOrigBatch14] sizeOfHeaders 声明 unsigned：fwrite 第 2 实参为
    // size_t，int 源会多一条 `mov mem,%eax; mov %eax,%edx` 转换中转（ORIG
    // 0x0808abec 单段 `mov -0x1c(%ebp),%edx`；P265 unsigned 恒单段）。
    unsigned int sizeOfHeaders = 0;
    int baseOfCode = 0;
    FILE* f = fopen(path, "rb");
    if (f != 0)
    {
        fseek(f, 0x3c, 0);
        fread(&e_lfanew, 4, 1, f);
        fseek(f, e_lfanew + 0x2c, 0);
        fread(&sizeOfCode, 4, 1, f);
        fseek(f, e_lfanew + 0x34, 0);
        fread(&baseOfCode, 4, 1, f);
        fseek(f, e_lfanew + 0x1c, 0);
        fread(&sizeOfHeaders, 4, 1, f);
        fclose(f);
    }
    char buf[0x100];
    strncpy(buf, path, 0x100);
    // [b28d-solo-OnlyOrigBatch14] P303：ORIG 0x0808ab98 段是 strcat 内建展开
    // （strlen + lea (%ebx,%eax,1) + mov (%esp),%eax 双中转 + memcpy），必须
    // `__builtin_strcat`；手写 memcpy(dst+strlen(dst),...) 恒 DIFF。
    __builtin_strcat(buf, ".code.bin");
    FILE* out = fopen(buf, "wb");
    if (out != 0)
    {
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x0808abec：size=sizeOfHeaders(0x4(%esp))、
        // nmemb=1(0x8(%esp))，即 `fwrite(ptr, size, 1, fp)`（与我们的 1/size 反序）。
        fwrite((const void*)(sizeOfCode + baseOfCode), sizeOfHeaders, 1, out);
        fclose(out);
    }
}

// ORIG T 0x089bbe1e _Z32getActivePropertyTypeToEnumValueRSs：
// 函数局部 static map + SpinLock 惰性填充；键 "slow resistance" 等 16 项。
int getActivePropertyTypeToEnumValue(std::string& s)
{
    // ORIG 用 less_TCHAR（strcmp 内容比较）：查找键为 s.c_str() 动态指针，
    // 默认 std::less<const char*>（指针比较）必然 miss → 恒返回 17（错误）。
    // ORIG 16 处登记各只有两次 call：先就地构造 map::value_type
    // （_ZNSt4pairIKPKciEC1IRA<N>_S0_iEEOT_OT0_ = pair<const char* const,int>
    //  的 pair<char const(&)[N], int> 模板构造），再 insert(value_type const&)。
    // 用 make_pair 会多出 pair<const char*,int> → value_type 的转换临时。
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    for (;;)
    {
    init_retry:
        // ORIG 的 continue 直接跳回 +0xc（__lookup 的 __cxa_guard 检查），
        // 说明三个函数级静态声明在循环体内部。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it = __lookup.find(s.c_str());
            v = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            v = 0;
        }
        // ORIG 0x89bc019 起为 cmp/je 判定树（因含 case 0 值域稀疏），
        // case 0（未初始化态）与首个 case 0x2c4b 合并；每个 case 体在
        // !__inited 时登记一条键值后**贯穿**到下一 case 继续登记，
        // 直到落入 default 释放锁并置位 __inited 重跑（ORIG +0x919）。
        switch (v)
        {
        case 0:
        case 0x2c4b:
            if (!__inited) { __lookup.insert(LookupMap::value_type("slow resistance", 0x2c4b)); }
            else return 0;
            /* fall through */
        case 0x2c4d:
            if (!__inited) { __lookup.insert(LookupMap::value_type("freeze resistance", 0x2c4d)); }
            else return 1;
            /* fall through */
        case 0x2c4f:
            if (!__inited) { __lookup.insert(LookupMap::value_type("poison resistance", 0x2c4f)); }
            else return 2;
            /* fall through */
        case 0x2c51:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stun resistance", 0x2c51)); }
            else return 3;
            /* fall through */
        case 0x2c53:
            if (!__inited) { __lookup.insert(LookupMap::value_type("curse resistance", 0x2c53)); }
            else return 4;
            /* fall through */
        case 0x2c55:
            if (!__inited) { __lookup.insert(LookupMap::value_type("blind resistance", 0x2c55)); }
            else return 5;
            /* fall through */
        case 0x2c57:
            if (!__inited) { __lookup.insert(LookupMap::value_type("lightning resistance", 0x2c57)); }
            else return 6;
            /* fall through */
        case 0x2c59:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stone resistance", 0x2c59)); }
            else return 7;
            /* fall through */
        case 0x2c5b:
            if (!__inited) { __lookup.insert(LookupMap::value_type("sleep resistance", 0x2c5b)); }
            else return 8;
            /* fall through */
        case 0x2c5d:
            if (!__inited) { __lookup.insert(LookupMap::value_type("burn resistance", 0x2c5d)); }
            else return 9;
            /* fall through */
        case 0x2c5f:
            if (!__inited) { __lookup.insert(LookupMap::value_type("weapon break resistance", 0x2c5f)); }
            else return 10;
            /* fall through */
        case 0x2c61:
            if (!__inited) { __lookup.insert(LookupMap::value_type("bleeding resistance", 0x2c61)); }
            else return 11;
            /* fall through */
        case 0x2c63:
            if (!__inited) { __lookup.insert(LookupMap::value_type("piercing resistance", 0x2c63)); }
            else return 12;
            /* fall through */
        case 0x2c65:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stuck resistance", 0x2c65)); }
            else return 13;
            /* fall through */
        case 0x2c67:
            if (!__inited) { __lookup.insert(LookupMap::value_type("confuse resistance", 0x2c67)); }
            else return 14;
            /* fall through */
        case 0x2c69:
            if (!__inited) { __lookup.insert(LookupMap::value_type("hold resistance", 0x2c69)); }
            else return 15;
            /* fall through */
        default:
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                // [b28d-solo-OnlyOrigBatch14] ORIG 0x89bc75d 段 `movb $1,__inited;
                // jmp +0xc（循环头）`；`continue` 在 4.4 下经 switch 尾再绕两条
                // 中继 jmp（ours 多 2 条）。goto 循环头标签直接产单条 jmp。
                goto init_retry;
            }
            return 17;
        }
    }
}
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
void FAIR_PVP_SKILL_TREE_SKILL_PER_POINT::clear()
{
    m_addSkills.clear();
    m_removeSkills.clear();
    m_giveSkills.clear();
    m_pointCost = 0;
}
// ORIG C1 0x08a5e7e6：构造成员后调 clear()。
FAIR_PVP_SKILL_TREE_SKILL_PER_POINT::FAIR_PVP_SKILL_TREE_SKILL_PER_POINT()
{
    clear();
}

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
int ImportDifficultPenaltyInfoScript(const char* path, DifficultPenaltyInfo& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x1bdb;
    }
    std::string token;
    // ORIG 0x0890a33e：flag(-0x15) 为**函数级未初始化 bool**，ScanType 成功后
    // 每轮无条件置 true（0890a39a movb $1），内层 ScanInt(&flag) 复用同槽；
    // v(-0xc) 每轮内层先置 0（0890a3c4 movl $0）再 ScanInt 落值。
    bool flag;
    int i;
    int v;
    // [b28d-solo-OnlyOrigBatch14] P164：ORIG 0x0890a378 条件在**循环头**（入口
    // jmp 条件块 + body 尾 jmp 回条件），4.4 对 while 恒旋转为尾测 ⇒ 改
    // `for(;;){ if(!ScanType) break; ... }` 复现头部条件 + xor$1 物化。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        flag = true;
        if (token == "[COMMON TRAP EXPLOSION]")
        {
            for (i = 0; i <= 4; ++i)
            {
                v = 0;
                v = ScanInt(&flag);
                if (flag)
                {
                    out.m_values[i] = v;
                }
            }
        }
    }
    return 0;
}

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
int ImportTileScript(STTileScript* script, const char* path)
{
    // [b28d-solo-OnlyOrigBatch14；gap 5→0 / DIFF→AE 本波收敛（208=208）]
    // 旧判词 loadRDARScriptFile 首参有误：ORIG 0x089dcbc5 实证为空串，已改
    // `loadRDARScriptFile("", path)`。本波三条真缺口（直方图 jmp-1/nop-3/xor-1）：
    //   (a) 主 token 循环 ORIG 089dcc3a-3f `xor $1,%eax; test %al,%al;
    //       jne <退出>` = 负极性 `for(;;){ if(!ScanType(token,true)) break; }`
    //       （4.4 对 while(ScanType) 恒发 test+jne 正极性，少 xor）；
    //   (b) ORIG 089dcc21-26 两个空 else 占位岛（`jmp 循环头; nop; jmp 循环头;
    //       nop`，4.1 if-join 岛）随 (a) 的 break 落点与 if/else 结构自然复现；
    //   (c) ORIG 089dce41 的 break 出口 nop 随 (a) 复现。
    // 另修 P0 槽合并：ORIG 089dcd80 `movl $0,-0xc` 与 089dcdcf `movl $1,-0xc`
    // 复用同一槽 ⇒ 原码是**同一个** `int i`（死初始化 + for(i=1;…)），
    // 拆成 rowIdx+i 两个局部会多占 4B 使全帧槽深移（旧判词 N98 槽序残差即此）。
    // 收敛后 208=208 助记符序列全等、槽号逐条对齐（AE）。
    if (!loadRDARScriptFile("", path))
    {
        printf("TILE LOAD FAIL! [%s]\n", path);
        return 0;
    }
    script->Clear();

    // ORIG 089dcc3f/089dcc47：Clear 后即写 bool ok=false、int n=0 ——
    // 两临时为**函数级**（[pass type] 段共用其槽 -0x11/-0x18）。
    bool ok = false;
    int n = 0;
    std::string token;
    std::string str;
    // [gap-5 修复] ORIG 089dcc35-cc3f：`call ScanType; xor $1,%eax; test %al,%al;
    // jne <退出>` = 负极性 `if(!ScanType) break`（4.4 对 while(ScanType) 恒发
    // test+jne 正极性，少 xor 一条）；退出点 089dce41 的 nop 为 4.1 break 出口占位。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        if (token == "[IMAGE]")
        {
            std::pair<std::string, int> entry;
            ScanStr(&entry.first);
            entry.second = ScanInt((bool*)0);
            script->m_actions.push_back(entry);
        }
        else if (token == "[tile type]")
        {
            ScanStr(&str);
            if (str == "[basic]")
            {
                script->m_flagC = 0;
            }
            else if (str == "[extended]")
            {
                script->m_flagC = 1;
            }
        }
        else if (token == "[img pos]")
        {
            script->m_field1c = ScanInt((bool*)0);
        }
        else if (token == "[pass type]")
        {
            script->m_collisions.clear();
            // ORIG 089dcd80：clear 后即 `movl $0x0,-0xc(%ebp)`，且 089dcdcf
            // `movl $0x1,-0xc(%ebp)`（for 初值）**复用同一槽** ⇒ 原码是同一个
            // 具名 int i（死初始化 + for(i=1;…) 复用），不是两个局部（P0）。
            int i = 0;
            (void)i;
            // ORIG 089dcd8d：row vector<int> 在循环**外**构造一次（跨轮
            // 复用，loop top 089dcd92 直落 ScanInt）；行内 089dcdfb 显式
            // row.clear()；首个 ok 检查 fail → je 089dce1b 跳统一 dtor 岛
            // （+0x265 nop;D1;jmp 尾），循环体不回跳 dtor。
            std::vector<int> row;
            for (;;)
            {
                n = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                row.clear();
                row.push_back(n);
                for (i = 1; i <= 0xd; ++i)
                {
                    row.push_back(ScanInt((bool*)0));
                }
                script->m_collisions.push_back(row);
            }
        }
    }
    return 1;
}



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
bool ImportPassiveObjectScript(STPassiveObjectScript* script, const char* path,
                               bool bRepair)
{
    // [BigPassiveObj-wave10 2026-09-09 判词] §6.2 大件专项：本波按「call 锚点间隔
    // 逐段」法定位真缺口（p167 对 995/995 全等仍漏检「缺返回值检查」类缺口）。
    // 本波 4 项修复（均 ORIG 逐地址实证，fresh check_tu_routed 四数不回退）：
    //   ① **12 处 ScanStr 返回值检查缺失**（真语义缺口，ORIG 每处为
    //      `call; xor $0x1,%eax; test %al,%al; je; mov $0x0,%ebx; jmp`）：
    //      0x8a2cba3(m_name) / 0x8a2cd88(str→m_i18) / 0x8a2ceb7(m_s17c) /
    //      0x8a2d065(str→[pass type]) / 0x8a2dd57(m_s44) / 0x8a2ddd7(m_s54) /
    //      0x8a2e276(m_s64) / 0x8a2e2f6(m_s68) / 0x8a2e75f(str→m_i94) /
    //      0x8a314bc(m_s154) / 0x8a320dc(vs) / 0x8a3350b(ns)
    //      ⇒ 全改 `if (!ScanStr(&x)) return false;`。修后 ORIG 25 处 ScanStr
    //      调用与重建一一对应（23 带检查 + 2 裸调用），本波前为 12/13。
    //   ② **1 处多余检查删除**：0x8a34e8b（[passive object type] 的 ScanStr）
    //      ORIG call 后直落 `movl $0x2,-0x28` + guard 检查（无 xor/test/je）
    //      ⇒ 原码为裸调用，旧译文多写 5 条。
    //   ③ **m_i270 落盘缓冲形态**：ORIG 用局部 -0x28（初值 2 于 0x8a34e90；
    //      两个子 token 的 else 分支改写 0/1 于 0x8a350fd/0x8a35163；公共尾
    //      0x8a35190 `movl -0x28 → this+0x270`）。旧译文直接 3 次成员写（含
    //      3 次 script 指针重载 + 重复 if-else 比较）⇒ 改具名 `t270` 后 −7 insn。
    //   ④ **P304**：`reinterpret_cast<STScriptFileList*>(&g_stPassiveObjectScriptFileList)`
    //      （0x8a3579b）令 4.4 物化 `mov $imm,%eax; mov %eax,(%esp)` 并挤占 %eax；
    //      extern 声明改权威型 STScriptFileList（NpcScript.h:61，+0 即 map）后
    //      复现 ORIG `movl $imm,(%esp)`，−1 insn。
    // 【本波读数】clean（normdiff/compare_tu 口径）orig 8417 / ours 8370→**8426**
    //   （gap +47→**−9**）；raw objdump 8490 / 8452→**8512**（+38→−22）；
    //   p167 calls **995=995**（MISSING/EXTRA 仅 C1↔C2 / D1↔D2 同址镜像，P93 豁免）；
    //   check_tu_routed 四数 identical=391 ae=782 near=0 diff=20 / missing=189
    //   **与基线逐项相同（不回退）**。
    // 【残余 −9 归因（逐块 classify 后）】均非语义缺口：
    //   a) 我方多 5：`info.m_ints44[i]` 的 4.4 寻址展开 `add $0x10; 槽往返; shl $2;
    //      add base`（ORIG 3 条 scaled `mov v,-0xdc0(%ebp,%edx,4)`）——N125/N117 族；
    //   b) 我方多 24（ORIG 同形 12 条在 5167 处）：内层子表 case 叶块物理序 +
    //      4.4 `cmpl $imm,<S>` 内存比较 vs ORIG `mov <S>,%eax; cmp $imm,%eax`
    //      （P324 叶块序 / P209 无符号比较恒载寄存器）；
    //   c) ORIG 多 4（序言 `movl $0,<S>; mov $0,%eax; mov %eax,<S>; movb $0,<S>`）
    //      = ORIG 源码 3 个死局部（P171 死写入），补回会使 |gap| 反向增大 ⇒ 不补；
    //   d) 其余 ±1~3 为 call 间隔边界的错位对（同一段代码在两侧落于相邻 call 之间）。
    //   ⇒ 剩余全部属 N82/N125/N117/P324 跨版本布局族；语义已逐段核对闭合。
    // [b28d-solo-OnlyOrigBatch14] §6.2 超大件（orig 8490 insn）：本波以
    // game_func_report gap（8490-8452=38）+ 调用点差集 + 逐块比对为验收口径
    // （ndiff 对超大件失真，N82）。18 处 static map 惰性注册链、token 分派树与
    // ORIG 逐段对齐；残余为 4.1/4.4 帧槽复用策略差（N82 族）。
    // [BigGM2-wave9 双判据复核（Main 2026-09-09 口径：gap+call 多重集为主，①不作派工
    // 序）] triage ①「26 段/1537 行」经逐项取证判为**布局假性缺口**，无未翻译语义：
    //   ① 指令缺口 38/8490 = 0.45%（形族：false 岛 `mov $0x0,%eax` 我方 37 vs ORIG 1
    //     + lea+add 拆分寻址）；② call 多重集 **995/995 逐名全等**（含 pair ctor 模板
    //     实参按 key 长度分桶 IRA{5:1,7:8,…,16:13,…,42:1} **129 键直方图逐一相等**，
    //     ScanInt 72/72、ScanStr 25/25、ScanTypeOrStr 9/9、insert 129/129、
    //     guard 三件套 36/36、$0xffffffff(v=-1) 22/22）⇒ 18 张 static map 注册内容与
    //     全部分派/失败动作 1:1 在场。①段实体 = ORIG 建表体连续段 + 异常 dtor island
    //     与我方分散 case 落码的排布差（N82/N126 族）。判 N 封顶，不再照 ① 行数硬补。
    // 【验收标注 §6.2】超大函数（ORIG 8490 insn 切片）。调用点差集归零
    // （995=995，ORIG 侧 libstdc++/PLT 无符号地址已聚类反查对位）；
    // leaf id 125/125（五子表 case-0 修复后跳表 2→0，注册轮 id=0 由
    // ORIG 树根 test/je 实证路由链首，P163）。残余 gap -38 归因：
    // ①0x2152/0x2193/0x21e1/0x2241 嵌套块内 4.1 提前装载 + 叶局部
    // string 临时槽物化（N76/N82 槽号系）；②0x227e 族「子表 switch
    // 注册 + 体外 if(id) 执行」等价重排（语义同、cmp 形每位 ±2）。
    // 语义已逐块核对完成（子表 26 tag 字段解析修复前从未执行）。
    if (!loadRDARScriptFile(g_szPassiveObjectScriptBaseDirectory, path))
    {
        return false;
    }
    script->clear();
    script->m_destroy.clear();
    script->m_addEffect.clear();
    getDirectoryPath(script->m_s90, "%s%s",
                     g_szPassiveObjectScriptBaseDirectory, path);

    std::string token;
    std::string str;
    bool nameSet = false;   // ORIG -0x8d（[name] 已读标志）
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        int v;
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        for (;;)
        {
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                v = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                v = 0;
            }
            switch (v)
            {
            case 0:
            case 0x1e0d:   // // [name] → m_name；m_layer = m_name；nameSet = true
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[name]", 0x1e0d));
                }
                else
                {
                    // ORIG 0x8a2cba3：ScanStr 后 `xor $1; test %al; je` ⇒ 原码检查返回值。
                    if (!ScanStr(&script->m_name))
                    {
                        return false;
                    }
                    script->m_layer = script->m_name;
                    nameSet = true;
                        break;
                }
                /* FALL THROUGH */
            case 0x1e16:   // // [width] → m_width、m_height
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[width]", 0x1e16));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_width = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_height = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e1f:   // // [floating height] → m_i14
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[floating height]", 0x1e1f));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i14 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e24:   // // [layer] → getDrawLayer → m_i18
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[layer]", 0x1e24));
                }
                else
                {
                    // ORIG 0x8a2cd88：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&str))
                    {
                        return false;
                    }
                    script->m_i18 = getDrawLayer(str.c_str());
                        break;
                }
                /* FALL THROUGH */
            case 0x1e29:   // // [layer level] → m_i15c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[layer level]", 0x1e29));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i15c = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e2e:   // // [burning death path] → m_s17c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[burning death path]", 0x1e2e));
                }
                else
                {
                    // ORIG 0x8a2ceb7：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s17c))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e32:   // // [is hp by difficulty] → m_b158
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[is hp by difficulty]", 0x1e32));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b158 = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e37:   // // [hp gage] → m_b159
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[hp gage]", 0x1e37));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b159 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e3c:   // // [pass type] → m_i1c（子映射 0..7）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[pass type]", 0x1e3c));
                }
                else
                {
                {
                    // ORIG 0x8a2d065：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&str))
                    {
                        return false;
                    }
                    int pt;
                    static LookupMap __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    for (;;)
                    {
                        if (__inited)
                        {
                            LookupMap::const_iterator it = __lookup.find(str.c_str());
                            pt = (it != __lookup.end()) ? it->second : -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            pt = 0;
                        }
                        // ORIG 0x8a2d271 分派链：整数 switch；未登记轮由每个 case 头
                        // 登记自身键值后贯穿下一 case（0x8a2d353/0x8a2d3bf/0x8a2d42b/…），
                        // 链末落 0x8a2d64d（default）→ 0x8a2d662 公共尾解锁置位后重查。
                        switch (pt)
                        {
                        case 0:
                        case 0x1e42:   // [pass all] → m_i1c = 0
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass all]", 0x1e42));
                                goto __t_1e45;   // ORIG 0x8a2d353
                            }
                        {
                            script->m_i1c = 0;
                            break;
                        }
                        __t_1e45:
                        case 0x1e45:   // [pass only float type] → m_i1c = 1
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only float type]", 0x1e45));
                                goto __t_1e48;   // ORIG 0x8a2d3bf
                            }
                        {
                            script->m_i1c = 1;
                            break;
                        }
                        __t_1e48:
                        case 0x1e48:   // [do not pass] → m_i1c = 2
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[do not pass]", 0x1e48));
                                goto __t_1e4b;   // ORIG 0x8a2d42b
                            }
                        {
                            script->m_i1c = 2;
                            break;
                        }
                        __t_1e4b:
                        case 0x1e4b:   // [pass only red team] → m_i1c = 3
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only red team]", 0x1e4b));
                                goto __t_1e4e;   // ORIG 0x8a2d497
                            }
                        {
                            script->m_i1c = 3;
                            break;
                        }
                        __t_1e4e:
                        case 0x1e4e:   // [pass only blue team] → m_i1c = 4
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only blue team]", 0x1e4e));
                                goto __t_1e51;   // ORIG 0x8a2d503
                            }
                        {
                            // ORIG 0x8a2d505：写 4 之后**无 break**，贯穿到 0x8a2d50f
                            // （[pass only character team] 的 case 头），登记轮已完成，
                            // 故直接执行其分支体把 m_i1c 覆盖为 5 —— ORIG 真实行为。
                            script->m_i1c = 4;
                        }
                        __t_1e51:
                        case 0x1e51:   // [pass only character team] → m_i1c = 5
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only character team]", 0x1e51));
                                goto __t_1e54;   // ORIG 0x8a2d56a
                            }
                        {
                            script->m_i1c = 5;
                            break;
                        }
                        __t_1e54:
                        case 0x1e54:   // [pass only monster team] → m_i1c = 6
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only monster team]", 0x1e54));
                                goto __t_1e59;   // ORIG 0x8a2d5d6
                            }
                        {
                            script->m_i1c = 6;
                            break;
                        }
                        __t_1e59:
                        case 0x1e59:   // [pass only character] → m_i1c = 7
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only character]", 0x1e59));
                                break;   // 登记链末 → ORIG 0x8a2d63f 直落公共尾
                            }
                        {
                            script->m_i1c = 7;
                            break;
                        }
                        default:   // ORIG 0x8a2d64d：已登记 → 返回 false；登记轮 → 落尾
                            if (__inited)
                            {
                                return false;
                            }
                            break;
                        }
                        // ORIG 0x8a2d662 子表公共尾。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1e62:   // // [piercing power] → m_i20
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[piercing power]", 0x1e62));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i20 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e67:   // // [piercing tolerance] → m_i24
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[piercing tolerance]", 0x1e67));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i24 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e6c:   // // [category] → m_category（getObjectCategory；0x2c 跳过）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[category]", 0x1e6c));
                }
                else
                {
                {
                    script->m_category.clear();
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/category]")
                            {
                                return false;
                            }
                            break;
                        }
                        const int cat = getObjectCategory(s);
                        if (cat == 0x2c)
                        {
                            continue;
                        }
                        script->m_category.push_back(cat);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1e83:   // // [basic motion] → m_s34（必须 .ani；非法扩展名 → false）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[basic motion]", 0x1e83));
                }
                else
                {
                {
                    std::string s;
                    if (!ScanStr(&s))
                    {
                        return false;
                    }
                    // ORIG 0x8a2da37：esi = 1 ^ isValidExtension；valid==0 → 失败返回 0。
                    if (!isValidExtension(s, ".ani"))
                    {
                        return false;
                    }
                    script->m_s34 = s;
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1e8f:   // // [etc motion] → m_basicActions
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[etc motion]", 0x1e8f));
                }
                else
                {
                {
                    script->m_basicActions.clear();
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/etc motion]")
                            {
                                return false;
                            }
                            break;
                        }
                        // ORIG 0x8a2dc2a：esi = 1 ^ isValidExtension；valid==0 → 失败返回 0。
                        if (!isValidExtension(s, ".ani"))
                        {
                            return false;
                        }
                        script->m_basicActions.push_back(s);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1ea9:   // // [basic action] → m_s44
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[basic action]", 0x1ea9));
                }
                else
                {
                    // ORIG 0x8a2dd57：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s44))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ead:   // // [last action] → m_s54
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[last action]", 0x1ead));
                }
                else
                {
                    // ORIG 0x8a2ddd7：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s54))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1eb1:   // // [etc action] → m_etcActions
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[etc action]", 0x1eb1));
                }
                else
                {
                {
                    script->m_etcActions.clear();
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/etc action]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_etcActions.push_back(s);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1ec6:   // // [under gravity] → m_b58
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[under gravity]", 0x1ec6));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b58 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ecc:   // // [vanish on move collision] → m_b61=1、m_b60
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[vanish on move collision]", 0x1ecc));
                }
                else
                {
                    script->m_b61 = 1;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b60 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ed3:   // // [trap] → m_b5a
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[trap]", 0x1ed3));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b5a = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ed9:   // // [allow friend hit] → m_b62
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[allow friend hit]", 0x1ed9));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b62 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1edf:   // // [team] → m_i5c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[team]", 0x1edf));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i5c = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ee5:   // // [destroy particle] → m_s64
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[destroy particle]", 0x1ee5));
                }
                else
                {
                    // ORIG 0x8a2e276：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s64))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ee9:   // // [attack info] → m_s68
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[attack info]", 0x1ee9));
                }
                else
                {
                    // ORIG 0x8a2e2f6：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s68))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1eed:   // // [etc attack info] → m_attackInfos
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[etc attack info]", 0x1eed));
                }
                else
                {
                {
                    script->m_attackInfos.clear();
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/etc attack info]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_attackInfos.push_back(s);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1f01:   // // [int data] → m_intData（ScanTypeOrInt 循环）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[int data]", 0x1f01));
                }
                else
                {
                {
                    std::string s;
                    script->m_intData.clear();
                    for (;;)
                    {
                        bool isType = false;
                        bool isInt = false;
                        const int val = ScanTypeOrInt(isType, s, isInt, 1);
                        if (!isInt)
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/int data]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_intData.push_back(val);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1f18:   // // [string data] → m_stringData
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[string data]", 0x1f18));
                }
                else
                {
                {
                    std::string s;
                    script->m_stringData.clear();
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/string data]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_stringData.push_back(s);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1f2e:   // // [quest type] → m_i94
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[quest type]", 0x1f2e));
                }
                else
                {
                {
                    // ORIG 0x8a2e75f：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&str))
                    {
                        return false;
                    }
                    if (str == "[none]")
                    {
                        script->m_i94 = 0;
                    }
                    else if (str == "[must be destroyed]")
                    {
                        script->m_i94 = 1;
                    }
                    else if (str == "[chaos object]")
                    {
                        script->m_i94 = 2;
                    }
                    else
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1f3c:   // // [hp max] → m_i98
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[hp max]", 0x1f3c));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i98 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1f42:   // // [hp destroy] → m_b59
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[hp destroy]", 0x1f42));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b59 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1f48:   // // [object destroy condition] → m_destroy 各子字段
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[object destroy condition]", 0x1f48));
                }
                else
                {
                {
                    // ORIG 0x8a2e926：段内 while(ScanTypeOrStr) 逐词解析（非主循环 token
                    // 喂入）。行内子 token 词以反引号字符串形式出现（isType==false，
                    // 内容形如 "[destroy condition]"），段尾 [/object destroy condition]
                    // 为裸 TYPE 词；未知子 token 忽略继续（ORIG 0x8a2f2f3 出口）。
                    std::string w;
                    std::string w2;   // ORIG -0xc34：构造/析构但从未使用的空串槽
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, w, true))
                        {
                            break;   // EOF → 回主循环（主循环随后 EOF 统一收尾）
                        }
                        if (isType)
                        {
                            if (w != "[/object destroy condition]")
                            {
                                return false;
                            }
                            break;
                        }
                        int dv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(w.c_str());
                                dv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                dv = 0;
                            }
                        switch (dv)
                        {
                        case 0x1f58:   // [destroy condition] → m_type（子 token 0..4 / -1）
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[destroy condition]", 0x1f58));
                                goto __reg_1f7a;   // ORIG 0x8a2ec87：登记后贯穿下一 case
                            }
                        {
                            if (!ScanStr(&str))
                            {
                                return false;
                            }
                            if (str == "[on end of animation]")
                            {
                                script->m_destroy.m_type = 0;
                            }
                            else if (str == "[z pos equal zero]")
                            {
                                script->m_destroy.m_type = 1;
                            }
                            else if (str == "[time limite]")
                            {
                                script->m_destroy.m_type = 2;
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    script->m_destroy.m_i3c = v;
                                }
                            }
                            else if (str == "[not destroy and z pos equal zero]")
                            {
                                script->m_destroy.m_type = 3;
                            }
                            else if (str == "[parent dead]")
                            {
                                script->m_destroy.m_type = 4;
                            }
                            else
                            {
                                script->m_destroy.m_type = -1;
                            }
                            break;
                        }
                        __reg_1f7a:
                        case 0x1f7a:   // [destroy action] → m_flag04
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[destroy action]", 0x1f7a));
                                goto __reg_1f8b;   // ORIG 0x8a2ee2c
                            }
                        {
                            if (!ScanStr(&str))
                            {
                                return false;
                            }
                            if (str == "[only destroy]")
                            {
                                script->m_destroy.m_flag04 = 0;
                            }
                            else if (str == "[object create after destroy]")
                            {
                                script->m_destroy.m_flag04 = 1;
                            }
                            else
                            {
                                script->m_destroy.m_flag04 = -1;
                            }
                            break;
                        }
                        __reg_1f8b:
                        case 0x1f8b:   // [on attack] → m_i40
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[on attack]", 0x1f8b));
                                goto __reg_1f9c;   // ORIG 0x8a2ef1a
                            }
                        {
                            if (!ScanStr(&str))
                            {
                                return false;
                            }
                            if (str == "[only destroy]")
                            {
                                script->m_destroy.m_i40 = 0;
                            }
                            else if (str == "[object create after destroy]")
                            {
                                script->m_destroy.m_i40 = 1;
                            }
                            else
                            {
                                script->m_destroy.m_i40 = -1;
                            }
                            break;
                        }
                        __reg_1f9c:
                        case 0x1f9c:   // [add object index] → m_ints08 × n（ORIG push 0xe0，无 reserve）
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[add object index]", 0x1f9c));
                                goto __reg_1fa8;   // ORIG 0x8a2f008
                            }
                        {
                            int n;
                            if (!ScanInt(&n))
                            {
                                return false;
                            }
                            for (int i = 0; i < n; ++i)
                            {
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    script->m_destroy.m_ints08.push_back(v);
                                }
                            }
                            break;
                        }
                        __reg_1fa8:
                        case 0x1fa8:   // [add object particle file] → reserve + m_strs14 × n
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[add object particle file]", 0x1fa8));
                                goto __reg_1fb7;   // ORIG 0x8a2f0f7
                            }
                        {
                            int n;
                            if (!ScanInt(&n))
                            {
                                return false;
                            }
                            script->m_destroy.m_strs14.reserve(n);
                            for (int i = 0; i < n; ++i)
                            {
                                std::string s;
                                if (!ScanStr(&s))
                                {
                                    return false;
                                }
                                script->m_destroy.m_strs14.push_back(s);
                            }
                            break;
                        }
                        __reg_1fb7:
                        case 0x1fb7:   // [add object index on attack] → reserve + m_ints44 × n
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[add object index on attack]", 0x1fb7));
                                break;   // 登记链末：ORIG 0x8a2f249 直落公共尾 0x8a2f2f3
                            }
                        {
                            int n;
                            if (!ScanInt(&n))
                            {
                                return false;
                            }
                            script->m_destroy.m_ints44.reserve(n);
                            for (int i = 0; i < n; ++i)
                            {
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    script->m_destroy.m_ints44.push_back(v);
                                }
                            }
                            break;
                        }
                        default:   // 未知子 token → ORIG 忽略继续（0x8a2f2f3 出口）
                            break;
                        }
                        // ORIG 0x8a2f2f3 子表公共尾：登记轮结束后解锁、置位并重查本 token。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1fc8:   // [sound category] → m_strs20/m_ints2c（5 槽）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[sound category]", 0x1fc8));
                }
                else
                {
                {
                    script->m_destroy.m_strs20.clear();
                    script->m_destroy.m_ints2c.clear();
                    script->m_destroy.m_strs20.resize(5, std::string());
                    script->m_destroy.m_ints2c.resize(5, 0);
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/sound category]")
                            {
                                return false;
                            }
                            break;
                        }
                        int slot;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                slot = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                slot = 0;
                            }
                            // ORIG 0x8a2f72c：5 个类别 token 各占一个 case，登记轮由
                            // case 头登记自身后贯穿下一 case（0x8a2f7d2/0x8a2f83b/…），
                            // 真轮只写 slot 局部（-0x70），统一落到 0x8a2f99b 公共尾后
                            // 由后面的共享体读名字与音量。
                            switch (slot)
                            {
                            case 0:
                            case 0x1fe2:   // [after object create] → slot 0
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[after object create]", 0x1fe2));
                                    goto __c_1fe5;   // ORIG 0x8a2f7d2
                                }
                            {
                                slot = 0;
                                break;
                            }
                            __c_1fe5:
                            case 0x1fe5:   // [on frame] → slot 1
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[on frame]", 0x1fe5));
                                    goto __c_1fe8;   // ORIG 0x8a2f83b
                                }
                            {
                                slot = 1;
                                break;
                            }
                            __c_1fe8:
                            case 0x1fe8:   // [on attack] → slot 2
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[on attack]", 0x1fe8));
                                    goto __c_1feb;   // ORIG 0x8a2f8a4
                                }
                            {
                                slot = 2;
                                break;
                            }
                            __c_1feb:
                            case 0x1feb:   // [on damage] → slot 3
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[on damage]", 0x1feb));
                                    goto __c_1fee;   // ORIG 0x8a2f90d
                                }
                            {
                                slot = 3;
                                break;
                            }
                            __c_1fee:
                            case 0x1fee:   // [on destroyobject] → slot 4
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[on destroyobject]", 0x1fee));
                                    break;   // 登记链末 → ORIG 0x8a2f976 直落公共尾
                                }
                            {
                                slot = 4;
                                break;
                            }
                            default:   // ORIG 0x8a2f981：已登记 → false；登记轮 → 落尾
                                if (__inited)
                                {
                                    return false;
                                }
                                break;
                            }
                            // ORIG 0x8a2f99b 子表公共尾。
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        if (slot < 0)
                        {
                            return false;
                        }
                        // ORIG 在类别 token 后另读一个字符串作为声音文件名；
                        // 类别本身只用于选择 5 槽索引，不能写入数据槽。
                        if (!ScanStr(&str))
                        {
                            return false;
                        }
                        script->m_destroy.m_strs20[slot] = str;
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_destroy.m_ints2c[slot] = v;
                        }
                        if (slot == 1)   // [on frame] 额外 [on frame] 值 → m_i38
                        {
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_destroy.m_i38 = v;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x200a:   // // [add object effect] → m_addEffect
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[add object effect]", 0x200a));
                }
                else
                {
                {
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/add object effect]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_addEffect.m_strs00.push_back(s);
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_addEffect.m_ints0c.push_back(v);
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2022:   // // [add particles] → m_particles
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[add particles]", 0x2022));
                }
                else
                {
                {
                    std::string s1;
                    std::string s2;
                    std::string s3;
                    for (;;)
                    {
                        if (!ScanType(s1, true))
                        {
                            return false;
                        }
                        if (s1 == "[/add particles]")
                        {
                            break;
                        }
                        // ORIG 0x8a2fd0c/0x8a2fd31：eq "[particle]" 命中才进入对象构造，
                        // 其它外层 token 被忽略并继续（ORIG je 回循环头）。
                        if (!(s1 == "[particle]"))
                        {
                            continue;
                        }
                        STPassiveObjectAdditionalParticleInfo* obj =
                            new STPassiveObjectAdditionalParticleInfo();
                        for (;;)
                        {
                            if (!ScanType(s2, true))
                            {
                                // ORIG 0x8a2fd87：失败直接走出口，不 delete obj。
                                return false;
                            }
                            if (s2 == "[/particle]")
                            {
                                break;
                            }
                            int pv;
                            static LookupMap __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            for (;;)
                            {
                                if (__inited)
                                {
                                    LookupMap::const_iterator it = __lookup.find(s2.c_str());
                                    pv = (it != __lookup.end()) ? it->second : -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    pv = 0;
                                }
                            switch (pv)
                            {
                            case 0:
                            case 0x203e:   // [file] → m_s00
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[file]", 0x203e));
                                    goto __p_2043;   // ORIG 0x8a2d…登记链贯穿
                                }
                                if (!ScanStr(&s3))
                                {
                                    return false;
                                }
                                obj->m_s00 = s3;
                                break;
                            __p_2043:
                            case 0x2043:   // [create delay] → m_i0c
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[create delay]", 0x2043));
                                    goto __p_2048;
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    obj->m_i0c = v;
                                }
                                break;
                            __p_2048:
                            case 0x2048:   // [create sound] → m_s04
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[create sound]", 0x2048));
                                    goto __p_204d;
                                }
                                if (!ScanStr(&s3))
                                {
                                    return false;
                                }
                                obj->m_s04 = s3;
                                break;
                            __p_204d:
                            case 0x204d:   // [follow parent] → m_s08 + 循环写 m_i10..m_i18
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[follow parent]", 0x204d));
                                    goto __p_2057;
                                }
                                obj->m_s08 = s2;
                                // ORIG 0x8a3023b..0x8a3028a：单 ScanInt 调用点，
                                // 循环 i=0..2 索引写 obj+0x10+i*4。
                                for (int i = 0; i < 3; ++i)
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    int* base = &obj->m_i10;
                                    base[i] = v;
                                }
                                break;
                            __p_2057:
                            case 0x2057:   // [loop] → m_b1c
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[loop]", 0x2057));
                                    break;   // 登记链末 → ORIG 0x8a30328 公共尾
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    obj->m_b1c = (v != 0);
                                }
                                break;
                            default:
                                // ORIG 0x8a2fd33 前的失败路径同样不 delete。
                                if (__inited)
                                {
                                    return false;
                                }
                                break;
                            }
                            // ORIG 0x8a30328 子表公共尾：登记轮完成后解锁置位并重查本 token。
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                        }
                            break;
                        }
                            }
                        script->m_particles.push_back(obj);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2066:   // // [homing] → homing 字段组
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[homing]", 0x2066));
                }
                else
                {
                {
                    // ORIG 0x8a3043a：段内 while(ScanType) 逐词解析（子 token 与段尾
                    // [/homing] 均为裸 TYPE 词，非主循环喂入）；未知子 token 忽略继续
                    // （ORIG 0x8a31103：inited==1 时无操作回循环头）。
                    std::string hw;
                    for (;;)
                    {
                        if (!ScanType(hw, true))
                        {
                            break;   // EOF → 回主循环统一收尾
                        }
                        if (hw == "[/homing]")
                        {
                            break;
                        }
                        int hv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(hw.c_str());
                                hv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                hv = 0;
                            }
                        switch (hv)
                        {
                    case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                    case 0x2072:   // [homing use] → m_b9c
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing use]", 0x2072));
                            goto __h_2078;   // ORIG 0x8a30843 贯穿
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_b9c = (v != 0);
                        }
                        break;
                    __h_2078:
                    case 0x2078:   // [homing follow] → m_ia0/m_ia4
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing follow]", 0x2078));
                            goto __h_2097;   // ORIG 0x8a30a22
                        }
                    {
                        std::string fs;   // ORIG -0xc7c 局部槽
                        if (!ScanStr(&fs))
                        {
                            return false;
                        }
                        if (fs == "[ME]")
                        {
                            script->m_ia0 = 0;
                        }
                        else if (fs == "[ENEMY]")
                        {
                            script->m_ia0 = 1;
                        }
                        else if (fs == "[CHARACTER]")
                        {
                            script->m_ia0 = 3;
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_ia4 = v;
                            }
                        }
                        else if (fs == "[MONSTER]")
                        {
                            script->m_ia0 = 2;
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_ia4 = v;
                            }
                        }
                        else
                        {
                            return false;
                        }
                        break;
                    }
                    __h_2097:
                    case 0x2097:   // [homing velocity] → m_fa8/m_fac
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing velocity]", 0x2097));
                            goto __h_20a2;   // ORIG 0x8a30aea
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fa8 = v;
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fac = v;
                        }
                        break;
                    __h_20a2:
                    case 0x20a2:   // [homing check gap] → m_ib0
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing check gap]", 0x20a2));
                            goto __h_20a8;   // ORIG 0x8a30b7f
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_ib0 = v;
                        }
                        break;
                    __h_20a8:
                    case 0x20a8:   // [sync animation rotation] → m_bb4
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[sync animation rotation]", 0x20a8));
                            goto __h_20ae;   // ORIG 0x8a30c19
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_bb4 = (v != 0);
                        }
                        break;
                    __h_20ae:
                    case 0x20ae:   // [max rotation] → m_fb8
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[max rotation]", 0x20ae));
                            goto __h_20b4;   // ORIG 0x8a30cae
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fb8 = v;
                        }
                        break;
                    __h_20b4:
                    case 0x20b4:   // [init rotation] → m_fbc
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[init rotation]", 0x20b4));
                            goto __h_20ba;   // ORIG 0x8a30d43
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fbc = v;
                        }
                        break;
                    __h_20ba:
                    case 0x20ba:   // [diff rotation] → m_fc0
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[diff rotation]", 0x20ba));
                            goto __h_20c0;   // ORIG 0x8a30dd8
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fc0 = v;
                        }
                        break;
                    __h_20c0:
                    case 0x20c0:   // [homing time] → m_ic4
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing time]", 0x20c0));
                            goto __h_20c6;   // ORIG 0x8a30e6d
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_ic4 = v;
                        }
                        break;
                    __h_20c6:
                    case 0x20c6:   // [homing z axis] → m_ic8
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing z axis]", 0x20c6));
                            goto __h_20cc;   // ORIG 0x8a30f02
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_ic8 = v;
                        }
                        break;
                    __h_20cc:
                    case 0x20cc:   // [homing direction] → m_icc
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing direction]", 0x20cc));
                            goto __h_20d2;   // ORIG 0x8a30f97
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_icc = v;
                        }
                        break;
                    __h_20d2:
                    case 0x20d2:   // [homing keep distance] → m_id0
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing keep distance]", 0x20d2));
                            goto __h_20d9;   // ORIG 0x8a3102c
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_id0 = v;
                        }
                        break;
                    __h_20d9:
                    case 0x20d9:   // [sync target move] → m_bd4
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[sync target move]", 0x20d9));
                            goto __h_20de;   // ORIG 0x8a31097
                        }
                        script->m_bd4 = 1;
                        break;
                    __h_20de:
                    case 0x20de:   // [straight homing] → m_bd5
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[straight homing]", 0x20de));
                            break;   // 登记链末 → ORIG 0x8a31103 公共尾
                        }
                        script->m_bd5 = 1;
                        break;
                        default:   // 未知子 token → ORIG 忽略继续
                            break;
                        }
                        // ORIG 0x8a31103 子表公共尾。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x20e7:   // [for tournament] → m_b14c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[for tournament]", 0x20e7));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b14c = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x20ed:   // // [for school] → m_b14d
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[for school]", 0x20ed));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b14d = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x20f3:   // // [die when parent die] → m_b14e
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[die when parent die]", 0x20f3));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b14e = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x20f8:   // // [transparent on meet player] → m_ub14f
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[transparent on meet player]", 0x20f8));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_ub14f = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x20fd:   // // [pvp room transparent value for observer] → m_us150
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[pvp room transparent value for observer]", 0x20fd));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_us150 = static_cast<unsigned short>(v);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x2102:   // // [create sound] → m_s154
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[create sound]", 0x2102));
                }
                else
                {
                    // ORIG 0x8a314bc：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s154))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x2106:   // // [is step on]：仅 ScanInt 校验，值丢弃（ORIG 0x8a31533）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[is step on]", 0x2106));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x210d:   // // [can beat index] → m_onAttackData
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[can beat index]", 0x210d));
                }
                else
                {
                {
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        bool isInt = false;
                        const int val = ScanTypeOrInt(isType, s, isInt, 1);
                        if (!isInt)
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/can beat index]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_onAttackData.push_back(int(val));
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2126:   // // [is blocking obj] → m_b16c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[is blocking obj]", 0x2126));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b16c = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x212b:   // // [apply physics] → m_f170
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[apply physics]", 0x212b));
                }
                else
                {
                    {
                        float v;
                        if (!ScanFloat(&v))
                        {
                            return false;
                        }
                        script->m_f170 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x2131:   // // [following immortal] → m_i174（1；==1 时 2）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[following immortal]", 0x2131));
                }
                else
                {
                    script->m_i174 = 1;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        if (v == 1)
                        {
                            script->m_i174 = 2;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x213f:   // // [follow parent] → m_i178
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[follow parent]", 0x213f));
                }
                else
                {
                {
                    std::string ps;   // ORIG -0xc88 局部槽
                    if (!ScanStr(&ps))
                    {
                        return false;
                    }
                    if (ps == "[path gate]")
                    {
                        script->m_i178 = 1;
                    }
                    else if (ps == "[parent]")
                    {
                        script->m_i178 = 2;
                    }
                    else if (ps == "[from parent]")
                    {
                        script->m_i178 = 3;
                    }
                    else if (ps == "[keep position]")
                    {
                        script->m_i178 = 4;
                    }
                    else
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2152:   // // [speed control passive objects] → m_speedCtrls
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[speed control passive objects]", 0x2152));
                }
                else
                {
                {
                    std::string s;
                    script->m_speedCtrls.clear();
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/speed control passive objects]")
                        {
                            break;
                        }
                        int sv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                sv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                sv = 0;
                            }
                            // ORIG 0x8a31cb4 分派（test/cmp 0x2161）+ 0x8a31cc6 case 头：
                            // 登记轮在此登记自身，真轮交回下方循环体处理。
                            switch (sv)
                            {
                            case 0:
                            case 0x2161:   // [passive object]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[passive object]", 0x2161));
                                }
                                break;
                            default:   // ORIG 0x8a31cbb else 边 → 0x8a321e2 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        // ORIG 0x8a31cbb：非 [passive object] 的词直接 jmp 子表尾
                        // 0x8a321e2 → 0x8a32209 jmp 段扫描头 0x8a31a7a（忽略、继续）。
                        if (sv != 0x2161)
                        {
                            continue;
                        }
                        std::string s2;   // ORIG -0xc94：内层循环独立词槽
                        SpeedCtrlPassiveObjInfo info;
                        for (;;)
                        {
                            if (!ScanType(s2, true))
                            {
                                return false;
                            }
                            if (s2 == "[/passive object]")
                            {
                                if (info.m_field0 == -1)
                                {
                                    return false;
                                }
                                script->m_speedCtrls.push_back(info);
                                break;
                            }
                            int iv;
                            static LookupMap __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            for (;;)
                            {
                                if (__inited)
                                {
                                    LookupMap::const_iterator it = __lookup.find(s2.c_str());
                                    iv = (it != __lookup.end()) ? it->second : -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    iv = 0;
                            }
                            // ORIG 0x8a31f5d 分派 + 0x8a31fd3/0x8a32065 两个 case 头。
                            switch (iv)
                            {
                            case 0:
                            case 0x2177:   // [index]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[index]", 0x2177));
                                    goto __i6_217c;   // ORIG 0x8a32042 贯穿
                                }
                                break;
                            __i6_217c:
                            case 0x217c:   // [speed]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[speed]", 0x217c));
                                }
                                break;
                            default:   // ORIG 0x8a3217e 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                                }
                            if (iv == 0x2177)   // [index] → m_field0
                            {
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    info.m_field0 = v;
                                }
                            }
                            else if (iv == 0x217c)   // [speed] → m_b4（[up]/[down]）
                            {
                                std::string vs;   // ORIG -0xca4 局部槽
                                // ORIG 0x8a320dc：ScanStr 返回值检查（xor $1/test/je 实证）。
                                if (!ScanStr(&vs))
                                {
                                    return false;
                                }
                                if (vs == "[up]")
                                {
                                    info.m_b4 = 1;
                                }
                                else if (vs == "[down]")
                                {
                                    info.m_b4 = 0;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else
                            {
                                // ORIG 0x8a3217e 子表尾：非 [index]/[speed] 的词不报错，
                                // 0x8a321a5 jmp 段扫描头 0x8a31d46（忽略并继续）。
                                continue;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2193:   // // [objects] → m_genObjects
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[objects]", 0x2193));
                }
                else
                {
                {
                    std::string s;
                    script->m_genObjects.clear();
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/objects]")
                        {
                            break;
                        }
                        int ov;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                ov = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                ov = 0;
                            }
                            // ORIG 0x8a324b9 分派 + 0x8a32516 case 头。
                            switch (ov)
                            {
                            case 0:
                            case 0x21a1:   // [object]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[object]", 0x21a1));
                                }
                                break;
                            default:   // ORIG 0x8a32de5 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        // ORIG 0x8a324dd else 边 → 子表尾 0x8a32de5 → 回段扫描头。
                        if (ov != 0x21a1)
                        {
                            continue;
                        }
                        std::string s2;   // ORIG -0xcb0：内层循环独立词槽
                        PassiveObjGeneratorObject obj;
                        for (;;)
                        {
                            if (!ScanType(s2, true))
                            {
                                return false;
                            }
                            if (s2 == "[/object]")
                            {
                                script->m_genObjects.push_back(obj);
                                break;
                            }
                            int iv;
                            static LookupMap __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            for (;;)
                            {
                                if (__inited)
                                {
                                    LookupMap::const_iterator it = __lookup.find(s2.c_str());
                                    iv = (it != __lookup.end()) ? it->second : -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    iv = 0;
                            }
                            switch (iv)
                            {
                            case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                            case 0x21b5:   // [index] → m_i00
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[index]", 0x21b5));
                                    goto __o8_21bc;   // ORIG 0x8a3289c 贯穿
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    obj.m_i00 = v;
                                }
                                break;
                            __o8_21bc:
                            case 0x21bc:   // [probability] → m_f04
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[probability]", 0x21bc));
                                    goto __o8_21c3;   // ORIG 0x8a3292e
                                }
                                {
                                    float v;
                                    if (!ScanFloat(&v))
                                    {
                                        return false;
                                    }
                                    obj.m_f04 = v;
                                }
                                break;
                            __o8_21c3:
                            case 0x21c3:   // [warning] → ScanStr → m_s08
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[warning]", 0x21c3));
                                    break;   // 登记链末 → ORIG 0x8a32d50 公共尾
                                }
                            {
                                std::string w;
                                if (!ScanStr(&w))
                                {
                                    return false;
                                }
                                int wv;
                                static LookupMap __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                for (;;)
                                {
                                    if (__inited)
                                    {
                                        LookupMap::const_iterator it = __lookup.find(w.c_str());
                                        wv = (it != __lookup.end()) ? it->second : -1;
                                    }
                                    else
                                    {
                                        __init_lock.enter();
                                        if (__inited)
                                        {
                                            __init_lock.leave();
                                            continue;
                                        }
                                        wv = 0;
                                }
                                // ORIG 0x8a32b9f 分派 + 0x8a32bf4/0x8a32c4f/0x8a32caa 三个
                                // case 头。真轮三者都贯穿到 0x8a32d07 的同一实体
                                // （obj.m_s08 = 词），故下面只做 <0 判定后统一赋值。
                                switch (wv)
                                {
                                case 0:
                                case 0x21cb:   // none
                                    if (!__inited)
                                    {
                                        __lookup.insert(
                                            LookupMap::value_type("none", 0x21cb));
                                        goto __w9_21cc;   // ORIG 0x8a32c4f
                                    }
                                    break;
                                __w9_21cc:
                                case 0x21cc:   // danger
                                    if (!__inited)
                                    {
                                        __lookup.insert(
                                            LookupMap::value_type("danger", 0x21cc));
                                        goto __w9_21cd;   // ORIG 0x8a32caa
                                    }
                                    break;
                                __w9_21cd:
                                case 0x21cd:   // recovery
                                    if (!__inited)
                                    {
                                        __lookup.insert(
                                            LookupMap::value_type("recovery", 0x21cd));
                                        break;   // 登记链末 → ORIG 0x8a32d05 公共尾
                                    }
                                    break;
                                default:   // ORIG 0x8a32d24
                                    break;
                                }
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                                    }
                                if (wv < 0)
                                {
                                    return false;
                                }
                                obj.m_s08 = w;
                                break;
                            }
                            default:
                                return false;
                            }
                            // ORIG 子表公共尾：登记轮完成后解锁置位并重查本 token。
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x21e1:   // // [patterns] → m_genPatterns
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[patterns]", 0x21e1));
                }
                else
                {
                {
                    std::string s;
                    script->m_genPatterns.clear();
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/patterns]")
                        {
                            break;
                        }
                        int pv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                pv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                pv = 0;
                            }
                            // ORIG 0x8a33115 分派 + 0x8a33182 case 头。
                            switch (pv)
                            {
                            case 0:
                            case 0x21f0:   // [pattern]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[pattern]", 0x21f0));
                                }
                                break;
                            default:   // ORIG 0x8a3384d 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        // ORIG 0x8a33146 else 边 → 子表尾 0x8a3384d → 回段扫描头。
                        if (pv != 0x21f0)
                        {
                            continue;
                        }
                        std::string s2;   // ORIG -0xcd4：内层循环独立词槽
                        PassiveObjGeneratorPattern pat;
                        for (;;)
                        {
                            if (!ScanType(s2, true))
                            {
                                return false;
                            }
                            if (s2 == "[/pattern]")
                            {
                                script->m_genPatterns.push_back(pat);
                                break;
                            }
                            int iv;
                            static LookupMap __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            for (;;)
                            {
                                if (__inited)
                                {
                                    LookupMap::const_iterator it = __lookup.find(s2.c_str());
                                    iv = (it != __lookup.end()) ? it->second : -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    iv = 0;
                            }
                            switch (iv)
                            {
                            case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                            case 0x2203:   // [name] → m_s04
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[name]", 0x2203));
                                    goto __o11_220c;   // ORIG 0x8a334f1 贯穿
                                }
                                {
                                    std::string ns;   // ORIG -0xcf8 局部槽
                                    // ORIG 0x8a3350b：ScanStr 返回值检查（xor $1/test/je 实证）。
                                    if (!ScanStr(&ns))
                                    {
                                        return false;
                                    }
                                    pat.m_s04 = ns;
                                }
                                break;
                            __o11_220c:
                            case 0x220c:   // [interval] → m_i0c
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[interval]", 0x220c));
                                    goto __o11_2214;   // ORIG 0x8a335e6
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    pat.m_i0c = v;
                                }
                                break;
                            __o11_2214:
                            case 0x2214:   // [columns] → m_i08
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[columns]", 0x2214));
                                    goto __o11_221c;   // ORIG 0x8a33678
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    pat.m_i08 = v;
                                }
                                break;
                            __o11_221c:
                            case 0x221c:   // [data] → m_b00=0 + m_v10 byte 流
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[data]", 0x221c));
                                    break;   // 登记链末 → ORIG 0x8a337e9 公共尾
                                }
                            {
                                std::string ds;
                                pat.m_b00 = 0;
                                for (;;)
                                {
                                    bool isType = false;
                                    bool isInt = false;
                                    const int val = ScanTypeOrInt(isType, ds, isInt, 1);
                                    if (!isInt)
                                    {
                                        return false;
                                    }
                                    if (isType)
                                    {
                                        // ORIG 0x8a33786 eq + je fail → 正向等式判定。
                                        if (ds == "[/data]")
                                        {
                                            break;
                                        }
                                        return false;
                                    }
                                    unsigned char byte = static_cast<unsigned char>(val);
                                    pat.m_v10.push_back(byte);
                            }
                                break;
                        }
                            default:
                                return false;
                            }
                            // ORIG 子表公共尾。
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                        break;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2241:   // // [sequences] → m_genSequences
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[sequences]", 0x2241));
                }
                else
                {
                {
                    std::string s;
                    script->m_genSequences.clear();
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/sequences]")
                        {
                            break;
                        }
                        int sv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                sv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                sv = 0;
                            }
                            // ORIG 0x8a345f6 分派 + 0x8a346bf 单 case 头（[pattern]）。
                            switch (sv)
                            {
                            case 0:
                            case 0x2250:   // [pattern]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[pattern]", 0x2250));
                                }
                                break;
                            default:   // ORIG 0x8a34d4e 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        // ORIG 0x8a34626 else 边 → 子表尾 0x8a34d4e → 回段扫描头。
                        if (sv != 0x2250)
                        {
                            continue;
                        }
                        std::string s2;   // ORIG -0xd10：内层循环独立词槽
                        PassiveObjGeneratorSequence seq;
                        for (;;)
                        {
                            bool isType = false;
                            if (!ScanTypeOrStr(isType, s2, 1))
                            {
                                return false;
                            }
                            if (isType)
                            {
                                if (s2 != "[/pattern]")
                                {
                                    return false;
                                }
                                script->m_genSequences.push_back(seq);
                                break;
                            }
                            seq.m_s00 = s2;
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                seq.m_i04 = v;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2271:   // // [warning] → m_warnDanger/m_warnRecovery
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[warning]", 0x2271));
                }
                else
                {
                {
                    std::string s;
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/warning]")
                        {
                            break;
                        }
                        int wv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                wv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                wv = 0;
                            }
                            // ORIG 0x8a340ac 分派 + 0x8a340ed/0x8a341eb 两个 case 头。
                            switch (wv)
                            {
                            case 0:
                            case 0x227e:   // [min distance]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[min distance]", 0x227e));
                                    goto __w13_228b;   // ORIG 0x8a3417c 贯穿
                                }
                                break;
                            __w13_228b:
                            case 0x228b:   // [max distance]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[max distance]", 0x228b));
                                }
                                break;
                            default:   // ORIG 0x8a342e3 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        if (wv == 0x227e)   // [min distance] → m_warnDanger 3 int
                        {
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnDanger.m_a = v;
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnDanger.m_b = v;
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnDanger.m_c = v;
                            }
                        }
                        else if (wv == 0x228b)   // [max distance] → m_warnRecovery 3 int
                        {
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnRecovery.m_a = v;
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnRecovery.m_b = v;
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnRecovery.m_c = v;
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x22a0:   // // [sniper type] → m_sniper
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[sniper type]", 0x22a0));
                }
                else
                {
                {
                    std::string s;
                    PassiveIObjSniperInfo info;
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/sniper type]")
                        {
                            script->m_sniper = info;
                            break;
                        }
                        int sv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                sv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                sv = 0;
                        }
                        switch (sv)
                        {
                        case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                        case 0x22ae:   // [accelerate] → m_i30
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[accelerate]", 0x22ae));
                                goto __o14_22b3;   // ORIG 0x8a34738 贯穿
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i30 = v;
                            }
                            break;
                        __o14_22b3:
                        case 0x22b3:   // [init velocity] → m_i34
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[init velocity]", 0x22b3));
                                goto __o14_22b8;   // ORIG 0x8a347ca
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i34 = v;
                            }
                            break;
                        __o14_22b8:
                        case 0x22b8:   // [shot area] → m_b40=1 + 6 int → m_ints44[0..5]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[shot area]", 0x22b8));
                                goto __o14_22c1;   // ORIG 0x8a34882
                            }
                        {
                            info.m_b40 = 1;
                            // ORIG 0x8a34880：循环写对象 +0x44+i*4（i=0..5）。
                            // 栈对象基址 -0xdc4（08a34d95 传入 operator= 实证），写址
                            // -0xdc0+(i+0x10)*4 = +0x44..+0x58 → m_ints44[6]（与
                            // PassiveIObjSniperInfo::clear 同区清零循环一致）。
                            for (int i = 0; i < 6; ++i)
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_ints44[i] = v;
                            }
                            break;
                        }
                        __o14_22c1:
                        case 0x22c1:   // [shot distance] → m_b38=1、m_i3c
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[shot distance]", 0x22c1));
                                goto __o14_22c8;   // ORIG 0x8a3491b
                            }
                            info.m_b38 = 1;
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i3c = v;
                            }
                            break;
                        __o14_22c8:
                        case 0x22c8:   // [move check time] → m_i5c
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[move check time]", 0x22c8));
                                goto __o14_22cd;   // ORIG 0x8a349ad
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i5c = v;
                            }
                            break;
                        __o14_22cd:
                        case 0x22cd:   // [invoke check time] → m_i60
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[invoke check time]", 0x22cd));
                                goto __o14_22d2;   // ORIG 0x8a34a3f
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i60 = v;
                            }
                            break;
                        __o14_22d2:
                        case 0x22d2:   // [invoke delay time] → m_i64
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[invoke delay time]", 0x22d2));
                                goto __o14_22d7;   // ORIG 0x8a34ad1
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i64 = v;
                            }
                            break;
                        __o14_22d7:
                        case 0x22d7:   // [wait time] → m_i68
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[wait time]", 0x22d7));
                                goto __o14_22dc;   // ORIG 0x8a34b63
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i68 = v;
                            }
                            break;
                        __o14_22dc:
                        case 0x22dc:   // [shake area] → 3 int → m_ints74[0..2]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[shake area]", 0x22dc));
                                goto __o14_22e4;   // ORIG 0x8a34c14
                            }
                        {
                            // ORIG 0x8a34c12：循环写对象 +0x74+i*4（i=0..2）。
                            // 写址 -0xdc0+(i+0x1c)*4 = +0x74..+0x7c → m_ints74[3]
                            //（与 PassiveIObjSniperInfo::clear 同区清零循环一致）。
                            for (int i = 0; i < 3; ++i)
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_ints74[i] = v;
                            }
                            break;
                        }
                        __o14_22e4:
                        case 0x22e4:   // [invoke time] → m_i6c
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[invoke time]", 0x22e4));
                                goto __o14_22e9;   // ORIG 0x8a34ca6
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i6c = v;
                            }
                            break;
                        __o14_22e9:
                        case 0x22e9:   // [invoke count] → m_i70
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[invoke count]", 0x22e9));
                                break;   // 登记链末 → ORIG 0x8a34d4e 公共尾
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i70 = v;
                            }
                            break;
                        default:
                            return false;
                        }
                        // ORIG 0x8a34dcc 子表公共尾。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x22f6:   // // [passive object type] → m_i270
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[passive object type]", 0x22f6));
                }
                else
                {
                {
                    // ORIG 0x8a34e8b：此 ScanStr 返回值**不检查**（call 后直落
                    // `movl $0x2,-0x28` 与 guard 检查，无 xor/test/je）⇒ 原码为裸调用。
                    ScanStr(&str);
                    // ORIG -0x28 落盘缓冲：初值 2（0x8a34e90），两个子 token 的
                    // else 分支分别改写为 0/1（0x8a350fd / 0x8a35163），未知 token
                    // 不改写；公共尾 0x8a35190 统一 `movl -0x28 → this+0x270`。
                    int t270 = 2;
                    int tv;
                    static LookupMap __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    for (;;)
                    {
                        if (__inited)
                        {
                            LookupMap::const_iterator it = __lookup.find(str.c_str());
                            tv = (it != __lookup.end()) ? it->second : -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            tv = 0;
                        }
                        // ORIG 0x8a35063 分派 + 0x8a350a0/0x8a35106 两个 case 头。
                        switch (tv)
                        {
                        case 0:
                        case 0x22fd:   // [missile fairy]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[missile fairy]", 0x22fd));
                                goto __t15_2301;   // ORIG 0x8a350d3 贯穿
                            }
                            t270 = 0;   // ORIG 0x8a350fd
                            break;
                        __t15_2301:
                        case 0x2301:   // [random item fairy]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[random item fairy]", 0x2301));
                            }
                            else
                            {
                                t270 = 1;   // ORIG 0x8a35163
                            }
                            break;
                        default:   // ORIG 0x8a3516a 公共尾
                            break;
                        }
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                    }
                    // ORIG 0x8a35190 公共尾：`movl -0x28 → this+0x270`。
                    script->m_i270 = t270;
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x231a:   // // [random item fairy] → m_itemFairies
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[random item fairy]", 0x231a));
                }
                else
                {
                {
                    bool active = true;
                    RandomItemFairy fairy;
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;
                        }
                        int fv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(token.c_str());
                                fv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                fv = 0;
                        }
                        switch (fv)
                        {
                        case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                        case 0x2325:   // [/random item fairy]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[/random item fairy]", 0x2325));
                                goto __o16_232a;   // ORIG 0x8a354c2 贯穿
                            }
                            active = false;
                            break;
                        __o16_232a:
                        case 0x232a:   // [entry]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[entry]", 0x232a));
                                goto __o16_232f;   // ORIG 0x8a35532
                            }
                            fairy.clear();
                            break;
                        __o16_232f:
                        case 0x232f:   // [/entry]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[/entry]", 0x232f));
                                goto __o16_2334;   // ORIG 0x8a355af
                            }
                            script->m_itemFairies.push_back(fairy);
                            break;
                        __o16_2334:
                        case 0x2334:   // [passiveobject] → 4 int 直写 fairy（ORIG 无检查）
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[passiveobject]", 0x2334));
                                break;   // 登记链末 → ORIG 0x8a3565d 公共尾
                            }
                            // ORIG 0x8a3561c..0x8a35658：四个 ScanInt 直写 fairy 字段，
                            // 返回值不检查（失败仅留下未初始化字段）。
                            ScanInt(&fairy.m_i00);
                            ScanInt(&fairy.m_ints04[0]);
                            ScanInt(&fairy.m_ints04[1]);
                            ScanInt(&fairy.m_ints04[2]);
                            break;
                        default:   // 未知子 token → ORIG 忽略继续（0x8a3565d：inited 后无操作回循环头）
                            break;
                        }
                        // ORIG 0x8a3565d 子表公共尾。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                        if (!active)
                        {
                            break;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x234c:   // // [passive object icon] → m_b280/m_s284 + 尾部两 int
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[passive object icon]", 0x234c));
                }
                else
                {
                    script->m_b280 = 1;
                    ScanStr(&script->m_s284);
                    // ORIG 0x8a35715/0x8a35725：m_s284 之后连续两次 ScanInt 写
                    // this+0x288（m_i288）与 this+0x28c（m_i28c，主类末 int）。
                    // ORIG 0x8a35715/0x8a35725：ScanInt 直写两字段，返回值不检查。
                    ScanInt(&script->m_i288);
                    ScanInt(&script->m_i28c);
                        break;
                }
                /* FALL THROUGH */
            default:
                if (__inited)
                {
                    return false;
                }
                break;
                return false;
            }
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                continue;
            }
            break;
        }

        // 每 case 处理后的公共出口（ORIG 0x8a3577e）：bRepair 且 [name] 已读
        // 时提前结束；否则回到主 token 循环。
        if (bRepair && nameSet)
        {
            break;
        }
    }

    // ORIG 0x8a3579b：`movl $&g_stPassiveObjectScriptFileList,(%esp);
    // mov 0xc(%ebp),%eax; mov %eax,0x4(%esp); call FindIndexByFullScan`
    // ⇒ 权威型成员直调（P304），勿再 cast。
    script->m_idx = g_stPassiveObjectScriptFileList.FindIndexByFullScan(path);
    return true;
}

// ===== _Z17GetSlicedIntervalPlS_S_S_lll —— ORIG 0x085732d8 =====
// 切片区间工具：arg6>0 时写 [0,arg6] 到 out1/out2（count=1）；
// 若 arg7+arg6 < arg5（总长），第二段 [arg7+arg6, arg5-(arg7+arg6)] 写
// out3/out4（count==0 时写 out1/out2），count 增 1。返回段数。
long GetSlicedInterval(long* out1, long* out2, long* out3, long* out4,
                       long total, long firstLen, long step)
{
    long count = 0;
    if (firstLen > 0)
    {
        *out1 = 0;
        *out2 = firstLen;
        // b28c-l23：ORIG 此处为 `addl $0x1,-0x4(%ebp)`（自增，非赋值）。
        ++count;
    }
    if (firstLen + step < total)
    {
        if (count == 0)
        {
            *out1 = firstLen + step;
            *out2 = total - (firstLen + step);
        }
        else
        {
            *out3 = firstLen + step;
            *out4 = total - (firstLen + step);
        }
        count += 1;
    }
    return count;
}

// ===== _Z24CheckDayScheduleTimeOveril —— ORIG 0x0846c370 =====
// time(&local)（ORIG 0x0846c37a `lea -0xc(%ebp),%eax; call time` 传局部地址，
// 非 time(NULL)）+ day * (-86400) > checkTime（0xfffeae80 == -86400 补码）。
bool CheckDayScheduleTimeOver(int day, long checkTime)
{
    time_t now;
    time(&now);
    return now + static_cast<long>(day) * (-86400L) > checkTime;
}

// ===== _Z29DecreaseTradeLimitCountByUserR5CUserii —— ORIG 0x0812017d =====
// curCharac 缺失 → false；GetInvenRef(space, slot) 缺失 → false；
// 否则委托 DecreaseTradeLimitCount(Inven_Item&)。
bool DecreaseTradeLimitCountByUser(CUser& user, int space, int slot)
{
    if (user.getCurCharacR() == 0)
    {
        return false;
    }
    // ORIG getCurCharacInvenW 结果寄存器直传（无槽，P0 内联右值）。
    Inven_Item* item = user.getCurCharacInvenW()->GetInvenRef(space, slot);
    if (item == 0)
    {
        return false;
    }
    return DecreaseTradeLimitCount(*item);
}

// ===== _Z18GiveItemReturnUserP5CUser —— ORIG 0x084bfd28 =====
// find_item(0x1f15) 失败直接返回；make_item 填局部 Inven_Item、set_add_info(1)。
// 按最后游玩 dungeon 是否存在分两路拼邮件：正文
// findString(4,"return_user_msg_1",0) + snprintf("%s", dungeonName) 或
// findString(4,"return_user_msg_2",0)；发件人 findString(4,"return_user_msg_title",0)；
// ReqDBSendNewSystemMail(title, item, 0, characNo, body, strlen(body), 7,
//                        serverGroup, false, false)。
void GiveItemReturnUser(CUser* user)
{
    // [b14 复核·AE 归零（145=145，gap 0）] 三项修复（旧判词 N82/N115 作废）：
    // ①P329/P0：两处 `findString(4,"return_user_msg_title",0)` 原写成具名
    //   `const char* title` 局部 ⇒ 求值被提前到 GetServerGroup 之前且多一槽；
    //   内联回 ReqDBSendNewSystemMail 首实参位后，4.4 按 cdecl 右→左求值复现
    //   ORIG 0x084bfe31 段顺序（GetServerGroup→strlen→getCurCharacNo→findString）。
    // ②删除 dead `char pad[8]`（title 内联后槽自然重排，item 落 -0x69 与 ORIG
    //   一致，且不再多发 `movb $0x0,-0x34(%ebp)`）；帧 0x19c / push edi 亦自动对齐。
    // ③P223 多岛形：两个 return 改「两条独立 goto + 尾标签串」
    //   （`if(def==0) goto ret1;` / `if(dungeon==0) goto ret2;` + 尾
    //   `ret1:; ret2:;`）⇒ 复现 ORIG 0x084bff7f 的 `nop;jmp` 与 0x084bff82 的
    //   `nop` 双岛（4.4 对裸 return 恒 jump-thread 掉第一个 jmp）。
    // 虚调用核对（P193/P282）：`def->make_item(item)` ⇒ `mov (%eax),%eax;
    // add $0x8,%eax; mov (%eax),%edx; call *%edx` = CItem vptr+8 = vtable 槽 @2；
    // ORIG `_ZTV5CItem`@0x08c96da0 word[4]=0x08724fe0（纯虚）与 CItem.h 声明序
    // （~CItem D1/D0 占 @0/@1，make_item @2）一致，且与 ORIG 逐条同偏移。
    int itemIdx;
    itemIdx = 0x1f15;
    int nAddInfo;
    nAddInfo = 1;
    Inven_Item item;
    CItem* def = G_CDataManager()->find_item(itemIdx);
    if (def == 0)
    {
        goto give_item_ret1;
    }
    item.m_addInfo = itemIdx;
    def->make_item(item);
    item.set_add_info(nAddInfo);

    if (user->getCurCharacLastPlayDungeonIndex() != 0)
    {
        CDungeon* dungeon =
            G_CDataManager()->find_dungeon(user->getCurCharacLastPlayDungeonIndex());
        if (dungeon == 0)
        {
            goto give_item_ret2;
        }
        char buf[0x100];
        OS_API::snprintf(
            buf, 0xff,
            g_scriptStringManager_.findString(4, "return_user_msg_1", 0),
            dungeon->GetDungeonName());
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0),
            item, 0, user->getCurCharacNo(), buf,
            static_cast<unsigned int>(strlen(buf)), 7,
            user->GetServerGroup(), false, false);
    }
    else
    {
        const char* body =
            g_scriptStringManager_.findString(4, "return_user_msg_2", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0),
            item, 0, user->getCurCharacNo(), body,
            static_cast<unsigned int>(strlen(body)), 7,
            user->GetServerGroup(), false, false);
    }
give_item_ret1:;
give_item_ret2:;
}

// ===== _Z38GetInstanceDeathTowerPlaydataStatisticv —— ORIG 0x0860f451 =====
// ORIG：mov $0x94733a8 直返（.bss 静态实例，无 guard）。重建以文件级
// static 对象承载同一地址语义。
CDeathTowerPlayDataStatistic* GetInstanceDeathTowerPlaydataStatistic()
{
    static CDeathTowerPlayDataStatistic s_instance;
    return &s_instance;
}
