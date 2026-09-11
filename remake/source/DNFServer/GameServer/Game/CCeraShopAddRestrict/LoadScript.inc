// b28d-solo-CCeraShopAddRestrict: AE（83=83，仅 .o 重定位显示差；P283 头部条件 +
// P232 三个 dead bool 预清）。
int ScriptLoader::LoadScript()
{
    if (!loadRDARScriptFile("Etc/", "NewCashShop_Restrict.etc"))
        return -1;
    std::string section;
    // [b28d-solo P232] ORIG 0x08abf404 三条 `movb $0x0,-0xb|-0xa|-0x9(%ebp)` 是
    // 原码三个 dead bool 局部的初始化（section 因此落 -0x10 槽）；探针
    // /tmp/probe_bool.cpp f2 实测 4.4 同形（未使用亦不 DCE）。
    bool isAccountSection = false;
    bool isCharacSection = false;
    bool sectionHandled = false;
    // [b28d-solo P283/P164] ORIG 0x08abf3f9：循环头直落 ScanType（条件在头部），
    // 成功路径经 +0x46/+0x49/+0x4c 三个 nop 岛回跳；4.4 对 `while(cond)` 恒
    // 旋转成「body 在前、cond 在后」。`while(true){ if(!ScanType) break; ... }`
    // 复现头部条件 + `xor $1; test %al; jne 退出` 形。
    while (true) {
        if (!ScanType(section, true))
            break;
        if (section == "[account restrict]") {
            if (LoadScriptDaily(RestrictType::RestrictType_Account) < 0)
                return -1;
        } else if (section == "[character restrict]") {
            if (LoadScriptDaily(RestrictType::RestrictType_Charac) < 0)
                return -1;
        }
    }
    return 0;
}