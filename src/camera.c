#include"camera.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

double moveStep = 4;
double angleStep = 15 * M_PI / 180;

Camera* Camera_new(Point *position, Vector front, Vector up, double fov){
      if(Vector_dot(front, up) != 0){
            printf("ERROR::CAMERA::Front and Up direction must be perpendicular\n");
            return NULL;
      }

      Camera* camera = malloc(sizeof(Camera));
      if(camera == NULL){
            return NULL;
      }

      camera->position = position;
      camera->front = Vector_normalize(front);
      camera->up = Vector_normalize(up);
      camera->right = Vector_normalize(Vector_crossProduct(camera->front, camera->up));
      camera->fov = fov;

      return camera;
}

void Camera_ProcessMovement(Camera *camera, CameraMovement movement){
      if(movement == CAMERA_MOVEMENT_FORWARD){
            camera->position = Point_translate(camera->position, Vector_scale(camera->front, moveStep));
      }
      else if(movement == CAMERA_MOVEMENT_BACKWARD){
            camera->position = Point_translate(camera->position, Vector_scale(camera->front, -moveStep));
      }
      else if(movement == CAMERA_MOVEMENT_RIGHT){
            camera->position = Point_translate(camera->position, Vector_scale(camera->right, moveStep));
      }
      else if(movement == CAMERA_MOVEMENT_LEFT){
            camera->position = Point_translate(camera->position, Vector_scale(camera->right, -moveStep));
      }
      else if(movement == CAMERA_MOVEMENT_UP){
            camera->position = Point_translate(camera->position, Vector_scale(camera->up, moveStep));
      }
      else if(movement == CAMERA_MOVEMENT_DOWN){
            camera->position = Point_translate(camera->position, Vector_scale(camera->up, -moveStep));
      }
      else if(movement == CAMERA_MOVEMENT_ROTATE_RIGHT){
            camera->front = Vector_rotate(camera->front, camera->up, angleStep);
            camera->right = Vector_crossProduct(camera->front, camera->up);
      }
      else if(movement == CAMERA_MOVEMENT_ROTATE_LEFT){
            camera->front = Vector_rotate(camera->front, camera->up, -angleStep);
            camera->right = Vector_crossProduct(camera->front, camera->up);
      }
      else if(movement == CAMERA_MOVEMENT_ROTATE_UP){
            camera->front = Vector_rotate(camera->front, camera->right, -angleStep);
            camera->up = Vector_crossProduct(camera->right, camera->front);
      }
      else if(movement == CAMERA_MOVEMENT_ROTATE_DOWN){
            camera->front = Vector_rotate(camera->front, camera->right, angleStep);
            camera->up = Vector_crossProduct(camera->right, camera->front);
      }
}