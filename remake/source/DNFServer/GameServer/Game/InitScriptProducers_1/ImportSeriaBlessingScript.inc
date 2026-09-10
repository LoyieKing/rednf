int ImportSeriaBlessingScript(const char* path, stSeriaBlessingScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x2a74;

    std::string token;
    bool ok = false;

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        stSeriaBlessingScript::Value value;      // 每轮构造（ORIG 同；随即全覆盖）

        // b28d-lane25：P167 差集实证 ORIG 调 operator==（非 !=）；P191/P222 正向包裹。
        if (token == "[Seria Blessing]")
        {
            while (true)
            {
                int key = ScanInt(&ok);
                if (!ok)
                    break;

                value.m_minLv = ScanInt(&ok);
                if (!ok) return 0x2a89;
                value.m_maxLv = ScanInt(&ok);
                if (!ok) return 0x2a8d;
                value.m_field8 = ScanInt(&ok);
                if (!ok) return 0x2a91;
                value.m_fieldC = ScanInt(&ok);
                if (!ok) return 0x2a95;
                value.m_field10 = ScanInt(&ok);
                if (!ok) return 0x2a99;

                out.m_map0[key] = value;         // operator[] 值初始化为 0（同 Value ctor）后覆盖
            }
        }
    }
    return 0;
}
