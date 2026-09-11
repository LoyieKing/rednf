// getQuizIndex @ 0x080cae1e（while 前置 jmp、size() 每次求值 setg 有符号比较、
// operator[] 传 int 下标 ixEj、this 直用不落局部——照 ORIG 汇编）。
int CBingo::getQuizIndex()
{
    int i = 0;
    while (i < static_cast<int>((reinterpret_cast<std::vector<int>*>(this))->size()))
    {
        if ((*(reinterpret_cast<std::vector<int>*>(this)))[i] == 0)
        {
            return i;
        }
        ++i;
    }
    return -1;
}