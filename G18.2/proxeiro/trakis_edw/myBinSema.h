int my_init(pthread_mutex_t *mutex, int val);

int my_up(pthread_mutex_t *mutex);

int my_down(pthread_mutex_t *mutex);

int my_destroy(pthread_mutex_t *mutex);

int priority_wait(pthread_mutex_t *mutex);
