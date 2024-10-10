#include "Plot.hpp"

Plot::Plot(glm::vec2 xBounds, glm::vec2 yBounds)
   : m_xBounds(xBounds), m_yBounds(yBounds)
{
   // Assert min < max
   ZE_ASSERT(xBounds.x < xBounds.y, "X Bounds degenerate");
   ZE_ASSERT(yBounds.x < yBounds.y, "Y Bounds degenerate");

   // Set vertex layout
   m_layout.add<glm::vec2>(1); // Pos
   m_layout.add<glm::vec4>(1); // Color
   m_layout.add<glm::vec2>(1); // UVs

   m_vboG.bind();
   m_vaoG.setLayout(m_layout);

   m_vboM.bind();
   m_vaoM.setLayout(m_layout);

   generateGrid();
}

void Plot::generateGrid()
{
   // Set vertical grid lines vertices (brighter if origin axis (x = 0))
   float originHL = 0.15f;

   for (int i = (int) m_xBounds.x; i <= (int) (m_xBounds.x + glm::ceil(glm::length(m_xBounds))); ++i)
   {
      m_grid.push_back(zg::Vertex2D({ (float)i, m_yBounds.x }, { 1.f, 1.f, 1.f, i ? .15f : originHL }));
      m_grid.push_back(zg::Vertex2D({ (float)i, m_yBounds.y }, { 1.f, 1.f, 1.f, i ? .15f : originHL }));
   }

   // Set horizontal grid lines vertices (brighter if origin axis (y = 0))
   for (int i = (int)m_yBounds.x; i <= (int) (m_yBounds.x + glm::ceil(glm::length(m_yBounds))); ++i)
   {
      m_grid.push_back(zg::Vertex2D({ m_xBounds.x, (float)i }, {1.f, 1.f, 1.f, i ? .15f : originHL}));
      m_grid.push_back(zg::Vertex2D({ m_xBounds.y, (float)i }, {1.f, 1.f, 1.f, i ? .15f : originHL}));
   }
}

void Plot::render()
{
   // TODO Inefficient grid rendering technic
   for (int j = 0; j < m_grid.size(); j += 2)
   {
      // Init and render 1 line in the grid
      m_vaoG.bind();

      int dataCount = 8;
      m_vboG.resize(2 * dataCount * sizeof(float));

      {
         float* buffer = m_vboG.map<float>();

         for (size_t i = 0; i < 2; ++i)
         {
            buffer[i * 8 + 0] = reinterpret_cast<glm::vec2 const*>(m_grid.at(j+i).getData(0))->x;
            buffer[i * 8 + 1] = reinterpret_cast<glm::vec2 const*>(m_grid.at(j+i).getData(0))->y;

            buffer[i * 8 + 2] = reinterpret_cast<glm::vec4 const*>(m_grid.at(j + i).getData(1))->r;
            buffer[i * 8 + 3] = reinterpret_cast<glm::vec4 const*>(m_grid.at(j + i).getData(1))->g;
            buffer[i * 8 + 4] = reinterpret_cast<glm::vec4 const*>(m_grid.at(j + i).getData(1))->b;
            buffer[i * 8 + 5] = reinterpret_cast<glm::vec4 const*>(m_grid.at(j + i).getData(1))->a;

            buffer[i * 8 + 6] = reinterpret_cast<glm::vec2 const*>(m_grid.at(j + i).getData(2))->x;
            buffer[i * 8 + 7] = reinterpret_cast<glm::vec2 const*>(m_grid.at(j + i).getData(2))->y;
         }

         m_vboG.unmap();
      }

      glDrawArrays(GL_LINES, 0, 2);
   }

   // Draw mesh if it has been set
   if (m_mesh.size())
   {
      m_vaoM.bind();
      m_eboM.bind();
      glDrawElements(GL_LINES, m_elementCount, GL_UNSIGNED_INT, 0);
   }
}

void Plot::setMesh(Mesh const* mesh)
{
   // Create a vertex for every nodes
   for (Node const& node : mesh->getNodes())
      m_mesh.push_back(zg::Vertex2D(node.pos, zg::Colors::Red));

   m_vaoM.bind();
   int dataCount = 8;
   m_vboM.resize(m_mesh.size() * dataCount * sizeof(float));

   // Set every vertices/nodes location data (pos, color, uv)
   {
      float* buffer = m_vboM.map<float>();

      for (size_t i = 0; i < m_mesh.size(); ++i)
      {
         buffer[i * 8 + 0] = reinterpret_cast<glm::vec2 const*>(m_mesh.at(i).getData(0))->x;
         buffer[i * 8 + 1] = reinterpret_cast<glm::vec2 const*>(m_mesh.at(i).getData(0))->y;

         buffer[i * 8 + 2] = reinterpret_cast<glm::vec4 const*>(m_mesh.at(i).getData(1))->r;
         buffer[i * 8 + 3] = reinterpret_cast<glm::vec4 const*>(m_mesh.at(i).getData(1))->g;
         buffer[i * 8 + 4] = reinterpret_cast<glm::vec4 const*>(m_mesh.at(i).getData(1))->b;
         buffer[i * 8 + 5] = reinterpret_cast<glm::vec4 const*>(m_mesh.at(i).getData(1))->a;

         buffer[i * 8 + 6] = reinterpret_cast<glm::vec2 const*>(m_mesh.at(i).getData(2))->x;
         buffer[i * 8 + 7] = reinterpret_cast<glm::vec2 const*>(m_mesh.at(i).getData(2))->y;
      }

      m_vboM.unmap();
   }

   m_eboM.bind();
   m_eboM.resize(sizeof(unsigned int) * 2 * 3 * mesh->getElementCount());
   m_elementCount = 2 * 3 * (int)mesh->getElementCount();

   // Set EBO for this mesh (link vertices together to render lines)
   {
      unsigned int* buffer = m_eboM.map<unsigned int>();

      for (size_t i = 0; i < mesh->getElementCount(); ++i)
      {
         buffer[i * 6 + 0] = mesh->getElement(i).nodeId[0] - 1;
         buffer[i * 6 + 1] = mesh->getElement(i).nodeId[1] - 1;
         buffer[i * 6 + 2] = mesh->getElement(i).nodeId[1] - 1;
         buffer[i * 6 + 3] = mesh->getElement(i).nodeId[2] - 1;
         buffer[i * 6 + 4] = mesh->getElement(i).nodeId[2] - 1;
         buffer[i * 6 + 5] = mesh->getElement(i).nodeId[0] - 1;
      }

      m_eboM.unmap();
   }
}