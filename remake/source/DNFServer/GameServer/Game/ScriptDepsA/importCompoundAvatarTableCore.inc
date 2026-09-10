int importCompoundAvatarTableCore(const char* dir, const char* path,
                                           tagCompoundAvatarTable* table)
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：808/830，gap=-22，
    // p167 88/88 CLEAR）：本波修 [ancient bind cube] 终止守卫 `==`+break（P133）
    // + 4 处 tail/cube push_back 直构 pair（P192）+ tagCompoundAvatar 改用
    // value-init 临时作 resize 实参（消 C1ERKS 参数副本，ORIG 0x0890334e 形）。
    // 残差=巨件解析树帧/槽平移 + vector/map 模板实例内联差。
    if (!loadRDARScriptFile(dir, path))
        return 0xf1e;

    tagCompoundAvatarTable* curTable = 0;
    tagCompoundAvatar* avatar = 0;
    int grade = 0;
    bool ok = false;
    std::string token;
    std::string str;

    while (true)
    {
        if (!ScanType(token, true))
            break;                                     // P164 配套：EOF 落点移到循环外

        if (token == "[character type]")
        {
            if (!ScanStr(&str))
                return 0xf2d;
            const int job = getCharacterJob(str.c_str());
            if (job == 0xb)
                return 0xf32;
            curTable = table + job;                       // base + job*0xc
        }
        else if (token == "[grade]")
        {
            grade = ScanInt(&ok);
            if (!ok)
                return 0xf37;
            if (curTable == 0)
                return 0xf39;
            // [b28d-solo-ScriptDepsA] ORIG 0x0890334e 段：rep stos 0x308B 前置清零
            // + 就地 C1Ev（value-init 临时）+ 直接作 resize 第二实参（无具名 tmp
            // ⇒ 无 C1ERKS 参数副本）。
            curTable->m_avatars.resize(grade + 1, tagCompoundAvatar());
            avatar = &curTable->m_avatars[grade];
            avatar->Clean();
            avatar->m_idx = grade;
        }
        else if (token == "[rare rate]")
        {
            while (ScanStr(&str))
            {
                const int rate = ScanInt(&ok);
                if (!ok)
                    return 0xf48;
                const int parts = getCompoundAvatarPartsType(str);
                if (parts == 0x16 || avatar == 0)
                    return 0xf4c;
                avatar->m_a[parts] = rate;
            }
        }
        else if (token == "[upper grade]")
        {
            if (avatar == 0)
                return 0xf53;
            avatar->m_flag2dc = ScanInt(&ok);
            if (!ok)
                return 0xf56;
        }
        else if (token == "[upper rare rate]")
        {
            if (avatar == 0)
                return 0xf5b;
            while (ScanStr(&str))
            {
                const int rate = ScanInt(&ok);
                if (!ok)
                    return 0xf63;
                const int parts = getCompoundAvatarPartsType(str);
                if (parts == 0x16)
                    return 0xf67;
                avatar->m_e[parts] = rate;
            }
        }
        else if (token == "[hat avatar]")
        {
            if (avatar == 0) return 0xf6e;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(0)))
                return 0xf71;
        }
        else if (token == "[hair avatar]")
        {
            if (avatar == 0) return 0xf76;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(1)))
                return 0xf79;
        }
        else if (token == "[face avatar]")
        {
            if (avatar == 0) return 0xf7e;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(2)))
                return 0xf81;
        }
        else if (token == "[neck avatar]")
        {
            if (avatar == 0) return 0xf86;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(6)))
                return 0xf89;
        }
        else if (token == "[coat avatar]")
        {
            if (avatar == 0) return 0xf8e;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(3)))
                return 0xf91;
        }
        else if (token == "[pants avatar]")
        {
            if (avatar == 0) return 0xf96;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(4)))
                return 0xf99;
        }
        else if (token == "[belt avatar]")
        {
            if (avatar == 0) return 0xf9e;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(7)))
                return 0xfa1;
        }
        else if (token == "[shoes avatar]")
        {
            if (avatar == 0) return 0xfa6;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(5)))
                return 0xfa9;
        }
        else if (token == "[skin avatar]")
        {
            if (avatar == 0) return 0xfae;
            if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(8)))
                return 0xfb1;
        }
        else if (token == "[aurora avatar]")
        {
            if (avatar == 0) return 0xfb7;
            do
            {
                if (!setCompoundAvatarData(avatar, static_cast<ENUM_EQUIPMENTTYPE>(9)))
                    return 0xfba;
            } while (false);
        }
        else if (token == "[material]")
        {
            if (avatar == 0) return 0xfca;
            const int count = ScanInt(&ok);
            if (!ok) return 0xfcd;
            for (int i = 0; i < count; ++i)
            {
                int item = ScanInt(&ok);
                if (!ok) break;
                int weight = ScanInt(&ok);
                if (!ok) return 0xfd4;
                avatar->m_tailA.push_back(std::pair<int, int>(item, weight));
                if (weight != 1) return 0xfd9;
            }
        }
        else if (token == "[material_upper]")
        {
            if (avatar == 0) return 0x100c;
            const int count = ScanInt(&ok);
            if (!ok) return 0x100f;
            for (int i = 0; i < count; ++i)
            {
                int item = ScanInt(&ok);
                if (!ok) break;
                int weight = ScanInt(&ok);
                if (!ok) return 0x1016;
                avatar->m_tailB.push_back(std::pair<int, int>(item, weight));
                if (weight != 1) return 0x101b;
            }
        }
        else if (token == "[material_lower]")
        {
            if (avatar == 0) return 0x1023;
            const int count = ScanInt(&ok);
            if (!ok) return 0x1026;
            for (int i = 0; i < count; ++i)
            {
                int item = ScanInt(&ok);
                if (!ok) break;
                int weight = ScanInt(&ok);
                if (!ok) return 0x102d;
                avatar->m_tailC.push_back(std::pair<int, int>(item, weight));
                if (weight != 1) return 0x1032;
            }
        }
        else if (token == "[material_ancient]")
        {
            if (avatar == 0) return 0x103a;
            const int count = ScanInt(&ok);
            if (!ok) return 0x103d;
            for (int i = 0; i < count; ++i)
            {
                int item = ScanInt(&ok);
                if (!ok) break;
                int weight = ScanInt(&ok);
                if (!ok) return 0x1044;
                avatar->m_cube.m_vec.push_back(std::pair<int, int>(item, weight));
                if (weight != 1) return 0x1049;
            }
        }
        else if (token == "[ancient bind cube]")
        {
            if (avatar == 0) return 0x104f;
            while (ScanType(token, true))
            {
                if (token == "[/ancient bind cube]")
                    break;
                if (token == "[hat rare avatar]" || token == "[hair rare avatar]" ||
                    token == "[face rare avatar]" || token == "[neck rare avatar]" ||
                    token == "[coat rare avatar]" || token == "[pants rare avatar]" ||
                    token == "[belt rare avatar]" || token == "[shoes rare avatar]")
                {
                    if (!setCompoundRareAvatarData(
                            avatar,
                            static_cast<ENUM_EQUIPMENTTYPE>(
                                getCompoundRareAvatarType(token))))
                        return 0x105d;
                }
            }
        }
    }
    return 0;
