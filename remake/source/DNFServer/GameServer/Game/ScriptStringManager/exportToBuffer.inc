int ScriptStringManager::exportToBuffer(unsigned char* buffer, int buffSize) const
{
    const int count = static_cast<int>(strings_.size());
    *reinterpret_cast<int*>(buffer) = count;        // 8acf43d：words[0] 无条件写
    if (buffSize <= 3)
        return -1;                                  // 8acf444：需能写 buffer+4
    int offset = 4 + 4 * count;                     // offsets[0] = 4 + 4*count
    // ORIG 8acf44e-8acf4b8：字节游标 `p`（写指针）+ `pos`（游标字节数）两具名局部
    // 跨循环使用 ⇒ 4.1 分别 spill 到 -0x20/-0x24；do-while 形（入口 jle + 尾部
    // 回跳，无旋转）。哨兵写用 break 前的 p（Ghidra C `*local_24 = iVar2`）。
    int* p = reinterpret_cast<int*>(buffer + 4);
    int pos = 8;
    if (count > 0)
    {
        *p = offset;                                // 8acf472
        // ORIG 8acf47c `cmpl $0x7; jg`：buffSize<=7 时不进循环、直接 -1
        // （照抄原码不越界写 buffer+8，P171）。
        if (buffSize > 7)
        {
            int i = 0;
            do
            {
                p = reinterpret_cast<int*>(buffer + pos);
                ++i;
                offset += static_cast<int>(strings_[i - 1].size());  // 8acf4a7
                // ORIG 8acf4ad `jl`（i<count 落 *p 写点）⇒ 正极性包裹（P222）。
                if (i < count)
                {
                    *p = offset;                    // 8acf488
                    pos += 4;
                }
                else
                {
                    pos += 4;                       // 8acf4af：读满 → 写哨兵
                    goto after_offsets;
                }
            } while (pos <= buffSize);              // 8acf491：越界 → -1
        }
        return -1;  // 8acf482（buffSize<=7）/ 8acf52d（循环越界）
    }
after_offsets:
    *p = offset;                                    // 8acf4b8：哨兵 offsets[count]
    if (pos > buffSize)                             // 8acf4c5
        return -1;
    unsigned char* dst = reinterpret_cast<unsigned char*>(p + 1);
    for (int i = 0; i < static_cast<int>(strings_.size()); ++i)
    {
        const std::size_t len = strings_[i].size();
        std::memcpy(dst, strings_[i].data(), len);  // 8acf515
        dst += len;
        pos += static_cast<int>(len);
        if (pos > buffSize)                         // 8acf528：memcpy 后检查
            return -1;
    }
    return static_cast<int>(dst - buffer);          // 写入总字节数
