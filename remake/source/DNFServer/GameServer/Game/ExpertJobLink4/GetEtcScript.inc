STExpertJobEtcScript* CDisjointer::GetEtcScript()
{
    static STExpertJobEtcScript* script = 0;   // ORIG GetEtcScript()::script
    if (script == 0)
        script = (STExpertJobEtcScript*)G_CDataManager()->GetExpertJobEtcScript();
    return script;
