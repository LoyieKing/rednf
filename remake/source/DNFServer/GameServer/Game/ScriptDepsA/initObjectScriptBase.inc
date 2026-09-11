// ORIG initObjectScriptBase T 0x08a0aeb8：initObjectCategory 的布尔透传包装。
bool initObjectScriptBase()
{
    // ORIG `xor $1,%eax; test %al; je`：initObjectCategory 返回 bool，`!` 翻转直测寄存器
    // （P198② 判定内联直测）；return-0 作 fall-through、return-1 作岛。直写透传会丢此形。
    if (!initObjectCategory())
        return 0;
    return 1;
}