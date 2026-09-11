// exportToFile @ 0x8acf2a0：fopen(fileName,"wb") → fwrite count(4B) →
// count>0 时逐串 fwrite 累加 offset 字（每串后 total+=len）→ fwrite 最终
// total 哨兵 → count>0 时逐串 fwrite 数据（len!=0 才写）→ fclose 后 true；
// 任一 fwrite 失败 → fclose 后 false；fopen 失败 → false（ORIG 无 fclose）。
// 布局与 exportToBuffer 完全一致。ORIG 无 fileName==NULL 检查（P171 照抄）。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N7**（112 vs 115 条，
// p167 CLEAR）。已复现：无 NULL 护栏、count/offset 预清零（P198①）、正向 if +
// 共享 fail 尾（P222）、第二循环 do-while 消 4.4 旋转（P283）。残差 = 第三段
// 循环 this/file/finish 的寄存器角色（ORIG file spill@-0x2c、this 保活 %ebx；
// 我方 this spill@-0x2c、file 保活 %esi）。
bool ScriptStringManager::exportToFile(const char* fileName) const
{
    // ORIG 8acf2af/8acf2b6：两个具名 int 局部在 fopen 前预清零（P198①），
    // 且**无** fileName==NULL 护栏（ORIG 直接 fopen，P171 照抄不写）。
    int count = 0;
    int offset = 0;
    std::FILE* file = std::fopen(fileName, "wb");   // 8acf2c8（mode "wb"）
    if (file == 0)
        return false;
    count = static_cast<int>(strings_.size());      // 8acf2d5
    // ORIG 8acf303 `je +0x78`：正向 if（成功路径跳 fail 块），失败路径
    // fall-through 到函数尾的共享 fclose+return false（Ghidra C 同形）。
    if (std::fwrite(&count, 4, 1, file) == 1)       // 8acf2fb：写 count
    {
        offset = 4 + 4 * static_cast<int>(strings_.size());  // 8acf321（local_24）
        // ORIG 8acf335-8acf375：入口 `jle` + 尾部回跳（4.1 while 未旋转）
        // ⇒ 写 do-while + 前置空检查复现（4.4 对 while 恒旋转）。
        if (0 < static_cast<int>(strings_.size()))
        {
            int i = 0;
            do
            {
                if (std::fwrite(&offset, 4, 1, file) != 1)  // 8acf352：写累加 offset
                    goto fail;
                offset += static_cast<int>(strings_[i].size());  // 8acf368
                ++i;
            } while (i < static_cast<int>(strings_.size()));
        }
        if (std::fwrite(&offset, 4, 1, file) == 1)  // 8acf391：最终 total 哨兵
        {
            for (int i = 0; i < static_cast<int>(strings_.size()); ++i)
            {
                const std::size_t len = strings_[i].size();
                if (len != 0 && std::fwrite(strings_[i].data(), len, 1, file) != 1)
                    goto fail;
            }
            std::fclose(file);                      // 8acf3fe：成功路径
            return true;
        }
    }
fail:
    std::fclose(file);                              // 8acf305：共享失败出口
    return false;
}