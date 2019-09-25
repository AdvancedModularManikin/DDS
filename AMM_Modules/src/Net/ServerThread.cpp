#include "ServerThread.h"

using namespace std;

pthread_mutex_t ServerThread::mutex;

ServerThread::ServerThread() = default;

int ServerThread::Create(void *Callback, void *args) {
    int tret = 0;

    tret = pthread_create(&this->tid, nullptr, (void *(*)(void *)) Callback, args);

    if (tret != 0) {
        cerr << "Error while creating threads." << endl;
        return tret;
    }

    // cout << "Thread successfully created." << endl;
    return 0;
}

int ServerThread::Join() {
    pthread_join(this->tid, nullptr);
    return 0;
}

int ServerThread::InitMutex() {

    if (pthread_mutex_init(&ServerThread::mutex, nullptr) < 0) {
        cerr << "Error while initializing mutex" << endl;
        return -1;
    }

    // cout << "Mutex initialized." << endl;
    return 0;
}

/*
    LockMutex():
                Blocks until mutex becomes available
*/
int ServerThread::LockMutex(const string &identifier) {
    if (pthread_mutex_lock(&ServerThread::mutex) == 0) {
        return 0;
    }

    cerr << "Error while " << identifier << " was trying to acquire the lock"
         << endl;
    return -1;
}

int ServerThread::UnlockMutex(const string &identifier) {
    if (pthread_mutex_unlock(&ServerThread::mutex) == 0) {
        return 0;
    }

    cerr << "Error while " << identifier << " was trying to release the lock"
         << endl;
    return -1;
}
