#include "vbo.hpp"

Vertex::Vertex() : pos(0), norm(0), uv(0) {}

Vertex::Vertex(const glm::vec3 & pos, const glm::vec3 & norm, const glm::vec2 & uv)
    : pos(pos), norm(norm), uv(uv) {}

Vertex Vertex::operator+(const Vertex & other) const {
    return Vertex(pos + other.pos, norm + other.norm, uv + other.uv);
}

Vertex Vertex::operator+(const glm::vec3 & offset) const {
    return Vertex(pos + offset, norm, uv);
}

Vertex Vertex::operator-(const Vertex & other) const {
    return Vertex(pos - other.pos, norm - other.norm, uv - other.uv);
}

Vertex & Vertex::operator+=(const Vertex & other) {
    pos += other.pos;
    norm += other.norm;
    uv += other.uv;
    return *this;
}

Vertex & Vertex::operator-=(const Vertex & other) {
    pos -= other.pos;
    norm -= other.norm;
    uv -= other.uv;
    return *this;
}

void draw_array(const std::vector<Vertex> & vertices, GLenum mode) {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          &vertices[0].pos.x);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          &vertices[0].norm.x);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          &vertices[0].uv.x);

    glDrawArrays(mode, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void draw_quad(const glm::vec2 & pos, const glm::vec2 & size) {
    std::vector<Vertex> vertices {
        {{pos.x, pos.y + size.y, 0.0}, {0, 0, 1}, {0, 1}},
        {{pos.x, pos.y, 0.0}, {0, 0, 1}, {0, 0}},
        {{pos.x + size.x, pos.y, 0.0}, {0, 0, 1}, {1, 0}},

        {{pos.x, pos.y + size.y, 0.0}, {0, 0, 1}, {0, 1}},
        {{pos.x + size.x, pos.y, 0.0}, {0, 0, 1}, {1, 0}},
        {{pos.x + size.x, pos.y + size.y, 0.0}, {0, 0, 1}, {1, 1}},
    };

    draw_array(vertices, GL_TRIANGLES);
}

VBO::VBO(VBO::Mode mode, VBO::Usage usage)
    : mode(mode), usage(usage), vbo(0), nPoints(0) {

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)(6 * sizeof(float)));
}

VBO::~VBO() {
    glDeleteBuffers(1, &vbo);
}

GLuint VBO::getVBO() const {
    return vbo;
}

size_t VBO::size() const {
    return nPoints;
}

VBO::Mode VBO::getMode() const {
    return mode;
}

void VBO::setMode(VBO::Mode mode) {
    this->mode = mode;
}

VBO::Usage VBO::getUsage() const {
    return usage;
}

void VBO::setUsage(VBO::Usage usage) {
    this->usage = usage;
}

void VBO::loadFromPoints(const std::vector<Vertex> & points) {
    nPoints = points.size();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nPoints, points.data(), usage);
}

void VBO::draw() const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawArrays(mode, 0, nPoints);
}
