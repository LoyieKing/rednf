// ORIG 0x080c883c：按 map 顺序填 entries（i>0x3e7 截断），写 count=i。
// （characNo@+0 不由本函数写，由 _saveData 填。）
void BestClearTime::getData(char* pData) const
{
    // ORIG 0x080c883c 结构：data(-0x10)/i(-0xc) 具名 + begin 具名迭代器(-0x18)；
    // 循环体先 i>0x3e7 直接 return（不写 count）；逐字段重算 i*0x18（无 entry
    // 指针缓存，P183）；first 一次 ptEv、second 一次取值后连读 5 个 int。
    SIG_BEST_CLEAR_TIME_VIEW* data =
        reinterpret_cast<SIG_BEST_CLEAR_TIME_VIEW*>(pData);
    int i = 0;
    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::const_iterator it =
        m_map.begin();
    while (it != m_map.end())
    {
        if (i > 0x3e7)
            return;
        // [M3S4 实测-4形 · 76=74+2 封顶] key 直赋 13 条（lea+add+store）vs ORIG
        // 11 条（索引折叠 `mov %ecx,0x8(%eax,%edx,1)`）：①显式 `(char*)data+i*0x18+8`
        // 同 13；②`(char*)data+(8+i*0x18)` 同 13（树序不变产物）；③4 字节聚合拷贝
        // （局部 KeyCell）更劣（提前物化地址 + 多 1 mov）⇒ 跨版本 codegen 差
        // （ORIG 4.1 索引折叠 vs 4.4 标量 store 恒 lea+add）不可翻。
        // 本函数最大改善：删 `const SIG& d = it->second` 具名引用 + 5 条逐字段赋值
        // ⇒ 单条 `m_data = it->second` 聚合拷贝（113→76，m_data 段与 ORIG 逐条恒等）。
        data->m_entries[i].m_key = it->first;
        // ORIG 0x080c88a7-0x080c88d2：第二次 operator-> 后**连读源 5 个 int**
        // 直写目标 +0xc..+0x1c（无具名引用槽）⇒ 整体结构体赋值形。
        data->m_entries[i].m_data = it->second;
        ++it;
        ++i;
    }
    data->m_count = i;
}