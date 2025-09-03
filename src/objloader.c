#include"objloader.h"
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define LINE_MAX_LEN 256

int ExtractVertexIndex(const char *token) {
	int index = 0;
	sscanf(token, "%d", &index);
	return index;
}


int LoadColors(char *fileName, char ***names, Color **color) {
	FILE *file = fopen(fileName, "r");
	if (!file) {
		perror("Error opening file");
		return 0;
	}

	char line[LINE_MAX_LEN];
	int colorCapacity = 8;
	int colorCount = 0;

	Color *colors = malloc(colorCapacity * sizeof(Color));
	char **colorNames = malloc(colorCapacity * sizeof(char*));

	char *currentName = NULL;
	double r, g, b;
	int hasColor = 0;

	while (fgets(line, sizeof(line), file)) {
		char *word = strtok(line, " \t\r\n");

		if (!word) continue;

		if (strcmp(word, "newmtl") == 0) {
			currentName = strdup(strtok(NULL, " \t\r\n"));
			hasColor = 0;
		} else if (strcmp(word, "Kd") == 0 && currentName != NULL) {
			r = atof(strtok(NULL, " \t\r\n"));
			g = atof(strtok(NULL, " \t\r\n"));
			b = atof(strtok(NULL, " \t\r\n"));

			if (colorCount >= colorCapacity) {
				colorCapacity *= 2;
				colors = realloc(colors, colorCapacity * sizeof(Color));
				colorNames = realloc(colorNames, colorCapacity * sizeof(char*));
			}

			colors[colorCount] = Color_fromRGB(r, g, b);
			colorNames[colorCount] = currentName;
			colorCount++;
			hasColor = 1;
		}
	}
	fclose(file);

	*names = colorNames;
	*color = colors;
	return colorCount;
}

int getIndex(char *word, char **array, int n){
	for(int i = 0; i < n; i++){
		if(strcmp(array[i], word) == 0) return i;
	}
	return -1;
}


Model* Model_fromOBJ(const char *fileName) {
	char *fullPath = GetFullPath((char *)fileName);
	FILE *file = fopen(fullPath, "r");
	if (!file) {
		perror("Failed to open OBJ file");
		return NULL;
	}

	char *directoryPath = GetDirectoryPath(fullPath);

	Point **points = NULL;
	int pointCapacity = 64, pointCount = 0;
	points = malloc(sizeof(Point*) * pointCapacity);

	Triangle **triangles = NULL;
	int triCapacity = 64, triCount = 0;
	triangles = malloc(sizeof(Triangle*) * triCapacity);

	char line[LINE_MAX_LEN];
	char **colorNames;
	Color *colors, actualColor = COLOR_WHITE;
	int numColors;
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			double x, y, z;
			sscanf(line + 2, "%lf %lf %lf", &x, &y, &z);
			if (pointCount >= pointCapacity) {
				pointCapacity *= 2;
				points = realloc(points, sizeof(Point*) * pointCapacity);
			}
			Point *p = malloc(sizeof(Point));
			p->x = x; p->y = y; p->z = z;
			points[pointCount++] = p;
		} else if (line[0] == 'f' && line[1] == ' ') {
			char *tokens[3];
			int tokenCount = 0;

			char *ptr = strtok(line + 2, " \t\r\n");
			while (ptr && tokenCount < 3) {
				tokens[tokenCount++] = ptr;
				ptr = strtok(NULL, " \t\r\n");
			}

			if (tokenCount != 3) continue;

			int i1 = ExtractVertexIndex(tokens[0]);
			int i2 = ExtractVertexIndex(tokens[1]);
			int i3 = ExtractVertexIndex(tokens[2]);

			if (i1 <= 0 || i2 <= 0 || i3 <= 0) continue;

			if (triCount >= triCapacity) {
				triCapacity *= 2;
				triangles = realloc(triangles, sizeof(Triangle*) * triCapacity);
			}

			Triangle *t = malloc(sizeof(Triangle));
			t->a = points[i1 - 1];
			t->b = points[i2 - 1];
			t->c = points[i3 - 1];
			t->color = actualColor;
			triangles[triCount++] = t;
		}
		else{
			char *word = strtok(line, " \t\r\n");
			if(strcmp(word, "mtllib") == 0){
				word = strtok(NULL, " \t\r\n");
				numColors = LoadColors(strcat(directoryPath, word), &colorNames, &colors);
			}
			else if(strcmp(word, "usemtl") == 0){
				word = strtok(NULL, " \t\r\n");
				int index = getIndex(word, colorNames, numColors);
				actualColor = colors[index];
			}
		}
	}

	fclose(file);

	Point *center = malloc(sizeof(Point));
	center->x = center->y = center->z = 0;
	for (int i = 0; i < pointCount; i++) {
		center->x += points[i]->x;
		center->y += points[i]->y;
		center->z += points[i]->z;
	}
	center->x /= pointCount;
	center->y /= pointCount;
	center->z /= pointCount;

	double maxDist = 0;
	for (int i = 0; i < pointCount; i++) {
		double d = Point_distanceSquared(center, points[i]);
		if (d > maxDist) maxDist = d;
	}

	Model *model = malloc(sizeof(Model));
	model->numTriangles = triCount;
	model->triangles = triangles;
	model->center = center;
	model->maxDistanceFromCenter = sqrt(maxDist);
	model->color = COLOR_WHITE;
	model->reflexivity = 0.0;
	model->shininess = 0.0;
	model->type = GENERIC;

	return model;
}

int Model_toOBJ(const Model *model, const char *fileName) {
	if (!model || !fileName) return -1;

	FILE *file = fopen(fileName, "w");
	if (!file) {
		perror("Failed to open file for writing");
		return -1;
	}

	int vertexIndex = 1;

	for (int i = 0; i < model->numTriangles; ++i) {
		Triangle *t = model->triangles[i];
		fprintf(file, "v %f %f %f\n", t->a->x, t->a->y, t->a->z);
		fprintf(file, "v %f %f %f\n", t->b->x, t->b->y, t->b->z);
		fprintf(file, "v %f %f %f\n", t->c->x, t->c->y, t->c->z);
	}

	for (int i = 0; i < model->numTriangles; ++i) {
		fprintf(file, "f %d %d %d\n", vertexIndex, vertexIndex + 1, vertexIndex + 2);
		vertexIndex += 3;
	}

	fclose(file);
	return 0;
}