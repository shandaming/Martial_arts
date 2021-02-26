#include "people.h"

int main()
{
	Mediator* m = new House_mediator();

	People* r = new Renter(m);
	People* l = new Landlord(m);

	m->set_people(l, r);

	r->sent_msg("我是租户");
	l->sent_msg("我是房东");
}
