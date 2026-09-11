// ORIG T 0x080e4ade：vtable+0x1c 虚调用 _reset → memcpy(this+5,data,0x10) → sendInformNoticeFlag → 1。
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
bool CCharacInformNotice::loadData(CUser* user, char* data)
{
    _reset();
    memcpy(m_flags, data, sizeof(m_flags));
    sendInformNoticeFlag(user);
    return true;
}