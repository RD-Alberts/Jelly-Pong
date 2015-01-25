//imported libraries
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <sstream>
#include <stdlib.h>
#include <ctime>

using namespace std;

//variables
SDL_Surface *screen = NULL;
SDL_Surface *temp = NULL;
SDL_Surface *Surface = NULL;
SDL_Surface *PlayerImage = NULL;
SDL_Surface *AIScoreSurface = NULL;
SDL_Surface *PlayerScoreSurface = NULL;
SDL_Surface *BallImage = NULL; 
SDL_Surface *AiImage = NULL;  
SDL_Surface *BackgroundImage = NULL;

//event
SDL_Event occur;

//rectangle variables
SDL_Rect PlayerPaddle;
SDL_Rect AIPaddle;
SDL_Rect Ball;

//bal
const int BALL_X = 390;
const int BALL_Y = 290;

//bal velocity
int xVel = 0, yVel = 0;

//screen
const int ScreenWidht = 800;
const int ScreenHeight = 600;
const int ScreenBPP = 32;

//score variables
int PlayerScore, AIScore;
TTF_Font *times;
SDL_Rect PlayScoreRect, AIScoreRect;
SDL_Color Pink = {255, 0, 255};

//functions
bool PointInRect(int x, int y, SDL_Rect rec)
{
	if (x > rec.x && y > rec.y &&
		x < rec.x + rec.w && y < rec.y + rec.h)
	{
		return true;
	}

	return false;
}

bool CheckCollision(SDL_Rect r1, SDL_Rect r2)
{
	//the collsion if the ball hits something
	if(PointInRect(r1.x, r1.y , r2) == true ||
		PointInRect(r1.x + r1.w, r1.y , r2) == true ||
		PointInRect(r1.x, r1.y + r1.h , r2) == true ||
		PointInRect(r1.x + r1.w, r1.y + r1.h , r2) == true)
	{
		return true;
	}

	return false;

}

int GetRandomNumber(int high, int low)
{
	//generizing a random number
	return rand() % high + low;
}

void ResetBall()
{
	//setting the position of the ball
	Ball.x = BALL_X;
	Ball.y = BALL_Y;

	//makes the ball go in a random direction
	xVel = GetRandomNumber(2 , -2);
	yVel = GetRandomNumber(2 , -2);
}

void LoadGame()
{
	//starting SDL and TTF
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	//setting the screen
	SDL_WM_SetCaption("Jelly Pong", NULL);
	screen = SDL_SetVideoMode(ScreenWidht, ScreenHeight, ScreenBPP, SDL_HWSURFACE);

	//the player
	PlayerPaddle.x = 20;
	PlayerPaddle.y = 250;
	PlayerPaddle.h = 100;
	PlayerPaddle.w = 20;

	//the ai
	AIPaddle.x = 760;
	AIPaddle.y = 250;
	AIPaddle.h = 100;
	AIPaddle.w = 20;

	//the ball
	Ball.x = BALL_X;
	Ball.y = BALL_Y;
	Ball.w = 20;
	Ball.h = 20;

	//srand(time(NULL));

	ResetBall();

	//giving the text a font and the letter type
	times = TTF_OpenFont("Times.ttf", 20);

	//the position of the player score and the ai score
	PlayScoreRect.x = 100;
	PlayScoreRect.y = 50;
	AIScoreRect.x = 675;
	AIScoreRect.y = 50;

}

void Logic()
{
	Uint8 *keystates = SDL_GetKeyState(NULL);

	//Player moves
	if(keystates[SDLK_w])
	{
		PlayerPaddle.y -= 3;
	}

	if(keystates[SDLK_s])
	{
		PlayerPaddle.y += 3;
	}

	//player paddle stays in the screen
	if(PlayerPaddle.y < 1)
	{
		PlayerPaddle.y = 1;
	}

	if(PlayerPaddle.y + PlayerPaddle.h > ScreenHeight -1)
	{
		PlayerPaddle.y = (ScreenHeight - 1) - PlayerPaddle.h;
	}

	//ai paddle moves
	if(AIPaddle.y + 0.6 * AIPaddle.h > Ball.y + 0.6 * Ball.h)
	{
		AIPaddle.y -= 1;
	}
	else
		AIPaddle.y += 1;

	//ai paddle stays in the screen
	if(AIPaddle.y < 1)
	{
		AIPaddle.y = 1;
	}

	if(AIPaddle.y + AIPaddle.h > ScreenHeight -1)
	{
		AIPaddle.y = (ScreenHeight - 1) - AIPaddle.h;
	}

	//ball moves
	Ball.x += xVel;
	Ball.y += yVel;

	//ball stays in the screen
	if(Ball.y < 1)
	{
		yVel = -yVel;
	}

	if(Ball.y + Ball.h > ScreenHeight -1)
	{
		yVel = -yVel;
	}

	//if the ball passes the paddle you get a point
	if(Ball.x <  2)
	{
		ResetBall();
		AIScore += 1;
	}
	
	if(Ball.x + Ball.w > ScreenWidht - 2)
	{
		ResetBall();
		PlayerScore += 1;
	}

	//collision check with the paddles
	if(CheckCollision(Ball, PlayerPaddle) == true)
	{
		xVel = -xVel;
	}

	if(CheckCollision(Ball, AIPaddle) == true)
	{
		xVel = -xVel;
	}
}

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old image
        SDL_FreeSurface( loadedImage );

        //If the image was optimized just fine
        if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 100, 100, 100 );
            //Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );

            }
    }

    //Return the optimized image
    return optimizedImage;
}

void ApplySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
	//make a temporary rectangle to hold the offsets
	SDL_Rect offset;

	//give the offsets to the rectangle
	offset.x = x;
	offset.y = y;

	//blit the surface
	SDL_BlitSurface(source, NULL, destination, &offset);
}

bool load_files()
{
    //Load the image
		//background
	temp = IMG_Load("./JELLYJELLY_BACKGROUND.png");
	BackgroundImage = SDL_DisplayFormat(temp);
	BackgroundImage = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);
		
		//player
	temp = IMG_Load("./BAR_LEFT.png");
	PlayerImage = SDL_DisplayFormat(temp);
	PlayerImage = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);
		
		//ai
	temp = IMG_Load("./BAR_RIGHT.png");
	AiImage = SDL_DisplayFormat(temp);
	AiImage = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);

		//ball
	temp = IMG_Load("./CHERRIE.png");
	BallImage = SDL_DisplayFormat(temp);
	BallImage = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);

    //If there was an error in loading the image
    if( BackgroundImage == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void DrawScore()
{
	stringstream Pscore;
	stringstream Ascore;
	
	Pscore << PlayerScore;
	Ascore << AIScore;

	//Loading the score to the screen
		//player score 
	temp = TTF_RenderText_Solid(times, Pscore.str().c_str(), Pink);
	PlayerScoreSurface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

		//Ai score
	temp = TTF_RenderText_Solid(times, Ascore.str().c_str(), Pink);
	AIScoreSurface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
}

void Quit()
{
	//cleaning the screen
	SDL_FreeSurface(BackgroundImage);
	SDL_FreeSurface(PlayerScoreSurface);
	SDL_FreeSurface(AIScoreSurface);
	SDL_FreeSurface(PlayerImage);
	SDL_FreeSurface(AiImage);
	SDL_FreeSurface(BallImage);

	TTF_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//framerate variables
	const int FPS = 290;
    Uint32 start;

	LoadGame();

	bool running = true;

	//Load the files
    if( load_files() == false )
    {
        return 1;
    }

	while (running == true)
	{
		start = SDL_GetTicks();
		SDL_PollEvent(&occur);

		if (occur.type == SDL_QUIT)
		{
			running = false;
		}
		Logic();
		//Apply the surface to the screen
		ApplySurface( 0, 0, BackgroundImage, screen );
		ApplySurface( Ball.x, Ball.y, BallImage, screen );
		ApplySurface( PlayerPaddle.x, PlayerPaddle.y, PlayerImage, screen );
		ApplySurface( AIPaddle.x, AIPaddle.y, AiImage,screen);
		ApplySurface( AIScoreRect.x,  AIScoreRect.y, AIScoreSurface, screen);
		ApplySurface( PlayScoreRect.x, PlayScoreRect.y, PlayerScoreSurface, screen);

		DrawScore();
		
		//Update the screen
		if( SDL_Flip( screen ) == -1 )
		{
			return 1;
		}

		if(1000/FPS > SDL_GetTicks()-start) 
        {               
			 SDL_Delay(1000/FPS-(SDL_GetTicks()-start));
		}
	}
	Quit();
	return 0;
}