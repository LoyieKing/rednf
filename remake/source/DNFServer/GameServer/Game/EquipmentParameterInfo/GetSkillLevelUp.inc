bool GetSkillLevelUp(std::vector<float>* v)
{
    std::string s;
    bool isType = false;
    int val = -1;
    for (;;)
    {
        if (!ScanTypeOrStr(isType, s, true)) return 0;
        if (isType)
        {
            // [b28d-solo-EquipmentParameterInfo] ORIG 0x089910ec `test %al; jne <尾共享
            // mov $1,%ebx>` + `mov $0,%ebx` 失败块 ⇒ P223 尾标签法（goto 跳出循环到
            // 共享 return 1 出口）。
            if (s == "[/skill levelup]") goto L_skillLevelUp_ret1;
            return false;
        }
        v->push_back(29.0f);
        if (s == "[common]")
        {
            v->push_back(11.0f);
        }
        else
        {
            // ORIG 0x0899115c `mov $0,%edx; mov %eax,-0x38; mov %edx,-0x34; fildll`
            // ⇒ 返回值按 unsigned 走零扩展转 float（signed 会生 fildl）。
            v->push_back((float)(unsigned int)getCharacterJob(s.c_str()));
        }
        // [b28d-solo-EquipmentParameterInfo] P167：ORIG 0x08991198/0x089911cf
        // `mov val,%eax; mov %eax,-0x2c; fildl; fstps -0x24/-0x28` ⇒ (float)val
        // 先物化到具名 float，push_back 取左值（push_backERKf 非 EOf）。
        if (!ScanInt(&val)) return 0;
        float f1 = (float)val;
        v->push_back(f1);
        if (!ScanInt(&val)) return 0;
        float f2 = (float)val;
        v->push_back(f2);
    }
L_skillLevelUp_ret1:
    return true;
