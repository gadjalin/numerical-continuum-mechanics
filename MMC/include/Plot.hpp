#ifndef PLOT_HPP
#define PLOT_HPP

#include <zengine/zengine.hpp>
#include <zgraphics2D/zgraphics2D.hpp>

#include "Mesh.hpp"

class Plot
{
public:
   void render();

   void setMesh(Mesh const* mesh);

   Plot(glm::vec2 xBounds, glm::vec2 yBounds);

private:
   void generateGrid();

private:
   glm::vec2 m_xBounds;
   glm::vec2 m_yBounds;

   zg::VertexLayout m_layout;
   zg::VertexBuffer m_vboG;
   zg::VertexArray m_vaoG;

   zg::VertexBuffer m_vboM;
   zg::VertexArray m_vaoM;
   zg::IndexBuffer m_eboM;
   int m_elementCount;

   std::vector<zg::Vertex2D> m_grid;
   std::vector<zg::Vertex2D> m_mesh;
};

#endif // PLOT_HPP