CMutex::CMutex() throw() {
    pthread_mutex_init(&mutex, NULL);
}
