#ifndef SIGNALHANDLER_H_
#define SIGNALHANDLER_H_

class LinuxService;
bool Neof_registerSignalHandlers();
void Neof_setCoreLimit();
bool Neof_sendTerminateSignal();
void Neof_sendSuspendSignal();
void Neof_dumpCoreFile();
void Neof_SignalHandler(int sig);
bool Neof_registerSignalHandler(int sig, void (*handler)(int));
void Neof_SignalLog(const char* msg);

#endif // SIGNALHANDLER_H_
