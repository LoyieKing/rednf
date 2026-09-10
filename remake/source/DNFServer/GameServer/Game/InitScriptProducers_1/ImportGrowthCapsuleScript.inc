int ImportGrowthCapsuleScript(const char* path, stGrowthCapsuleScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0x2ba2;

    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto failEof;                        // ORIG 失败块 0x089113b5 落在循环体之后

        if (token == "[gage percent]")
        {
            script.m_gagePercent = ScanInt(&ok);
            if (!ok) return 0x2bb0;
        }
        else if (token == "[max gage exp]")
        {
            script.m_maxGageExp = ScanInt(&ok);
            if (!ok) return 0x2bb6;
        }
        else if (token == "[capsule id]")
        {
            script.m_capsuleId = ScanInt(&ok);
            if (!ok) return 0x2bbc;
        }
        else if (token == "[capsule exp]")
        {
            script.m_capsuleExp = ScanInt(&ok);
            if (!ok) return 0x2bc2;
        }
        // b28d-lane25：P167 差集实证 ORIG 调 operator==（非 !=）；正向包裹。
        else if (token == "[capsule limit lv]")
        {
            script.m_capsuleLimitLv = ScanInt(&ok);
            if (!ok)
                return 0x2bc8;
        }
    }
failEof:
    return 0;
}
