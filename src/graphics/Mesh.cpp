#include "graphics/Mesh.h"


void Mesh::setupMesh() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    LOG_INFO("Mesh::setupMesh: Generated VAO: {}, VBO: {}, EBO: {}", m_VAO, m_VBO, m_EBO);

    glBindVertexArray(m_VAO);
    LOG_INFO("Mesh::setupMesh: Bound VAO: {}", m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    LOG_INFO("Mesh::setupMesh: Bound VBO: {} with size: {}", m_VBO, m_vertices.size() * sizeof(Vertex));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
    LOG_INFO("Mesh::setupMesh: Bound EBO: {} with size: {}", m_EBO, m_indices.size() * sizeof(unsigned int));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
    LOG_INFO("Mesh::setupMesh: Set vertex attribute pointers for position, normal, and texCoords.");
    glBindVertexArray(0); // Unbind VAO
    LOG_INFO("Mesh::setupMesh: Unbound VAO: {}", m_VAO);
}

void Mesh::bind() const {
    glBindVertexArray(m_VAO);
    LOG_INFO("Mesh::bind: Bound VAO: {}", m_VAO);
}

void Mesh::unbind() const {
    glBindVertexArray(0);
    LOG_INFO("Mesh::unbind: Unbound VAO: {}", m_VAO);
}

void Mesh::draw() const {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}