#ifndef FLOPPYDISK_H
#define FLOPPYDISK_H

class FloppyDisk:public Equipment
{
	public:
		FloppyDisk(const char*);
		virtual ~FloppyDisk();

		virtual wait power();
		virtual Currency netPrice();
		virtual Currency discountPrice();
};

#endif
