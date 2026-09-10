int Dispatcher_FatigueAttendance::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_FATIGUE_ATTENDANCE& req = reinterpret_cast<MSG_FATIGUE_ATTENDANCE&>(msg);
    // stFatigueAttendance 字段（现 m_pad[0x40] 占位；建议宿主字段化）：
    //   +0x04 int  errNo（check_error 结果，send 消费）
    //   +0x08 int  当日序号（= req.m_0d 回填，send 以 short 发出）
    //   +0x0c unsigned 打卡位图（setBit 后的 history.first 快照，send put_int）
    //   +0x10 int  今日时间值（get_today_time，send put_int）
    char* pres = reinterpret_cast<char*>(&param);

    // ORIG +0x008：param+8 = *(int*)(msg+0xd)（先于 check_error 回填请求当日序号）
    *reinterpret_cast<int*>(pres + 8) = req.m_0d;

    int err = check_error(user, msg);
    *reinterpret_cast<int*>(pres + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x49c5,
            __PRETTY_FUNCTION__,
            err, 0);
    }

    // ORIG：每个使用点重取 G_CDataManager()->get_event_script_mng()（P48 无 CSE）。
    unsigned char day = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_today_time_day(); // +0x09d
    int recordType = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_record_type(0x56); // +0x0bd

    // ORIG 0x081daffa：get_ingame_event_history(recordType, history) 读历史
    std::pair<int, int> history;
    user->get_ingame_event_history(recordType, history);
    unsigned int attendFlags = static_cast<unsigned int>(history.first);

    // 已打过当日卡（bit day-1 已置位）→ 0x12
    if (isSetBit_Uint(attendFlags, static_cast<int>(day) - 1))
    {
        *reinterpret_cast<int*>(pres + 4) = 0x12;
        return 0;
    }

    // 疲劳合计：server_fatigue_count(day)（short）+ fatigue_count_db(day)
    int totalFatigue = static_cast<int>(
        user->get_server_fatigue_count(day));
    totalFatigue += user->get_fatigue_count_db(static_cast<int>(day));

    EventClassify::CEventEntity* entity = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_event_entity(0x56);   // +0x166
    if (entity == 0)
    {
        return 0;
    }

    int todayTime = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_today_time();
    // ORIG：start > today || end < today → 不在活动期 → 7
    if (entity->get_event_start_time() > todayTime ||
        entity->get_event_end_time() < todayTime)
    {
        *reinterpret_cast<int*>(pres + 4) = 7;
        return 0;
    }

    // 疲劳超上限（fatigue_condition < total）→ 0x16
    EventClassify::CAttendance* attendance =
        static_cast<EventClassify::CAttendance*>(entity);
    if (attendance->get_fatigue_condition() < totalFatigue)
    {
        *reinterpret_cast<int*>(pres + 4) = 0x16;
        return 0;
    }

    // 置当日打卡位并回写历史（ORIG 0x081daffa：set_ingame_event_history(recordType, attendFlags, 0)）
    setBit_Uint(attendFlags, true, static_cast<int>(day) - 1);
    user->set_ingame_event_history(recordType, attendFlags, 0);

    *reinterpret_cast<unsigned int*>(pres + 0xc) = attendFlags;          // +0x24a
    int nowTime = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_today_time();
    *reinterpret_cast<int*>(pres + 0x10) = nowTime;                      // +0x265

    int passDay = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())
        ->get_pass_day(entity->get_event_start_time(), nowTime);
    attendance->send_attend_reward_item(user, passDay);                  // T 0x0810e878

    // ORIG 0x081daffa：process_update_ingame_history(user) 后 reset_ingame_event_history_update()
    ((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
        ->process_update_ingame_history(user);
    user->reset_ingame_event_history_update();

    int finalCondition = attendance->get_final_day_condition();
    attendance->send_attend_final_rward_item(user, static_cast<int>(attendFlags),
                                             finalCondition);            // T 0x0810e9c0
    return 0;
