#include <SDL.h>
#include <SDL_ttf.h>
#include <linux/input.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int IsEventDevice(const struct dirent *dir) {
	return strncmp("event", dir->d_name, 5) == 0;
}

int main(int argc, char *argv[])
{
	int deviceNumber, fd;
	struct dirent **devicesList;
	int devicesCount = scandir("/dev/input/", &devicesList, IsEventDevice, versionsort);
	if (devicesCount <= 0)
		return 1;
	printf("Available devices:\n");
	for (int i = 0; i < devicesCount; i++) {
		char deviceFileName[64];
		char deviceName[256] = "???";

		snprintf(deviceFileName, sizeof(deviceFileName), "/dev/input/%s", devicesList[i]->d_name);
		fd = open(deviceFileName, O_RDONLY);
		if (fd < 0)
			continue;
		ioctl(fd, EVIOCGNAME(sizeof(deviceName)), deviceName);

		printf("[%i] %s (%s)\n", i + 1, deviceName, deviceFileName);
		close(fd);


		free(devicesList[i]);
	}
	printf("Select Touch Screen device: ");
	int match = scanf("%d", &deviceNumber);
	if (match < 1 || deviceNumber > devicesCount || deviceNumber < 0)
		return 1;

	char deviceFileName[64];
	snprintf(deviceFileName, sizeof(deviceFileName), "/dev/input/event%d", deviceNumber - 1);

	if ((fd = open(deviceFileName, O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to open device\n");
	}
	int maxXpos, maxYpos;
	{
		int abs[6] = {0};
		ioctl(fd, EVIOCGABS(ABS_X), abs);
		maxXpos = abs[2];
		ioctl(fd, EVIOCGABS(ABS_Y), abs);
		maxYpos = abs[2];
	}
	printf("Max X: %6d, Max Y: %6d\n", maxXpos, maxYpos);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		fprintf(stderr, "Unable to initialize SDL: %s", SDL_GetError());
        return 1;
	}
	SDL_DisplayMode displayMode;
	SDL_GetDesktopDisplayMode(0, &displayMode);


	SDL_Window* window = SDL_CreateWindow(
		"EVDEV Calibration",
		SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        displayMode.w,
        displayMode.h,
        SDL_WINDOW_FULLSCREEN 
	);



	if (window == NULL) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return 2;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL) {
    	fprintf(stderr, "Could not create renderer %s\n", SDL_GetError());
    	return 3;
    }

    bool isRunning = true;

	const size_t ev_size = sizeof(struct input_event);
	struct input_event ev;
    ssize_t size;

    int corner = 0, touchScreenWidth, touchScreenHeight, touchAreaXoffset, touchAreaYoffset;
    int lastX, lastY;

    while (isRunning) {
		SDL_SetRenderDrawColor(renderer, 176, 224, 230, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		switch (corner) {
			case 0: {
				int x = displayMode.w * 0.2;
				int y = displayMode.h * 0.2;
				SDL_RenderDrawLine(renderer, x - 20, y, x + 20, y);
				SDL_RenderDrawLine(renderer, x, y - 20, x, y + 20);
			}
				break;
			case 1: {
				int x = displayMode.w * 0.8;
				int y = displayMode.h * 0.2;
				SDL_RenderDrawLine(renderer, x - 20, y, x + 20, y);
				SDL_RenderDrawLine(renderer, x, y - 20, x, y + 20);
			}
				break;
			case 2: {
				int x = displayMode.w * 0.8;
				int y = displayMode.h * 0.8;
				SDL_RenderDrawLine(renderer, x - 20, y, x + 20, y);
				SDL_RenderDrawLine(renderer, x, y - 20, x, y + 20);
			}
				break;
			case 3: {
				int x = displayMode.w * 0.2;
				int y = displayMode.h * 0.8;
				SDL_RenderDrawLine(renderer, x - 20, y, x + 20, y);
				SDL_RenderDrawLine(renderer, x, y - 20, x, y + 20);
			}
				break;

		}

        SDL_RenderPresent(renderer);

        size = read(fd, &ev, ev_size);
        if (size < ev_size) {
            fprintf(stderr, "Error size when reading\n");
            return 1;
        }

        if (ev.type == EV_ABS) {
            if (ev.code == ABS_X) {
            	lastX = ev.value;
            }
            else if (ev.code == ABS_Y) {
            	lastY = ev.value;
            }
        }
        else if (ev.type == EV_KEY && ev.code == BTN_LEFT && ev.value == 0) {
        	switch (corner) {
        		case 0:
        			touchAreaXoffset = (maxXpos * 0.2) - lastX;
        			touchAreaYoffset = (maxYpos * 0.2) - lastY;
        			break;
        		case 2:
        			touchScreenWidth = maxXpos - ((lastX - (maxXpos * 0.8)) * 0.5);
        			touchScreenHeight = maxYpos - ((lastY - (maxYpos * 0.8)) * 0.5);
        			break;
        	}
        	printf("%i: X: %i, Y: %i\n", corner, lastX, lastY);
        	corner++;
        }
        if (corner == 4) {
        	printf("Width: %d, Height: %d\nOffset X: %d, Offset Y: %d\n", touchScreenWidth, touchScreenHeight, touchAreaXoffset, touchAreaYoffset);
        	float c0 = (float)touchScreenWidth / (float)maxXpos;
			float c2 = (float)touchScreenHeight / (float)maxYpos;
			float c1 = (float)touchAreaXoffset / (float)maxXpos;
			float c3 = (float)touchAreaYoffset / (float)maxYpos;
			char deviceName[256];
			ioctl(fd, EVIOCGNAME(sizeof(deviceName)), deviceName);
			char xinputCommand[256];
			snprintf(xinputCommand, sizeof(xinputCommand), "xinput set-prop '%s' 'Coordinate Transformation Matrix' %.6f 0 %.6f 0 %.6f %.6f 0 0 1", deviceName, c0, c1, c2, c3);
			printf("Execute this: \"%s\"\n", xinputCommand);
			system(xinputCommand);
			isRunning = false;
        }
        SDL_Delay(1000 / 64);
    }

	close(fd);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;

}