#ifndef MESH_H
#define MESH_H

#include "vec3.h"
#include "triangle.h"
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class mesh {
    public: 
        mesh(const string filename, const color& kDiffuse);

        vector<vec3*> get_vertices() {
            return vertices;
        }
        
        vector<objs*> get_faces() {
            return faces;
        }

        void calculate_normals();

    public:
        vector<vec3*> vertices;
        vector<objs*> faces;
        vector<vec3*> indices;
        // vector<objs*> normals;
};

mesh::mesh(const string filename, const color& kDiffuse) {
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
            triangle* t = new triangle(*vertices[x - 1], *vertices[y - 1], *vertices[z - 1], kDiffuse);
            faces.push_back(t);
            indices.push_back(new vec3(x - 1, y - 1, z - 1));
            int n = indices.size() - 1;
            // cerr << indices[n]->x() << " " << indices[n]->y() << " " << indices[n]->z() << "\n";

            // vec3 normal = t->surface_normal(point3(0,0,0));
            // cerr << normal[0] << " " << normal[1] << " " << normal[2] << "\n";
            
            // vec3 n = *normals[x - 1] + normal;
            // cerr << n << "\n";
            // normals[x - 1] += &normal;
        }
        i++;
        
    }
    
    // cerr << vertices.size() << " " << faces.size() << "\n";
    calculate_normals();
}

void mesh::calculate_normals() {
    vector<vec3> normals(vertices.size());
    // cerr << vertices.size() << " " << normals.size() << " " << indices.size() << " " << faces.size() << "\n";
    // for (int i = 0; i < normals.size(); i++) {
    //     cerr << "v" << i << "-> vertex normal: " << normals[i] << "\n";
    // }
    // cerr << "\n\n";
    for (int i = 0; i < faces.size(); i++) {
        vec3 normal = faces[i]->surface_normal(point3(0.0,0.0,0.0));
        // cerr << "t" << i << "-> surface normal: " << normal << "\n";
        vec3 index = *indices[i];
        // cerr << "index: " << index << "\n";
        // cerr << normal[0] << " " << normal[1] << " " << normal[2] << "\n";
        // // cerr << normals[index->x()]->x() << " " << normals[index->x()]->y() << " " << normals[index->x()]->z() << " " << "\n\n\n";
        // // normals[index->x()] += &normal;
        // cerr << "v" << index.x() << "-> vertex normal: " << normals[index.x()] << "\n";
        // cerr << "v" << index.y() << "-> vertex normal: " << normals[index.y()] << "\n";
        // cerr << "v" << index.z() << "-> vertex normal: " << normals[index.z()] << "\n";
        auto a = (normal) + normals[index.x()];
        auto b = (normal) + normals[index.y()];
        auto c = (normal) + normals[index.z()];
        normals[index.x()] = a;
        normals[index.y()] = b;
        normals[index.z()] = c;
        // cerr << "v" << index.x() << "-> vertex normal: " << normals[index.x()] << "\n";
        // cerr << "v" << index.y() << "-> vertex normal: " << normals[index.y()] << "\n";
        // cerr << "v" << index.z() << "-> vertex normal: " << normals[index.z()] << "\n";
        // cerr << "\n";
        // cerr << a[0] << " " << a[1] << " " << a[2] << "\n\n\n";
    }

    // cerr << "\n\n";
    for (int i = 0; i < normals.size(); i++) {
        // cerr << "v" << i << "-> vertex normal: " << normals[i] << "\n";
        vec3 unit = unit_vector(normals[i]);
        normals[i] = unit;
        // cerr << "v" << i << "-> normalized: " << normals[i] << "\n";
        // // cerr << normals[i]->x() << " " << normals[i]->y() << " " << normals[i]->z() << "\n";
    }

    for (int i = 0; i < faces.size(); i++) {
        triangle* t = (triangle*) faces[i];
        vec3* index = indices[i];
        // cerr << index->x() << " "<< index->y() << " "<< index->z() << "\n";
        // cerr << "(" << normals[index->x()] << ") ("<< normals[index->y()] << ") ("<< normals[index->z()] << ")\n\n";
        t->set_vertex_normals(normals[index->x()], normals[index->y()], normals[index->z()]);
        // cerr << "triangle point: " << t->a_t() << "\n";
    }
}

#endif