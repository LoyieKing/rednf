void StrikerScript::readSkillPenalty(std::vector<TagSkillPenalty>& out)
{
    while (true)
    {
        bool ok = true;
        TagSkillPenalty penalty;                       // ORIG C1 T 0x08a9d768
        penalty.m_skillIdx = ScanInt(&ok);             // +0x00（-0x20）
        if (!ok)
            break;
        penalty.m_value = ScanInt((bool*)0);           // +0x04（-0x18）
        std::string tok;
        ScanStr(&tok);
        if (tok == "%")                            // ORIG test/je + movb $1/$0
            penalty.m_isPercent = true;            // 条件赋值形态，保持指令同构
        else
            penalty.m_isPercent = false;           // +0x08（-0x14）
        penalty.m_pad = ScanInt((bool*)0);             // +0x0c（-0x10，真实第 4 数据字段）
        out.push_back(penalty);
    }
