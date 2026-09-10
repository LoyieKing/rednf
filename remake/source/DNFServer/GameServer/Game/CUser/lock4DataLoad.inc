bool CUser::lock4DataLoad()
{
    // ORIG 0x864a9ac：test %al,%al; je WRITE; mov $0; jmp merge; WRITE: movb $1,0x8d0f0; mov $1
    //（bool 返回 if() 直判；isLocked4DataLoad 定义置于调用点之后以松帧，CDMSB 技巧）
    if (isLocked4DataLoad())
        return 0;
    m_field8d0f0 = 1;
    return 1;
