struct supported_platforms {
	char* name;
	struct supported_platforms* next;
};

typedef struct engines
{
	char* name;
	char* tech_render;
	unsigned polygons;
	struct supported_platforms supported_platforms;
	char physics_quality;
	unsigned license_cost;
	char* community;
	char* doc;
	float rating;
} ENGINES;