#include <iostream>
#include <thread>
#include "Picture.h"
#include "CircularQueue.h"


void maker(CircularQueue<Picture>* q, int id)
{
    for (int i = 0; i < 100; i++) {
        int width = 3840;
        int height = 2160;
        if (i % 4 == 0) {
            width = 1920;
            height = 1080;
        }
        Picture picture(width, height);
        picture.pts = i;
        picture.thread_id = id;
        picture.fill();
        std::cout << "maker " << picture.toString() << std::endl;
        try {
            q->push(picture);
        }
        catch (const QueueClosedException& e) {
            std::cout << "maker " << e.what() << std::endl;
        }
    }
}

void taker(CircularQueue<Picture>* q, int id)
{
    while (q->isOpen()) {
        try {
            Picture picture = q->pop();
            std::cout << "taker " << picture.toString() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
        }
        catch (const QueueClosedException& e) {
            std::cout << "taker " << e.what() << std::endl;
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    time_t start_time = time(NULL);
    srand(start_time);
    CircularQueue<Picture> q(10);

    int number_of_producers = 4;
    int number_of_consumers = 10;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < number_of_producers; i++)
        producers.push_back(std::thread(maker, &q, i));
    for (int i = 0; i < number_of_consumers; i++)
        consumers.push_back(std::thread(taker, &q, i));

    for (std::thread& producer : producers)
        producer.join();

    q.flush();

    for (std::thread& consumer : consumers)
        consumer.join();

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    std::cout << "elapsed: " << elapsed << std::endl;
}
