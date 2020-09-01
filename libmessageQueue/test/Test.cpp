#include "messageQueue.h"
#include "Tester.hpp"
#include <functional>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

// Test that MsgUIDs generated in two different threads simultaneously are unique
void testMsgUID()
{
    const int N = 1000;
    std::vector<MAX_Message::MsgUID> uids1, uids2;

    auto createMsgs = [](int count, std::vector<MAX_Message::MsgUID>& uids)
    {
        for (int i = 0; i < count; ++i)
            uids.push_back(MAX_Message::Msg(1).getUniqueId());
    };

    std::thread t1(createMsgs, N, std::ref(uids1));
    std::thread t2(createMsgs, N, std::ref(uids2));
    t1.join();
    t2.join();

    for (auto uid1 : uids1)
    {
        for (auto uid2 : uids2)
        {
            TEST_NOT_EQUALS(uid1, uid2);
        }
    }
}

// Test that messages are received in order in 1-to-1 one-way messaging scenario
void testMsgOrder()
{
    const int N = 1000;
    MAX_Message::Queue queue;

    auto sender = [](int count, MAX_Message::Queue& q)
    {
        for (int i = 0; i < count; ++i)
            q.put(MAX_Message::Msg(i));
    };

    auto receiver = [](int count, MAX_Message::Queue& q)
    {
        for (int i = 0; i < count; ++i)
        {
            auto m = q.get();
            TEST_EQUALS(m->getMsgId(), i);
        }
    };

    std::thread t1(sender, N, std::ref(queue));
    std::thread t2(receiver, N, std::ref(queue));
    t1.join();
    t2.join();
}

// Test that messages are received in order in 2-to-1 one-way messaging scenario
void test2To1MsgOrder()
{
    const int N = 1000;
    MAX_Message::Queue queue;

    auto sender = [](int msgId, int count, MAX_Message::Queue& q)
    {
        for (int i = 0; i < count; ++i)
            q.put(MAX_Message::DataMsg<int>(msgId, i));
    };

    auto receiver = [](int count, MAX_Message::Queue& q)
    {
        int expectedData1 = 0;
        int expectedData2 = 0;

        for (int i = 0; i < count; ++i)
        {
            auto m = q.get();
            auto& dm = dynamic_cast<MAX_Message::DataMsg<int>&>(*m);

            if (dm.getMsgId() == 1)
            {
                TEST_EQUALS(dm.getPayload(), expectedData1);
                ++expectedData1;
            }
            else if (dm.getMsgId() == 2)
            {
                TEST_EQUALS(dm.getPayload(), expectedData2);
                ++expectedData2;
            }
            else
            {
                TEST_FAIL("Unexpected message id");
            }
        }
    };

    std::thread t1(sender, 1, N, std::ref(queue));
    std::thread t2(sender, 2, N, std::ref(queue));
    std::thread t3(receiver, 2 * N, std::ref(queue));
    t1.join();
    t2.join();
    t3.join();
}

// Test putting DataMsg through the queue
void testDataMsg()
{
    MAX_Message::Queue q;
    q.put(MAX_Message::DataMsg<std::string>(42, "foo"));
    auto m = q.get();
    auto& dm = dynamic_cast<MAX_Message::DataMsg<std::string>&>(*m);
    TEST_EQUALS(dm.getMsgId(), 42);
    TEST_EQUALS(dm.getPayload(), std::string("foo"));
    // Test modifying the payload data
    dm.getPayload() += "bar";
    TEST_EQUALS(dm.getPayload(), std::string("foobar"));
}

// Test putting DataMsg through the queue
void testMatMsg()
{
    MAX_Message::Queue q;
    cv::Mat ones =cv::imread("/home/shining/Pictures/617286e5cc296b0bb03d1cbd0d318f9c.jpg");
    q.put(MAX_Message::DataMsg<cv::Mat>(42, ones));
    q.put(MAX_Message::DataMsg<cv::Mat>(42, ones));
    auto size =  q.size();
    printf("\n Queue size:%5d \n",size);
    auto m = q.get();
    size =  q.size();
    printf("\n Queue size:%5d \n",size);
    q.put(MAX_Message::DataMsg<cv::Mat>(42, ones));
    auto m_2 = q.get(4);


    auto& dm = dynamic_cast<MAX_Message::DataMsg<cv::Mat>&>(*m);
    auto& dm_2 = dynamic_cast<MAX_Message::DataMsg<cv::Mat>&>(*m_2);
    TEST_EQUALS(dm.getMsgId(), 42);
    TEST_EQUALS(dm.getPayload().cols, ones.cols);
    TEST_EQUALS(dm.getPayload().rows, ones.rows);
    // Test modifying the payload data
    auto am = dm.getPayload();
    cv::imwrite("save.jpg",am);
}


// Test putting DataMsg through the queue
void testMatMsgPointer()
{
    static MAX_Message::Queue * q;

    cv::Mat ones =cv::imread("/home/shining/Pictures/617286e5cc296b0bb03d1cbd0d318f9c.jpg");
    q->put(MAX_Message::DataMsg<cv::Mat>(42, ones));
    q->put(MAX_Message::DataMsg<cv::Mat>(42, ones));
    auto size =  q->size();
    printf("\n Queue size:%5d \n",size);
    auto m = q->get();
    size =  q->size();
    printf("\n Queue size:%5d \n",size);
    auto m_2 = q->get(MAX_Message::WAIT_MESSAGE_FOREVER);


    auto& dm = dynamic_cast<MAX_Message::DataMsg<cv::Mat>&>(*m);
    auto& dm_2 = dynamic_cast<MAX_Message::DataMsg<cv::Mat>&>(*m_2);
    TEST_EQUALS(dm.getMsgId(), 42);
    TEST_EQUALS(dm.getPayload().cols, ones.cols);
    TEST_EQUALS(dm.getPayload().rows, ones.rows);
    // Test modifying the payload data
    auto am = dm.getPayload();

}


// Test timeout when getting message from the queue
void testReceiveTimeout()
{
    MAX_Message::Queue q;

    // Test first with a Msg in the queue that specifying timeout for get() doesn't have an effect
    q.put(MAX_Message::Msg(1));

    auto start = std::chrono::steady_clock::now();
    auto m = q.get(10);
    auto end = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    TEST_EQUALS(m->getMsgId(), 1);
    //TEST_LESS_THAN(dur, 5LL);

    // Then test with empty queue
    start = std::chrono::steady_clock::now();
    auto m2 = q.get(10);
    end = std::chrono::steady_clock::now();
    dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    printf("\n testReceiveTimeout:%5d \n",dur);
//    TEST_EQUALS(m2->getMsgId(), MAX_Message::MSG_TIMEOUT);
    //TEST_LESS_THAN(5LL, dur);
    //TEST_LESS_THAN(dur, 15LL);
}

// Test 2-to-1 request-response scenario
void testRequestResponse()
{
    const int N = 1000;

    MAX_Message::Queue queue;

    auto requester1 = [](int count, MAX_Message::Queue& q)
    {
        for (int i = 0; i < count; ++i)
        {
            TEST_EQUALS(q.request(MAX_Message::Msg(i))->getMsgId(), i + count);
        }
    };

    auto requester2 = [](int count, MAX_Message::Queue& q)
    {
        for (int i = 0; i < count; ++i)
        {
            TEST_EQUALS(q.request(MAX_Message::Msg(i + 2 * count))->getMsgId(), i + 3 * count);
        }
    };

    auto responder = [](int count, MAX_Message::Queue& q)
    {
        for (int i = 0; i < 2 * count; ++i)
        {
            auto m = q.get();
            q.respondTo(m->getUniqueId(), MAX_Message::Msg(m->getMsgId() + count));
        }
    };

    std::thread t1(requester1, N, std::ref(queue));
    std::thread t2(requester2, N, std::ref(queue));
    std::thread t3(responder, N, std::ref(queue));
    t1.join();
    t2.join();
    t3.join();
}

class b{
public:
    static b *instance()
    {
        static b oneself;
        return &oneself;
    }
    MAX_Message::Queue * queue;


    void got_queue(MAX_Message::Queue * other ){
        queue = other;
        if(0 ==  MAX_Message::OK ){

        }
    }
    void put(){

        cv::Mat ones =cv::imread("/home/shining/Pictures/617286e5cc296b0bb03d1cbd0d318f9c.jpg");
        queue->put(MAX_Message::DataMsg<cv::Mat>(42, ones));
        queue->put(MAX_Message::DataMsg<cv::Mat>(42, ones));
    };
    void  get();
};

class a{
public:
    MAX_Message::Queue queue;
    void send_queue(){
        b::instance()->got_queue(&queue);
        cv::Mat pic = cv::Mat();
        if(pic.empty()){
            printf("\n pic size:%5d \n",pic.empty());
        }

    }
    int size(){
        return  queue.size();
    }
};

int main()
{
    // Statically assert that messages can't be copied or moved
    static_assert(!std::is_move_constructible<MAX_Message::Msg>::value, "Msg can't be copyable");
    static_assert(!std::is_move_assignable<MAX_Message::Msg>::value, "Msg can't be copyable");
    static_assert(!std::is_move_constructible<MAX_Message::DataMsg<int>>::value, "DataMsg can't be copyable");
    static_assert(!std::is_move_assignable<MAX_Message::DataMsg<int>>::value, "DataMsg can't be copyable");

    Tester tester("Test MAX_Message");
    tester.addTest(testMsgUID, "Test MsgUID generation");
    tester.addTest(testMsgOrder, "Test 1-to-1 message order");
    tester.addTest(test2To1MsgOrder, "Test 2-to-1 message order");
    tester.addTest(testDataMsg, "Test DataMsg");
    tester.addTest(testMatMsg, "Test MatMsg");
    tester.addTest(testReceiveTimeout, "Test receive timeout");
    tester.addTest(testRequestResponse, "Test 2-to-1 request-response");
    tester.runTests();

    a m_a;
    m_a.send_queue();
    b::instance()->put();
    auto size =  m_a.size();;
    printf("\n Queue size:%5d \n",size);
}
