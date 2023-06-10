#ifndef _SYLAR_LOG_H
#define _SYLAR_LOG_H
#include<iostream>
#include<stdint.h>
#include<string>
#include<memory>
#include<list>
#include<sstream>
#include<fstream>
#include<vector>

namespace sylar
{
class logger;

class logLevel{
public:
    enum level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(logLevel::level level);

    static logLevel::level FromString(const std::string& str);
};


//日志事件
class logEvent{   
public:
    typedef std::shared_ptr<logEvent> ptr;

    logEvent(std::shared_ptr<logger> log,logLevel::level level,const char* file,int32_t line,uint32_t elapse,uint32_t threadid,uint32_t fiberid,uint64_t time,const std::string& threadname);

    const char* getFilename()const {return m_file;}

    int32_t getLine()const {return m_line;}

    uint32_t getElapse()const {return m_elapse;}

    uint32_t getThreadId()const {return m_threadId;}

    uint32_t getFiberId()const {return m_fiberId;}//协程id

    uint64_t getTime() const {return m_time;}//时间戳

    std::string getContent() const { return m_ss.str();}//返回日志内容

    const std::string& getThreadName() const { return m_threadName;}

    std::shared_ptr<logger> getLogger() const { return m_log;}

    logLevel::level getLevel() const { return m_level;}

    std::stringstream& getSS() { return m_ss;}//字符串流

    void format(const char* fmt,...);

    // void format(const char* fmt,)
private:
    const char* m_file  = nullptr;
    int32_t m_line ;//行号
    uint32_t m_elapse ;//程序启动到现在
    uint32_t m_threadId ;//线程id
    uint32_t m_fiberId ;//协程id
    uint64_t m_time;//时间戳
    std::stringstream m_ss;//内容
    std::string m_threadName;//线程名
    std::shared_ptr<logger> m_log;
    logLevel::level m_level;

};


//日志格式
class logFormatter{
public:
    typedef std::shared_ptr<logFormatter> ptr;
    logFormatter(const std::string& pattern);

    std::string format(std::shared_ptr<logger> log,logLevel::level level,logEvent::ptr event);
    std::ostream& format(std::ostream& ofs,std::shared_ptr<logger> log,logLevel::level level,logEvent::ptr event);

    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem(){};
        virtual void format(std::ostream& os,std::shared_ptr<logger> log ,logLevel::level level,logEvent::ptr event) = 0;
    };

    
    void init();
    bool isError() const { return m_error;}
    const std::string getPattern() const { return m_pattern;}
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;
};


//日志输出地
class logAppender{
friend class Logger;
public:
    typedef std::shared_ptr<logAppender> ptr;
    virtual ~logAppender(){}

    virtual void log(std::shared_ptr<logger> log ,logLevel::level level,logEvent::ptr event) = 0;

    void setFormatter(logFormatter::ptr val);
    logFormatter::ptr getFormatter();
    logLevel::level getLevel() const { return m_level;}

protected:
    logLevel::level m_level = logLevel::DEBUG;
    bool m_hasFormatter = false;
    logFormatter::ptr m_formatter;

};



//日志器
class logger : public std::enable_shared_from_this<logger>{ 
public:
    typedef std::shared_ptr<logger> ptr;

    logger(const std::string& name = "root");
    void log(logLevel::level,logEvent::ptr event);    

    void debug(logEvent::ptr event);    
    void info(logEvent::ptr event);    
    void error(logEvent::ptr event);    
    void warn(logEvent::ptr event);
    void fatal(logEvent::ptr event);    
    const std::string& getName() const { return m_name;}


    void addAppender(logAppender::ptr appender);
    void delAppender(logAppender::ptr appender);
    logLevel::level get()const {return m_level;}
    void setLevel(logLevel::level level) {m_level = level;}
private:
    std::string m_name; //日志名称
    logLevel::level m_level;//等级
    std::list<logAppender::ptr> m_appender;//集合
    logFormatter::ptr m_formatter;
    logger::ptr m_root; //主
};



//输出到标准输出
class stdoutLogAppender : public logAppender{
public:
    typedef std::shared_ptr<stdoutLogAppender> ptr;
    void log(std::shared_ptr<logger> log, logLevel::level level,logEvent::ptr event)override;
};

//输出到文件
class fileLogAppender : public logAppender{
public: 
    fileLogAppender(const std::string name);
    typedef std::shared_ptr<fileLogAppender> ptr;
    void log(std::shared_ptr<logger> log,logLevel::level level,logEvent::ptr event)override;
    bool reopen();//重新打开
private:
    std::string m_name;
    std::ofstream m_filestream;
};


} // namespace sylar

#endif