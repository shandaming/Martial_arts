/*
 * Copyright (C) 2018
 */

#ifndef HANDLER_H
#define HANDLER_H

#include <iostream>
#include <string>

enum class Email_type
{
	spam,
	fan,
	complaint,
	new_loc
};

class Handler
{
	public:
		explicit Handler(Handler* h) : 
			handler_(h) {}

		virtual ~Handler() {}

		virtual void handle_request(const std::string& email,
				Email_type type) = 0;
	protected:
		Handler* handler_;
};

class Spam_handler : public Handler
{
	public:
		Spam_handler(Handler* h) :
			Handler(h)
		{}

		void handle_request(const std::string& email,
				Email_type type) override
		{
			if(type == Email_type::spam)
				std::cout << "正在处理垃圾邮件：" << email << "\n";
			else
			{
				std::cout << "无法处理，转交给下一级\n";
				handler_->handle_request(email, type);
			}
		}
};

class Fan_handler : public Handler
{
	public:
		Fan_handler(Handler* h) :
			Handler(h)
		{}

		void handle_request(const std::string& email,
				Email_type type) override
		{
			if(type == Email_type::fan)
				std::cout << "正在处理粉丝邮件：" << email << "\n";
			else
			{
				std::cout << "无法处理，转交给下一级\n";
				handler_->handle_request(email, type);
			}
		}
};

class Complaint_handler : public Handler
{
	public:
		Complaint_handler(Handler* h) :
			Handler(h)
		{}

		void handle_request(const std::string& email,
				Email_type type) override
		{
			if(type == Email_type::complaint)
				std::cout << "正在处理抱怨邮件：" << email << "\n";
			else
			{
				std::cout << "无法处理，转交给下一级\n";
				handler_->handle_request(email, type);
			}
		}
};

class New_loc_handler : public Handler
{
	public:
		New_loc_handler(Handler* h) :
			Handler(h)
		{}

		void handle_request(const std::string& email,
				Email_type type) override
		{
			if(type == Email_type::new_loc)
				std::cout << "正在处理新的邮件：" << email << "\n";
			else
			{
                                //do nothing
			}
		}
};

#endif
