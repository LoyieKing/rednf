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
            // [laneE1 2026-09-11 P167 实例修复] ORIG 该点是
            //   `_ZNK9__gnu_cxx17__normal_iteratorIPSt4pairIiiESt6vectorIS2_SaIS2_EEEdeEv`
            //   （operator*）而非 `ptEv`（operator->）：p167 差集 MISSING deEv×2 /
            //   EXTRA ptEv×2 ⇒ 源写法必须是 `(*it).first`（b28c C28L23 同族实证：
            //   `(*it).second` 替 `it->second` 归零）。
            fprintf(fp, "%d %d\n", (*it).first, (*it).second);
        }
        fwrite("[/skill]\n", 1, 9, fp);
    }
}
