int importDailyScheduleEtcScript(const char* path,
                                 std::vector<_DailySchedule>& out)
{
    // ORIG 0891826c：loadRDARScriptFile 先于任何局部构造（失败直接 ret 0x36f7，
    // 无 string 析构），随后才 basic_string() token（8918298）→ -0x14 置 0
    // （89182a3）→ ok=false（89182aa）。
    if (!loadRDARScriptFile("", path))
    {
        return 0x36f7;
    }

    std::string token;                              // ORIG -0x2c
    int nUnusedCounter = 0;                         // ORIG -0x14：仅 89182a3 置 0，全函数无其它引用
    bool ok = false;                                // ORIG -0x2d
    (void)nUnusedCounter;

    // ORIG 89182c9：ScanType 失败是 break 出外层循环（跳到 8918782 的
    // nop/mov $0x0,%ebx 尾块），不是就地 return。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[schedule]")
        {
            // ORIG 89182e6..8918318：5 个 std::string 在 [schedule] 分支入口
            // 一次性默认构造（-0x34/-0x38/-0x3c/-0x40/-0x44），循环体内复用，
            // 循环出口 89186a6.. 统一析构；不在每轮迭代内重建。
            std::string strStart;                   // -0x34
            std::string strEnd;                     // -0x38
            std::string strWeekday;                 // -0x3c
            std::string strStartTime;               // -0x40
            std::string strEndTime;                 // -0x44

            for (;;)
            {
                _DailySchedule entry;               // ORIG -0x6c
                memset(&entry, 0, sizeof(entry));   // 8918333 memset(...,0,0x24)

                // ORIG 891833e/891835b/891837e：第 1、3 枚 ScanInt 先落局部
                // （-0x10/-0xc）再回填字段，第 2 枚直接写 entry+0x04。
                int nEventId = ScanInt(&ok);        // -0x10
                if (!ok) { break; }                 // 三枚普通 ScanInt 失败仅回外层
                entry.m_type = ScanInt(&ok);        // +0x04（直接写字段）
                if (!ok) { break; }
                entry.m_eventId = nEventId;         // +0x00（8918372）
                int nIdx = ScanInt(&ok);            // -0xc
                if (!ok) { break; }
                entry.m_idx = nIdx;                 // +0x08（8918395）

                if (!ScanStr(&strStart))
                {
                    break;                          // 首串失败同样回外层（ORIG 89183ab）
                }
                if (!ScanStr(&strEnd))
                {
                    return 0x3721;
                }
                if (!ScanStr(&strWeekday))
                {
                    return 0x3723;
                }
                if (!ScanStr(&strStartTime))
                {
                    return 0x3725;
                }
                if (!ScanStr(&strEndTime))
                {
                    return 0x3727;
                }

                // ORIG 8918435/89184a4/8918513/891857e/89185fa：按值形参的
                // std::string 拷贝直接由实参构造（唯一一次 _ZNSsC1ERKSs），
                // 不存在额外的具名 tmp 中转。
                if (!ConvertStringToTime(strStart, *(long*)&entry.m_startSec))
                {
                    return 0x372a;                  // +0x0c
                }
                if (!ConvertStringToTime(strEnd, *(long*)&entry.m_endSec))
                {
                    return 0x372c;                  // +0x10
                }
                unsigned int wday = 0;              // ORIG -0x48（8918505）
                if (!ConvertWeekdayStringToInt(strWeekday, wday))
                {
                    return 0x3730;                  // +0x14
                }
                entry.m_wday = static_cast<int>(wday);
                // ORIG 8918589..89185a7：三个出参直接取 entry+0x18/0x19/0x1a
                if (!ConvertHHMMSSStringToInt(strStartTime, entry.m_startHour,
                                              entry.m_startMin, entry.m_startSec2))
                {
                    return 0x3735;
                }
                // ORIG 8918605..891861f：entry+0x1b/0x1c/0x1d
                if (!ConvertHHMMSSStringToInt(strEndTime, entry.m_endHour,
                                              entry.m_endMin, entry.m_endSec2))
                {
                    return 0x3737;
                }

                out.push_back(entry);               // 回 do 头继续下一记录
            }
        }
    }

    return 0;                                       // ORIG 8918783 mov $0x0,%ebx
