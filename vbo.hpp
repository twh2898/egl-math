#pragma once

#include <GLES2/gl2.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

/**
 * A single point in the format accepted by VBO, Mesh and Model.
 */
struct Vertex {
    /// The position
    glm::vec3 pos;
    /// The normal
    glm::vec3 norm;
    /// The UV texture coordinate
    glm::vec2 uv;

    /**
     * Create a new vertex with all fields set to 0.
     */
    Vertex();

    /**
     * Create a new Vertex and set it fields to pos, norm and uv.
     *
     * @param pos the position
     * @param norm the normal
     * @param uv the texture coordinate
     */
    Vertex(const glm::vec3 & pos, const glm::vec3 & norm, const glm::vec2 & uv);

    /// Default copy constructor
    Vertex(const Vertex & other) = default;

    /// Default move constructor
    Vertex(Vertex && other) = default;

    /// Default copy assign operator
    Vertex & operator=(const Vertex & other) = default;

    /// Default move assign operator
    Vertex & operator=(Vertex && other) = default;

    /**
     * Add the fields element wise of two Vertex objects.
     *
     * @param other the Vertex to add to
     *
     * @return a new Vertex with the sum of both objects.
     */
    Vertex operator+(const Vertex & other) const;

    /**
     * Add offset to just the position field.
     *
     * @param offset the offset to add to pos
     *
     * @return a new Vertex with the sum of pos and offset
     */
    Vertex operator+(const glm::vec3 & offset) const;

    /**
     * Subtract the fields element wise of two Vertex objects.
     *
     * @param other the Vertex to subtract from this
     *
     * @return a new Vertex with the difference of both objects.
     */
    Vertex operator-(const Vertex & other) const;

    /**
     * Add the fields element wise of other to the fields of this.
     *
     * @param other the Vertex to add to this
     *
     * @return a reference to this
     */
    Vertex & operator+=(const Vertex & other);

    /**
     * Subtract the fields element wise of other from the fields of this.
     *
     * @param other the Vertex to subtract from this
     *
     * @return a reference to this
     */
    Vertex & operator-=(const Vertex & other);
};

void draw_array(const std::vector<Vertex> & vertices, GLenum mode);

void draw_quad(const glm::vec2 & pos, const glm::vec2 & size);

/**
 * Manages a single vertex buffered object and vertex array object.
 */
class VBO {
public:
    /**
     * OpenGL Draw mode.
     */
    enum Mode {
        Lines = GL_LINES,
        LineStrip = GL_LINE_STRIP,
        LineLoop = GL_LINE_LOOP,

        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan = GL_TRIANGLE_FAN,
    };

    /**
     * OpenGL buffer usage.
     */
    enum Usage {
        /// Data will be buffered once and used multiple times
        Static = GL_STATIC_DRAW,
        /// Data will be buffered and used a few times
        Stream = GL_STREAM_DRAW,
        /// Data will be buffered and used once
        Dynamic = GL_DYNAMIC_DRAW,
    };

private:
    GLuint vbo;
    Mode mode;
    Usage usage;
    size_t nPoints;

public:
    using Ptr = std::shared_ptr<VBO>;
    using ConstPtr = std::shared_ptr<const VBO>;

    /**
     * Create a new VBO with mode and usage.
     *
     * @param mode the OpenGL draw mode
     * @param usage the OpenGL buffer usage
     */
    VBO(Mode mode = Mode::Triangles, Usage usage = Usage::Static);

    /**
     * Free OpenGL buffers.
     */
    virtual ~VBO();

    /**
     * Get the OpenGL vbo id.
     *
     * @return the vbo id
     */
    GLuint getVBO() const;

    /**
     * Get the number of points.
     *
     * @return the number of points
     */
    size_t size() const;

    /**
     * Get the OpenGL draw mode.
     *
     * @return the VBO::Mode
     */
    Mode getMode() const;

    /**
     * Set the OpenGL draw mode.
     *
     * @param mode the new mode
     */
    void setMode(Mode mode);

    /**
     * Get the OpenGL buffer usage.
     *
     * @return the VBO::Usage
     */
    Usage getUsage() const;

    /**
     * Set the OpenGL buffer usage.
     *
     * @param usage the new usage
     */
    void setUsage(Usage usage);

    /**
     * Load buffer with data from points.
     *
     * @param points the data to send to the buffer
     */
    void loadFromPoints(const std::vector<Vertex> & points);

    /**
     * Load buffer with data from points.
     *
     * @param points the data to send to the buffer
     * @param n the numebr of points
     */
    void loadFromPoints(const Vertex * points, size_t n);

    /**
     * Bind the vao, then draw, then unbind the vao.
     */
    void draw() const;
};