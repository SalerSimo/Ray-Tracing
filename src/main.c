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

Color GetPixelColor(double i, double j, ThreadData *data){
    Point *p;
    Scene *scene = data->scene;
    Point *camera = scene->camera;
    SDL_Surface *surface = data->surface;
    double distance = 1;
    int factor = data->antiAliasingFactor;
    double aspectRatio = (double)surface->h / surface->w;

    double fieldOfView = scene->fieldOfView;
    double viewportWidth = 2 * distance * tan(fieldOfView / 2);
    double viewportHeight = viewportWidth * aspectRatio;

    int width = surface->w*factor;
    int height = surface->h*factor;

    double alpha = scene->rotationAngle;

    double x, y, z;
    x = -viewportWidth / 2 + viewportWidth*(i+0.5)/width;
    y = camera->y + viewportHeight/2 - viewportHeight*(j+0.5)/height;
    z = -distance;
    double rotatedX = camera->x + z*sin(alpha) + x*cos(alpha);
    double rotatedZ = camera->z + z*cos(alpha) - x*sin(alpha);
    
    p = Point_init(rotatedX, y, rotatedZ);

    Vector direction = Vector_normalize(Vector_fromPoints(camera, p));
    Line *l = Line_init(camera, &direction);

    return TraceRay(scene, l);
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

Scene *CreateScene(){
    Point *camera = Point_init(0, 0, 20);

    double fieldOfView = 90 * M_PI / 180;
    Scene *scene = Scene_init(camera, fieldOfView);    
    
    double floorY = -10;
    Surface *floor = Surface_createRectXZ(Point_init(-500, floorY, -500), 1000, 1000, 0, 0, COLOR_BLUE);

    int numSphere = 8;
    Surface **spheres = malloc(numSphere * sizeof(Surface*));

    double radius = 10;
    spheres[0] = Surface_createSphere(Point_init(10, floorY+radius, -20), radius, 1, 0.5, COLOR_GREEN);
    radius = 3;
    spheres[1] = Surface_createSphere(Point_init(-5, floorY+radius, -10), radius, 0, 0, COLOR_RED);
    radius = 1;
    spheres[2] = Surface_createSphere(Point_init(10, floorY+radius, -10), radius, 0, 0, COLOR_YELLOW);
    radius = 2;
    spheres[3] = Surface_createSphere(Point_init(5, floorY+radius, -12), radius, 0, 0, COLOR_GREEN);
    radius = 4;
    spheres[4] = Surface_createSphere(Point_init(20, floorY+radius, -12), radius, 0, 0.1, COLOR_ORANGE);
    radius = 15;
    spheres[5] = Surface_createSphere(Point_init(-20, floorY+radius, -30), radius, 0.2, 0.5, COLOR_YELLOW);
    radius = 1.5;
    spheres[6] = Surface_createSphere(Point_init(5, floorY+radius, -2), radius, 0, 0.2, COLOR_CYAN);
    radius = 1.5;
    spheres[7] = Surface_createSphere(Point_init(-10, floorY+radius, 0), radius, 0, 0.2, COLOR_MAGENTA);

    Light *lightSource = Light_new(Point_init(10, 5, 0), 1, COLOR_WHITE);
    Scene_fill(scene, lightSource, &floor, 1);
    Scene_addSurfaces(scene, spheres, numSphere);
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
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                double angleStep = 10;
                angleStep = angleStep * M_PI / 180;

                switch(event.button.button){
                    case SDL_BUTTON_LEFT:
                        break;
                    case SDL_BUTTON_RIGHT:
                        angleStep *= -1;
                        break;
                    default:
                        continue;
                }

                for(int i = 0; i < numFrame; i++){
                    scene->rotationAngle += angleStep / numFrame;
                    Display(scene, window, nThread, 0, antiAliasingFactor);
                }
            }
            else if(event.type == SDL_EVENT_KEY_DOWN){
                SDL_Keycode key = event.key.key;
                double moveStep = 4;
                double deltaX = 0, deltaZ = 0, deltaY = 0;
                double angle = scene->rotationAngle;
                switch(key){
                    case SDLK_W:
                        deltaZ = -moveStep*cos(angle);
                        deltaX = -moveStep*sin(angle);
                        break;
                    case SDLK_S:
                        deltaZ = moveStep*cos(angle);
                        deltaX = moveStep*sin(angle);
                        break;
                    case SDLK_D:
                        deltaZ = -moveStep*sin(angle);
                        deltaX = moveStep*cos(angle);
                        break;
                    case SDLK_A:
                        deltaZ = moveStep*sin(angle);
                        deltaX = -moveStep*cos(angle);
                        break;
                    case SDLK_UP:
                        deltaY = 2;
                        break;
                    case SDLK_DOWN:
                        deltaY = -2;
                        break;
                    default:
                        break;
                }
                for(int i = 0; i < numFrame; i++){
                    scene->camera->x += deltaX / numFrame;
                    scene->camera->z += deltaZ / numFrame;
                    scene->camera->y += deltaY / numFrame;
                    Display(scene, window, nThread, 0, antiAliasingFactor);
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
    srand(time(NULL));
    SDL_Window* window = InitWindow();
    if(window == NULL) return 1;

    Scene *scene = CreateScene();
    SimulateScene(scene, window, 2);
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
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    if(verbose) printf("loop done, it took %.2f seconds\n", time);
}
