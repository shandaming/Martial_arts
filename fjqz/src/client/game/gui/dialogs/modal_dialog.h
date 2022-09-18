//
// Created by mylw on 22-9-17.
//

#ifndef FJQZ_MODAL_DIALOG_H
#define FJQZ_MODAL_DIALOG_H

#include <string>

class modal_dialog
{
public:
	modal_dialog();
	virtual ~modal_dialog();

	bool show(const unsigned auto_close_time = 0);
private:
	int retval_;
	std::string focus_;
};

#endif //FJQZ_MODAL_DIALOG_H
