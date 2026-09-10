void CMutex::unlock() {
    pthread_mutex_unlock(&mutex);
