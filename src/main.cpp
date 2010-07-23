#include <SDL/SDL.h>
#include <PDL.h>

//how many milliseconds per frame (30 fps)
#define TICKS_PER_FRAME 1000/30

void draw_frame (SDL_Surface *surface);
void process_event (SDL_Event event);
Uint32 limiter (Uint32 interval, void *param);

bool paused = false;

int main (int argc, char *argv[])
{
	SDL_Surface *screen;
	int before = 0;
	int delta = 0;

	if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		fprintf (stderr, "Failed to init SDL: %s\n", SDL_GetError ());
		return -1;
	}
	atexit (SDL_Quit);

	PDL_Init (0);
	atexit (PDL_Quit);

	screen = SDL_SetVideoMode (0, 0, 0, SDL_SWSURFACE);
	if (!screen) {
		fprintf (stderr, "Failed to set video mode: %s\n", SDL_GetError ());
		return -1;
	}

	SDL_Event event;
	while (true) {
		if (paused) {
			//switch to WaitEvent on pause because it blocks
			SDL_WaitEvent (&event);
			if (event.type == SDL_ACTIVEEVENT && event.active.gain == 1 && event.active.state & SDL_APPACTIVE) {
				paused = false;
				continue;
			}

			//while not active the OS may ask us to draw anyway, don't ignore it
			if (event.type == SDL_VIDEOEXPOSE) {
				draw_frame (screen);
			}
		}
		else {
			before = SDL_GetTicks ();

			while (SDL_PollEvent (&event)) {
				process_event (event);
			}

			draw_frame (screen);

			//we don't want to draw too fast, limit framerate
			delta = SDL_GetTicks () - before;
			while (delta < TICKS_PER_FRAME) {
				//we setup a timer that sends a user (custom) event
				SDL_TimerID timer = SDL_AddTimer (TICKS_PER_FRAME - delta, limiter, NULL);

				//clear the event type and wait for another event
				event.type = -1;
				SDL_WaitEvent (&event);

				//if it wasn't the user event process it and loop
				SDL_RemoveTimer (timer);

				if (event.type != SDL_USEREVENT) {
					process_event (event);

					//some time has passed, reset delta
					delta = SDL_GetTicks () - before;
				}
				else {
					break;
				}
			}
			printf ("FPS: %d\n", 1000 / (SDL_GetTicks () - before));
		}
	}
	return 0;
}

void draw_frame (SDL_Surface *surface)
{
	//do drawing here
}

void process_event (SDL_Event event)
{
	if (event.type == SDL_QUIT) {
		exit (0);
	}

	if (event.type == SDL_ACTIVEEVENT && event.active.gain == 0 && event.active.state & SDL_APPACTIVE) {
		paused = true;
	}
}

Uint32 limiter (Uint32 interval, void *param)
{
	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent (&event);

    return 0;
}
