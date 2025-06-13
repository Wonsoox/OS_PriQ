#include <iostream>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "queue.cpp"

using namespace std;
using namespace chrono;

#define REQUESTS_PER_CLIENT 10000
#define MAX_VALUE_SIZE 1024  // 최대 value 크기

atomic<long long> sum_key = 0;
atomic<long long> sum_value = 0;

typedef enum { GET, SET, GETRANGE } Operation;

typedef struct {
    Operation op;
    Item item;
} Request;

void client_func(Queue* queue, Request requests[], int n_request) {
    for (int i = 0; i < n_request; i++) {
        Reply reply = { false, {0, nullptr, 0} };

        if (requests[i].op == GET) {
            reply = dequeue(queue);
        } else { // SET
            reply = enqueue(queue, requests[i].item);
        }

        if (reply.success) {
            sum_key += reply.item.key;

            // value는 char*로 가정하고 int로 변환해서 누적
            if (reply.item.value_size >= sizeof(int) && reply.item.value != nullptr) {
                int val;
                memcpy(&val, reply.item.value, sizeof(int));
                sum_value += val;
            }

            // 반환된 value 해제 (GET의 경우만, value가 nullptr이 아닐 때만)
            if (requests[i].op == GET && reply.item.value != nullptr) {
                delete[] static_cast<char*>(reply.item.value);
            }
        }
    }
}

int main() {
    srand((unsigned int)time(NULL));

    const int NUM_CLIENTS = 32;
    Request** requests = new Request*[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; i++) {
        requests[i] = new Request[REQUESTS_PER_CLIENT];
    }

    for (int c = 0; c < NUM_CLIENTS; c++) {
        for (int i = 0; i < REQUESTS_PER_CLIENT / 2; i++) {
            requests[c][i].op = SET;
            requests[c][i].item.key = i + c * REQUESTS_PER_CLIENT;

            int val = rand() % 1000000;
            int size = sizeof(int);  // 최소 테스트용으로 int 크기만 사용
            char* buffer = new char[size];
            memcpy(buffer, &val, size);

            requests[c][i].item.value = buffer;
            requests[c][i].item.value_size = size;
        }

        for (int i = REQUESTS_PER_CLIENT / 2; i < REQUESTS_PER_CLIENT; i++) {
            requests[c][i].op = GET;
            requests[c][i].item = {0, nullptr, 0}; // GET에는 item 의미 없음
        }
    }

    Queue* queue = init();

    auto start_time = high_resolution_clock::now();

    thread clients[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; i++) {
        clients[i] = thread(client_func, queue, requests[i], REQUESTS_PER_CLIENT);
    }

    for (int i = 0; i < NUM_CLIENTS; i++) {
        clients[i].join();
    }

    auto end_time = high_resolution_clock::now();
    duration<double> elapsed = end_time - start_time;

    // range 테스트: key 0~10000 범위의 아이템 개수와 key/value 합 출력
    Key range_start = 0;
    Key range_end = 10000;
    Queue* subq = range(queue, range_start, range_end);

    cout << "[range test] subq->size = " << subq->size << endl;
    long long range_key_sum = 0;
    long long range_value_sum = 0;
    for (int i = 0; i < subq->size; ++i) {
        range_key_sum += subq->heap[i].key;
        if (subq->heap[i].value_size >= sizeof(int) && subq->heap[i].value != nullptr) {
            int val;
            memcpy(&val, subq->heap[i].value, sizeof(int));
            range_value_sum += val;
        }
    }
    cout << "[range test] key sum: " << range_key_sum << ", value sum: " << range_value_sum << endl;

    release(subq);

    release(queue);

    int total_requests = NUM_CLIENTS * REQUESTS_PER_CLIENT;
    double throughput = total_requests / elapsed.count();

    cout << "Sum of returned keys: " << sum_key.load() << endl;
    cout << "Sum of returned values: " << sum_value.load() << endl;
    cout << "Total time: " << elapsed.count() << " seconds" << endl;
    cout << "Throughput: " << throughput << " requests/second" << endl;

    // 메모리 해제
    for (int i = 0; i < NUM_CLIENTS; i++) {
        delete[] requests[i];
    }
    delete[] requests;

    return 0;
}
