STExpertJobScript* CDisjointer::GetScript()
{
    static STExpertJobScript* script = 0;   // ORIG GetScript()::script
    if (script == 0)
        script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(3);
    return script;
