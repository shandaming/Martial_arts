#ifndef ICONWINDOW_H
#define ICONWINDOW_H

class IconWindow:public Window
{
	public:
		virtual void drawContents() 
		{
			WindowImp* imp=getWindowImp();
			if(imp!=0)
				img->DeviceBitmap(bitmapName,0.0,0.0);
		}
	private:
		const char* bitmapName;
};

#endif
