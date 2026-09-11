// ORIG makeFileGoldContents 0x084ad6d6：gold 统计追加到文件缓冲；容量不足时提示。
// （ORIG static：makeRequest.md 反编译注释 __PRETTY_FUNCTION__ 无 this，
//   报告实证 arg0=CUser*。）
// b28d-solo-GmCmds_E：89=89 AE（P214③ `char line[1024]={0}` 复现头部 rep stos；
// 删 used/addLen 具名局部改内联 strlen 相加）。原 memset(line,0,0x100) 少清 768B 已修。
void CMonsterDropTest::makeFileGoldContents(CUser* user, char* fileBuf, DropStat& stat)
{
    // ORIG 0x084ad6e1 头部 rep stos %eax ×0x100 dwords（=1024 字节）= 大数组
    // 显式零初始化（P214③）；原 `memset(line,0,0x100)` 少清 768 字节（真错）。
    char line[1024] = {0};

    for (std::map<int, int>::iterator it = stat.begin(); it != stat.end();)
    {
        sprintf(line, "\"%s\"\t%d\t%d\n", "gold", it->first, it->second);
        // ORIG 无 used/addLen 具名局部（两次 strlen 结果 %ebx/%eax 直相加）。
        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            CBoosterTest::SendChatMsg(user, const_cast<char*>("\xb0\xe1\xb0\xfa \xc6\xc4\xc0\xcf \xc0\xcc\xbe\xee\xba\xd9\xc8\xfb"));
            return;
        }
        strcat(fileBuf, line);
        memset(line, 0, sizeof(line));
        stat.erase(it++);
    }
}