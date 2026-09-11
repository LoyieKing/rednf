// ORIG T 0x0826333a：经 CTEA 虚 Decrypt 解密；失败记日志返回 false。
// [b28c-31] N7：虚调用 callee ORIG %ecx vs 4.4 %eax + 实参装载寄存器 %edx↔%ecx
// （0x0826335x 段），探针/写法穷举不可翻（N7 判词）。
bool Dispatcher_MouseRegister::decryptPassword(char const* in, char* out, size_t len)
{
    // ORIG 0x0826333a（报告 Dispatcher_MouseRegister/decryptPassword.md 全文已读）：
    // 虚表 +0x10 解密调用；ret<0 → logFormat 后 return false（mov $0），否则 mov $1。
    int ret = GlobalData::s_pcryptTEA_MousePassword_->Decrypt(in, out, (size_t)len);
    if (ret < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Secu.cpp",
            __PRETTY_FUNCTION__,
            0x1b1, "[%s][%d][%s]", "PacketDispatcher_Secu.cpp", 0x1b1, "Decrypt Fail");
        return false;
    }
    return true;
}