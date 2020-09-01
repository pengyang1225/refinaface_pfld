#include "messageQueue.h"

#include <chrono>
#include <condition_variable>
#include <queue>
#include <map>
#include <mutex>
#include <utility>
#include <iostream>

namespace MAX_Message {

    class Queue::Impl
    {
    public:
        Impl()
                : queue_(), queueMutex_(), queueCond_(), responseMap_(), responseMapMutex_(),maxMessageSize(-1)
        {
        }

        void setAttribute(int messageMaxSize){
            maxMessageSize = messageMaxSize;
        }

        size_t size(){
            std::lock_guard<std::mutex> lock(queueMutex_);
            return queue_.size();
        }

        bool clear(){
            try{
                std::lock_guard<std::mutex> lock(queueMutex_);
                queue_.clear();
                return true;
            }
            catch(...){
                return false;
            }

        }
        Error_Code put(Msg&& msg ,int timeoutMillis,Message_Send_Priorities priority)
        {
            /// 不同的模式下消息队列处理模式不同
            Error_Code ret = OK;
            while(1){
                if (size() >= maxMessageSize) {

                    if(timeoutMillis == WAIT_FOREVER){

//                          std::cout<<msg.getUniqueId()<<"PUT WAIT_FOREVER , maxMessageSize:"<< queue_.size()<<std::endl;
    //                    std::unique_lock<std::mutex> waitLock(queueMutex_);
    //                    queueCond_.wait(waitLock, [this]{return (queue_.size() < maxMessageSize);});
                        if (size() < maxMessageSize){

                            std::lock_guard<std::mutex> lock(queueMutex_);
                            if(priority == MESSAGE_PRI_NORMAL){
                                queue_.push_back(msg.move());
                                }
                            else{
                                queue_.push_front(msg.move());
                            }

                           queueCond_.notify_all();
                           break;
                        }
                    }
                    else if(timeoutMillis == NO_WAIT){
                      ret = CAN_NOT_INSERT_MESSAGE;
                      break;
                    }
                    else{
                        ret = CAN_NOT_INSERT_MESSAGE;
                        break;
                    }

                 }
                else
                {
                    std::lock_guard<std::mutex> lock(queueMutex_);
                    if(priority == MESSAGE_PRI_NORMAL){
                        queue_.push_back(msg.move());
                        }
                    else{
                        queue_.push_front(msg.move());
                    }
                    queueCond_.notify_all();
                    ret = OK;
                    break;
                }

            }



            return ret;
        }

        Error_Code put(Msg&& msg )
        {
            return put(std::move(msg) , NO_WAIT , MESSAGE_PRI_NORMAL);
        }

        std::unique_ptr<Msg> get(int timeoutMillis)
        {
            std::unique_lock<std::mutex> lock(queueMutex_);

            if (timeoutMillis <= WAIT_FOREVER){

                queueCond_.wait(lock, [this]{return !queue_.empty();});
            }
            else if (timeoutMillis == NO_WAIT){
                queue_.emplace(queue_.end(),new Msg(EMPTY_QUEUE));

            }
            else
            {

                // wait_for returns false if the return is due to timeout
                auto timeoutOccured = !queueCond_.wait_for(
                        lock,
                        std::chrono::milliseconds(timeoutMillis),
                        [this]{return !queue_.empty();});

                if (timeoutOccured){
                    queue_.emplace(queue_.end(),new Msg(MSG_TIMEOUT));

                }

            }

            auto msg = queue_.front()->move();
            queue_.pop_front();


            return msg;
        }

        std::unique_ptr<Msg> request(Msg&& msg)
        {
            // Construct an ad hoc Queue to handle response Msg
            std::unique_lock<std::mutex> lock(responseMapMutex_);
            auto it = responseMap_.emplace(
                    std::make_pair(msg.getUniqueId(), std::unique_ptr<Queue>(new Queue))).first;
            lock.unlock();

            put(std::move(msg));
            auto response = it->second->get(); // Block until response is put to the response Queue

            lock.lock();
            responseMap_.erase(it); // Delete the response Queue
            lock.unlock();

            return response;
        }

        void respondTo(MsgUID reqUid, Msg&& responseMsg)
        {
            std::lock_guard<std::mutex> lock(responseMapMutex_);
            if (responseMap_.count(reqUid) > 0)
                responseMap_[reqUid]->put(std::move(responseMsg));
        }

    private:
        // Queue for the Msgs
        std::deque<std::unique_ptr<Msg>> queue_;

        // Mutex to protect access to the queue
        std::mutex queueMutex_;

        // Condition variable to wait for when getting Msgs from the queue
        std::condition_variable queueCond_;

        // Map to keep track of which response handler queues are associated with which request Msgs
        std::map<MsgUID, std::unique_ptr<Queue>> responseMap_;

        // Mutex to protect access to response map
        std::mutex responseMapMutex_;

        int maxMessageSize;


    };

    Queue::Queue()
            : impl_(new Impl)
    {
    }

    Queue::~Queue()
    {
    }

    void Queue::setAttribute(int messageMaxSize = -1){
        impl_->setAttribute(messageMaxSize);
    }

    size_t Queue::size()
    {
        return impl_->size();
    }

    bool Queue::clear() {
        return impl_->clear();
    }

    Error_Code Queue::put(Msg&& msg)
    {
        return impl_->put(std::move(msg));
    }

    Error_Code Queue::put(Msg&& msg, int  timeoutMillis, Message_Send_Priorities priority)
    {
        return impl_->put(std::move(msg),timeoutMillis, priority);
    }

    std::unique_ptr<Msg> Queue::get(int timeoutMillis)
    {
        return impl_->get(timeoutMillis);
    }

    std::unique_ptr<Msg> Queue::request(Msg&& msg)
    {
        return impl_->request(std::move(msg));
    }

    void Queue::respondTo(MsgUID reqUid, Msg&& responseMsg)
    {
        impl_->respondTo(reqUid, std::move(responseMsg));
    }

}
