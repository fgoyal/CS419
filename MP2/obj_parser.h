#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include "vec3.h"
#include "triangle.h"
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class obj_parser {
    public: 
        obj_parser(const string filename, const color& kDiffuse);

        vector<vec3*> get_vertices() {
            return vertices;
        }
        
        vector<objs*> get_faces() {
            return faces;
        }

    public:
        vector<vec3*> vertices;
        vector<objs*> faces;
};

obj_parser::obj_parser(const string filename, const color& kDiffuse) {
    ifstream file(filename);
    string str;
    char a;
    float x, y, z;
    int i = 0;
    while (file >> a >> x >> y >> z) {
        // if (i > 49990) {
        // cerr << a << " " << x << " " << y << " " << z << "\n";
        // }
        // cerr << x << "\n\n";
        if (a == 'v') {
            vertices.push_back(new vec3(x, y, z));
        }
        if (a == 'f') {
            // cerr << "face " << vertices[x] << "\n";
            faces.push_back(new triangle(*vertices[x - 1], *vertices[y - 1], *vertices[z - 1], kDiffuse));
        }
        i++;
        
    }
    
    cerr << vertices.size() << " " << faces.size() << "\n";

}

#endif