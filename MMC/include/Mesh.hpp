#ifndef MESH_HPP
#define MESH_HPP

#include <zengine/zengine.hpp>
#include <zgraphics2D/zgraphics2D.hpp>

#include <array>
#include <vector>
#include <map>
#include <list>

typedef struct
{
   glm::vec2 pos;
   bool nullDisplacement;
} Node;

typedef struct
{
   int geometryId;
   std::array<int, 3> nodeId;
   bool isSurface;
} Element;

typedef struct
{
   float E; // Young's modulus
   float nu; // Poisson's ratio
   float rho; // Material density
} Material;

class Mesh
{
public:
   inline size_t getNodeCount() const noexcept { return m_nodes.size(); }
   inline Node getNode(size_t index) const { return m_nodes.at(index); }
   inline std::vector<Node> const& getNodes() const noexcept { return m_nodes; }
   inline void setNode(size_t index, Node const& node) { m_nodes.at(index) = node; }

   inline void addElement(Element const& element) { m_elements.push_back(element); }
   inline Element getElement(size_t index) const { return m_elements.at(index); }
   inline std::vector<Element> const& getElements() const noexcept { return m_elements; }
   inline size_t getElementCount() const noexcept { return m_elements.size(); }

   Mesh(size_t node_count);

protected:
   std::vector<Node> m_nodes;
   std::vector<Element> m_elements;
};

// @param n Node count on each side
// @param opposite_length Triangle opposite side length
// @param adjacent_length Triangle adjacent side length
Mesh create_right_triangle_shaped_mesh(size_t n, float opposite_length, float adjacent_length);

// @param reference Reference mesh
// @param g Gravitational acceleration
// @param rho_w Water density (kg/m3)
// @param barrage Barrage material properties
// @param fillingLevel Filling ratio upstream of the dam
Mesh compute_barrage_distortion(Mesh const& reference, float g, float rho_w, Material barrage, float fillingLevel);

#endif // MESH_HPP