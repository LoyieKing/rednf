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
// [B24D判词 N] 语义正确：readGuard+find 未命中 0，operator[] 翻转 0/1 返回新模式；
// diff=ORIG movzbl(%eax),%ebx 直存 vs 我方 movzbl(%eax),%eax;mov %eax,%ebx（多1条，
//   P 寄存器分配），81/82 纯 lowering，语义等价。
// [b28c-13 N18a] 复核：ORIG 0x0829923e `movzbl (%eax),%ebx`（零扩展直落
// callee-saved 返回值缓存）；4.4 恒 `movzbl (%eax),%eax; mov %eax,%ebx`。裸 return/
// 语句表达式/显式 cast 三形实测同形（P226 语句表达式仅对 call 结果升格有效）。
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
    // b28d-37 [N18a 复核]：ORIG 0x0829923e `movzbl (%eax),%ebx`（零扩展直落
    // callee-saved 返回值缓存）；4.4 恒 `movzbl (%eax),%eax; mov %eax,%ebx`。
    // P248 档③字段侧（m_mode bool→uchar）、P226 语句表达式、P231 纯 register
    // 三形均已实测同形 ⇒ N18a 封顶（P0 禁绑定，不再试）。
    register unsigned char mode = m_gmMap[accId].m_mode;
    return mode;
}

// ORIG 符号: _ZN11CGM_Manager16GetCurrentGmModeEj @ 0x8299276
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 读 +0 模式字节。
// [B24D判词 N] 语义正确：readGuard+find 未命中 0，operator[] 读 +0 字节；
// diff=movzbl(%eax),%ebx vs 我方 movzbl(%eax),%eax（寄存器分配），60/61 纯 lowering，
//   语义等价。
// [b28c-13 N18a] 同 TurnGmMode：ORIG `movzbl (%eax),%ebx` 直落 vs 4.4 经 %eax 两步。
char CGM_Manager::GetCurrentGmMode(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    // b28d-37 [N18a 封顶]：同 TurnGmMode，ORIG 0x082992f3 `movzbl (%eax),%ebx`
    // 直落 vs 4.4 `movzbl (%eax),%eax; mov %eax,%ebx`；P226/P231 两形实测同形。
    register char mode = m_gmMap[accId].m_mode;
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
bool CGM_Manager::LoadGmDataFromDb(unsigned int accId)
{
    // b28d-37 [N104/N82 封顶，ORIG 0x8299740]：EH 尾块相对序（4.4 把 epilogue 就地内联、unwind 压末）+ 槽位残差（cMyTrace -0x24 vs ORIG -0x30）；123/118。
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
    writeGuard<rwLock> guard(&m_lock);     // ORIG 槽 -0x34（函数级）
    m_db->fetch();
    ok = m_db->get_int(0, m_gmMap[accId].m_level);
    if (m_gmMap[accId].m_level == 0)
    {
        cMyTrace t2(__PRETTY_FUNCTION__, 0x15b4, 0);   // ORIG 槽 -0x20（块内局部）
        t2("GM: %s exists in GM manifest but doen't have proper level. check DB",
           NumberToString(accId, 0));
        return false;
    }
    return ok;
}
