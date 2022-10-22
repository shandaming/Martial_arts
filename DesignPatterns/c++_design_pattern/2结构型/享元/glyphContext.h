#ifndef GLYPHCONTEXT_H
#define GLYPHCONTEXT_H

class GlyphContext
{
	public:
		GlyphContext();
		virtual ~GlyphContext();

		virtual void next(int setp=1);
		virtual void insert(int quantity=1);

		virtual Font* getFont();
		virtual void setFont(Font*,int span=1);
	private:
		int index;
		BTree* fonts;
};

#endif
