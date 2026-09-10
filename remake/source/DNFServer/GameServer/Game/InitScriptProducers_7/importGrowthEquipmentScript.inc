bool importGrowthEquipmentScript(growthEquipmentScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (s == NULL)
    {
        return false;
    }

    std::string token;
    bool ok = false;

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0895eb3f：循环头 ScanType 检查
    //（P164 `for(;;){ if(!ScanType) break; }`），五处 ok 失败各发独立 nop 岛
    // + `jmp 0895ed68` 共享 `mov $1,%ebx` 返回岛（P223 多源 goto 尾标签）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[gift growth weapon box]")
        {
            s->m_giftBox = ScanInt(&ok);
            if (!ok) goto done1;             // ORIG：失败跳出解析但仍返回 1
        }
        if (token == "[growth material item]")
        {
            s->m_materialItem = ScanInt(&ok);
            if (!ok) goto done2;
        }
        if (token == "[growth material item max]")
        {
            s->m_materialMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) goto done3;
        }
        if (token == "[event bubble max]")
        {
            s->m_eventBubbleMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) goto done4;
        }
        if (token == "[event sand max]")
        {
            s->m_eventSandMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) goto done5;
        }
        if (token == "[growth equipment reward list]")
        {
            for (;;)
            {
                std::pair<int, int> entry;
                entry.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                entry.second = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                s->m_rewardMap.insert(entry);
            }
        }
    }
done1:;
done2:;
done3:;
done4:;
done5:;
    return true;
