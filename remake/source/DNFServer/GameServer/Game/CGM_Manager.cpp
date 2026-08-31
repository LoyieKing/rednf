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
bool CGM_Manager::IsGm(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())   // ORIG 用 operator==（eq），非 operator!=
        return true;
    return false;
}

// ORIG 符号: _ZN11CGM_Manager10TurnGmModeEj @ 0x829917c
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 翻转 +0 模式字节并返回新值。
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
    return m_gmMap[accId].m_mode;
}

// ORIG 符号: _ZN11CGM_Manager16GetCurrentGmModeEj @ 0x8299276
// readGuard(m_lock)；find 未命中返回 0；否则经 operator[] 读 +0 模式字节。
char CGM_Manager::GetCurrentGmMode(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    if (it == m_gmMap.end())
        return 0;
    return m_gmMap[accId].m_mode;
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
