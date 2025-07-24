#include<SDL3/SDL.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<pthread.h>
#include"project.h"

#define WIDTH 500
#define HEIGHT 300


typedef struct{
    SDL_Surface *surface;
    SDL_Window *window;
    Scene *scene;
    int i;
    int start, end;
}threadData;

void Display(Scene *scene, SDL_Window *window, int nThread, bool verbose);

void *thread_function(void *args){
    threadData *data = (threadData*)args;
    Point *p;
    Scene *scene = data->scene;
    Point *camera = scene->camera;
    SDL_Surface *surface = data->surface;
    SDL_Window *window = data->window;
    double distance = scene->cameraDistance;
    double aspectRatio = (double)surface->w / surface->h;
    double viewportHeight = 3; // or whatever field of view you want
    double viewportWidth = viewportHeight * aspectRatio;

    int width = surface->w;
    int height = surface->h;

    double alpha = scene->rotationAngle;

    for(int i = data->start; i < data->end; i++){
        for(int j = 0; j < height; j++){
            double x, y, z;
            x = -viewportWidth / 2 + viewportWidth*(i+0.5)/width;
            y = camera->y + viewportHeight/2 - viewportHeight*(j+0.5)/height;
            z = -distance;

            double rotatedX = camera->x + z*sin(alpha) + x*cos(alpha);
            double rotatedZ = camera->z + z*cos(alpha) - x*sin(alpha);
            
            p = Point_init(rotatedX, y, rotatedZ);

            Vector direction = Vector_fromPoints(camera, p);
            Line *l = Line_init(camera, &direction);

            Color color = TraceRay(scene, l);

            SDL_Rect pixel = (SDL_Rect) {i, j, 1, 1};
            SDL_FillSurfaceRect(surface, &pixel, Color_extract(color));
        }
        SDL_UpdateWindowSurface(window);
    }
}

Scene *createScene(){
    Point *camera = Point_init(0, 0, 0);

    Scene *scene = Scene_init();
    scene->cameraDistance = 1;

    Surface *sphere1 = Surface_createSphere(Point_init(15, 0, -20), 7.5, 1, 0.5, COLOR_RED);
    Surface *sphere2 = Surface_createSphere(Point_init(-15, 0, -20), 7.5, 0.2, 0.5, COLOR_BLUE);

    Surface *rightWall = Surface_createRectYZ(Point_init(100, -75, -100), 200, 150, 0, 0, COLOR_GREEN);
    Surface *leftWall = Surface_createRectYZ(Point_init(-100, -75, -100), 200, 150, 0, 0, COLOR_GREEN);
    Surface *frontWall = Surface_createRectXY(Point_init(-100, -75, -100), 200, 150, 0, 0.1, Color_scale(COLOR_RED, 0.9));
    Surface *backWall = Surface_createRectXY(Point_init(-100, -75, 100), 200, 150, 0, 0, COLOR_RED);
    Surface *topWall = Surface_createRectXZ(Point_init(-100, 75, -100), 200, 200, 0, 0, COLOR_BLUE);
    Surface *baseWall = Surface_createRectXZ(Point_init(-100, -75, -100), 200, 200, 0, 0, COLOR_BLUE);

    int n = 8;
    Surface **list = malloc(n * sizeof(Surface*));
    list[0] = sphere1;
    list[1] = sphere2;
    list[2] = frontWall;
    list[3] = rightWall;
    list[4] = leftWall;
    list[5] = topWall;
    list[6] = baseWall;
    list[7] = backWall;

    Point *lightSource = Point_init(5, 5, 5);
    Scene_fill(scene,lightSource, list, n);
    scene->camera = camera;
}

void SimulateScene(Scene *scene, SDL_Window *window){
    int nThread = 20;
    Display(scene, window, nThread, 1);

    SDL_Event event;
    int numFrame = 1;
    while(1){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                return;
            }
            else if(event.type == SDL_EVENT_WINDOW_RESIZED){
                Display(scene, window, nThread, 1);
            }
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                double angleStep = 10;
                angleStep = angleStep * M_PI / 180;
                if(event.button.button == SDL_BUTTON_RIGHT){
                    angleStep = -angleStep;
                }
                else if(event.button.button != SDL_BUTTON_LEFT){
                    continue;
                }

                for(int i = 0; i < numFrame; i++){
                    scene->rotationAngle += angleStep / numFrame;
                    Display(scene, window, nThread, 0);
                }
            }
            else if(event.type == SDL_EVENT_KEY_DOWN){
                SDL_Keycode key = event.key.key;
                double moveStep = 4;
                double deltaX = 0, deltaZ = 0;
                double angle = scene->rotationAngle;
                if(key == SDLK_W){
                    deltaZ = -moveStep*cos(angle);
                    deltaX = -moveStep*sin(angle);
                }
                else if(key == SDLK_S){
                    deltaZ = moveStep*cos(angle);
                    deltaX = moveStep*sin(angle);
                }
                else if(key == SDLK_D){
                    deltaZ = -moveStep*sin(angle);
                    deltaX = moveStep*cos(angle);
                }
                else if(key == SDLK_A){
                    deltaZ = moveStep*sin(angle);
                    deltaX = -moveStep*cos(angle);
                }
                for(int i = 0; i < numFrame; i++){
                    scene->camera->x += deltaX / numFrame;
                    scene->camera->z += deltaZ / numFrame;
                    Display(scene, window, nThread, 0);
                }
            }
        }
        SDL_Delay(50);
    }
}

SDL_Window* InitWindow(){
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Window* window = SDL_CreateWindow("RayTracing", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }
    return window;
}

int main() {
    SDL_Window* window = InitWindow();
    if(window == NULL) return 1;

    Scene *scene = createScene();
    SimulateScene(scene, window);
}

void Display(Scene *scene, SDL_Window *window, int nThread, bool verbose){
    pthread_t *tid = malloc(nThread * sizeof(pthread_t));
    threadData **thread_data = malloc(nThread * sizeof(threadData*));
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    clock_t start = clock();
    for (int i = 0; i < nThread; i++) {
        thread_data[i] = malloc(sizeof(threadData));
        thread_data[i]->surface = surface;
        thread_data[i]->window = window;
        thread_data[i]->scene = scene;
        thread_data[i]->start = i * surface->w / nThread;
        thread_data[i]->end = (i + 1) * surface->w / nThread;

        pthread_create(&tid[i], NULL, thread_function, thread_data[i]);
    }
    for(int i = 0; i < nThread; i++){
        pthread_join(tid[i], NULL);
    }

    SDL_UpdateWindowSurface(window);
    clock_t end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    if(verbose) printf("loop done, it took %.2f seconds\n", time);
}
