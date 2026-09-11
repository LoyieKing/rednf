// NOTE(ORIG 0x8230f2a)：另清 +8 int 与 +c int=2，待头文件补字段。
stTeleportParam::stTeleportParam()
{  // ORIG 0x8231362：写入序 +4、+8(word)、+10、+11、+14
    m_4 = 0;
    m_8 = 0;
    m_10 = 0;
    m_11 = 0;
    m_14 = 0;
}