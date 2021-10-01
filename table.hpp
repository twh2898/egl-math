#pragma once

#include <GLES2/gl2.h>

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Shader.hpp"

class Table {
    GLuint texId;
    std::string name;
    std::vector<int> table;
    int width, height;

    int index(int row, int col) {
        return row * width + col;
    }

public:
    typedef std::shared_ptr<Table> Ptr;

    Table(const std::string_view & name)
        : table(), name(name), width(0), height(0) {
        glGenTextures(1, &texId);
    }
    ~Table() {
        glDeleteTextures(1, &texId);
    }

    GLuint getTexId() {
        return texId;
    }

    const int * data() {
        return table.data();
    }

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }

    void setCell(int val, int row, int col) {
        table[index(row, col)] = val;
    }

    int getCell(int row, int col) {
        return table[index(row, col)];
    }

    void loadTable(const std::vector<int> & table, int width, int height) {
        this->table = table;
        this->width = width;
        this->height = height;
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, table.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    void bind(int index, const Shader::Ptr & shader) {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, texId);
        shader->setInt(name, index);
    }

    static Table::Ptr fromTable(const std::string_view & name,
                                const std::vector<int> & table,
                                int width,
                                int height) {
        auto buff = std::make_shared<Table>(name);
        if (buff) {
            buff->loadTable(table, width, height);
        }
        return buff;
    }
};
