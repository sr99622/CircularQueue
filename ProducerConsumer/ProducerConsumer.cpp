#include <iostream>
#include <thread>
#include "Picture.h"
#include "CircularQueue.h"

void mimo_maker(CircularQueue<Picture>* q, int id)
{
    int width = 3840;
    int height = 2160;
    Picture picture(width, height);

    for (int i = 0; i < 20; i++) {

        // test for inconsistent input response
        //if (i % 4 == 0) {
        //    width = 1920;
        //    height = 1080;
        //}

        picture.set_pts(i);
        picture.set_thread_id(id);
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

void mimo_taker(CircularQueue<Picture>* q, int id)
{
    Picture picture;
    while (q->isOpen()) {
        try {
            q->pop(picture);
            //Picture picture = q->pop(); // make new picture alternate call is slower
            std::cout << "taker " << picture.toString() << std::endl;
        }
        catch (const QueueClosedException& e) {
            std::cout << "taker " << e.what() << std::endl;
            break;
        }
    }
}

void mimo_test()
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
        producers.push_back(std::thread(mimo_maker, &q, i));
    for (int i = 0; i < number_of_consumers; i++)
        consumers.push_back(std::thread(mimo_taker, &q, i));

    for (std::thread& producer : producers)
        producer.join();

    q.flush();

    for (std::thread& consumer : consumers)
        consumer.join();

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    std::cout << "elapsed: " << elapsed << std::endl;
}

void siso_maker(CircularQueue<Picture>* q, int id)
{
    int width = 3840;
    int height = 2160;
    Picture picture(width, height);

    for (int i = 0; i < 20; i++) {
        picture.set_pts(i);
        picture.set_thread_id(id);
        picture.fill();
        std::cout << "maker " << picture.toString() << std::endl;
        try {
            q->push(picture);
        }
        catch (const QueueClosedException& e) {
            std::cout << "maker " << e.what() << std::endl;
        }
    }
    picture.inValidate();
    q->push(picture);
    std::cout << "siso maker finished" << std::endl;
}

void siso_taker(CircularQueue<Picture>* q, int id)
{
    Picture picture;

    while (true) {
        try {
            q->pop(picture);
            if (picture.isValid()) {
                std::cout << "taker " << picture.toString() << std::endl;
            } 
            else {
                std::cout << "received eof signal" << std::endl;
                break;
            }

        }
        catch (const QueueClosedException& e) {
            std::cout << "taker " << e.what() << std::endl;
            break;
        }
    }

    std::cout << "siso taker finished" << std::endl;
}

void siso_test()
{
    time_t start_time = time(NULL);
    srand(start_time);
    CircularQueue<Picture> q(10);
    PictureQueueMonitor monitor;
    q.setName("Picture Queue");
    q.mntrAction = monitor.q_action;
    q.mntrLock = monitor.q_lock;

    std::thread producer(siso_maker, &q, 1);
    std::thread consumer(siso_taker, &q, 1);

    producer.join();
    consumer.join();

    std::cout << "end" << std::endl;

}

int main(int argc, char* argv[])
{
    siso_test();
}
