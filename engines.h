#pragma once
struct supported_platforms {
	char* name;
	struct supported_platforms* next;
};

typedef struct engines
{
	char* name;
	char* tech_render;
	char* community;
	char* doc;
	struct supported_platforms* supported_platforms;
	
	unsigned polygons;
	unsigned license_cost;
	float rating;

	char physics_quality;
} ENGINES;