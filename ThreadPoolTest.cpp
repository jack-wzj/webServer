#include <iostream>
#include <string>
#include "src/ThreadPool.h"

using namespace std;

void print(int a, double b, const char *c, string d){
    cout << a << b << c << d << endl;
}

void test(){
    std::cout << "testing" << std::endl;
}


int main() {
    ThreadPool *pool = new ThreadPool();
    function<void()> func = bind(print, 1, 2.0, "seu", "string");
    pool->addTask(func);
    func = test;
    pool->addTask(func);
    delete pool;
    return 0;
}