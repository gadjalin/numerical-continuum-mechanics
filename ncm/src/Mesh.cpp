#include "Mesh.hpp"

#include "alglib/linalg.h"

Mesh::Mesh(size_t nodeCount)
   : m_nodes(nodeCount, { {0.f, 0.f} }) {}

// let n be the number of nodes on the side of the triangle
// so the total number of node in this mesh is n(n + 1)/2
Mesh create_right_triangle_shaped_mesh(size_t n, float opposite_length, float adjacent_length)
{
   // Put a upper limit (just in case)
   ZE_ASSERT(n >= 2 && n < 1000, "Triangle can't have less than 2 vertices on its side");

   size_t node_count = n * (n + 1) / 2;
   Mesh mesh(node_count);

   // Mesh the mesh (divide in finite elements)
   int rowStart = 1;
   float opp_division = opposite_length / (float)(n - 1);
   float adj_division = adjacent_length / (float)(n - 1);
   // Init every node
   for (int y = 0; y < n; ++y)
   {
      for (int x = 0; x < (n - y); ++x)
         mesh.setNode(rowStart + x - 1, { { adj_division * (float)x, opp_division * (float)y }, y ? false : true });

      rowStart += (int)n - y;
   }

   // Create every elementd
   rowStart = (int)n + 1; // Start at row number 2
   int previousRow = 1;
   for (int y = 1; y < n; ++y)
   {
      mesh.addElement({ 1, { previousRow, previousRow + 1, rowStart }, true });
      for (int x = 1; x < (n - y); ++x)
      {
         mesh.addElement({ 2, { previousRow + x, rowStart + x, rowStart + x - 1 }, false });
         mesh.addElement({ 1, { previousRow + x, previousRow + x + 1, rowStart + x }, false });
      }

      previousRow = rowStart;
      rowStart += (int)n - y;
   }

   return mesh;
}

void debugPrintMatrix(alglib::real_2d_array const& m, std::string const& name)
{
   return;

   std::cout << name << std::endl;

   for (int i = 0; i < m.rows(); ++i)
   {
      for (int j = 0; j < m.cols(); ++j)
      {
         std::cout << "\t" << m[i][j];
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;
}

typedef struct
{
   float doubleArea;
   alglib::real_2d_array Ke; // elemental stiffness matrix
} Geometry;

// Here begins a messy algorithm...
Mesh compute_barrage_distortion(Mesh const& reference, float g, float rho_w, Material barrage, float h)
{
   //ZE_ASSERT(fillingLevel > 0.f && fillingLevel < 1.f, "Barrage overflow");

   // Lamé constants
   float mu = barrage.E / (2.f * (1.f + barrage.nu));
   float lamda = barrage.nu * barrage.E / ((1.f + barrage.nu) * (1.f - 2.f * barrage.nu));

   Geometry g1;
   Geometry g2;

   double D_Matrix[] =
   {
      lamda + 2.f * mu,     lamda     ,     0,
           lamda    , lamda + 2.f * mu,     0,
             0      ,       0       ,    mu
   };

   alglib::real_2d_array D;
   D.setcontent(3, 3, D_Matrix);

   debugPrintMatrix(D, "D Matrix");

   // First geometry
   {
      Element const& element = reference.getElement(0);

      // Full algorithm ----------------------------------------------------------------
      glm::vec2 n1 = reference.getNode(element.nodeId[0] - 1).pos;
      glm::vec2 n2 = reference.getNode(element.nodeId[1] - 1).pos;
      glm::vec2 n3 = reference.getNode(element.nodeId[2] - 1).pos;

      double B_Matrix[] =
      {
         n2.y - n3.y,      0     , n3.y - n1.y,      0     , n1.y - n2.y,      0     ,
              0     , n3.x - n2.x,      0     , n1.x - n3.x,      0     , n2.x - n1.x,
         n3.x - n2.x, n2.y - n3.y, n1.x - n3.x, n3.y - n1.y, n2.x - n1.x, n1.y - n2.y
      };

      alglib::real_2d_array B;
      B.setcontent(3, 6, B_Matrix);

      debugPrintMatrix(B, "B Matrix");

      g1.doubleArea = glm::length(glm::cross(glm::vec3(n2 - n1, 0.f), glm::vec3(n3 - n1, 0.f)));
      g1.Ke.setlength(6, 6);

      alglib::real_2d_array Int;
      Int.setlength(3, 6);
      alglib::rmatrixgemm(3, 6, 3, 1.0,
                          D, 0, 0, 0,
                          B, 0, 0, 0,
                          0.0, Int, 0, 0); // Intermediate multiplication : D*B

      alglib::rmatrixgemm(6, 6, 3, 1.0,
                          B, 0, 0, 1,
                          Int, 0, 0, 0,
                          0.0, g1.Ke, 0, 0); // Intermediate multiplication : tB*Int = tB*D*B

      double beta = 1.0 / (2.0 * g1.doubleArea);

      alglib::rmatrixgemm(6, 6, 6, 0.0,
                          g1.Ke, 0, 0, 0,
                          g1.Ke, 0, 0, 0,
                          beta, g1.Ke, 0, 0); // Final multiplication Ke = 1/(2*doubleArea) * tB * D * B
   }

   debugPrintMatrix(g1.Ke, "Geometry 1 Ke Matrix");

   // Second geometry
   if (reference.getElementCount() > 1)
   {
      Element const& element = reference.getElement(1);

      // Full algorithm ----------------------------------------------------------------
      glm::vec2 n1 = reference.getNode(element.nodeId[2] - 1).pos;
      glm::vec2 n2 = reference.getNode(element.nodeId[0] - 1).pos;
      glm::vec2 n3 = reference.getNode(element.nodeId[1] - 1).pos; // Reorder nodes

      double B_Matrix[] =
      {
         n2.y - n3.y,      0     , n3.y - n1.y,      0     , n1.y - n2.y,      0     ,
              0     , n3.x - n2.x,      0     , n1.x - n3.x,      0     , n2.x - n1.x,
         n3.x - n2.x, n2.y - n3.y, n1.x - n3.x, n3.y - n1.y, n2.x - n1.x, n1.y - n2.y
      };

      alglib::real_2d_array B;
      B.setcontent(3, 6, B_Matrix);

      debugPrintMatrix(B, "Geometry 2 B Matrix");

      g2.doubleArea = glm::length(glm::cross(glm::vec3(n2 - n1, 0.f), glm::vec3(n3 - n1, 0.f)));
      g2.Ke.setlength(6, 6);

      alglib::real_2d_array Int;
      Int.setlength(3, 6);
      alglib::rmatrixgemm(3, 6, 3, 1.0,
         D, 0, 0, 0,
         B, 0, 0, 0,
         0.0, Int, 0, 0); // Intermediate multiplication : D*B

      alglib::rmatrixgemm(6, 6, 3, 1.0,
         B, 0, 0, 1,
         Int, 0, 0, 0,
         0.0, g2.Ke, 0, 0); // Intermediate multiplication : tB*Int = tB*D*B

      double beta = 1.0 / (2.0 * g2.doubleArea);

      alglib::rmatrixgemm(6, 6, 6, 0.0,
         g2.Ke, 0, 0, 0,
         g2.Ke, 0, 0, 0,
         beta, g2.Ke, 0, 0); // Final multiplication Ke = 1/(2*doubleArea) * tB * D * B
   
      debugPrintMatrix(g2.Ke, "Geometry 2 Ke Matrix");
   }

   int coordinateCount = (int)reference.getNodeCount() * 2;
   alglib::real_2d_array K; // Global stiffness matrix
   K.setlength(coordinateCount, coordinateCount);

   // Assemble stiffness matrix
   {
      double* K_Matrix = new double[coordinateCount * coordinateCount];
      std::memset(K_Matrix, 0, coordinateCount * coordinateCount * sizeof(double));

      for (Element const& element : reference.getElements())
      {
         Geometry* g = element.geometryId == 1 ? &g1 : &g2;

         for (int i = 0; i < 3; ++i)
         {
            for (int j = 0; j < 3; ++j)
            {
               K_Matrix[((element.nodeId[i] - 1) * 2 + 0) * coordinateCount + (element.nodeId[j] - 1) * 2 + 0] += g->Ke[i * 2 + 0][j * 2 + 0];
               K_Matrix[((element.nodeId[i] - 1) * 2 + 0) * coordinateCount + (element.nodeId[j] - 1) * 2 + 1] += g->Ke[i * 2 + 0][j * 2 + 1];
               K_Matrix[((element.nodeId[i] - 1) * 2 + 1) * coordinateCount + (element.nodeId[j] - 1) * 2 + 0] += g->Ke[i * 2 + 1][j * 2 + 0];
               K_Matrix[((element.nodeId[i] - 1) * 2 + 1) * coordinateCount + (element.nodeId[j] - 1) * 2 + 1] += g->Ke[i * 2 + 1][j * 2 + 1];
            }
         }
      }

      K.setcontent(coordinateCount, coordinateCount, K_Matrix);
      debugPrintMatrix(K, "Global K Matrix");
      delete[] K_Matrix;
   }

   alglib::real_1d_array F;
   F.setlength(coordinateCount);
   double* zeros = new double[coordinateCount];
   std::memset(zeros, 0, coordinateCount * sizeof(double));
   F.setcontent(coordinateCount, zeros);
   
   double thirdS = g1.doubleArea / 6.0;

   // Surface and volume strain
   {
      for (Element const& element : reference.getElements())
      {
         F[(element.nodeId[0] - 1) * 2 + 1] = -barrage.rho * g * thirdS;
         F[(element.nodeId[1] - 1) * 2 + 1] = -barrage.rho * g * thirdS;
         F[(element.nodeId[2] - 1) * 2 + 1] = -barrage.rho * g * thirdS;

         if (element.isSurface)
            F[(element.nodeId[2] - 1) * 2] = rho_w * g * (h - reference.getNode(element.nodeId[2] - 1).pos.y);
      }
   }

   for (int i = 0; i < F.length(); ++i)
      F[i] | tee;

   // Simplification
   {
      int actualNodeCount = (int)reference.getNodeCount();
      std::vector<int> remainingCoordinates;

      for (int i = 0; i < reference.getNodeCount(); ++i)
      {
         if (reference.getNode(i).nullDisplacement)
            actualNodeCount--;
         else
         {
            remainingCoordinates.push_back(i * 2);
            remainingCoordinates.push_back(i * 2 + 1);
         }
      }

      alglib::real_1d_array actualF;
      alglib::real_2d_array actualK;

      actualF.setlength(actualNodeCount * 2);
      actualK.setlength(actualNodeCount * 2, actualNodeCount * 2);

      int actualI = 0;
      for (int i : remainingCoordinates)
      {
         actualF[actualI] = F[i];

         int actualJ = 0;
         for (int j : remainingCoordinates)
         {
            actualK[actualI][actualJ] = K[i][j];
            actualJ++;
         }
         actualI++;
      }

      F = actualF;
      K = actualK;
      coordinateCount = actualNodeCount * 2;

      debugPrintMatrix(K, "Global K Matrix");
   }

   alglib::real_1d_array U; // Displacement vector (the unknown)
   U.setlength(coordinateCount); // 2 coordinates per node (x, y)
   U.setcontent(coordinateCount, zeros);

   delete[] zeros;

   // Compute distortion
   alglib::real_2d_array Kinv(K);
   alglib::ae_int_t info;
   alglib::matinvreport rep;
   alglib::rmatrixinverse(Kinv, info, rep);

   // here goes nothing...
   alglib::rmatrixgemv(coordinateCount, coordinateCount, 1.0,
                       Kinv, 0, 0, 0, 
                       F, 0,
                       0.0, U, 0);

   // Create distorted mesh
   Mesh distortedMesh(reference);

   int actualI = 0;
   for (int i = 0; i < distortedMesh.getNodeCount(); ++i)
   {
      if (distortedMesh.getNode(i).nullDisplacement) continue;

      Node newNode = distortedMesh.getNode(i);
      newNode.pos += glm::vec2( U[actualI * 2], U[actualI * 2 + 1] );
      distortedMesh.setNode(i, newNode);

      actualI++;
   }

   return distortedMesh;
}