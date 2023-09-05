#include "Logger.h"
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

namespace ULogger
{
    LOGGER_LEVEL_DEFINE(Info)
    LOGGER_LEVEL_DEFINE(Warning)
    LOGGER_LEVEL_DEFINE(Error)

#ifndef EOF
#define EOF -1
#endif

	FILE*                        Logger::m_file                      = nullptr;
	bool                         Logger::m_file_is_open              = false;
	bool                         Logger::PrintToConsole              = false;
	bool                         Logger::WriteWithTime               = false;
	bool                         Logger::FlushWhenDestruct           = false;
	bool                         Logger::m_loglevel_printed          = false;
	int                          Logger::m_file_log_max              = 1;
	int                          Logger::m_file_log_cur              = 0;
	int		                     Logger::m_operator_count            = 0;
	int			                 Logger::LogBufferLength             =2048;
	Logger::c_printf_time_pt     Logger::OnPrintTimeDelegate         = nullptr;
	Logger::c_printf_level_pt    Logger::OnPrintLevelDelegate        = nullptr;
	Logger::c_printf_body_pt     Logger::OnPrintBodyDelegate         = nullptr;
	Logger::c_printf_end_pt      Logger::OnPrintEndDelegate          = nullptr;

	std::string Logger::m_file_cache_path = "";
#ifdef _WIN32
	std::mutex Logger::m_logger_mutex;
#else
	pthread_mutex_t Logger::m_logger_mutex;
#endif

	Logger::Logger()
	{
	}

	Logger::Logger(const std::string &Path)
	{
		if (Path.empty())
		{
			Open(m_file_cache_path);
		}
		else
		{
			Open(Path);
		}
	}

	Logger::~Logger()
	{
		if (Logger::FlushWhenDestruct)
		{
			Logger::m_operator_count = 0;
			Logger::m_file_log_cur = m_file_log_max;
			Logger::GotoNextLine();
		}
	}

	void Logger::Open(const std::string&Path) 
	{
		if (!Logger::m_file_is_open)
		{
			if (Path.empty())
			{
				m_file_cache_path = "./logger_saved.log";
			}
			else
			{
				m_file_cache_path = Path;
			}
#ifndef _WIN32
			Logger::m_file = __Myfopen(m_file_cache_path.c_str(), "a");
#else
			__Myfopen_s(&Logger::m_file, m_file_cache_path.c_str(), "a");
#endif // !_WIN32
			if (Logger::m_file == nullptr)
			{
#ifndef _WIN32
				Logger::m_file = __Myfopen("./logger_saved.log", "a");
#else
				__Myfopen_s(&Logger::m_file, "./logger_saved.log", "a");
#endif // !_WIN32
				Logger::m_file_cache_path = "./logger_saved.log";
			}
			if (m_file != nullptr)
			{
				__Myfprintf(Logger::m_file, "Log file open,%s \n", Logger::GetStrFromTime_Now().c_str());
				Logger::m_file_is_open = true;
			}
#ifndef _WIN32
			pthread_mutex_init(&m_logger_mutex, nullptr);
#endif // !_WIN32
		}
	}

	void Logger::Close() 
	{
		if (Logger::m_file != nullptr)
		{
			__Myfclose(Logger::m_file);
			Logger::m_file = nullptr;
			Logger::m_file_is_open = false;
		}
#ifndef _WIN32
		pthread_mutex_destroy(&m_logger_mutex);
#endif // !_WIN32
	}

	void Logger::Format(const Level::Logger_Level_Base&Level, const char*Format, ...) 
	{
		if (Logger::m_file_is_open)
		{
#ifdef _WIN32
			std::unique_lock<std::mutex> lck(m_logger_mutex);
#else
			pthread_mutex_lock(&m_logger_mutex);
#endif
			Logger::WriteTimeFlagNoChecked();
			__Myfprintf(m_file, "%s", Level.m_self_string.c_str());
			if (Logger::PrintToConsole)
			{
				__Myprintf("%s", Level.m_self_string.c_str());
				if (Logger::OnPrintLevelDelegate != nullptr)
				{
					Logger::OnPrintLevelDelegate(Level.m_self_string.c_str(), Level.Color);
				}
			}
			va_list args;
			va_start(args, Format);
			__Myvfprintf(m_file, Format, args);
			va_end(args);
            if (Logger::PrintToConsole)
            {
                va_list args;
                va_start(args, Format);
                __Myvprintf(Format, args);
                char* TempBuffer=new char[LogBufferLength+1];
                memset(TempBuffer, 0, LogBufferLength + 1);
#ifndef _WIN32
                __Myvsnprintf(TempBuffer, LogBufferLength, Format, args);
#else
                __Myvsnprintf_s(TempBuffer, LogBufferLength, LogBufferLength, Format, args);
#endif
                if (Logger::OnPrintBodyDelegate != nullptr)
                {
					Logger::OnPrintBodyDelegate(TempBuffer);
                }
                delete[] TempBuffer;
                TempBuffer = nullptr;
                va_end(args);
             }
			__Myfprintf(m_file, "\n");
			if (Logger::PrintToConsole)
			{
				__Myprintf("\n");
				if (Logger::OnPrintEndDelegate != nullptr)
				{
					Logger::OnPrintEndDelegate();
				}
			}
			Logger::FlushNoChecked();
#ifndef _WIN32
			pthread_mutex_unlock(&m_logger_mutex);
#endif
		}
	}

	void Logger::FlushNoChecked() 
	{
		Logger::m_file_log_cur++;
		if (Logger::m_file_log_cur >= Logger::m_file_log_max)
		{
			Logger::m_file_log_cur = 0;
			__Myfflush(m_file);
		}
	}

	void Logger::WriteTimeFlagNoChecked()
	{
		std::string str= Logger::GetStrFromTime_Now();
		if (Logger::WriteWithTime)
		{
			__Myfprintf(m_file, "[%s]", str.c_str());
			if (Logger::PrintToConsole)
			{
				__Myprintf("[%s]", str.c_str());
				if (Logger::OnPrintTimeDelegate != nullptr)
				{
					Logger::OnPrintTimeDelegate(str.c_str());
				}
			}
		}
	}

	std::wstring Logger::StringToWstring(const std::string str)
	{
		unsigned len = str.size() * 2;
		setlocale(LC_CTYPE, "");     
		wchar_t *p = new wchar_t[len];
#ifdef _WIN32
		size_t ret = 0;
		mbstowcs_s(&ret, p, len, str.c_str(), len);
#else
		mbstowcs(p, str.c_str(), len);
#endif // _WIN32
		std::wstring str1(p);
		delete[] p;
		return str1;
	}

	std::string Logger::WstringToString(const std::wstring str)
	{
		unsigned len = str.size() * 4;
		setlocale(LC_CTYPE, "");
		char *p = new char[len];
#ifdef _WIN32
		size_t ret = 0;
		wcstombs_s(&ret,p,len,str.c_str(), len);
#else
		wcstombs(p, str.c_str(), len);
#endif // _WIN32
		std::string str1(p);
		delete[] p;
		return str1;
	}

	std::string Logger::GetStrFromTime(const time_t& iTimeStamp)
	{
		struct tm t;
		char temp[256] = { 0 };
#ifndef _WIN32
		tm *pTmp = localtime(&iTimeStamp);
		if (pTmp != nullptr)
		{
			t = *pTmp;
			__Mysprintf(temp, "%d-%d-%d %d:%d:%d", t.tm_year + 1900, t.tm_mon + 1,
			t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
		}
#else
		localtime_s(&t, &iTimeStamp);
		__Mysprintf_s(temp, 256, "%d-%d-%d %d:%d:%d", t.tm_year + 1900, t.tm_mon + 1,
			t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
#endif
		return temp;
	}

	std::string Logger::GetStrFromTime_Now()
	{
		time_t tm_;
		time(&tm_);
		return 	GetStrFromTime(tm_);
	}

	std::string Logger::GetAppName()
	{
		std::string _exeName = "";
#ifdef _WIN32
#else
#endif // _WIN32
		return _exeName;
	}

	void Logger::GotoNextLine()
	{
		if (Logger::m_file_is_open)
		{
#ifndef _WIN32
			pthread_mutex_lock(&m_logger_mutex);
#else
			std::unique_lock<std::mutex> lck(Logger::m_logger_mutex);
#endif
			__Myfputs("\n", Logger::m_file);
			if (Logger::PrintToConsole)
			{
				__Myprintf("\n");
				if (Logger::OnPrintEndDelegate != nullptr)
				{
					Logger::OnPrintEndDelegate();
				}
			}
			Logger::m_operator_count = 0;
			Logger::m_loglevel_printed = false;
			Logger::FlushNoChecked();
#ifndef _WIN32
			pthread_mutex_unlock(&m_logger_mutex);
#endif
		}
	}

	void Logger::PrintLogLevel(const Level::Logger_Level_Base & Level)
	{
		if (Logger::m_file_is_open&&!Logger::m_loglevel_printed&&Logger::m_operator_count == 0)
		{
#ifdef _WIN32
			std::unique_lock<std::mutex> lck(Logger::m_logger_mutex);
#else
			pthread_mutex_lock(&Logger::m_logger_mutex);
#endif
			Logger::m_loglevel_printed = true;
			if (Logger::m_operator_count == 0)
			{
				Logger::WriteTimeFlagNoChecked();
			}
			__Myfprintf(Logger::m_file, "%s", Logger::GetLevelString(Level).c_str());
			if (Logger::PrintToConsole)
			{
				__Myprintf("%s", Logger::GetLevelString(Level).c_str());
				if (Logger::OnPrintLevelDelegate != nullptr)
				{
					Logger::OnPrintLevelDelegate(Logger::GetLevelString(Level).c_str(), Logger::GetLevelLogColor(Level));
				}
			}
			Logger::FlushNoChecked();
		}
#ifndef _WIN32
		pthread_mutex_unlock(&m_logger_mutex);
#endif
	}

	const std::string &Logger::GetLevelString(const Level::Logger_Level_Base &Level)
	{
		return Level.m_self_string;
	}

	Level::Logger_Level_Base::LogColor Logger::GetLevelLogColor(const Level::Logger_Level_Base & Level)
	{
		return Level.Color;
	}

}
