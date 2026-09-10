bool importEquipmentPartSetInfo(const char* path, ENUM_MODULE_TYPE moduleType)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x168b,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 "
                              "\xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 "
                              "\xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.",
                              path);
        return false;
    }

    std::string token;
    std::string name;
    std::string pieceName;
    std::string tag;
    // ORIG 用 ebx 承载 ok（无栈槽；0x08907503 mov $0,%ebx / 0x089075a5 mov $1,%ebx /
    // 0x089075cc test %ebx,%ebx）⇒ 原码是 register 形（P231 纯 register 合法，非 asm
    // 绑定）。int 版 while(ok) 实测多 setne 物化（gap −2）；bool 版 testb %bl 最接近。
    register bool ok;

    g_equipmentPartSetMap_.clear();

    // P164：主循环 goto 形复位头部条件块（ORIG 0x08907477）；ScanType 失败落
    // scan_done（直接进入下方 for 循环，非 return）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        // ORIG 0x0890749d 正向 `if (token == X)` 形（用 operator==，无 operator!=）。
        if (token == "[equipment part set]")
        {
            STEquipmentPartSetData data;
            // ORIG 直接 `data+0 = ScanInt(0)`（无独立 key 局部；键取 data.m_field0）。
            data.m_field0 = ScanInt((bool*)0);
            ScanStr(&name);
            data.m_name4 = name;

            do
            {
                STEquipmentPartSetPiece piece;           // ctor 三 int=-1
                // ORIG 0x089074fc 正向 `if (!ScanStr)`：失败臂 mov $0,%ebx、成功臂尾
                // mov $1,%ebx（两臂各写 ok，非 `ok = ScanStr(...)` 单赋值）。
                if (!ScanStr(&pieceName))
                {
                    ok = false;
                }
                else
                {
                    piece.m_name = pieceName;
                    ScanStr(&tag);
                    piece.m_field4 = getSameStringIndex(tag.c_str(),
                                                        characterEquipmentTypeTags,
                                                        0x1a);
                    piece.m_field8 = ScanInt((bool*)0);
                    piece.m_fieldc = ScanInt((bool*)0);
                    data.m_pieces.push_back(piece);

                    g_equipmentPartSetMap_[data.m_field0] = data;
                    ok = true;
                }
            } while (ok);
        }
    }
    goto scan_top;
scan_done:

    // ---- 逐集合装载装备脚本并合并 [-1] 参数条目 ----
    // P183：ORIG 无 script / paramMap 具名局部——每处重新 `call operator->;
    // mov 0xc(%eax),%eax` 取 it->second.m_script（0x08907653/0x0890765e/0x08907686/
    // 0x089076b7/0x089076fd/0x08907729/0x08907756/0x089077a4/0x089077d9/0x089077f4）。
    for (std::map<int, STEquipmentPartSetData>::iterator it =
             g_equipmentPartSetMap_.begin();
         it != g_equipmentPartSetMap_.end(); ++it)
    {
        // ORIG 0x08907619：new 结果经 ebx 跨 ctor 保活后直存成员（无具名指针局部）。
        it->second.m_script = new STEquipmentScript;         // ORIG 0x894 字节

        if (!ImportEquipmentScript(it->second.m_script,
                                   it->second.m_name4.c_str(), false,
                                   moduleType, it->first))
        {
            LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                  __FUNCTION__, 0x16c2,
                                  "ERROR : \xec\x9e\xa5\xeb\xb9\x84\xeb\xb6\x80"
                                  "\xec\x9c\x84\xec\x85\x8b\xed\x8a\xb8 \xea\xb5"
                                  "\xac\xec\x84\xb1\xed\x92\x88 %s \xed\x8c\x8c"
                                  "\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d"
                                  "\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb"
                                  "\x8b\x88\xeb\x8b\xa4.",
                                  it->second.m_name4.c_str());
            delete it->second.m_script;
            return false;
        }

        // ORIG 0x08907741 先 map::end、0x08907775 后 map::find（单表达式形，P293）。
        if (it->second.m_script->m_paramMap678.find(-1) !=
            it->second.m_script->m_paramMap678.end())
        {
            // ORIG 用 `paramMap[-1]` 取引用再拷贝构造（p167 MISSING ixERS4_），
            // 非 `def->second`（后者多一次 EquipmentParameterInfo 迭代器 ptEv）。
            const EquipmentParameterInfo merged(
                it->second.m_script->m_paramMap678[-1]);
            it->second.m_script->m_paramMap678.clear();
            it->second.m_script->m_paramMap678[it->second.m_pieces.size()] =
                merged;
        }
    }
    return true;
