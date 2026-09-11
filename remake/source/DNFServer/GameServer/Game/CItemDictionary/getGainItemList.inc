// ORIG 0x0811dba6（ret $0x4 克隆；逐条转写）：遍历位图（+0x5 起），置位处
// itemIdx=i*8+bit 经 G_CDataManager()->m_mapA940（+0xa940）翻译后
// push_back(map[itemIdx])；map 无此键则跳过。
// ORIG 为按值返回（ret $0x4 NRV 隐藏参数 @ ebp+8、this @ ebp+0xc；引用返回
// 少 NRV 参数、直接返回 &成员，符号同 mangled 但语义不同），故用局部对象。
std::vector<int> CItemDictionary::getGainItemList() {
    int itemIdx = 0;      // ORIG：先声明 itemIdx（-0x2c 清零在 vector 构造前）
    std::vector<int> result;
    result.clear();       // ORIG：vectorC1 构造后显式 clear（0x0811dbb2）
    for (int i = 0; i < static_cast<int>(kBinaryLength); ++i)   // ORIG setle（有符号）
    {
        unsigned char c = m_data[i];   // ORIG 缓存 -0x1d（movzbl 无符号复用）
        for (int bit = 0; bit < kItemDictionaryBit; ++bit)
            if ((c >> bit) & 1)
            {
                itemIdx = i * kItemDictionaryBit + bit;
                std::map<int, int>::iterator it =
                    G_CDataManager()->m_mapA940.find(itemIdx);
                // ORIG 0x0811dc6e `test %al,%al; jne +0xe8` + body 尾 `jmp +0xe9` +
                // `+0xe8: nop`：4.1 发射「jne → 空块 nop 中继岛」。4.4 对空 then/else
                // 恒 jump-threading（探针 /tmp/bl9p/nop2.o fA：无岛），唯 `goto` 到函数
                // 体尾的空标签可复现（fC：`jne <nop>; body; jmp <nop 后>; nop`）。
                if (it == G_CDataManager()->m_mapA940.end())
                    goto GAIN_NO_HIT;
                result.push_back(it->second);
            GAIN_NO_HIT:;
            }
    }
    return result;
}
