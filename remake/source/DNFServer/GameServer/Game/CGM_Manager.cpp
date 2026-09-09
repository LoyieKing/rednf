#include "MySQL.h"          // MySQL::{set_query,exec,get_n_rows,fetch,get_int}
#include "GameTypes.h"      // cMyTrace / rwLock / writeGuard
#include "CGM_Manager.h"
#include <stdio.h>          // NumberToString（W8LT_DBTask TU 同款局部声明）

char* NumberToString(unsigned int value, int radix);   // ORIG T 0x081090b4

// ORIG C1/C2 0x082990b4：movb $0,(+0)；movl $0,(+4)（函数体赋值形态，无 ecx 缓存）。
CGMRecord::CGMRecord()
{
    m_mode = 0;
    m_level = 0;
}

// ORIG D1/D2 0x082990ca：空析构（用户定义，生成 T 符号）。
CGMRecord::~CGMRecord()
{
}

// ORIG 符号: _ZN11CGM_Manager4IsGmEj @ 0x829948c
// readGuard(m_lock)；find 结果与 end() 用 operator== 比较，相等（命中）返回 true。
// [B24D判词 N] 语义正确：readGuard+find，it==end() 返回 true 否则 false（ORIG
//   operator==）；diff=mov $0x0/mov $0x1 分支汇聚顺序互换（GCC4.1 vs 4.4），
//   55/55 纯 lowering，语义等价。
bool CGM_Manager::IsGm(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    // ORIG 0x082994ee：`test %al,%al; je +0x6b(mov $1)` ⇒ al==0（find 未命中）
    // 返回 **1**；命中返回 0。原实现 `if(it==end()) return true;` 语义与 ORIG
    // **相反**（真 bug：命中 GM 反而返回 false）——按 ORIG 汇编改正极性。
    if (it == m_gmMap.end())
        return false;
    return true;
}

// ORIG 符号: _ZN11CGM_Manager10TurnGmModeEj @ 0x829917c
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 翻转 +0 模式字节并返回新值。
// [b28d-solo-CGM_Manager 判词 AE] ORIG 81 = our 81（compare_tu AE 口径）。旧 B24D
//   「N」与 b28c-13「N18a」判词作废：ORIG 0x0829923e `movzbl (%eax),%ebx` 直落
//   callee-saved 可由「register bool 局部 + return」复现（P231 纯存储类提示，
//   非绑定形；详见函数体内注释与 /tmp/b28d_s78_probe1.cpp f_regbool）。
unsigned char CGM_Manager::TurnGmMode(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    if (m_gmMap[accId].m_mode)
        m_gmMap[accId].m_mode = 0;
    else
        m_gmMap[accId].m_mode = 1;
    // b28d-solo-CGM_Manager [P231 纯 register + bool 局部]：ORIG 0x0829923e
    // `movzbl (%eax),%ebx`（零扩展直落 callee-saved 返回值缓存）。4.4.7 探针
    // （/tmp/b28d_s78_probe1.cpp f_regbool）实证：`register bool` 局部 + `return mode;`
    // 即直落 %ebx；`register unsigned char` 恒多一条 `mov %eax,%ebx`。纯 register
    // 存储类提示非绑定形（P0 合规，禁 `register asm`）。
    register bool mode = m_gmMap[accId].m_mode;
    return mode;
}

// ORIG 符号: _ZN11CGM_Manager16GetCurrentGmModeEj @ 0x8299276
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 读 +0 模式字节。
// [b28d-solo-CGM_Manager 判词 AE] ORIG 60 = our 60（compare_tu AE 口径）。旧 B24D
//   「N」与 b28c-13「N18a」判词作废：同 TurnGmMode，register bool 局部即直落 %ebx。
bool CGM_Manager::GetCurrentGmMode(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    // b28d-solo-CGM_Manager [P231 纯 register + bool 局部]：ORIG 0x082992f3
    // `movzbl (%eax),%ebx` 直落；探针 f_regbool 同法（register bool + return 即直落，
    // register char 恒多 `mov %eax,%ebx`）。纯 register 提示非绑定形（P0 合规）。
    register bool mode = m_gmMap[accId].m_mode;
    return mode;
}

// ORIG 符号: _ZN11CGM_Manager10GetGmLevelEj @ 0x082993d8
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 读 +0x04。
int CGM_Manager::GetGmLevel(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    return m_gmMap[accId].m_level;
}

// ORIG 符号: _ZN11CGM_Manager10SetGmLevelEji @ 0x0829932a
// writeGuard(m_lock)；find 命中后经 operator[] 写 +0x04（m_level）。无返回值。
void CGM_Manager::SetGmLevel(unsigned int accId, int level)
{
    writeGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it != m_gmMap.end())
    {
        m_gmMap[accId].m_level = level;
    }
}

// ORIG 符号: _ZN11CGM_Manager16LoadGmDataFromDbEj @ 0x08299740
// set_query("seLect level from gm_manifest where m_id = %s", NumberToString(accId,0))
// → exec(true) 失败：cMyTrace(__PRETTY_FUNCTION__, 0x15a2, 0) 记
// "Failed to get GM list from DB." 返 false；get_n_rows()==0 返 false；
// writeGuard(m_lock) 内 fetch() 一次，get_int(0, m_gmMap[accId].m_level)，
// 等级为 0：cMyTrace(__PRETTY_FUNCTION__, 0x15b4, 0) 记
// "GM: %s exists in GM manifest but doen't have proper level. check DB" 返 false；
// 否则返回 exec 结果。
// [b28d-solo-CGM_Manager 判词 AE] ORIG 123 = our 123（compare_tu AE 口径）。旧
//   b28d-37「N104/N82 封顶」作废，复现三钥匙：①guard 用显式作用域块——块内
//   `return false;`（失败臂析构前 `mov $0,%ebx; mov $0,%esi`）、块外 `return ok;`
//   （成功路径析构后才 `movzbl -0xd,%ebx`），4.4 即发 ORIG 的 %esi 选择器形；
//   ②t2 vararg 用全表达式临时链 `cMyTrace(...)(fmt, NumberToString(...))`（P227：
//   实参在 ctor 前求值并跨 ctor 保活 %ebx；具名两语句形实参序反）；③两把
//   cMyTrace 槽 -0x30/-0x20 与 guard -0x34 随①②归位。注：game_func_report 在
//   .o 阶段仍报 DIFF（未解析重定位标签显示差），以 compare_tu_game_orig.py 为准。
bool CGM_Manager::LoadGmDataFromDb(unsigned int accId)
{
    bool ok = false;                       // ORIG 槽 -0xd
    int rows = 0;                          // ORIG 槽 -0xc
    m_db->set_query("seLect level from gm_manifest where m_id = %s",
                    NumberToString(accId, 0));
    ok = m_db->exec(true);
    if (!ok)
    {
        cMyTrace t1(__PRETTY_FUNCTION__, 0x15a2, 0);   // ORIG 槽 -0x30（块内局部）
        t1("Failed to get GM list from DB.");
        return false;
    }
    rows = m_db->get_n_rows();
    if (rows == 0)
    {
        return false;
    }
    // b28d-solo-CGM_Manager [P227+P59]：ORIG 0x08299815 guard 槽 -0x34（最深），失败臂
    // 在析构**前**装 `mov $0,%ebx`、成功路径在析构**后**才 `movzbl -0xd,%ebx` ⇒ 源码
    // 是「块内 guard + 块内 return false + 块外 return ok」。t2 的 vararg 在 ctor 前
    // 求值并跨 ctor 保活 %ebx（P227 全表达式临时链；具名两语句形为反序）。
    {
        writeGuard<rwLock> guard(&m_lock);     // ORIG 槽 -0x34（函数级）
        m_db->fetch();
        ok = m_db->get_int(0, m_gmMap[accId].m_level);
        if (m_gmMap[accId].m_level == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x15b4, 0)(
                "GM: %s exists in GM manifest but doen't have proper level. check DB",
                NumberToString(accId, 0));
            return false;
        }
    }
    return ok;
}
