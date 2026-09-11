// ---- CDisjointer::GetScript / GetEtcScript（ORIG T 0x085d1e0a / 0x085d1e3a：
//      函数级静态缓存单例；ORIG 静态变量 b 0x09472f98 / 0x09472f94）----
STExpertJobScript* CDisjointer::GetScript()
{
    static STExpertJobScript* script = 0;   // ORIG GetScript()::script
    if (script == 0)
        script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(3);
    return script;
}