#ifndef TCPSTATE_H
#define TCPSTATE_H

class TCPState
{
	public:
		virtual void transmit(TCPConnection*,TCPOctetStream*) {}
		virtual void activeOpen(TCPConnection*) {}
		virtual void passiveOpen(TCPConnection*) {}
		virtual void close(TCPConnection*) {}
		virtual void synchronsize(TCPConnection*) {}
		virtual void acknowledge(TCPConnection*);
		virtual void send(TCPConnection*);
	protected:
		void changedState(TCPConnection* t,TCPState* s) {t->changeState(s);}
};

#endif
