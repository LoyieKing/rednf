int CUser::gainGuildSkillExp(int exp)
{
    if (get_charac_guildkey() == 0)
        return 0;
    if (getGuildMemberGrade() == 0)
        return 0;
    // ORIG 0x864fb7d-0x864fc2e 系：learned@-0x11 **dead init 0 + 赋值两段**（技巧 8）
    // → cmpb $0,-0x11; sete %al 物化 guard（技巧 22）；skill@-0x10 dead init；
    // job/gradeIdx 共用 %ebx 跨 call（块作用域分离）；gradeWeights 无具名槽
    // （skill+0x108 直发 op[]，T-CU15）；at() 结果 `mov (%eax),%eax` 落 weight 槽
    // -0xc 再 imul（具名物化补齐槽后 learned/skill 平移归位）。
    char learned = 0;
    learned = (char)IsGuildSkillLearn(0xc9);
    // P0/N96：去绑定。ORIG `cmpb $0,-0x11; sete %al; test; jne` 物化岛为 4.1
    // 槽比零产物（P197④：4.4 对槽比恒出 cmp/jne 直跳，源形不可复现）。
    // P225 修正 P197④ 边界：while(learned==0){return 0;} 复现
    // `cmpb $0; sete %al; test; je BODY` 物化形（probe p7.t5 逐条实证）。
    while (learned == 0)
    {
        return 0;
    }
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(get_charac_job(), 0xc9);
    // ORIG 0x0864fbdc-0x86fbfd：gradeIdx（=learned-1）跨 operator[] call 存活于
    // %ebx（callee-saved）；P200/P197② 直传形——拓宽表达式作 at() 实参直传，
    // 4.4 即出 `movsbl -0x11; sub $1; mov %eax,%ebx` 并入 %ebx（禁 register-asm
    // 中间变量：退化 cwtl+mov 两步，见 getBlueMarble/OnDisconnect probe s3）。
    int weight = (skill->m_field108)[0].at((int)learned - 1);
    return (exp * weight) / 100;
