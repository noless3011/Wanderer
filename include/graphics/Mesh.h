#pragma once
#include "pch.h"

struct Vertex {
    float position[3]; // x, y, z
    float normal[3];   // nx, ny, nz
    float texCoords[2]; // u, v
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        : m_vertices(std::move(vertices)), m_indices(std::move(indices)) {
        setupMesh();
    }
    ~Mesh() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept
        : m_vertices(std::move(other.m_vertices)), m_indices(std::move(other.m_indices)),
        m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO) {
        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }
    Mesh& operator=(Mesh&& other) noexcept {
        if (this != &other) {
            m_vertices = std::move(other.m_vertices);
            m_indices = std::move(other.m_indices);
            m_VAO = other.m_VAO;
            m_VBO = other.m_VBO;
            m_EBO = other.m_EBO;
            other.m_VAO = 0;
            other.m_VBO = 0;
            other.m_EBO = 0;
        }
        return *this;
    }
    void bind() const;
    void unbind() const;
    void draw() const;

private:
    void setupMesh();
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_VAO, m_VBO, m_EBO;

};