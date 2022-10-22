#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

class TCPConnection
{
	public:
		TCPConnection()
		{
			state=TCPClosed::Instance();
		}

		void activeOpen() {state->activeOpen(this);}
		void passiveOpen() {state->passiveOpen(this);}
		void close() {state->close(this);}

		void send();
		void acknowledge() {state->acknowlefge(this);}
		void synchronize() {state->synchronize(this);}

		void processOctet(TCPOctetStream*);
	private:
		friend class TCPState;
		void changedState(TCPState* s) {state=s;}

		TCPState* state;
};

#endif
