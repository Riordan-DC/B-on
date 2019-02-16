//Vertex Struct

struct Vertex{
    glm::vec3 vert;
    glm::vec3 norm;
    Vertex(glm::vec3 v, glm::vec3 n){
        vert = v;
        norm = n;
    }
};