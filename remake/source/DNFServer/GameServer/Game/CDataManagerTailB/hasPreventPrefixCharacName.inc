// ---------------------------------------------------------------------------
// hasPreventPrefixCharacName(char const*, int) —— ORIG T 0x0836360c sz 0x10a
// （objdump 0x0836360c..0x08363716 逐指令转写；报告缺失标 [报告缺失]）。
//   count = m_serverParameter.m_preventPrefixCount（CDM+0x194）；
//   entry(i) = m_serverParameter.m_preventPrefixEntries + i*0x1e（CDM+0x190+8 起）。
//   对每条：entryLen=strlen(entry)；21 字节栈缓存清零（P4 gcc 内建展开形态）
//   → memcpy(prefix, name, entryLen) → strcmp(entry, prefix)；==0 → 返回 1。
//   循环条件 count > i（setg）；扫完返回 0。第二参 unused ORIG 亦未读。
//   依 §8.3 已将 ServerParameterScript.m_pad12c 头部拆具名（sizeof 0x49f4 不变）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] N：P30/N18a —— 删 sps 引用/entry 局部后帧与
//   槽位全对齐（sub $0x30、prefix -0x25、i -0x10、entryLen -0xc），仅余 4.4.6
//   对 i*0x1e 索引展开的寄存器选择（`mov %edx,%ecx; sub; mov` vs `sub %eax,%edx`）
//   与 `lea 0x190(%edx)` 折成 `mov+add` 共 2 处 4 条；p167 3=3 CLEAR。
//   [l5f 2026-09-11 · 探针复核] /tmp/probe_hp.cpp 四形实测（f1 `&arr[i*0x1e]`＝
//   当前形、f2 数组名退化+指针加法、f4 十进制常量 i*30、f6 `0x1e*i` 乘法交换）
//   产物**逐条全同**：`mov %edx,%ecx; sub %eax,%ecx; mov %ecx,%eax; add $0x190,%eax`
//   ⇒ 4.4.7 后端寄存器选择（4.1 为原地 `sub %eax,%edx` + `lea 0x190(%edx),%eax`），
//   源形不可择 ⇒ 封 N18a（与旧判词同向，本次给出四形实证）。ORIG 0x08363620 段。
//   ORIG 0x0836360c。
// [K6 2026-09-11·P292 档①] int→bool（配合 CDataManager.h:3645）：ORIG 调用点
// 0x081c6b30 `call; test %al,%al; je`（裸 bool 直判）；定义体各分支为常量 return 0/1，
// 4.4 探针实测 bool 与 int 形态全等（mov $0/%eax）⇒ 体机器码不变。
bool CDataManager::hasPreventPrefixCharacName(const char* name, int /*unused*/)
{
    for (int i = 0; m_serverParameter.m_preventPrefixCount > i; ++i)
    {
        const size_t entryLen =
            strlen(&m_serverParameter.m_preventPrefixEntries[i * 0x1e]);
        char prefix[0x15] = { '\0' };
        memcpy(prefix, name, entryLen);
        if (strcmp(&m_serverParameter.m_preventPrefixEntries[i * 0x1e],
                   prefix) == 0)
            return 1;
    }
    return 0;
}