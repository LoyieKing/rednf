// ORIG 0x0812beb0 T _ZN33DisPatcher_EventDungeon_ClearRoom11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/check_error.md 全文转写：
//   push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret）
int DisPatcher_EventDungeon_ClearRoom::check_error(CUser* /*user*/, MSG_BASE& /*msg*/)

{
    return 0;
}
