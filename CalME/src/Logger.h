#pragma once
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#define LOGGER_API
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <locale.h>
#include <functional>

#ifdef _WIN32
#include <mutex>
#else
#include <pthread.h>
#endif // _WIN32

namespace ULogger
{
#define __Myprintf       printf
#define __Myvprintf      vprintf
#define __Myvfprintf     vfprintf
#define __Myfprintf      fprintf
#define __Myfopen        fopen
#define __Myfopen_s      fopen_s
#define __Myfputs        fputs
#define __Myfflush       fflush
#define __Myfclose       fclose
#define __Mysprintf      sprintf
#define __Mysprintf_s    sprintf_s
#define __Myvsnprintf    vsnprintf
#define __Myvsnprintf_s  _vsnprintf_s

#ifndef _WIN32
#define OPERATOR_FRIEND_FUNC_BASETYPE_IMP(optype,vtype,s,vv1)\
      friend Logger& operator optype(Logger&other,vtype v)\
      {\
        if(Logger::m_file_is_open)\
        {\
		if( Logger::m_operator_count==0)\
		{\
			Logger::PrintLogLevel(ULogger::LogInfo);\
		}\
        pthread_mutex_lock(&Logger::m_logger_mutex);\
        __Myfprintf(Logger::m_file,s,v vv1);\
        if(Logger::PrintToConsole)\
        {\
            __Myprintf(s,v vv1);\
            char*LogTemp=new char[Logger::LogBufferLength+1];\
            __Mysprintf(LogTemp, s,v vv1);\
            delete[] LogTemp;\
	        LogTemp = nullptr;\
        }\
        Logger::m_operator_count++;\
        Logger::FlushNoChecked();\
        pthread_mutex_unlock(&Logger::m_logger_mutex);\
        }\
        return other;\
    }
#define OPERATOR_FRIEND_FUNC_WCHAR_IMP(optype,vtype)\
    friend Logger& operator<<(Logger&other, vtype v)\
	{\
		if (Logger::m_file_is_open)\
		{\
		   if( Logger::m_operator_count==0)\
		   {\
			  Logger::PrintLogLevel(ULogger::LogInfo);\
		   }\
			pthread_mutex_lock(&Logger::m_logger_mutex);\
			std::string s1 = Logger::WstringToString(v);\
			__Myfprintf(Logger::m_file, "%s", s1.c_str());\
			if (Logger::PrintToConsole)\
			{\
				__Myprintf("%s", s1.c_str());\
                char*LogTemp=new char[Logger::LogBufferLength+1];\
                __Mysprintf(LogTemp,"%s", s1.c_str());\
                delete[] LogTemp;\
                LogTemp = nullptr;\
			}\
			Logger::FlushNoChecked();\
		}\
		pthread_mutex_unlock(&m_logger_mutex);\
		return other;\
	}
#else
#define OPERATOR_FRIEND_FUNC_BASETYPE_IMP(optype,vtype,s,v1)\
      friend Logger& operator optype(Logger&other,vtype v)\
    {\
        if(Logger::m_file_is_open)\
    {\
		if( Logger::m_operator_count==0)\
		{\
			Logger::PrintLogLevel(ULogger::LogInfo);\
		}\
        std::unique_lock<std::mutex> lck(Logger::m_logger_mutex);\
        __Myfprintf(Logger::m_file, s, v##v1); \
        if(Logger::PrintToConsole)\
    {\
        __Myprintf(s,v##v1);\
        char*LogTemp=new char[Logger::LogBufferLength+1];\
        __Mysprintf_s(LogTemp, Logger::LogBufferLength, s,v##v1);\
        delete[] LogTemp;\
	    LogTemp = nullptr;\
    }\
        Logger::m_operator_count++;\
        Logger::FlushNoChecked();\
    }\
        return other;\
    }

#define OPERATOR_FRIEND_FUNC_WCHAR_IMP(optype,vtype)\
	friend Logger& operator<<(Logger&other, vtype v)\
	{\
		if (Logger::m_file_is_open)\
		{\
			if( Logger::m_operator_count==0)\
			{\
				Logger::PrintLogLevel(ULogger::LogInfo);\
			}\
			std::unique_lock<std::mutex> lck(Logger::m_logger_mutex);\
			std::string s1 = Logger::WstringToString(v);\
			__Myfprintf(Logger::m_file, "%s", s1.c_str());\
			if (Logger::PrintToConsole)\
			{\
				__Myprintf("%s", s1.c_str());\
                char*LogTemp=new char[Logger::LogBufferLength+1];\
                __Mysprintf_s(LogTemp, Logger::LogBufferLength, "%s", s1.c_str());\
                delete[] LogTemp;\
	            LogTemp = nullptr;\
			}\
            Logger::m_operator_count++;\
			Logger::FlushNoChecked();\
		}\
		return other;\
	}
#endif

	class Logger;

	namespace  Level {
		struct LOGGER_API Logger_Level_Base
		{
		//public:
		//	virtual ~Logger_Level_Base() {}
		public:
			struct LogColor
		{
			int R = 0; 
			int G = 0; 
			int B = 0; 
		}; 
		LogColor  Color; 
		protected:
			Logger_Level_Base() { m_self_string = ""; }
			Logger_Level_Base(const Logger_Level_Base&) {}
			Logger_Level_Base& operator=(const Logger_Level_Base&) { return *this; }
		protected:
			std::string m_self_string;
		private:
			friend class ULogger::Logger;
		};
	}

#define LOGGER_LEVEL_DECLARE(name) \
namespace Level {\
    struct LOGGER_API Logger_Level_Log_##name :public Logger_Level_Base\
    {\
    public:\
        Logger_Level_Log_##name() {\
          m_self_string=#name;\
          m_self_string+=":";\
        }\
    protected:\
        Logger_Level_Log_##name(const Logger_Level_Log_##name&):Logger_Level_Base() {};\
        Logger_Level_Log_##name& operator=(const Logger_Level_Log_##name&) { return *this; }\
    };\
}\
extern struct Level::Logger_Level_Log_##name Log##name;

#define LOGGER_LEVEL_DEFINE(name) Level::Logger_Level_Log_##name Log##name;

	LOGGER_LEVEL_DECLARE(Info)
	LOGGER_LEVEL_DECLARE(Warning)
	LOGGER_LEVEL_DECLARE(Error)
#ifdef _WIN32
		;
#endif // _WIN32
	class LOGGER_API Logger
	{
		/**
		自定义打印时间
		@param time字符串
		*/
		typedef std::function<void(const char*)> c_printf_time_pt;
		/**
		自定义打印log类型
		@param log类型字符串
		@param log字符串的颜色(R,G,B)
		*/
		typedef std::function<void(const char*, Level::Logger_Level_Base::LogColor)> c_printf_level_pt;
		/**
		自定义打印字符串
		@param 格式化后的字符串
		*/
		typedef std::function<void(const char*)> c_printf_body_pt;
		/**
		自定义打印换行
		*/
		typedef std::function<void()> c_printf_end_pt;
	private:
		struct LOGGER_API Logger_Line
		{
		public:
			Logger_Line() { }
		private:
			Logger_Line(const Logger_Line&) { }
			Logger_Line& operator=(const Logger_Line&) { return *this; }
		private:
			friend class Logger;
		};
	public:
		Logger();
		//log的路径
		Logger(const std::string&);
		~Logger();
	public:
		/**
		打开log文件
		@param log文件路径
		*/
		static void Open(const std::string&Path);
		/**
		关闭log文件
		*/
		static void Close();
		/**
		格式化输出字符串
		@param log的类型
		@param 格式化列表
		@param 参数列表
		*/
		void Format(const Level::Logger_Level_Base&Level, const char*Format, ...);
	private:
		Logger(const Logger&) {}
		Logger& operator=(const Logger&) { return *this; }
		static void FlushNoChecked();
		static void WriteTimeFlagNoChecked();
		static std::wstring StringToWstring(const std::string str);
		static std::string WstringToString(const std::wstring str);
		static void GotoNextLine();
		static void PrintLogLevel(const Level::Logger_Level_Base&Level);
		static const std::string& GetLevelString(const Level::Logger_Level_Base&Level);
		static Level::Logger_Level_Base::LogColor GetLevelLogColor(const Level::Logger_Level_Base&Level);
		static std::string GetStrFromTime(const time_t& iTimeStamp);
		static std::string GetStrFromTime_Now();
		static std::string GetAppName();
	public:
		Logger_Line                 End;
	public:
		//是否打印到控制台
		static bool                 PrintToConsole;
		//是否在log的开头写入时间信息
		static bool                 WriteWithTime;
		//当Logger的对象销毁的时候是否调用fflush函数
		static bool                 FlushWhenDestruct;
		//当绑定了自定义打印字符串函数时,会生成一个缓存区,LogBufferLength就是缓冲区的大小
		static int                  LogBufferLength;
		//自定义打印时间
		static c_printf_time_pt     OnPrintTimeDelegate;
		//自定义打印log类型
		static c_printf_level_pt    OnPrintLevelDelegate;
		//自定义打印格式化后字符串
		static c_printf_body_pt     OnPrintBodyDelegate;
		//自定义打印换行
		static c_printf_end_pt      OnPrintEndDelegate;
	private:
		static FILE*         m_file;
		static bool          m_file_is_open;
		static int           m_file_log_max;
		static int           m_file_log_cur;
		static std::string   m_file_cache_path;
		static int           m_operator_count;
		static bool          m_loglevel_printed;
#ifdef _WIN32
		static std::mutex m_logger_mutex;
#else
		static pthread_mutex_t m_logger_mutex;
#endif
	public:
		OPERATOR_FRIEND_FUNC_BASETYPE_IMP(<< , int, "%d", )
	    OPERATOR_FRIEND_FUNC_BASETYPE_IMP(<< , float, "%f", )
		OPERATOR_FRIEND_FUNC_BASETYPE_IMP(<< , bool, "%d", ? 1 : 0)
		OPERATOR_FRIEND_FUNC_BASETYPE_IMP(<< , double, "%lf", )
		OPERATOR_FRIEND_FUNC_BASETYPE_IMP(<< , const std::string&, "%s", .c_str())
		OPERATOR_FRIEND_FUNC_BASETYPE_IMP(<< , const char*, "%s", )
		OPERATOR_FRIEND_FUNC_WCHAR_IMP(<< , const std::wstring&)
		OPERATOR_FRIEND_FUNC_WCHAR_IMP(<< , const wchar_t* )

		friend Logger& operator<<(Logger&other, const Logger_Line&)
		{
			GotoNextLine();
			return other;
		}

		friend Logger& operator<<(Logger&other, const Level::Logger_Level_Base&lv)
		{
			PrintLogLevel(lv);
			Logger::m_loglevel_printed = true;
			return other;
		}
	};
}
