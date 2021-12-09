#include <iostream>
#include <thread>
#include "Picture.h"
#include "CircularQueue.h"


void maker(CircularQueue<Picture*>* q)
{
    for (int i = 0; i < 100; i++) {
        Picture* picture = new Picture(3480, 2160);
        picture->pts = i;
        picture->fill();
        q->push(picture);
        std::cout << "maker q size: " << q->size() << std::endl;
    }
}

void taker(CircularQueue<Picture*>* q)
{
    while (q->isOpen()) {
        Picture* picture = q->pop();
        if (picture) {
            std::cout << " taker q size: " << q->size() << std::endl;
            std::cout << " picture pts: " << picture->pts << std::endl;
            uint64_t total = 0;
            for (int y = 0; y < picture->height; y++) {
                for (int x = 0; x < picture->width; x++) {
                    int z = y * picture->width + x;
                    total += picture->data[z];
                }
            }
            delete picture;
            std::cout << "total: " << total << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
        }
        else {
            q->close();
            break;
        }
    }
}

int main()
{
    time_t current_time = time(NULL);
    srand(current_time);
    CircularQueue<Picture*> q(10);

    int number_of_producers = 4;
    int number_of_consumers = 10;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < number_of_producers; i++)
        producers.push_back(std::thread(maker, &q));
    for (int i = 0; i < number_of_consumers; i++)
        consumers.push_back(std::thread(taker, &q));

    for (std::thread& producer : producers)
        producer.join();

    q.push(NULL);

    for (std::thread& consumer : consumers)
        consumer.join();
}
