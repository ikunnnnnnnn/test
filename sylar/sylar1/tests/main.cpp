#include<iostream>
#include"../src/log.h"
int main(){
    sylar::logger::ptr logger(new sylar::logger);
    logger->addAppender(sylar::logAppender::ptr(new sylar::stdoutLogAppender));

    sylar::logEvent::ptr event(new sylar::logEvent(logger,sylar::logLevel::DEBUG,__FILE__,__LINE__,0,1,2,time(0),"thread1"));

    logger->log(sylar::logLevel::DEBUG,event);

    return 0;
}