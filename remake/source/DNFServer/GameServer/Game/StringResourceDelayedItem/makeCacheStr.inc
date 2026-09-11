// ORIG @0x08ad538a：get 命中 → m_cacheStr = *out；未命中 →
//   CharString::format("getStr(%d) == none", m_id)（0x8e32c4c）赋 m_cacheStr；
//   最后 m_pCache = m_cacheStr.c_str()。
// [P2-S4 2026-09-11 已翻] 原 raw-buffer + 引用 + 显式 dtor 形（68 条，DIFF gap=-10）里
//   `CharString& msg = *reinterpret_cast<CharString*>(msgRaw);` 的**引用槽**（-0xc）
//   使 4.4 在 get 前多发 `lea -0x18(%ebp),%eax; mov %eax,-0xc(%ebp); lea -0x14(%ebp),%eax`
//   且 else 路径多一次整体赋值搬运。ORIG 0x08ad538a 实测：out 单槽 -0x10，
//   0x08ad53d7 `call CharString::format` 的 sret 直指匿名临时 -0xc（无独立 C1）、
//   0x08ad53ef `aSER` 拷贝赋值、0x08ad5417 尾 D1 仅在 else 路径 —— 即源形就是
//   **直接 `m_cacheStr = CharString::format(...)`**。改回该形后 58/59，verdict=AE gap=-1，
//   TU diff 归零（D1Ev/_Unwind_Resume 亦随 format 内部 EH 正常发射）。
void StringResourceDelayedItem::makeCacheStr()
{
    CharString const* out;
    if (get(&out)) {
        m_cacheStr = *out;
    } else {
        // ORIG 0x08ad53d7 `call format` 的 sret 目标 = 匿名临时 -0xc（format 内
        // placement C1 完成构造）→ 0x08ad53ef `aSER` 拷贝赋值 → 0x08ad5417 尾 D1
        // （仅 else 路径；hit 路径 0x08ad53bd 直落 c_str）。即源形就是**直接赋值
        // format 返回值**，4.4 同样以匿名临时承载返回值。
        m_cacheStr = CharString::format("getStr(%d) == none", m_id);
    }
    m_pCache = m_cacheStr.c_str();
}