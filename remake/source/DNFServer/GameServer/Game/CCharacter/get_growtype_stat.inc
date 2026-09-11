// ORIG get_growtype_stat @ 0x834883e：按 (firstGrow, secondGrow) 返回成长属性块
// 指针；越界（firstGrow>5 或 secondGrow>2）返回 NULL。secondGrow==0 时取
// firstGrow*0x52+0x52（= m_baseInfo[firstGrow+1]，块宽 sizeof(_Additioal_info)=0x52），
// 否则取 (secondGrow-1)*0x52+(firstGrow+1)*0xc0+0x294
// （= &m_secondGrowTypes[firstGrow+1].m_addInfo[secondGrow-1]，
//   m_secondGrowTypes @+0x294、步长 sizeof(STSecondGrowTypeInfo)=0xc0、
//   m_addInfo 元素步长 0x52；布局断言见 CCharacter.h）。
const _Additioal_info* CCharacter::get_growtype_stat(int firstGrow,
                                                     int secondGrow) const
{
    if (firstGrow > 5 || secondGrow > 2)
        return 0;
    if (secondGrow == 0)
        return &m_baseInfoGrow[firstGrow];
    return &m_secondGrowTypes[firstGrow + 1].m_addInfo[secondGrow - 1];
}