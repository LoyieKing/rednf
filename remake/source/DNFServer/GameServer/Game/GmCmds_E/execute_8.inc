void CRandomObjectTest::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    const char* err = checkCommandExecuteError();
    if (err != 0)
    {
        CBoosterTest::SendChatMsg(user, const_cast<char*>(err));
        return;
    }

    if (m_count == 0)
    {
        m_count = 10000;
    }
    char cnt[21];
    sprintf(cnt, "%d번 통계입니다", m_count);
    CBoosterTest::SendChatMsg(user, cnt);

    std::map<int, int> stat;
    if (m_mode == 1)
    {
        CBoosterTest::SendChatMsg(user, const_cast<char*>("vending machine 결과체크를 시작합니다"));
        getVendingMachineItem(stat);

        // ORIG 4.1 清零展开计数 ebx=0xff ⇒ fname 实为 255 字节（起点 -0x153 非 4
        // 对齐 ⇒ 触发 P312 对齐剥离慢路，且使 fileBuf 起点落在 -0x1746b）。
        char fname[255] = {0};
        char fileBuf[95000];
        // [round5-C05 2026-09-10] ORIG 0x084b01af 先 `lea -0x1746b(%ebp),%eax` 再压
        // size（`mov $0x17318,%edx; mov %edx,0x8(%esp)`）；探针 /tmp/ms_probe.cpp
        // f1..f18（字面量/sizeof/&buf[0]/(char*)/(void*)/enum/long/(size_t) cast/
        // const 局部/除法形）**全部**产 `movl $imm,0x8(%esp)` 直压 ⇒ 4.4.7/4.4.6
        // 前端无该中转形，P75 跨版本，手册「待解」项确认封顶。
        memset(fileBuf, 0, sizeof(fileBuf));

        bool loop = true;
        while (loop)
        {
            CBoosterTest::makeFileContents(user, fileBuf, stat);
            sprintf(fname, "etc_random_test_%d.txt", m_count);
            if (stat.empty())
            {
                loop = false;
            }
            CBoosterTest::printClient(user, loop, fname, fileBuf);
            memset(fileBuf, 0, sizeof(fileBuf));
        }

        CBoosterTest::SendChatMsg(user, const_cast<char*>("결과체크가 완료되었습니다"));
    }
