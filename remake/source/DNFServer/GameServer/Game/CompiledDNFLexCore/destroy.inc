// ---- destroy（ORIG 0x8ad0370）----
void CompiledDNFLex::destroy()
{
    if (stringManagerDelete_)
    {
        ScriptStringManager* p = stringManager_;
        if (p != 0)
        {
            void (**vt)(void*) = *(void (***)(void*))p;
            vt[1](p);
            stringManager_ = 0;
        }
        stringManagerDelete_ = false;
    }
    else
        stringManager_ = 0;
    if (stringBuffer_ != 0)
    {
        ::operator delete[](stringBuffer_);
        stringBuffer_ = 0;
    }
}