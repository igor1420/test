#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>

struct Cond
{
    pthread_cond_t *Cond;
    pthread_mutex_t *Mutex;
};

void *condProc(void *arg)
{
    Cond *c = static_cast<Cond *>(arg);
    if (pthread_mutex_lock(c->Mutex) != 0)
    {
        std::cerr << "pthread_mutex_lock" << std::endl;
    }
    while (!c->Cond)
    {
        if (pthread_cond_wait(c->Cond, c->Mutex) != 0)
        {
            std::cerr << "pthread_mutex_wait" << std::endl;
        }
    }
    if (pthread_mutex_unlock(c->Mutex) != 0)
    {
        std::cerr << "pthread_mutex_unlock" << std::endl;
    }
    std::cerr << "the cond is signaled" << std::endl;
}

void* barrierProc(void *arg)
{
    pthread_barrier_t *b = static_cast<pthread_barrier_t*>(arg);
    if (pthread_barrier_wait(b) != 0)
    {
        std::cerr << "pthread_barrier_wait" << std::endl;
    }
    std::cerr << "barrier is finished" << std::endl;
}

int main(int argc, char **argv)
{
    const std::string pidPath = "/home/igor/main.pid";
    std::ofstream pid(pidPath.c_str());
    pid << getpid() << std::endl;

    pthread_cond_t cond;
    if (pthread_cond_init(&cond, NULL) != 0)
    {
        std::cerr << "pthread_cond_init" << std::endl;
        return -1;
    }
    pthread_mutex_t m;
    if (pthread_mutex_init(&m, NULL) != 0)
    {
        std::cerr << "pthread_mutex_init" << std::endl;
    }
    if (pthread_mutex_lock(&m) != 0)
    {
        std::cerr << "pthread_mutex_lock" << std::endl;
    }
    Cond *c = new Cond;
    c->Cond = &cond;
    c->Mutex = &m;
    pthread_t condThread;
    void *retval;
    if (pthread_create(&condThread, NULL, condProc, c) != 0)
    {
        std::cerr << "pthread_create" << std::endl;
    }
    else
    {
        std::cerr << "before pthread_cond_broadcast" << std::endl;
        if (pthread_mutex_unlock(&m) != 0)
        {
            std::cerr << "pthread_mutex_unlock" << std::endl;
        }
        if (pthread_cond_broadcast(&cond) != 0)
        {
            std::cerr << "pthread_cond_signal" << std::endl;
        }
        if (pthread_join(condThread, &retval) != 0)
        {
            std::cerr << "pthread_join" << std::endl;
        }
    }
    if (pthread_cond_destroy(&cond) != 0)
    {
        std::cerr << "pthread_cond_destroy" << std::endl;
    }
    if (pthread_mutex_destroy(&m) != 0)
    {
        std::cerr << "pthread_mutex_destroy" << std::endl;
    }
    delete c;

    pthread_barrier_t b;
    if (pthread_barrier_init(&b, NULL, 1) != 0)
    {
        std::cout << "pthread_barrier_init" << std::endl;
        return -1;
    }
    if (pthread_create(&condThread, NULL, barrierProc, &b) != 0)
    {
        std::cerr << "pthread_create" << std::endl;
    }
    if (pthread_join(condThread, &retval) != 0)
    {
        std::cerr << "pthread_join" << std::endl;
    }
    if (pthread_barrier_destroy(&b) != 0)
    {
        std::cerr << "pthread_barrier_destroy" << std::endl;
    }

    return 0;
}