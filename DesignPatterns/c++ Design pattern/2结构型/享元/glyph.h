#ifndef GLYPH_H
#define GLYPH_H

class Glyph
{
	public:
		virtual ~Glyph();

		virtual void draw(Window*,GlyphContext&);

		virtual void setFont(Font*,GlyphContext&);
		virtual Font* getFont(GlyphContext&);
		virtual void first(GlyphContext&);
		virtual void next(GlyphContext&);
		virtual Glyph* current(GlyphContext&);

		virtual void insert(Glyph*,GlyphContext&);
		virtual void remove(GlyphContext&);
	protected:
		Glyph();
};

#endif
