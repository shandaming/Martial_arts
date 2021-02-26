#ifndef GLYPHFACTORY_H
#define GLYPHFACTORY_H

const int NCHARCODES=128;

class GlyphFactory
{
	public:
		GlyphFactory() 
		{
			for(int i=0;i<NCHARCODES;++i)
				character[i]=0;
		}
		virtual ~GlyphFactory();

		virtual Character* createCharacter(char c)
		{
			if(!character[c])
				character[c]=new Character(c);

			return character[c];
		}
		virtual row* createRow() {return new Row;}
		virtual column* createColumn() {return new Column;}
	private:
		Character* character[NCHARCODES];
};

#endif
