bool Dispatcher_MouseRegister::decryptPassword(char const* in, char* out, unsigned int len)
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
