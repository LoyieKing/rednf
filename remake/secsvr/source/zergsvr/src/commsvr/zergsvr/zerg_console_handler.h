// Auto-generated header stub from DWARF info
// Original path: src/commsvr/zergsvr/zerg_console_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_CONSOLE_HANDLER_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_CONSOLE_HANDLER_H_H_

// sizeof = 136
struct Zerg_Console_Handler : public MML_Console_Handler { // line 9
public:
Zerg_Console_Handler(const Zerg_Console_Handler &arg0);
Zerg_Console_Handler(ZEN_Reactor *arg0); // line 24
protected:
virtual ~Zerg_Console_Handler(); // line 29
virtual int process_mml_command(char *ret_string, size_t &str_len); // line 46
int cmd_get_zergstats(char *ret_string, size_t &str_len); // line 137
int cmd_get_peer_stats(char *ret_string, size_t &str_len); // line 180
int cmd_close_services(char *ret_string, size_t &str_len); // line 228
int cmd_close_socketpeer(char *ret_string, size_t &str_len); // line 240
int cmd_modify_logpriority(char *ret_string, size_t &str_len); // line 292
};

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_CONSOLE_HANDLER_H_H_
