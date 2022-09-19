#ifndef CHASSIS_H
#define CHASSIS_H

class Chassis:public CompositeEquipment
{
	public:
		explicit Chassis(const char*);
		virtual ~Chassis();

		virtual wait power();
		virtual Currency netPrice();
		virtual Currency discountPrice(0;
};

#endif
