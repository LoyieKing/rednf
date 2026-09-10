bool importAradEventScript(std::vector<SCRIPT::SEventPeriodData>& eventList,
                                   std::vector<SCRIPT::SRewardPeriodData>& rewardList)
{
    eventList.clear();    // ORIG +0x0e：vector<SEventPeriodData>::clear()
    rewardList.clear();   // ORIG +0x1b：vector<SRewardPeriodData>::clear()

    if (!loadRDARScriptFile("", "Etc/AradEventPeroid.etc"))
        return false;      // ORIG：loadRDARScriptFile 在 try 外，失败直接返回

    try
    {
        std::string token;
        bool ok = false;
        // [b28d-solo] ORIG 0x088b3bd8 外层循环同为「条件置顶 + 体尾回跳」
        // （条件块 `movl $1,0x4(%esp); lea token; call ScanType; xor $1; test;
        // jne 出口`），4.4 while 恒 loop rotation ⇒ goto 顶测形（P283 反向）。
event_scan:
        if (!ScanType(token, true))
            goto event_done;
        {
            if (token == "[event period]")
            {
                int eventCode = 0;
                short startShorts[4] = {0};   // ORIG 五个 movl $0 清零（-0x1c/-0x42/-0x3e/-0x4a/-0x46）
                short endShorts[4] = {0};
                for (;;)
                {
                    eventCode = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (!SCRIPT::readTime(startShorts))
                        throw "start time";
                    if (!SCRIPT::readTime(endShorts))
                        throw "end time";

                    SEventPeriodData data(eventCode,
                                          SCRIPT::uint32Time(startShorts),
                                          SCRIPT::uint32Time(endShorts));
                    if (std::find(eventList.begin(), eventList.end(), data)
                            != eventList.end())
                        throw "already exist";          // ORIG find != end 分支
                    eventList.push_back(data);
                }
            }
            else if (token == "[reward period]")
            {
                int eventCode = 0;
                short startShorts[4] = {0};   // ORIG 七个清零（-0x18/-0x64/-0x60/-0x6c/-0x68/-0x14/-0xe word）
                short endShorts[4] = {0};
                int itemIndex = 0;
                short itemCount = 0;
                for (;;)
                {
                    eventCode = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (!SCRIPT::readTime(startShorts))
                        throw "start time";
                    if (!SCRIPT::readTime(endShorts))
                        throw "end time";

                    itemIndex = ScanInt(&ok);
                    if (!ok)
                        throw "item index";
                    itemCount = (short)ScanInt(&ok);
                    if (!ok)
                        throw "item count";

                    SRewardPeriodData data(eventCode,
                                           (unsigned long)(unsigned int)itemIndex,
                                           itemCount,
                                           SCRIPT::uint32Time(startShorts),
                                           SCRIPT::uint32Time(endShorts));
                    if (std::find(rewardList.begin(), rewardList.end(), data)
                            != rewardList.end())
                        throw "already exist";
                    rewardList.push_back(data);
                }
            }
        }
        goto event_scan;
event_done:
        return true;    // ORIG 扫描完成 ebx=1。
    }
    catch (char const* msg)
    {
        LogManager::logFormat(
            1, "../localjapan/Arad_RDARScriptEtcParameter.cpp",
            __FUNCTION__, 0x99,
            "[ARAD SCRIPT] Load fail. AradEventPeroid.etc %s", msg);
        return false;
    }
