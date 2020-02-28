#include <stdio.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <map>
#include <string>
using namespace std;

typedef vector<glm::vec3> Polygon;

typedef map<int, Polygon> FACES;
//each face/Polygon is assigned an Index
typedef map<int, vector<int>> ADJ;
FACES faces;

float h_len = 0.5f;
bool SUBDIVIDED = false;

const glm::vec3 v[] = {
    {h_len, h_len, h_len},
    {h_len, h_len, -h_len},
    {-h_len, h_len, -h_len},
    {-h_len, h_len, h_len},
    {-h_len, -h_len, h_len},
    {-h_len, -h_len, -h_len},
    {h_len, -h_len, -h_len},
    {h_len, -h_len, h_len},
};

Polygon f0{v[2], v[1], v[0], v[3]}; //top
Polygon f1{v[2], v[3], v[4], v[5]}; //left
Polygon f2{v[4], v[7], v[6], v[5]}; //down
Polygon f3{v[0], v[1], v[6], v[7]}; //right
Polygon f4{v[3], v[0], v[7], v[4]}; //front
Polygon f5{v[1], v[2], v[5], v[6]}; //back
//initial object

void load_faces()
{
    faces[0] = f0;
    faces[1] = f1;
    faces[2] = f2;
    faces[3] = f3;
    faces[4] = f4;
    faces[5] = f5;
}

glm::vec3 add_vec(glm::vec3 a, glm::vec3 b)
{
    glm::vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

glm::vec3 div_vec(glm::vec3 a, float scale)
{
    glm::vec3 result = {a.x /= scale, a.y /= scale, a.z /= scale};
    return result;
}

int equ_vec(glm::vec3 a, glm::vec3 b)
{
    return a.x == b.x & a.y == b.y & a.z == b.z;
}

int in_face(glm::vec3 vertex, Polygon face)
{
    for (auto &&x : face)
    {
        if (equ_vec(vertex, x))
        {
            return 1;
        }
    }

    return 0;
}

int collison_faces(Polygon f1, Polygon f2)
{
    /*detect whether two faces share a same vertex*/
    for (int i = 0; i < f1.size(); i++)
    {
        for (int j = 0; j < f2.size(); j++)
        {
            if (equ_vec(f1[i], f2[j]))
            {
                return i + 1;
                //in case of i == 0
            }
        }
    }
    return 0;
}
int next_faces(Polygon f1, Polygon f2)
{
    /* detect whether two faces share a same edge */
    int vertex_number = 2;
    for (int i = 0; i < f1.size(); i++)
    {
        for (int j = 0; j < f2.size(); j++)
        {
            if (equ_vec(f1[i], f2[j]))
            {
                vertex_number--;
                if (vertex_number == 0)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

float dist_vec(glm::vec3 a, glm::vec3 b)
{
    float dist = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
    return dist;
}

glm::vec3 find_center(Polygon face)
{
    glm::vec3 face_point = {0, 0, 0};
    int size = face.size();
    for (int i = 0; i < size; i++)
    {
        face_point = add_vec(face_point, face[i]);
    }
    face_point = div_vec(face_point, size);
    return face_point;
}

float dist_face(Polygon f1, Polygon f2)
{
    /* return the dist of centers of two faces*/
    glm::vec3 center_1 = find_center(f1);
    glm::vec3 center_2 = find_center(f2);
    return dist_vec(center_1, center_2);
}

ADJ find_adj(FACES faces, char f_type)
{
    /* find all adjacent faces of each face */
    /* f_type : e:e_face v:v_face */
    ADJ adj_list;
    for (int i = 0; i < faces.size(); i++)
    {
        vector<int> adjacents;
        adj_list[i] = adjacents;
        // initialize list
    }

    for (int i = 0; i < faces.size() - 1; i++)
    {
        int size = faces[i].size();
        int index = 0;
        for (int j = i + 1; j < faces.size(); j++)
        {
            if (index == size)
            {
                break;
            }

            if (f_type == 'f')
            {
                if (next_faces(faces[i], faces[j]))
                {
                    adj_list[i].push_back(j);
                    index++;
                    //e_face use edge to detect the adjacent faces
                }
            }
            if (f_type == 'e')
            {
                if (collison_faces(faces[i], faces[j]))
                {
                    adj_list[i].push_back(j);
                    adj_list[j].push_back(i);
                    index++;
                    //full adjacent list will be used later
                    //v_face use vertex to detect the adjacent faces
                }
            }
        }
    }
    return adj_list;
}

Polygon new_face(Polygon face)
{
    /* return the F_face of a Polygon */
    Polygon new_face;
    glm::vec3 face_point = find_center(face);
    for (int i = 0; i < face.size(); i++)
    {
        glm::vec3 new_point = div_vec(add_vec(face[i], face_point), 2);
        new_face.push_back(new_point);
    }
    return new_face;
}


Polygon link_f_faces(Polygon f1, Polygon f2)
{
    /* link two f_faces based on distance */
    float min_dist = 20000;
    glm::vec3 pair1[2];
    int p1, p2;
    glm::vec3 pair2[2];
    for (int i = 0; i < f1.size(); i++)
    {
        for (int j = 0; j < f2.size(); j++)
        {

            float dist = dist_vec(f1[i], f2[j]);
            if (dist < min_dist)
            {
                min_dist = dist;
                pair1[0] = f1[i];
                pair1[1] = f2[j];
                p1 = i;
                p2 = j;
            }
        }
    }//第一对
    min_dist = 20000;
    for (int i = 0; i < f1.size(); i++)
    {
        if (i == p1) {continue;}
        for (int j = 0; j < f2.size(); j++)
        {
            if( j == p2) {continue;}
            float dist = dist_vec(f1[i], f2[j]);
            if (dist < min_dist)
            {
                min_dist = dist;
                pair2[0] = f1[i];
                pair2[1] = f2[j];
            }
        }
    } //第一对
    Polygon result{pair1[0], pair1[1], pair2[1], pair2[0]};
    return result;
}
int in_adj(int f1, int f2, ADJ adj_list)
{
    /* return whether f1 in adjacent list of f2 */
    vector<int> adjacent = adj_list[f2];
    vector<int>::iterator iter = find(adjacent.begin(), adjacent.end(), f1);
    return iter != adjacent.end();
}

int same_adj(int f1, int f2, ADJ adj_list)
{
    /* return the index(+1) of the same adjacent face of f1 and f2 */
    for (auto &&i : adj_list[f1])
    {
        if (in_adj(i, f2, adj_list))
        {
            return i + 1;
        }
    }
    return 0;
}

vector<int> find_circle(int f1, int f2, FACES faces, ADJ adj_list)
{
    /* find the circle starting from two faces */
    vector<int> empty;
    int count = 0;
    vector<vector<int>> poly;
    for (auto &&f3 : adj_list[f2]) //adjacent of f2
    {
        if (f3 == f1)
        {
            continue;
        }
        for (auto &&f4 : adj_list[f3]) //adjacent of f3
        {
            if (f4 == f2)
            {
                continue;
            }
            if (in_adj(f1, f4, adj_list))
            {
                vector<int> result = {f1, f2};
                result.push_back(f3);
                result.push_back(f4);
                poly.push_back(result);
                count++;
                break;
            }
        }
    }
    if (count > 1)
    {
        /* 返回小的多边形(v_face) */;
        float min_dist = 20000;
        vector<int> result;
        int cont;
        for (int i = 0; i < poly.size(); i++)
        {
            float dist = 0;
            Polygon new_face;
            for (int j = 0; j < poly[i].size(); j++)
            {
                int cur = poly[i][j];
                int nex = poly[i][(j + 1) % poly[i].size()];
                int col_number = collison_faces(faces[cur], faces[nex]);
                new_face.push_back(faces[cur][col_number - 1]);
            }
            for (int j = 0; j < new_face.size(); j++)
            {
                int n = (j + 1) % new_face.size();
                dist += dist_vec(new_face[j], new_face[n]);
            }
            if (dist < min_dist)
            {
                min_dist = dist;
                result = poly[i];
                cont = i;
            }
        }
        //cout << "min:" <<cont << endl;
        return result;
    }
    else if (count == 1 && same_adj(f1, f2, adj_list))
    {
        return poly[0];
    }
    return empty;
}

FACES link_adj(FACES faces, ADJ adj_list, char f_type)
{
    /* link all the adjacent faces*/
    FACES new_faces;
    int index = 0;
    int count = 0;
    for (int i = 0; i < faces.size(); i++)
    {
        if (f_type == 'f')
        {
            for (auto &&x : adj_list[i])
            {
                Polygon new_face = link_f_faces(faces[i], faces[x]);
                //new e_faces
                new_faces[index] = new_face;
                index++;
            }
        }
        else if (f_type == 'e')
        {
            for (auto x = adj_list[i].begin(); x != adj_list[i].end(); ++x)
            {
                Polygon new_face;

                vector<int>::iterator itr;
                if (!SUBDIVIDED)
                {
                    itr = remove(adj_list[*x].begin(), adj_list[*x].end(), i);
                    if (itr != adj_list[*x].end())
                    {
                        adj_list[*x].erase(itr);
                    }
                }

                vector<int> circle;

                if (!SUBDIVIDED)
                {
                    int triangle = same_adj(i, *x, adj_list);
                    if (triangle)
                    {
                        circle = {i, *x, triangle - 1};
                    }
                }
                else
                {
                    circle = find_circle(i, *x, faces, adj_list);
                }

                if (!circle.empty())
                {
                    for (int j = 0; j < circle.size(); j++)
                    {
                        int cur = circle[j];
                        int nex = circle[(j + 1) % circle.size()];
                        int col_number = collison_faces(faces[cur], faces[nex]);

                        new_face.push_back(faces[cur][col_number - 1]);

                        if (cur != i)
                        {
                            itr = remove(adj_list[cur].begin(), adj_list[cur].end(), nex);
                            if (itr != adj_list[cur].end())
                            {
                                adj_list[cur].erase(itr);
                            }
                            itr = remove(adj_list[nex].begin(), adj_list[nex].end(), cur);
                            if (itr != adj_list[nex].end())
                            {
                                adj_list[nex].erase(itr);
                            }
                            //remove the same face
                        }
                    }
                    new_faces[index] = new_face;
                    index++;
                }
            }
        }
    }
    return new_faces;
}

FACES subdivison(FACES faces, int N)
{
    /* return the new faces of subdivided object */
    if (N < 1)
    {
        return faces;
    }

    ADJ adj_list_f = find_adj(faces, 'f');
    for (int i = 0; i < faces.size(); i++)
    {
        Polygon f_face = new_face(faces[i]);
        faces[i] = f_face; //replace the old face;
    }

    /* link all the new f_faces */
    FACES e_faces = link_adj(faces, adj_list_f, 'f'); //store the created e_faces

    ADJ adj_list_e = find_adj(e_faces, 'e'); // get the adjacent list of e_faces

    FACES v_faces = link_adj(e_faces, adj_list_e, 'e'); //store the created v_faces

    int f_face_number = faces.size();
    for (int i = 0; i < e_faces.size(); i++)
    {
        faces[f_face_number + i] = e_faces[i];
    }
    // append e_faces to faces
    f_face_number = faces.size();

    for (int i = 0; i < v_faces.size(); i++)
    {
        faces[f_face_number + i] = v_faces[i];
    }
    // append V_faces to faces
    SUBDIVIDED = true;
    if (N == 1)
    {
        return faces;
    }
    else
    {
        return subdivison(faces, N - 1);
    }
}