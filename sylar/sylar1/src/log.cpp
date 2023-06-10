#include "log.h"
#include<string>
#include<vector>
#include<map>
#include<tuple>
#include<functional>

namespace sylar{



const char* logLevel::ToString(logLevel::level level){
    switch(level){
    #define XX(name) \
        case logLevel::name:\
            return #name;\
            break;

        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(FATAL);
        XX(ERROR);
    #undef XX
        default:
            return "UNKNOW";
        }
        return "UNKNOW";
}


logLevel::level logLevel::FromString(const std::string& str){
    #define XX(key,val)\
        if(str == #key){\
            return logLevel::val;\
        }

        XX(debug,DEBUG);
        XX(info,INFO);
        XX(error,ERROR);
        XX(fatal,FATAL);
        XX(warn,WARN);

        XX(DEBUG, DEBUG);
        XX(INFO, INFO);
        XX(WARN, WARN);
        XX(ERROR, ERROR);
        XX(FATAL, FATAL);
        return logLevel::UNKNOW;
    #undef XX
}



class stringFormatItem : public logFormatter::FormatItem{
public:
    stringFormatItem(const std::string str = "") : m_string(str){};
    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override{
        os<<m_string;
    }
private:
    std::string m_string;
};

class MessageFormatItem : public logFormatter :: FormatItem{
public:
    MessageFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< event->getContent();
    }
};

class LevelFormatItem : public logFormatter :: FormatItem{
public:
    LevelFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< logLevel::ToString(level);
    }
};

class ElapseFormatItem : public logFormatter :: FormatItem{
public:
    ElapseFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< event->getElapse();
    }
};

class NameFormatItem : public logFormatter :: FormatItem{
public:
    NameFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< event->getLogger()->getName();
    }
};

class ThreadIdFormatItem : public logFormatter :: FormatItem{
public:
    ThreadIdFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< event->getThreadId();
    }
};

class NewLineFormatItem : public logFormatter :: FormatItem{
public:
    NewLineFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< std::endl;
    }
};

class DateTimeFormatItem : public logFormatter :: FormatItem{
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S" ) : m_format(format){
        std::cout<<"DateTimeFormatItem init"<<std::endl;
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time,&tm);//线程安全的函数
        char buf[64];
        strftime(buf,sizeof(buf),m_format.c_str(),&tm);//转换为指定格式的时间字符串
        os<<buf;
    }
    private:
    std::string m_format;
};

class FilenameFormatItem : public logFormatter :: FormatItem{
public:
    FilenameFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< event->getFilename();
    }
};

class LineFormatItem : public logFormatter :: FormatItem{
public:
    LineFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< event->getLine();
    }
};

class TabFormatItem : public logFormatter :: FormatItem{
public:
    TabFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< "\t";
    }
};

class FiberIdFormatItem : public logFormatter :: FormatItem{
public:
    FiberIdFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< event->getFiberId();
    }
};

class ThreadNameFormatItem : public logFormatter :: FormatItem{
public:
    ThreadNameFormatItem(const std::string& str = " " ){}

    void format(std::ostream& os,logger::ptr log,logLevel::level level,logEvent::ptr event) override {
        os<< event->getThreadName();
    }
};

//===========================logEvent==================================================
logEvent::logEvent(std::shared_ptr<logger> log,logLevel::level level,
            const char* file,int32_t line,uint32_t elapse,
            uint32_t threadid,uint32_t fiberid,uint64_t time,
            const std::string& threadname) :
            m_log(log),
            m_level(level),
            m_file (file),
            m_line (line),
            m_elapse(elapse),
            m_threadId(threadid),
            m_fiberId(fiberid),
            m_time(time),
            m_threadName(threadname)

{
            
}


//===========================logger==================================================
logger::logger(const std::string& name) : m_name(name),m_level(logLevel::DEBUG)
{
    //%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    m_formatter.reset(new logFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t"));
}


void logAppender::setFormatter(logFormatter::ptr val) {
    m_formatter = val;
    if(m_formatter) {
        m_hasFormatter = true;
    } else {
        m_hasFormatter = false;
    }
}

void logger::log(logLevel::level level,logEvent::ptr event){ 
    if(level >= m_level){
        auto self = shared_from_this();
        for(auto& i : m_appender){
            std::cout<<"logger::log"<<std::endl;
            i->setFormatter(m_formatter);
            i->log(self,level,event);
        }
    }
}

void logger::debug(logEvent::ptr event){
    log(logLevel::DEBUG,event);
}    
void logger::info(logEvent::ptr event){
    log(logLevel::INFO,event);
}    
void logger::error(logEvent::ptr event){
    log(logLevel::ERROR,event);

}    
void logger::warn(logEvent::ptr event){
    log(logLevel::WARN,event);
}
void logger::fatal(logEvent::ptr event){
    log(logLevel::FATAL,event);

}   

void logger::addAppender(logAppender::ptr appender){//添加输出地
    m_appender.emplace_back(appender);
}

void logger::delAppender(logAppender::ptr appender){ //遍历删除指定输出地
    for(auto it = m_appender.begin(); it != m_appender.end(); ++it){
        if(*it == appender){
            m_appender.erase(it);
            break;  
        }
    }
}

void stdoutLogAppender::log(std::shared_ptr<logger> log, logLevel::level level,logEvent::ptr event){
    if(level >= m_level){
        std::cout<<"logger::log2"<<std::endl;
        m_formatter->format(std::cout,log,level,event);
    }
}

void fileLogAppender::log(std::shared_ptr<logger> log,logLevel::level level,logEvent::ptr event){
    if(level >= m_level){
        if(!m_formatter->format(m_filestream, log, level, event)){
            std::cout << "error" << std::endl;
        }
    }
}



bool fileLogAppender::reopen(){
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_name);
    return !!m_filestream;//不为空即为true
}

fileLogAppender::fileLogAppender(const std::string name): m_name(name){

}


logFormatter::logFormatter(const std::string& pattern)
    :m_pattern(pattern){
        init();
}

std::string logFormatter::format(std::shared_ptr<logger> log,logLevel::level level,logEvent::ptr event){
    std::stringstream ss;
    for(auto& i : m_items){
        i->format(ss,log,level,event);
    }
    return ss.str();
}

std::ostream& logFormatter::format(std::ostream& ofs,std::shared_ptr<logger> log,logLevel::level level,logEvent::ptr event){
    std::cout<<"logger::log3"<<std::endl;
    std::cout<<m_items.size()<<std::endl;
    for(logFormatter::FormatItem::ptr& i : m_items){
        i->format(ofs,log,level,event);
    }
    return ofs;
}


//三种格式 %xxx  %% %xxx{xxx}
void logFormatter::init(){
    //str fmt type
    std::cout<<"init"<<std::endl;
    std::vector<std::tuple<std::string,std::string,int>> vec;
    int size = m_pattern.size();
    std::string nstr;

    for(int i = 0; i < size; ++i){
        if(m_pattern[i] != '%'){
            nstr.append(1,m_pattern[i]);
            continue;
        }

        if(i + 1 < size && m_pattern[i + 1] == '%'){
            nstr.append(1,'%');
            continue;
        }

        size_t n = i + 1;
        
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        
        while(n < size){
            // if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')){
            //     str = m_pattern.substr(i + 1, n - i - 1);
            //     break;
            // }

            // if(fmt_begin == 0 && m_pattern[n] == '{'){
            //     str = m_pattern.substr(i + 1, n - i - 1);
            //     fmt_begin = n;
            //     fmt_status = 1;
            //     n++;
            //     continue;
            // }

            // if(fmt_begin == 1 && m_pattern[n] == '}'){
            //     fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
            //     fmt_status = 0;
            //     n++;
            //     break;
            // }

            // ++n;
            // if(n == m_pattern.size() && str.empty()){
            //     str = m_pattern.substr(i + 1);
            // }

             if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }

             ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0){
            if(!nstr.empty()){
                vec.emplace_back(std::make_tuple(nstr,std::string(),0));
                nstr.clear();
            }
            vec.emplace_back(std::make_tuple(str,fmt,1));
            i = n - 1;
        }else if(fmt_status == 1){
            std::cout<<"pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            m_error = true;
        }

        
    }
    
    if(!nstr.empty()){
        vec.emplace_back(std::make_tuple(nstr,std::string(),0));
    }

    static std::map<std::string ,std::function<FormatItem::ptr (const std::string& str)>> s_format_items = {
#define XX(str,C) \
    {#str,[](const std::string& fmt){return FormatItem::ptr(new C(fmt));}} 

    XX(m, MessageFormatItem),           //m:消息
    XX(p, LevelFormatItem),             //p:日志级别
    XX(r, ElapseFormatItem),            //r:累计毫秒数
    XX(c, NameFormatItem),              //c:日志名称
    XX(t, ThreadIdFormatItem),          //t:线程id
    XX(n, NewLineFormatItem),           //n:换行
    XX(d, DateTimeFormatItem),          //d:时间
    XX(f, FilenameFormatItem),          //f:文件名
    XX(l, LineFormatItem),              //l:行号
    XX(T, TabFormatItem),               //T:Tab
    XX(F, FiberIdFormatItem),           //F:协程id
    XX(N, ThreadNameFormatItem),        //N:线程名称

#undef XX
    };

    for(auto& i : vec){
        if(std::get<2>(i) == 0){
            m_items.emplace_back(FormatItem::ptr(new stringFormatItem(std::get<0>(i))));
        }else{
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.emplace_back(FormatItem::ptr(new stringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
                std::cout<<"error it == s_format_items.end()" <<std::endl;
            } else {
                m_items.emplace_back(it->second(std::get<1>(i)));
            }
        }
    }

    std::cout<<"m_item = " << m_items.size() <<std::endl; 

}


}//end sylar