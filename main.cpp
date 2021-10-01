#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <fmt/core.h>

#include <fstream>
#include <string>

#include "Shader.hpp"
#include "context.hpp"
#include "table.hpp"
#include "vbo.hpp"

const std::vector<int> one = {0, 1};
const std::vector<int> two = {2, 3};

void do_render();
void write_buffer(const std::string &, const Table::Ptr &);

int main() {
    Context context(2, 1);
    context.makeCurrent();
    fmt::print("Context created\n");

    auto shader = Shader::fromFragmentPath("../shader.frag");
    if (!shader)
        return 1;

    auto buff1 =
        Table::fromTable("one", one, context.getWidth(), context.getHeight());
    if (!buff1)
        return 2;

    auto buff2 =
        Table::fromTable("two", two, context.getWidth(), context.getHeight());
    if (!buff2)
        return 3;

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->bind();
    shader->setInt("width", context.getWidth());
    shader->setInt("height", context.getHeight());
    buff1->bind(0, shader);
    buff2->bind(1, shader);
    
    do_render();

    auto output = std::make_shared<Table>("output", context.getWidth(),
                                          context.getHeight());
    output->readFromPixels();
    write_buffer("output.csv", output);

    return 0;
}

void do_render() {
    VBO vbo;
    vbo.loadFromPoints({
        Vertex({1, -1, 0}, {0, 0, 0}, {1, 0}),
        Vertex({-1, -1, 0}, {0, 0, 0}, {0, 0}),
        Vertex({-1, 1, 0}, {0, 0, 0}, {0, 1}),

        Vertex({1, -1, 0}, {0, 0, 0}, {1, 0}),
        Vertex({-1, 1, 0}, {0, 0, 0}, {0, 1}),
        Vertex({1, 1, 0}, {0, 0, 0}, {1, 1}),
    });

    vbo.draw();
}

int back_to_int(const std::vector<uint8_t> & buff, size_t index) {
    int res = 0;
    for (int i = 0; i < 4; i++) {
        res = (res << 8) | buff[index * 4 + i];
    }
    return res;
}

void write_buffer(const std::string & filename, const Table::Ptr & table) {
    std::ofstream os(filename);

    for (int r = 0; r < table->getHeight(); r++) {
        for (int c = 0; c < table->getWidth(); c++) {
            if (c > 0) {
                os << ", ";
            }
            os << table->getCell(r, c);
        }
        os << std::endl;
    }
    os.close();
}