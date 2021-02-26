#ifndef CHARACTER_H
#define CHARACTER_H

class Character:public Glyph
{
	public:
		explicit Character(char);

		virtual void draw(Window*,GlyphContext&);
	private:
		char charcode;
};

#endif
