#ifndef MANDEL_BULB_H

#define MANDEL_BULB_H

class TfracSettings;

class iMandelBulb
{
public:
	virtual void	render(TfracSettings *settings) = 0;
};

iMandelBulb * createMandelBulb(void);
void releaseMandelBulb(iMandelBulb *b);

#endif
