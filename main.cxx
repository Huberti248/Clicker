#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

// TODO: Attribute Freepik

SDL_Renderer*renderer;
TTF_Font*font;
int windowWidth;
int windowHeight;
SDL_Point mousePos;

SDL_Texture* renderText(SDL_Texture* previousTexture, TTF_Font* font, SDL_Renderer* renderer, const std::string& text, SDL_Color color)
{
	if (previousTexture) {
		SDL_DestroyTexture(previousTexture);
	}
	SDL_Surface* surface;
	if (text.empty()) {
		surface = TTF_RenderUTF8_Blended(font, " ", color);
	}
	else {
		surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
	}
	if (surface) {
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		return texture;
	}
	else {
		return 0;
	}
}

int SDL_QueryTextureF(SDL_Texture* texture, Uint32* format, int* access, float* w, float* h)
{
	int wi, hi;
	int result = SDL_QueryTexture(texture, format, access, &wi, &hi);
	if(w){
	*w = wi;
	}
	if(h){
	*h = hi;
	}
	return result;
}

struct Text {
	std::string text;
	SDL_Texture* t = 0;
	SDL_FRect r{};

	void setText(std::string text, SDL_Color c = { 255,255,255 })
	{
		this->text = text;
		t = renderText(t, font, renderer, text, c);
	}

	void setText(int value, SDL_Color c = { 255,255,255 })
	{
		setText(std::to_string(value), c);
	}

	void adjustSize(float wMultiplier, float hMultiplier)
	{
		float w, h;
		SDL_QueryTextureF(t, 0, 0, &w, &h);
		r.w = w * wMultiplier;
		r.h = h * hMultiplier;
	}

	void draw()
	{
		SDL_RenderCopyF(renderer, t, 0, &r);
	}
};

SDL_Texture* load(std::string path)
{
SDL_Surface*s=IMG_Load(path.c_str());
SDL_Texture*t= SDL_CreateTextureFromSurface(renderer,s);
return t;
}

int random(int min, int max) //range : [min, max]
{
   static bool first = true;
   if (first) 
   {  
      std::srand( time(NULL) ); //seeding for the first time only!
      first = false;
   }
   return min + rand() % (( max + 1 ) - min);
}

int SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
	int offsetx, offsety, d;
	int status;


	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
		status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
		status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
		status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}


int SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
	int offsetx, offsety, d;
	int status;


	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {

		status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
			x + offsety, y + offsetx);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
			x + offsetx, y + offsety);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
			x + offsetx, y - offsety);
		status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
			x + offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}

                     SDL_bool SDL_PointInFRect(const SDL_Point* p,
                         const SDL_FRect*  r)
                         {
                         	
                         SDL_Rect foo;
                        foo.w=r->w;	
                        foo.h=r->h;
                        foo.x=r->x;
                        foo.y=r->y;
                        return SDL_PointInRect(p,&foo);
                         }
                         
                         enum State{
                         	Game,
                         	Shop,
                         };
                         
                         Text coins;
                         int power=1;
                         int formulas=0;
                         
                         int eventWatch(void* userdata, SDL_Event* event)
{
	// WARNING: Be very careful of what you do in the function, as it may run in a different thread
	if (event->type == SDL_APP_TERMINATING || event->type == SDL_APP_WILLENTERBACKGROUND) {
    std::ofstream ofs("data.txt");
    ofs
    <<coins.text
    <<std::endl
    <<power
    <<std::endl
    <<formulas
    <<std::endl;
	}
	return 0;
}

int firstItemPrice=100;
int secondItemPrice=200;

int main(int argc, char *argv[])
{
	SDL_Window *window;
    SDL_Event event;					SDL_Init(SDL_INIT_EVERYTHING);
TTF_Init();					
SDL_CreateWindowAndRenderer(320, 240, SDL_WINDOW_RESIZABLE, &window, &renderer);
SDL_GetWindowSize(window,&windowWidth,&windowHeight);
SDL_GetMouseState(&mousePos.x,&mousePos.y);
SDL_AddEventWatch(eventWatch, 0);
	font = TTF_OpenFont("/system/fonts/Roboto-Regular.ttf", 256);
	SDL_Texture*shopT=IMG_LoadTexture(renderer,"shops.png");
	coins.r.w=16;
	coins.r.h=32;
	coins.r.x=windowWidth/2-coins.r.w/2;
	coins.r.y=windowHeight/2-coins.r.h/2;
        	SDL_Rect shopBtnR;
        	shopBtnR.w=64;
        	shopBtnR.h=64;
        	shopBtnR.x=0;
        	shopBtnR.y=windowHeight-shopBtnR.h;
        	State state=State::Game;
        	{
        		std::ifstream ifs("data.txt");
        		if(ifs){
        std::string tmp;
        std::getline(ifs,tmp);
        coins.setText(tmp);
        std::getline(ifs,tmp);
        power=std::stoi(tmp);
        std::getline(ifs,tmp);
        formulas=std::stoi(tmp);
        		}
        		else{
        				coins.setText("0");
        		}       			        	SDL_QueryTextureF(coins.t,0,0,&coins.r.w,0);
        	coins.r.w/=10;        	}
        	#if 1 // NOTE: Shop
        	Text firstItemText;
        	firstItemText.setText("+1");
        	firstItemText.r.w=150;
        	firstItemText.r.h=150;
        	firstItemText.r.x=0;
            firstItemText.r.y=0;
            Text firstItemPriceText;
            firstItemPriceText.setText("Price: "+std::to_string(firstItemPrice));
        	firstItemPriceText.r.w=150;
        	firstItemPriceText.r.h=40;
        	firstItemPriceText.r.x=firstItemText.r.x+firstItemText.r.w/2-firstItemPriceText.r.w/2;
            firstItemPriceText.r.y=firstItemText.r.y+firstItemText.r.h;
Text firstItemBuyText;
        	firstItemBuyText.setText("buy");
        	firstItemBuyText.r.w=60;
        	firstItemBuyText.r.h=40;
        	firstItemBuyText.r.x=firstItemText.r.x+firstItemText.r.w/2-firstItemBuyText.r.w/2;
            firstItemBuyText.r.y=firstItemPriceText.r.y+firstItemPriceText.r.h+5;
            
            Text secondItemText;
        	secondItemText.setText("formula");
        	secondItemText.r.w=100;
        	secondItemText.r.h=30;
        	secondItemText.r.x=firstItemText.r.x+firstItemText.r.w;
            secondItemText.r.y=0;
            Text secondItemPriceText;
            secondItemPriceText.setText("Price: "+std::to_string(secondItemPrice));
        	secondItemPriceText.r.w=150;
        	secondItemPriceText.r.h=40;
        	secondItemPriceText.r.x=secondItemText.r.x+secondItemText.r.w/2-secondItemPriceText.r.w/2;
            secondItemPriceText.r.y=secondItemText.r.y+secondItemText.r.h;
Text secondItemBuyText;
        	secondItemBuyText.setText("buy");
        	secondItemBuyText.r.w=60;
        	secondItemBuyText.r.h=40;
        	secondItemBuyText.r.x=secondItemText.r.x+secondItemText.r.w/2-secondItemBuyText.r.w/2;
            secondItemBuyText.r.y=secondItemPriceText.r.y+secondItemPriceText.r.h+5;
            #endif
    while (1) {
        if(state==State::Game){
        while(SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT) {
            break;
        }
        else if (event.type
        ==SDL_MOUSEMOTION){
        	mousePos.x=event.motion.x;
        	mousePos.y=event.motion.y;
        }
        else if (event.type
        ==SDL_MOUSEBUTTONDOWN){
        	if(SDL_PointInRect(&mousePos,&shopBtnR)){
        		state=State::Shop;
        	}
        	else{
        		        	coins.setText(std::to_string(std::stoi(coins.text)+power));
        	SDL_QueryTextureF(coins.t,0,0,&coins.r.w,0);
        	coins.r.w/=10;
        	}
        }
        }	SDL_SetRenderDrawColor(renderer,0,0,0,0);
		SDL_RenderClear(renderer);
		coins.draw();	
		SDL_RenderCopy(renderer,shopT,0,&shopBtnR);
		Text firstFormulaText;
		firstFormulaText.setText("an=a1+(n-1)r");
		firstFormulaText.r.w=100;
		firstFormulaText.r.h=30;
		firstFormulaText.r.x=100;
		firstFormulaText.r.y=100;
		if(formulas>=1){
		firstFormulaText.draw();
		}
        SDL_RenderPresent(renderer);
    }    
    else if(state==State::Shop){
    	        while(SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT) {
            break;
        }
        else if (event.type
        ==SDL_MOUSEMOTION){
        	mousePos.x=event.motion.x;
        	mousePos.y=event.motion.y;
        }
        else if (event.type
        ==SDL_MOUSEBUTTONDOWN){
        	if(SDL_PointInRect(&mousePos,&shopBtnR)){
        		state=State::Game;
        	}
        	else if(SDL_PointInFRect(&mousePos,&firstItemBuyText.r)){
        		int c=std::stoi(coins.text);
        		if(c>=firstItemPrice){
        			c-=firstItemPrice;
        			++power;
        		}
        		        		coins.setText(std::to_string(c));
        		        	SDL_QueryTextureF(coins.t,0,0,&coins.r.w,0);
       		coins.r.w/=10;
        	}
        		        	else if(SDL_PointInFRect(&mousePos,&secondItemBuyText.r)){
        		int c=std::stoi(coins.text);
        		if(c>=secondItemPrice){
        			c-=secondItemPrice;
        			++formulas;
        		}   
        		        		coins.setText(std::to_string(c));
        		        	SDL_QueryTextureF(coins.t,0,0,&coins.r.w,0);
       		coins.r.w/=10;    		        	
        }
        }	SDL_SetRenderDrawColor(renderer,0,0,0,0);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer,0,255,0,0);
		SDL_RenderFillRect(renderer,&shopBtnR);
		firstItemText.draw();
		firstItemPriceText.draw();
		firstItemBuyText.draw();
		secondItemText.draw();
		secondItemPriceText.draw();
		secondItemBuyText.draw();
		Text powerText;
		powerText.setText("Power: "+std::to_string(power));
		powerText.r.w=110;
		powerText.r.h=30;
		powerText.r.x=windowWidth/2-powerText.r.w/2;
		powerText.r.y=windowHeight-powerText.r.h;
		Text coinsText;
		coinsText.setText("Coins: "+coins.text);
		coinsText.r.w=110;
		coinsText.r.h=30;
		coinsText.r.x=windowWidth/2-coinsText.r.w/2;
		coinsText.r.y=powerText.r.y-coinsText.r.h;
		coinsText.draw();
		powerText.draw();
        SDL_RenderPresent(renderer);
    }
    }
}}