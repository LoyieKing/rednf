// [b28d-20] Arad_GiftItem_Set::open（ORIG 0x081a2f1e，153/161，nd 34→32）：
//   本轮真语义修正 base 符号性（`unsigned int base`→`int base`，ORIG
//   0x081a2f?? 为 `mov %eax,%edx; sar $0x1f,%edx` 符号扩展，零扩展会改负值
//   时的 64 位 key）。残差 = Data/pair 插入路径多一次 Data 拷贝构造
//   （P81 引用 pair 与 4.4 转换构造差异）+ 槽 -0x3c/-0x54 漂移 ⇒ N7/P81 封顶。
// [b28d-solo-CItemList] **AE（153/153，ndiff 0，p167 14/14 CLEAR）**：
//   ① insert 实参改 `std::pair<const long long, Data>(key, data)` 直构 ⇒
//   复现 ORIG 0x081a302f `pair<const long long,Data>::C1<long long&,Data&>`
//   converting ctor（消原 EXTRA 的引用对 + Data 拷贝）；
//   ② insert 结果改具名局部 `ins` ⇒ 复现 ORIG 0x081a3059 先析构临时 pair、
//   0x081a3075 再 `movzbl -0x58; xor $1; test` 读 .second 的次序。
int Arad_GiftItem_Set::open(STStackableScript* script)
{
    // ORIG：if (*(int*)(param_1 + 0x178) != 0) —— Arad_STStackableScript.m_0（+0x178）
    if (script->m_arad178.m_0 == 0)
    {
        return 0;
    }

    // ORIG：kind = str_to_enum(c_str(script + 0x180))——调用真实 str_to_enum。
    // [b28d-solo-CItemList] P183 去缓存：ORIG 直把 c_str() 返回值压 0x4(%esp)
    //   （无 name 具名槽；我方具名槽多 mov store+load 一对）。
    const int kind = str_to_enum(script->m_arad178.m_str8.c_str());
    if (kind == 0)
    {
        return 0;
    }

    const int itemId = script->m_field0;                      // *(param_1 + 0x00)
    // [b28d-20] 原 unsigned 化致 4.4 零扩展（`mov $0x0,%edx`），ORIG 0x081a2f?? 为
    //   `mov %eax,%edx; sar $0x1f,%edx`（有符号 int 扩展）⇒ base 保持 int。
    const int base = script->m_field5c;                       // *(param_1 + 0x5c)

    // ORIG：for (local_10 = 0; local_10 != 0xb; ++local_10)，读取 +0x38 起 11 字节
    for (int i = 0; i != 0xb; ++i)
    {
        // STItemScript::m_usable@+0x38（继承成员具名，禁裸偏移）
        if (script->m_usable[i] != 0)
        {
            // ORIG +0x81-0xb5：L2R 64bit 进位链 kind<<32 → +i<<16(movzwl) → +base
            long long key =
                ((long long)kind << 32) +
                (long long)((unsigned short)i << 16) +
                (long long)base;
            Data data;
            data.m_item = itemId;
            data.m_str4 = script->m_arad178.m_strc;   // +0x184
            data.m_str8 = script->m_arad178.m_str10;  // +0x188
            // P81：ORIG pair<long long&, Data&> 引用对构造（非 make_pair 值对）
            // [b28d-solo] 具名局部形实测反劣（EXTRA 引用对构造 + 拷贝构造）；
            //   ORIG converting ctor `pair<const long long,Data>::C1<long long&,
            //   Data&>` 由内联临时形经 4.1 前端直构实现 ⇒ 保持内联。
            std::pair<std::map<long long, Data>::iterator, bool> ins =
                m_map.insert(std::pair<const long long, Data>(key, data));
            if (!ins.second)
            {
                return 0x122;   // 重复登记
            }
        }
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x127, 0)(
        "[!] Arad GiftItem Loding Sucess!! Item:%d", itemId);
    return 0;
}