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

    std::unique_lock<std::mutex> lock(dotMtx);
    dotCV.wait(lock, []() {return ready; });
   
    result += partial_sum;
    lock.unlock();
    dotCV.notify_one();

}

std::vector<int> calcLimits(int numTerms, int numThreads) {
    std::vector<int> limits;
    int delta = numTerms / numThreads;
    int remaining = numTerms % numThreads;

    int start = 0;
    int step = 0;

    limits.push_back(start);

    for (int i = 0; i < numThreads; i++) {
        step += delta;

        if (i == numThreads - 1) {
            step += remaining;
        }

        limits.push_back(step);
        start = step;
    }

    return limits;
}

int main()
{
    int numElements = 100000;
    int numThreads = THREAD_NUMBER;

    int result = 0;

    std::vector<std::thread> threads;

    std::vector<int> v1(numElements, 1);
    std::vector<int> v2(numElements, 3);

    std::vector<int> limits = calcLimits(numElements, numThreads);

    /*Fire off your threads here and do this calculation over a number of threads*/
    for (int i = 0; i < numThreads; i++) {
        threads.push_back(std::thread(dotProductCalc, std::ref(v1), std::ref(v2), std::ref(result), limits[i], limits[i + 1]));
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::unique_lock<std::mutex> lock(dotMtx);
    ready = true;
    std::cout << "Fire signal for data ready to be processed" << std::endl;
    lock.unlock();
    dotCV.notify_one();
    /*Call notify here*/

    /*Remember to join threads with main threads*/
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Result:" << result << std::endl;
}

