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

        picture.setPts(i);
        picture.setThreadID(id);
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
        picture.setPts(i);
        picture.setThreadID(id);
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
    PictureQueueMonitor monitor;
    CircularQueue<Picture> q(10, "Picture Queue", monitor.mntrAction, monitor.mntrWait);

    std::thread producer(siso_maker, &q, 1);
    std::thread consumer(siso_taker, &q, 1);

    producer.join();
    consumer.join();

    std::cout << "end" << std::endl;
}

Picture getFullPicture(int width, int height)
{
    Picture p(width, height, true);
    p.setPts(1);
    p.setThreadID(1);
    return p;
}

void semantics()
{
    Picture p1;
    Picture p2(1920, 1080);
    p2.setPts(6);
    p2.setThreadID(6);
    p2.fill();
    std::cout << "p2: " << p2.toString() << std::endl;
    Picture p3(p2);
    p1 = p3;
    std::cout << "p1: " << p1.toString() << std::endl;
    
    std::vector<Picture> pictures;
    pictures.reserve(10);
    for (int i = 0; i < 10; i++)
        pictures.push_back(Picture(1920, 1080, true));

    std::cout <<"p-: " << pictures[0].toString() << std::endl;

    Picture p5 = getFullPicture(1920, 1080);
    std::cout << "p51: " << p5.toString() << std::endl;

    p5 = pictures[0];
    std::cout <<"p52: " << p5.toString() << std::endl;

    Picture p6(1920, 1080);
    p6 = std::move(p5);
    std::cout << "p6: " << p6.toString() << std::endl;

    pictures.insert(pictures.begin() + 5, p2);

    for (Picture picture : pictures)
        std::cout << picture.toString() << std::endl;

    for (int i = 0; i < pictures.size(); i++)
        std::cout << pictures[i].toString() << std::endl;
}

int main(int argc, char* argv[])
{
    time_t start_time = time(NULL);
    srand(start_time);

    siso_test();
}
