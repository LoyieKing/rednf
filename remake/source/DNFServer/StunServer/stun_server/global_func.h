#ifndef GLOBAL_FUNC_H_
#define GLOBAL_FUNC_H_

typedef void (*handler)(int);

bool check_argv(int argc, char** argv);
bool check_pid_file();
int daemon_init();
bool save_pid();
void send_term_signal();
void rdar_terminate(int argv);
bool regist_signal(int sig, handler func_name);
void signal_handler(int sig);
bool regist_signals();
void make_dir(const char* szdir_name);
void write_log(const char* pLog, int argv);

#endif // GLOBAL_FUNC_H_
