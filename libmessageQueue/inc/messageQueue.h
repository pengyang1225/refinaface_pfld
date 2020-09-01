#ifndef POLYM_QUEUE_HPP
#define POLYM_QUEUE_HPP

#include "Msg.h"
#include <memory>

namespace MAX_Message {

/** Msg ID for timeout message */
enum Error_Code{
    EMPTY_QUEUE = -4,
    CAN_NOT_INSERT_MESSAGE = -3,
    MSG_TIMEOUT = -2,
    ERROR = -1,
    OK = 0,

};
enum Message_Status{
    WAIT_FOREVER = -1,
    NO_WAIT = 0,

};


enum Message_Send_Priorities{
    MESSAGE_PRI_NORMAL=0, /// 消息优先级，当MESSAGE_PRI_NORMAL 时从队尾加入，MESSAGE_PRI_URGENT 时从队头插入
    MESSAGE_PRI_URGENT
};

/**
 * Queue is a thread-safe message queue.
 * It supports one-way messaging and request-response pattern.
 */
    class Queue
    {
    public:
        Queue();

        ~Queue();
        /**
         * @brief setAttribute  设置消息队列属性
         * @param messageMaxSize 消息队列最大消息数 ，< 0 : 消息队列大小无限制；默认= -1
        */
        void setAttribute(int messageMaxSize );

        /**
         * @brief Put Msg to the queue.
         * @param msg 消息
         * @param timeoutMillis 延时时间 timeoutMillis 毫秒，  NO_WAIT = -1 不等待，当队列满了以后就直接退出,WAIT_FOREVER = 0 一直等待到将消息入队列,
         * @param priority 消息优先级，当MESSAGE_PRI_NORMAL 时从队尾加入，MESSAGE_PRI_URGENT 时从队头插入
         * @return
         */
        Error_Code put(Msg&& msg ,int  timeoutMillis,Message_Send_Priorities priority);
        /**
         * Put Msg to the end of the queue.
         *
         * @param msg Msg to put to the queue.

         */
        Error_Code put(Msg&& msg);
        /**
         * Get message from the head of the queue.
         * Blocks until at least one message is available in the queue, or until timeout happens.
         * If get() returns due to timeout, the returned Msg will have Msg ID MSG_TIMEOUT.
         *
         * @param timeoutMillis How many ms to wait for message until timeout happens.
         *                      0 = wait indefinitely(WAIT_FOREVER).
         */
        std::unique_ptr<Msg> get(int timeoutMillis = 0);

        /**
         * @brief Get message sizes
         * @return
         */
        size_t size();

        /**
         * clear Queue
         * @return
         */
        bool clear();


        /**
         * Make a request.
         * Call will block until response is given with respondTo().
         *
         * @param msg Request message. Is put to the queue so it can be retrieved from it with get().
         */
        std::unique_ptr<Msg> request(Msg&& msg);

        /**
         * Respond to a request previously made with request().
         *
         * @param reqUid Msg UID of the request message.
         * @param responseMsg Response message. The requester will receive it as the return value of
         *                    request().
         */
        void respondTo(MsgUID reqUid, Msg&& responseMsg);

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };

}

#endif
