#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include "vec3.h"
#include "triangle.h"
#include <vector>
#include <stdlib.h>

using std::vector;
using std::string;

class obj_parser {
    public: 
        obj_parser(const string filename);

    public:
        vector<vec3*> vertices;
        vector<triangle*> faces;
};

#endif