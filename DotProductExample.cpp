// DotProductExample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

#define THREAD_NUMBER 4

std::mutex dotMtx;
std::condition_variable dotCV;
bool ready = false;

/*Make this multithreaded. Use whatever you feel is best*/
void dotProductCalc(const std::vector<int>& v1, const std::vector<int>& v2, int& result, int low, int high) {

    int partial_sum = 0;

    for (int i = low; i < high; i++) {
        partial_sum += v1[i] * v2[i];
    }

   
    result += partial_sum;

}

int main()
{
    int numElements = 100000;
    int numThreads = THREAD_NUMBER;

    int result = 0;

    std::vector<std::thread> threads;

    std::vector<int> v1(numElements, 1);
    std::vector<int> v2(numElements, 3);

    /*Fire off your threads here and do this calculation over a number of threads*/
    dotProductCalc(v1, v2, result, 0, numElements);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::unique_lock<std::mutex> lock(dotMtx);
    ready = true;
    std::cout << "Fire signal for data ready to be processed" << std::endl;
    lock.unlock();

    /*Call notify here*/

    /*Remember to join threads with main threads*/

    std::cout << "Result:" << result << std::endl;
}

