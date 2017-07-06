#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>
#include "leetcode.h"
using namespace std;

mutex io_mutex;

class BlockingQueue {
public:
    BlockingQueue(int queueSize) : qsize(queueSize){
    }

    void push(int n){
        unique_lock<mutex> lk(m);
        cv_p.wait(lk, [&]{return q.size() < qsize;});
        q.push(n);
        lk.unlock();
        cv_c.notify_all();
    }

    int pop(){
        unique_lock<mutex> lk(m);
        cv_c.wait(lk, [&]{return !q.empty();});
        int ret = q.front();
        q.pop();
        if (q.size() + 1 == qsize) {
            cv_p.notify_one();
        }
        return ret;
    }
    int front(){
        unique_lock<mutex> lk(m);
        cv_c.wait(lk, [&]{return !q.empty();});
        return q.front();
    }
    int size(){
        lock_guard<mutex> lk(m);
        return q.size();
    }
private:
    queue<int> q;
    int qsize;
    mutex m;
    condition_variable cv_p;
    condition_variable cv_c;
};

void product(BlockingQueue& q){
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::srand(std::time(0));
    for(int i = 0; i < 21; i++){
        int ret = rand();
        q.push(ret);
        lock_guard<mutex> lk(io_mutex);
        cout << ret << " push" << endl;
    }

}

void consume(BlockingQueue& q, int id){
   std::this_thread::sleep_for(std::chrono::seconds(2));
    for(int i = 0; i < 7; i++){
        int ret = q.pop();
        lock_guard<mutex> lk(io_mutex);
        cout << "          " << id << " thread pop " << ret << endl;
    }
}

void readfront(BlockingQueue& q, int id) {
    //std::this_thread::sleep_for(std::chrono::seconds(1));

    int ret = q.front();
    lock_guard<mutex> lk(io_mutex);
    cout << "watcher " << id << " read the front [" << ret <<"]" << endl;
}
int main()
{
    BlockingQueue q(10);

    thread p(product, ref(q)), v1(consume, ref(q), 1), v2(consume, ref(q), 2), v3(consume, ref(q), 3);
    thread w1(readfront, ref(q), 1);
    thread w2(readfront, ref(q), 2);
    thread w3(readfront, ref(q), 3);
    w1.join();
    w2.join();
    w3.join();
    p.join();
    v1.join();
    v2.join();
    v3.join();

}
