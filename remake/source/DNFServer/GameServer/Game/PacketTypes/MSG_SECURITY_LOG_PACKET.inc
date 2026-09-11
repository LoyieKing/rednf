MSG_SECURITY_LOG_PACKET::MSG_SECURITY_LOG_PACKET()
{  // ORIG C1：memset(this,0,0x25)
    memset(this, 0, 0x25);    // ORIG C1：constants 先装(先 arg3/arg2 再 this)——去掉 (char*) 强转
}