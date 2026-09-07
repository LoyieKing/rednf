// StringResourceDelayedItem 实现。
// 证据：docs/class_func_reports/StringResourceDelayedItem/StringResourceDelayedItem.md
//   （仅覆盖 ctor @0x08ad52a2）；其余 7 函数报告缺失，按 nm 定址 +
//   objdump -d [0x08ad52d4, 0x08ad5439] 兜底转写（§3.2）。
// 布局：+0x00 m_pCache(char const*)、+0x04 m_cacheStr(CharString)、
//   +0x08 m_pRes(StringResource const*)、+0x0c m_id(int) → sizeof 0x10。
#include "StringResourceDelayedItem.h"

#include "StringResource.h"
#include "StringTypes.h"

//   m_pCache=0 在 CharString 构造之前 ⇒ 须走初始化列表（声明序先于子对象构造）。
StringResourceDelayedItem::StringResourceDelayedItem(StringResource const& res, int id)
    : m_pCache(0)
{
    m_pRes = &res;
    m_id = id;
}

// ORIG D1/D2 @0x08ad52d4：仅 call CharStringD1Ev，无异常路径 ⇒ 空体。
StringResourceDelayedItem::~StringResourceDelayedItem()
{
}
// ORIG @0x08ad5302：m_pRes->get(m_id, out)，返回 bool（test %al）。
bool StringResourceDelayedItem::get(CharString const** out) const
{
    return m_pRes->get(m_id, out);
}

// ORIG @0x08ad52f6：直接返回 +0x0c。
int StringResourceDelayedItem::getId() const
{
    return m_id;
}

// ORIG @0x08ad52ea：直接返回 +0x08。
StringResource const* StringResourceDelayedItem::getStringResource() const
{
    return m_pRes;
}

// ORIG @0x08ad538a：get 命中 → m_cacheStr = *out；未命中 →
//   CharString::format("getStr(%d) == none", m_id)（0x8e32c4c）赋 m_cacheStr；
//   最后 m_pCache = m_cacheStr.c_str()。
void StringResourceDelayedItem::makeCacheStr()
{
    CharString const* out;
    // ORIG 0x08ad538a：-0xc 槽**无 C1**、format 直调、aSER、尾显式 D1（两分支汇合后
    // 统一析构）⇒ ORIG 源形=union 包裹抑制默认构造（4.1.2 扩展允许非平凡 union 成员，
    // 4.4.7 硬拒——本仓实测）。忠实复刻=raw-buffer 引用形：零新符号、发射形逐条同 ORIG
    // （format 内部 placement C1 完成初始化，显式 D1 释放；无未初始化读）。
    unsigned long msgRaw[(sizeof(CharString) + sizeof(unsigned long) - 1) / sizeof(unsigned long)];
    CharString& msg = *reinterpret_cast<CharString*>(msgRaw);
    if (get(&out)) {
        m_cacheStr = *out;
    } else {
        msg.format("getStr(%d) == none", m_id);
        m_cacheStr = msg;
        msg.~CharString();   // ORIG 显式 D1 仅在 else 路径（0x8ad5400←0x3f4；hit 路径 0x3bd 直落 c_str）
    }
    m_pCache = m_cacheStr.c_str();
}

// ORIG @0x08ad532a（const，Itanium sret：ret@0x8(%ebp)、this@0xc(%ebp)）：
//   m_pCache==0 时先 makeCacheStr（ORIG const 方法调非 const 成员，
//   汇编无多余指令 ⇒ const_cast 还原，无运行时开销）；返回 m_cacheStr 拷贝。
StringResourceDelayedItem::operator CharString() const
{
    if (m_pCache == 0)
        const_cast<StringResourceDelayedItem*>(this)->makeCacheStr();
    return m_cacheStr;
}

// ORIG @0x08ad5368：同惰性缓存判断，直接返回 m_pCache。
StringResourceDelayedItem::operator char const*() const
{
    if (m_pCache == 0)
        const_cast<StringResourceDelayedItem*>(this)->makeCacheStr();
    return m_pCache;
}
