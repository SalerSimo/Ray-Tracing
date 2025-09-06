#ifndef CAMERA_H
#define CAMERA_H

#include<stddef.h>
#include"geometry.h"

typedef enum{
      CAMERA_MOVEMENT_FORWARD, CAMERA_MOVEMENT_BACKWARD, 
      CAMERA_MOVEMENT_LEFT, CAMERA_MOVEMENT_RIGHT, 
      CAMERA_MOVEMENT_UP, CAMERA_MOVEMENT_DOWN, 
      CAMERA_MOVEMENT_ROTATE_LEFT, CAMERA_MOVEMENT_ROTATE_RIGHT,
      CAMERA_MOVEMENT_ROTATE_UP, CAMERA_MOVEMENT_ROTATE_DOWN
}CameraMovement;

typedef struct{
      Point *position;
      Vector front;
      Vector up;
      Vector right;

      float fov;
}Camera;

Camera* Camera_new(Point *position, Vector front, Vector up, float fov);

void Camera_ProcessMovement(Camera *camera, CameraMovement movement);

size_t Camera_size(Camera *camera);

#endif