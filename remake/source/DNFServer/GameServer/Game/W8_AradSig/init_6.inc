// P210：init 定义先于 set（ORIG init 帧 0x18，4.4 若先见 set 定义则继承其小帧）。
void SigSetCharacCreateTime::init()
{
    set(0, 0);
}