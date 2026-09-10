void CMutex::lock() {
    pthread_mutex_lock(&mutex);
}
