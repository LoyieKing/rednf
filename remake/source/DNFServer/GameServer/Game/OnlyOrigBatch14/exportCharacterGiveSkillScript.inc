void exportCharacterGiveSkillScript(FILE* fp, std::vector<std::pair<int, int> >* vec)
{
    if (!vec->empty())
    {
        fwrite("[skill]\n", 1, 8, fp);
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x08a10083 增量段调 `ppEi`（后置
        // `operator++(int)`，3 参数含 sret 临时槽 -0xc）⇒ 必须写 `it++`（P28
        // 增量侧推论）；`++it` 走 ppEv 免临时槽、后续槽整体上移。
        for (std::vector<std::pair<int, int> >::iterator it = vec->begin();
             it != vec->end(); it++)
        {
            fprintf(fp, "%d %d\n", it->first, it->second);
        }
        fwrite("[/skill]\n", 1, 9, fp);
    }
