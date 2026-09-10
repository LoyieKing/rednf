// ============================================================================
// [G12 lane · 2026-09-10 round5] verdict=NEAR（447 口径；ServerXml.cpp 4 件 NEAR 之一）
// 逐行 diff 的**全部**实质差异 = 间接 call 目标寄存器角色互换（N7 族）：
//   ORIG 0x080c5e92: mov (%eax),%edx   ; vptr+0x2c 槽 → 目标 %edx
//   ORIG 0x080c5e9a: call *%edx        ; this 载 %eax
//   我方(4.4.7 后端): mov (%eax),%eax / call *%eax（目标 %eax、this %edx）
// 助记符序列与指令数全等（100/100），仅寄存器角色由后端决定 ⇒ 源码无解
// （N7 判词：临时/引用/限定名三形均无效，8 件实证）。
// 修复路径 = 把本 TU 路由到 C6446R（cc1plus 4.4.6）。P230 补：4.4.6 间接 call 目标用 %edx，
// 与 ORIG 4.1 同形。实测（同源码、仅换后端，compare_tu_game_orig 口径）：
//   identical 69 / ae 88 / near 4 / diff 1  →  identical 69 / ae 92 / near 0 / diff 1
//   （本件 NEAR→AE；同批 EventLoad 110/110、RGBALoad 93/93、ProcessLoad 83/83 一并归零）
// 路由表 source/toolchain/cmake/dnf_game_cxx.sh 为 Main 独占 ⇒ 已上报 Main 落
//   `*ServerCommon/ServerXml.cpp) exec "$C6446R" "$@"`。
// ============================================================================
void CServerXml::CharsetInit(TiXmlNode* pRootNode)
{
    const char* cTemp = (const char*)0;
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("option");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <option> Tag Error");
        exit(-1);
    }
    pRoot = pRoot->FirstChild("charset");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <charset> Tag Error");
        exit(-1);
    }
    cTemp = pRoot->ToElement()->Attribute("type");
    if (cTemp == (const char*)0)
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    if (strcmp(cTemp, "kor") == 0)
    {
        m_eCharSet = ENUM_CHARSET_KOR;
    }
    else if (strcmp(cTemp, "chn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_CHN;
    }
    else if (strcmp(cTemp, "jpn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_JPN;
    }
    else if (strcmp(cTemp, "usa") == 0)
    {
        m_eCharSet = ENUM_CHARSET_USA;
    }
    else if (strcmp(cTemp, "twn") == 0)
    {
        m_eCharSet = ENUM_CHARSET_TWN;
    }
    else
    {
        puts("[CServerXml] <type> Tag Error");
        exit(-1);
    }
    strcpy(m_cCharSet, cTemp);
