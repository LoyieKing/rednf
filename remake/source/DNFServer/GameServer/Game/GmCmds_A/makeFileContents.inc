// ORIG makeFileContents 0x084af504：map 结果追加为 TSV 行（超限提示续接并中止）。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af504 orig/ours=192/174
//   残差：槽/寄存器分配 + 字符串物化链差。
// [b28d-solo-GmCmds_A] verdict=IDENTICAL/AE ndiff=0 [P214③+P251+P0]
// 数组零初始化 + switch(it->first) 单次 ptEv 分发 + 各分支重读 it->first。
void CBoosterTest::makeFileContents(CUser* user, char* fileBuf,
                                    std::map<int, int>& result)
{
    // [b28d-solo-GmCmds_A] P214③：ORIG 首清零为内联 rep stos 0x100 dwords
    // （=0x400 字节，ORIG +0x00c 段）⇒ 数组零初始化；旧 `memset(line,0,0x100)`
    // 既多一次 call（p167 EXTRA memset）又少清 0x300 字节。
    char line[1024] = {0};

    for (std::map<int, int>::iterator it = result.begin(); it != result.end();)
    {
        // [b28d-solo-GmCmds_A] P0/P167：ORIG 无 itemId 具名槽，各分支重读
        // `it->first`（p167 MISSING ptEv×5：ORIG 11 次 ptEv）。
        CItem* info = G_CDataManager()->find_item(it->first);
        // [b28d-solo-GmCmds_A] P251：ORIG 分发段 `call ptEv; mov (%eax),%eax;
        // test; je <0>; cmp $1; je <1>; jmp <default>`（first 只读一次）
        // ⇒ 源形是 switch(it->first)，if 链会多一次 ptEv（p167 EXTRA ptEv）。
        switch (it->first)
        {
        case 0:
            sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", it->first, "gold", "unknown",
                    it->second);
            break;
        case 1:
            sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", it->first, "coin", "unknown",
                    it->second);
            break;
        default:
            if (info == 0)
            {
                sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", it->first, "unknown", "unknown",
                        it->second);
            }
            else
            {
                sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\t%d\t%d\t%d\n",
                        it->first, info->GetItemName(),
                        getRarityStr(static_cast<ENUM_RARITY>(info->get_rarity())),
                        it->second, info->get_price(), info->GetSellPrice(),
                        info->getUsableLevel());
            }
            break;
        }

        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            static char sAppend[] = "\xb0\xe1\xb0\xfa \xc6\xc4\xc0\xcf \xc0\xcc\xbe\xee\xba\xd9\xc8\xfb";
            SendChatMsg(user, sAppend);
            return;
        }
        strcat(fileBuf, line);
        memset(line, 0, 0x400);
        result.erase(it++);
    }
}