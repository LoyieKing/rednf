// ORIG C1 0x0848683c：IData C1 + PlayInfo C1（基类）+ Character::Reset()（0x08486818
// = PlayInfo::Reset() + movl $0,0x3c）。ORIG 不初始化 Character 自身字段。
Character::Character()
{
    Reset();
}