#include<SDL3/SDL.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<pthread.h>
#include"project.h"

#define WIDTH 750
#define HEIGHT 450


typedef struct{
	SDL_Surface *surface;
	SDL_Window *window;
	Scene *scene;
	int index, nThread;
	int *starts, *ends;
	int *threadStates;
	int *currents, *helped;
	int antiAliasingFactor;
	pthread_mutex_t *mutex;
}ThreadData;

void Display(Scene *scene, SDL_Window *window, int nThread, bool verbose, int antiAliasingFactor);

Color GetPixelColor(float i, float j, ThreadData *data){
	Point *p;
	Scene *scene = data->scene;
	SDL_Surface *surface = data->surface;
	int factor = data->antiAliasingFactor;
	float aspectRatio = (float)surface->h / surface->w;

	float viewportWidth = 2 * tan(scene->camera->fov / 2);
	float viewportHeight = viewportWidth * aspectRatio;

	int width = surface->w*factor;
	int height = surface->h*factor;

	float dx = ((i + 0.5)/width - 0.5) * viewportWidth;
	float dy = (0.5 - (j + 0.5)/height) * viewportHeight;

	Point *pixelPosition = scene->camera->position;
	pixelPosition = Point_translate(pixelPosition, scene->camera->front);
	pixelPosition = Point_translate(pixelPosition, Vector_scale(scene->camera->right, dx));
	pixelPosition = Point_translate(pixelPosition, Vector_scale(scene->camera->up, dy));

	Vector direction = Vector_normalize(Vector_fromPoints(scene->camera->position, pixelPosition));
	Ray *ray = Line_init(scene->camera->position, direction);

	return TraceRay(scene, ray);
}

void *thread_function(void *args){
	ThreadData *data = (ThreadData*)args;
	int index = data->index;
	int factor = data->antiAliasingFactor;
	int height = data->surface->h;
	SDL_Surface *surface = data->surface;
	SDL_Window *window = data->window;
	
	Color *colors = NULL;
	if(data->antiAliasingFactor > 1) colors = malloc(factor * factor * sizeof(Color));

	for(int i = data->starts[index]; i < data->ends[index]; i+=factor){
		pthread_mutex_lock(&data->mutex[index]);
		data->currents[index] = i;
		pthread_mutex_unlock(&data->mutex[index]);
		for(int j = 0; j < height*factor; j+=factor){
			Color color;
			
			if(factor == 1){
				color = GetPixelColor(i, j, data);
			}
			else{
				for(int k = i; k < i + factor; k++){
					for(int l = j; l < j + factor; l++){
						colors[(k-i)*factor + (l-j)] = GetPixelColor(k, l, data);
					}
				}
				color = Color_average(colors, factor*factor);
			}

			SDL_Rect pixel = (SDL_Rect) {i/factor, j/factor, 1, 1};
			SDL_FillSurfaceRect(surface, &pixel, Color_extract(color));
		}
		SDL_UpdateWindowSurface(window);
	}
	if(factor > 1) free(colors);
	pthread_mutex_lock(&data->mutex[index]);
	data->threadStates[index] = 1;
	pthread_mutex_unlock(&data->mutex[index]);

	pthread_t tid;
	for(int i = 0; i < data->nThread; i++){
		pthread_mutex_lock(&data->mutex[i]);
		if(data->threadStates[i] == 0 && data->helped[i] == 0){
			data->helped[i] = 1;
			data->ends[index] = data->ends[i];
			data->ends[i] = (data->currents[i] + data->ends[i]) / 2;
			data->starts[index] = data->ends[i];
			pthread_mutex_unlock(&data->mutex[i]);

			pthread_create(&tid, NULL, thread_function, data);
			pthread_join(tid, NULL);
			break;
		}else{
			pthread_mutex_unlock(&data->mutex[i]);
		}
	}
}

Scene *CreateScene(int numObj, char **objs){
	float fov = 90 * M_PI / 180;
	Camera *camera = Camera_new(Point_init(0, 0, 20), Vector_init(0, 0, -1), Vector_init(0, 1, 0), fov);
	Scene *scene = Scene_init(camera);
	
	float floorY = -10;
	Model *floor = Model_createRectXZ(Point_init(-500, floorY, -500), 1000, 1000, 0, 0, COLOR_BLUE);

	int numSphere = 8;
	Model **spheres = malloc(numSphere * sizeof(Model*));

	float radius = 10;
	spheres[0] = Model_createSphere(Point_init(10, floorY+radius, -20), radius, 1, 0.5, COLOR_GREEN);
	radius = 3;
	spheres[1] = Model_createSphere(Point_init(-5, floorY+radius, -10), radius, 0, 0, COLOR_RED);
	radius = 1;
	spheres[2] = Model_createSphere(Point_init(10, floorY+radius, -10), radius, 0, 0, COLOR_YELLOW);
	radius = 2;
	spheres[3] = Model_createSphere(Point_init(5, floorY+radius, -12), radius, 0, 0, COLOR_GREEN);
	radius = 4;
	spheres[4] = Model_createSphere(Point_init(20, floorY+radius, -12), radius, 0, 0.1, COLOR_ORANGE);
	radius = 15;
	spheres[5] = Model_createSphere(Point_init(-20, floorY+radius, -30), radius, 0.2, 0.5, COLOR_YELLOW);
	radius = 1.5;
	spheres[6] = Model_createSphere(Point_init(5, floorY+radius, -2), radius, 0, 0.2, COLOR_CYAN);
	radius = 1.5;
	spheres[7] = Model_createSphere(Point_init(-10, floorY+radius, 0), radius, 0, 0.2, COLOR_MAGENTA);

	Vector lightDirection = Vector_normalize(Vector_init(10, 5, 0));
	int scale = 20;
	Point* lightPos = Point_translate(Point_init(0, 0, 0), Vector_scale(lightDirection, scale));
	radius = 0.05;
	radius = 0;
	Light *lightSource = Light_new(lightPos, radius * scale, COLOR_WHITE);
	Light_setAttenuation(lightSource, 1, 0.0000, 0.0000);
	Scene_fill(scene, lightSource, &floor, 1);
	Scene_addModels(scene, spheres, numSphere);

	if(numObj <= 0) return scene;

	Model **objects = malloc(numObj * sizeof(Model*));
	for(int i = 0; i < numObj; i++){
		objects[i] = Model_fromOBJ(objs[i]);
	}

	Scene_addModels(scene, objects, numObj);

	for(int i = 0; i < scene->numModels; i++){
		Model_sortTriangles(scene->models[i], scene->camera->position);
	}

	printf("Scene size is %zu bytes\n", Scene_size(scene));

	return scene;
}

void SimulateScene(Scene *scene, SDL_Window *window, int antiAliasingFactor){
	int nThread = 12;
	Display(scene, window, nThread, 1, antiAliasingFactor);

	SDL_Event event;
	int numFrame = 1;
	while(1){
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_EVENT_QUIT){
				return;
			}
			else if(event.type == SDL_EVENT_WINDOW_RESIZED){
				Display(scene, window, nThread, 1, antiAliasingFactor);
			}
			else if(event.type == SDL_EVENT_KEY_DOWN){
				SDL_Keycode key = event.key.key;
				bool display = true;

				switch(key){
					case SDLK_ESCAPE:
						return;
						break;
					case SDLK_W:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_FORWARD);
						break;
					case SDLK_S:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_BACKWARD);
						break;
					case SDLK_D:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_RIGHT);
						break;
					case SDLK_A:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_LEFT);
						break;
					case SDLK_R:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_UP);
						break;
					case SDLK_F:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_DOWN);
						break;
					case SDLK_Q:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_ROTATE_LEFT);
						break;
					case SDLK_E:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_ROTATE_RIGHT);
						break;
					case SDLK_T:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_ROTATE_UP);
						break;
					case SDLK_G:
						Camera_ProcessMovement(scene->camera, CAMERA_MOVEMENT_ROTATE_DOWN);
						break;
					default:
						display = false;
						break;
				}

				if(display)
					Display(scene, window, nThread, 1, antiAliasingFactor);
			}
		}
		SDL_Delay(50);
	}
}

SDL_Window* InitWindow(){
	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
		printf("ERROR::SDL::Init:%s\n", SDL_GetError());
		return NULL;
	}
	SDL_Window* window = SDL_CreateWindow("Ray Tracing", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		printf("ERROR::SDL::CreateWindow::%s\n", SDL_GetError());
		SDL_Quit();
		return NULL;
	}

	SDL_Surface *icon = SDL_LoadBMP(GetFullPath("icon.bmp"));
	if (!icon) {
		printf("ERROR::SDL::ICON_LOADING::%s\n", SDL_GetError());
	} else {
		SDL_SetWindowIcon(window, icon);
		SDL_DestroySurface(icon);
	}

	return window;
}

int main(int argc, char **argv) {
	int antiAliasingFactor = 1;
	if(argc >= 2){
		antiAliasingFactor = atoi(argv[1]);
	}
	srand(time(NULL));
	SDL_Window* window = InitWindow();
	if(window == NULL) return 1;

	Scene *scene = CreateScene(argc - 2, argv + 2);
	SDL_Delay(200);
	SimulateScene(scene, window, antiAliasingFactor);
}

void Display(Scene *scene, SDL_Window *window, int nThread, bool verbose, int antiAliasingFactor){
	pthread_t *tid = malloc(nThread * sizeof(pthread_t));
	SDL_Surface *surface = SDL_GetWindowSurface(window);

	ThreadData **threadDatas = malloc(nThread * sizeof(ThreadData*));
	int *starts = malloc(nThread * sizeof(int));
	int *ends = malloc(nThread * sizeof(int));
	int *currents = calloc(nThread, sizeof(int));
	int *helped = calloc(nThread, sizeof(int));
	int *threadStates = calloc(nThread, sizeof(int));
	pthread_mutex_t *mutex = malloc(nThread * sizeof(pthread_mutex_t));
	clock_t start = clock();

	for (int i = 0; i < nThread; i++) {
		pthread_mutex_init(&mutex[i], NULL);
		starts[i] = i * surface->w / nThread * antiAliasingFactor;
		ends[i] = (i + 1) * surface->w / nThread * antiAliasingFactor;

		threadDatas[i] = malloc(sizeof(ThreadData));
		threadDatas[i]->threadStates = threadStates;
		threadDatas[i]->index = i;
		threadDatas[i]->nThread = nThread;
		threadDatas[i]->mutex = mutex;

		threadDatas[i]->surface = surface;
		threadDatas[i]->window = window;
		threadDatas[i]->scene = scene;
		threadDatas[i]->starts = starts;
		threadDatas[i]->ends = ends;
		threadDatas[i]->currents = currents;
		threadDatas[i]->helped = helped;
		threadDatas[i]->antiAliasingFactor = antiAliasingFactor;
	}

	for(int i = 0; i < nThread; i++){
		pthread_create(&tid[i], NULL, thread_function, threadDatas[i]);
	}
	for(int i = 0; i < nThread; i++){
		pthread_join(tid[i], NULL);
	}

	SDL_UpdateWindowSurface(window);
	clock_t end = clock();
	float time = (float)(end - start) / CLOCKS_PER_SEC * 1000;
	if(verbose) printf("Display took %.0f ms\n", time);
}
