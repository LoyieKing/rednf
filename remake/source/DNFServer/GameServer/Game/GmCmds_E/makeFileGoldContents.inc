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
            CBoosterTest::SendChatMsg(user, const_cast<char*>("결과 파일 이어붙힘"));
            return;
        }
        strcat(fileBuf, line);
        memset(line, 0, sizeof(line));
        stat.erase(it++);
    }
