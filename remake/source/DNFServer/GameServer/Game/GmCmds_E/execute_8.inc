// ORIG execute 0x084b0082：校验后按 mode==1 跑售货机统计并分片落盘。
// b28d-solo-GmCmds_E：P167 CLEAR（18/18）；162=160，ratio 0.9317。本次修正：
// ① fname 改 255 字节 + 声明序提前到 fileBuf 之前（ORIG 4.1 清零展开
//    `lea -0x153; mov $0xff,%ebx` 在前、fileBuf `call memset` 在后；改后
//    fname 起点 -0x153 非 4 对齐触发 P312 慢路，fileBuf 起点 -0x1746b 精确对齐）；
// ② 全函数槽位/帧已逐条对齐（唯一残差 = ORIG memset 首参 `lea` 先行 + size 经
//    `mov $0x17318,%edx` 中转，4.4 恒右到左直接压立即数；`&fileBuf[0]`/字面量/
//    sizeof 三形实测同形 ⇒ 跨版本 P75/手册「待解」项封顶）。
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
    sprintf(cnt, "%d\xb9\xf8 \xc5\xeb\xb0\xe8\xc0\xd4\xb4\xcf\xb4\xd9", m_count);
    CBoosterTest::SendChatMsg(user, cnt);

    std::map<int, int> stat;
    if (m_mode == 1)
    {
        CBoosterTest::SendChatMsg(user, const_cast<char*>("vending machine \xb0\xe1\xb0\xfa\xc3\xbc\xc5\xa9\xb8\xa6 \xbd\xc3\xc0\xdb\xc7\xd5\xb4\xcf\xb4\xd9"));
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

        CBoosterTest::SendChatMsg(user, const_cast<char*>("\xb0\xe1\xb0\xfa\xc3\xbc\xc5\xa9\xb0\xa1 \xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9"));
    }
}