void CompiledDNFLex::create(bool createStringTable)
{
    if (createStringTable)
    {
        if (stringManager_ == 0)
            stringManager_ = new ScriptStringManager();
        stringManagerDelete_ = true;
    }
    if (stringBuffer_ == 0)
        stringBuffer_ = new char[STRING_BUFFER_SIZE];
