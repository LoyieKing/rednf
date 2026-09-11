void stAvatarEmblemInfo_t::init()
{
    // ORIG 0x081506e8：循环 i=0..4，清零 stEmblemSocket[i] 的
    // emblemSocketType_(word, +6i) 与 emblemItemIdx_(dword, +6i+2)
    for (int i = 0; i < 5; ++i)
    {
        stEmblemSocket[i].emblemSocketType_ = 0;
        stEmblemSocket[i].emblemItemIdx_ = 0;
    }
}