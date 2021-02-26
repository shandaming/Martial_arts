#include "handler.h"

int main()
{
        Handler* new_loc = new New_loc_handler(nullptr);
        Handler* complaint = new Complaint_handler(new_loc);
        Handler* fan = new Fan_handler(complaint);
	Handler* spam = new Spam_handler(fan);
	
	
	

	spam->handle_request("垃圾邮件", Email_type::spam);
	spam->handle_request("粉丝邮件", Email_type::fan);
	spam->handle_request("抱怨邮件", Email_type::complaint);
	spam->handle_request("新的邮件", Email_type::new_loc);
}
