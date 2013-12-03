#ifndef PLAY_SPRITE_BUFFER_H

#define PLAY_SPRITE_BUFFER_H

class PlaySpriteBuffer
{
public:

	virtual void process(void) = 0;
	virtual void setFrame(int frame) = 0;
	virtual void release(void) = 0;

protected:
	virtual ~PlaySpriteBuffer(void)
	{
	}
};

PlaySpriteBuffer *createPlaySpriteBuffer(void);

#endif
