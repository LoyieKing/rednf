// [b28d-solo-InitScriptProducers_8 AE] ORIG 088d444f：clean 192/192（ORIG 194 insns）；
// p167 CLEAR。已对齐：m_colorMap/m_nameMap 双 insert、RGB 字节宽度 mov %al 实证、
// name 扫描失败退段。
bool importAvatarColorVariation(AvatarColorInfo* out)
{
    if (!loadRDARScriptFile("", "Etc/AvatarColorVariation.etc"))
        return false;

    out->m_colorMap.clear();

    std::string token;
    bool ok = false;
    int dead = 0;                                      // ORIG -0xc 死写入（P171）
    (void)dead;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;

        if (token == "[avatar color map]")
        {
            for (;;)
            {
                colorRGB rgb;                          // ctor：三字节置 0xff
                const int key = ScanInt(&ok);
                if (!ok)
                    break;
                rgb.m_r = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                    break;
                rgb.m_g = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                    break;
                rgb.m_b = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                    break;

                std::string name;                      // ORIG 循环内 -0x4c
                ok = ScanStr(&name);
                if (!ok)
                    break;
                out->m_colorMap.insert(std::make_pair(key, rgb));
                out->m_nameMap.insert(std::make_pair(key, name));
            }
        }
    }
}