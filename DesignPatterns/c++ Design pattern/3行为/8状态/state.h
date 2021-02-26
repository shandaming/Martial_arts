#ifndef STATE_H
#define STATE_H

class TCPEstablished:public TCPState
{
	public:
		static TCPState* instance();

		virtual void transmit(TCPConnection* t,TCPOctetStream* o) {t->processOctet(o);}
		virtual void close(TCPConnection* t) {changeState(t,TCPListen::instance());}
};

class TCPListen:public TCPState
{
	public:
		static TCPState* instance();

		virtual void send(TCPConnection* t) {changeState(tmTCPEstablished::instance());}
};

class TCPClosed:public TCPState
{
	public:
		static TCPState* instance();

		virtual void activeOpen(TCPConnection* t) {changeState(t,TCPEstablished::instance())};
		virtual void passiveOpen(TCPConnection*) {changeState(t,TCPListen::Instance());}
};

#endif
