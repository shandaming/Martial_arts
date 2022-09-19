#ifndef PMWINDOWIMP_H
#define PMWINDOWIMP_H

class PMWindowImp:public WindowImp
{
	public:
		PMWindowImp();

		virtual void deviceRect(Coord x0,Coord y0,Coord x1,Coord y1)
		{
			Coord left=min(x0,x1);
			Coord right=max(x0,x1);
			Coord bottom=min(y0,y1);
			Coord top=max(y0,y1);
			PPOINTL point[4];

			point[0].x=left; point[0].y=top;
			point[1].x=right; point[1].y=top;
			point[2].x=right; point[2].y=bottom;
			point[3].x=left; point[3].y=bottom;

			if((gpibeginPath(hps,1L)==false) ||
				(gpiSetCurrentPosition(hps,&point[3])==false) ||
				(gpiPolyLine(hps,&L,point)==GPI_ERROR) ||
				(gpiEndPath(hps)==false))
			{}
			else
				gpiStrokePath(hps,1L,0L);
		}
	private:
		HPS hps;
};

#endif
