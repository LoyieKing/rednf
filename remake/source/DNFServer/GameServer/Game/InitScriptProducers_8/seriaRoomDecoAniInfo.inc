// ORIG C1 0x089625e8：string 默认构造后调 clear（+0x04=7，其余 int 清零）。
seriaRoomDecoAniInfo::seriaRoomDecoAniInfo() throw()
{
    clear();
}