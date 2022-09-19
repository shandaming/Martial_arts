#ifndef SCANNER_H
#define SCANNER_H

class Scanner
{
	public:
		explicit Scanner(istream&);
		virtual ~Scanner();

		virtual Token& scan();
	private:
		istream& inputStream;
};

#endif
