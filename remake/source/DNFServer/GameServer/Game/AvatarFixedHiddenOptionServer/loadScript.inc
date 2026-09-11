// ORIG 0x088aedce loadScript。控制流忠实转写：外层 ScanType 失败即返回 0；
// "[upper]" / "[rare]" 分支内先 ScanType 再 parseAbility，parse 未命中回落外层；
// "[mystic circle]" 读一个整数到材料索引。打开脚本失败返回 -1。
int ARAD::SCRIPT::AvatarFixedHiddenOption::loadScript()
{
    if (!loadRDARScriptFile("Etc/Avatar_Roulette/", "AvatarFixedHiddenOptionList.etc"))
        return -1;

    std::string token;
    // [b28d-56 判词·P171/P232] ORIG 0x088aee06 在 token 构造后发一条 dead
    // `movb $0x0,-0x9(%ebp)`（全函数仅此一次读/写该槽）⇒ 原码有该 bool 局部
    // 的预清；补声明使 token 槽回 -0x10、该 bool 落 -0x9，全帧槽号对齐。
    bool flag = false;
    (void)flag;
    for (;;)
    {
        // [b28d-56 判词·P164] ORIG 0x088aee25 `jne <尾部 cleanup>`：ScanType 失败
        // 跳函数尾的 token 析构块（`mov $0x0,%ebx; lea token; call D1`）再统一
        // 返回；写 `return 0` 会被 4.4 就地内联到检查点后 ⇒ 改 break + 循环外
        // 统一 return 0 复现尾部共享块。
        if (!ScanType(token, true))
            break;
        if (token == "[upper]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (!parseAbility(m_abilities[2], token))   // ORIG this+0x18
                    break;
            }
        }
        else if (token == "[rare]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (!parseAbility(m_abilities[3], token))   // ORIG this+0x24
                    break;
            }
        }
        else if (token == "[mystic circle]")
        {
            m_materialItemIndex = ScanInt((bool*)NULL);           // ORIG *(int*)(this+0x48)
        }
    }
    return 0;
}