#!/usr/bin/env python3
import subprocess, sys
ORIG = "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394"
cus = [
 "comm_socket_channel_tcp_handler.cpp",
 "comm_socket_channel_bus.cpp",
 "comm_socket_channel_info.cpp",
 "comm_socket_channel_base_handler.cpp",
 "zen_os_adapt_rwlock.cpp",
 "zen_lock_thread_rw_mutex.cpp",
 "zen_lock_file_lock.cpp",
 "zen_os_adapt_dirent.cpp",
 "zen_server_status.cpp",
 "app_bus_channel_id_mgr.cpp",
]
for c in cus:
    print("########", c)
    r = subprocess.run([sys.executable, "cu_symbols.py", ORIG, c, "--sizes"],
                       capture_output=True, text=True)
    print(r.stdout[-20000:])
    if r.returncode:
        print("ERR", r.stderr[-2000:])
