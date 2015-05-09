#include "diagram.h"
#include <math.h>
#include <iostream>

#define PI 3.14159265358979323846
#define TAU (2 * PI)

Diagram::Diagram() {
    m_p = 2;
    m_q = 1;
    m_r = -1;
    m_s = -1;

    m_tess = false;
    m_displayEdges = true;
    m_allWhite = false;

    m_scale = 220;
    m_center = sf::Vector3f(300, 250, 0);
}

Diagram::Diagram(sf::RenderWindow* window, int centerX, int centerY, std::string str) :
    m_center(centerX, centerY, 0),
    m_shape(sf::Lines, 0)
{
    m_w = window;
    SetPQR(str);
    m_scale = 200;

    m_tess = false;
    m_displayEdges = true;
    m_allWhite = false;
};

bool Diagram::IsGood(sf::Vertex vert1, sf::Vertex vert2) {
    if(m_shape.getVertexCount() <= 1) // An empty set, or only one point, doesn't have any lines for us to overlap with
        return true;
        for(int iii = 0; iii < m_shape.getVertexCount() - 1; iii += 2) { // Check all lines in m_shape
        // Returns true if the two vertices appear in sequence anywhere in m_shape (not necessarily in order)
        if((m_shape[iii].position == vert1.position && m_shape[iii + 1].position == vert2.position) ||
           (m_shape[iii].position == vert2.position && m_shape[iii + 1].position == vert1.position))
        {
            return false;
        }
    }
    return true;
}

bool Diagram::IsGood(sf::Vector3f vect1, sf::Vector3f vect2) {
    for(int iii = 0; iii < m_vertices.size(); iii  += 2) {
        if((int)(m_vertices[iii].x + .5) == (int)(vect1.x + .5) && (int)(m_vertices[iii].y + .5) ==
           (int)(vect1.y + .5) && (int)(m_vertices[iii].z + .5) == (int)(vect1.z + .5) &&
           (int)(m_vertices[iii  + 1].x + .5) == (int)(vect2.x + .5) && (int)(m_vertices[iii  + 1].y + .5) ==
           (int)(vect2.y + .5) && (int)(m_vertices[iii  + 1].z + .5) == (int)(vect2.z + .5) ||
           (int)(m_vertices[iii].x + .5) == (int)(vect2.x + .5) && (int)(m_vertices[iii].y + .5) ==
           (int)(vect2.y + .5) && (int)(m_vertices[iii].z + .5) == (int)(vect2.z + .5) &&
           (int)(m_vertices[iii  + 1].x + .5) == (int)(vect1.x + .5) && (int)(m_vertices[iii  + 1].y + .5) ==
           (int)(vect1.y + .5) && (int)(m_vertices[iii  + 1].z + .5) == (int)(vect1.z + .5))
            return false;
    }
    return true;
}

bool Diagram::IsGood(std::vector<sf::Vector3f> face) {
    // Compare the coordinates of the midpoints of each existing face and the input to see if the input already exists
    for(int iii = 0; iii < m_faces.size(); iii++) {
        double avgX1 = 0, avgX2 = 0,
            avgY1 = 0, avgY2 = 0,
            avgZ1 = 0, avgZ2 = 0;
        for(int jjj = 0; jjj < m_faces[iii].size(); jjj++) {
            avgX1 += m_faces[iii][jjj].x;
            avgX2 += face[jjj].x;
            avgY1 += m_faces[iii][jjj].y;
            avgY2 += face[jjj].y;
            avgZ1 += m_faces[iii][jjj].z;
            avgZ2 += face[jjj].z;
        }
        avgX1 /= face.size();
        avgX2 /= face.size();
        avgY1 /= face.size();
        avgY2 /= face.size();
        avgZ1 /= face.size();
        avgZ2 /= face.size();
        if((int)(avgX1 + .5) == (int)(avgX2 + .5) &&
           (int)(avgY1 + .5) == (int)(avgY2 + .5) &&
           (int)(avgZ1 + .5) == (int)(avgZ2 + .5))
            return false;
    }
    return true;
}

bool Diagram::IsGood(sf::Color c) {
    return true;
}

void Diagram::SetPQR(std::string str) {
    m_p = 2; // Empty schlafli set = line segment
    m_q = 1;
    m_r = -1;
    m_s = 1;

    if(str.find('{') != std::string::npos) {  // If '{' is present (which it should be)
        str = str.substr(str.find('{') + 1, str.length() - 1); // Edit the string to remove it
    }
    if(str.find('}') != std::string::npos) {  // If '}' is present (which it also should be)
        str = str.substr(0, str.find('}')); // Edit the string to remove it
    }

    if(str == "") return;

    std::string pqStr = str.substr(0, str.find(',')); // Look only up until the first comma, if there is one
    if(pqStr.find('/') != std::string::npos) { // If there's a slash, we need to set both p and q
        m_p = ToInt(pqStr.substr(0, pqStr.find('/')));
        m_q = ToInt(pqStr.substr(pqStr.find('/') + 1, std::string::npos));
    } else { // Otherwise, we can just set p
            m_p = ToInt(pqStr); // No commas, slashes or braces, so the string is just p
    }
    if(str.find(',') != std::string::npos) { // There's a comma, so we'll have to at least set r as well
        str = str.substr(str.find(',') + 1, std::string::npos); // Look only past the first comma
        if(str.find('/') != std::string::npos) { // There's a slash in the second part
            m_r = ToInt(str.substr(0, str.find('/')));
            m_s = ToInt(str.substr(str.find('/') + 1, std::string::npos));
        } else { // No slash present
            m_r = ToInt(str);
        }
    }

    while(m_q > m_p / 2) m_q = abs(m_p - m_q); // Put q in (1, p/2) for convenience

    //(p - 2) * 180 / p is the interior angle of the regular shape
    if(m_r * ((m_p - 2) * PI / m_p) == TAU) {
        m_tess = true; // This is a tessellation of a plane
    } else {
        m_tess = false; // We fold up, so this is a 3D solid (if it's valid)
    }
}

double Diagram::GetAngle() {
    double thetaFace = (m_p - 2 * m_q) * PI / m_p; // Interior angle of the polygon forming the face
    double thetaVertex = TAU / m_r; // Interior angle of the vertex figure
    if(m_s <= 1) { // The second number doesn't have a slash
        return asin(sqrt((1 - cos(thetaFace)) / (1 - cos(thetaVertex))));
    } else {
        return asin(sqrt((1 - cos(thetaFace)) / 2) / sin(thetaVertex));
    }
}

void Diagram::OrderFaces() {
    // Create a vector for the z position of the midpoint of each face
    std::vector<double> zPos = std::vector<double>(m_faces.size());
    for(int iii = 0; iii < zPos.size(); iii++) { // Go through each face
            double avgZ = 0; // The average position; start at 0
        for(int jjj = 0; jjj < m_faces[iii].size(); jjj++) {
            avgZ += m_faces[iii][jjj].z; // Sum the z coordinates of each vertex
        }
        zPos[iii] = avgZ / m_faces[iii].size(); // Divide by the number of vertices to get an average z
    }
    std::vector<double> orderedZPos = zPos; // A new vector, which is just zPos in ascending order
    std::sort(orderedZPos.begin(), orderedZPos.end()); // Put our new vector in ascending order
    // A new vector which will be our faces in ascending ourder
    std::vector<std::vector<sf::Vector3f> > orderedFaces = std::vector<std::vector<sf::Vector3f> >(0);
    // A new vector which will be our corresponding reordered colors
    std::vector<sf::Color> newColors = std::vector<sf::Color>(0);

    // Apply the same transformation to our faces that we did to zPos to order it
    int prevLoc = 0; // In case we have multiple faces on the same z-level
    for(int iii = 0; iii < m_faces.size(); iii++) {
        int loc = 0; // The original index of the iiith element in orderedZPos
        for(int jjj = prevLoc + 1; jjj < orderedZPos.size(); jjj++) { // If we already found one element, start past it
            if(zPos[jjj] == orderedZPos[iii]) { // If we have a match
                loc = jjj; // Set loc to the index of the match

                // If we're looking for the same number again next time
                if(jjj + 1 < orderedZPos.size() && orderedZPos[jjj + 1] == orderedZPos[jjj])
                    prevLoc = loc; // Set prevloc to where we stopped so that we don't justs find the same index again
                else
                    prevLoc = 0; // Otherwise, start at the beginning again
                break; // Once we've found our location, we're done
            }
        }
        orderedFaces.push_back(m_faces[loc]); // Add the determined face to the end of our ordered list of faces
        // Apply the same transformation to colors, so that the faces don't change color as the change z position
        newColors.push_back(m_colors[loc]);
    }
    m_faces = orderedFaces; // Set our ordered lists to be our new stored lists
    m_colors = newColors;
}

void Diagram::MakePoly(std::string str) {
    if(str != "")
        SetPQR(str);
    if(m_r == -1) { // 2D Polygon
        MakePolygon();
    } else if(m_tess){ // 2D tessellation
        Tessellate();
    } else { // 3D Polyhedron
        MakePolyhedron();
    }
}

void Diagram::MakePolygon() {
    m_scale = (m_w->getSize().y * .99) / 2.2; // Scale to window size
    m_shape = sf::VertexArray(sf::Lines, 2 * m_p);
    // The initial angle (for the first vertex)
    double angle = PI / 2 + (PI / m_p * ((m_p + 1) % 2)); // Ensure there is a flat line on the bottom
    // Calculate the location of each vertex
    int gcf = GreatestCommonFactor(m_p, m_q);
    for(int iii = 0, jjj = 0; iii < m_shape.getVertexCount() - 1; iii += 2, jjj++) {
        // Adds the vertices to the diagram
        m_shape[iii].position = sf::Vector2f(m_center.x + cos(angle) * m_scale,
                                             m_center.y - sin(angle) * m_scale);
        angle += m_q * (TAU / m_p);
        m_shape[iii + 1].position = sf::Vector2f(m_center.x + cos(angle) * m_scale,
                                                 m_center.y - sin(angle) * m_scale);
        angle -= (m_q - 1) * (TAU / m_p);
        if(!m_allWhite) {
            m_shape[iii].color = Colorgen(iii);
            m_shape[iii + 1].color = Colorgen(iii + 1);
        } else {
            m_shape[iii].color = HSVtoRGB((jjj % gcf) * 100, 1, 1);
            m_shape[iii + 1].color = HSVtoRGB((jjj % gcf) * 100, 1, 1);
        }
    }
}

void Diagram::Tessellate() {
    m_scale = 50; // smaller tessellations
    m_shape = sf::VertexArray(sf::Lines, 1);
    double delta = (m_p - 2) * PI / m_p; // The amount by which we rotate for the tessellation
    m_shape[0] = sf::Vertex(sf::Vector2f(m_center.x, m_center.y)); // Create our starting vertex at the center
    m_shape[0].color = Colorgen(639); // Set starting color
    for(int iii = 0; iii < m_shape.getVertexCount(); iii += 2) { // Go until all vertices are off screen
        int x = m_shape[iii].position.x, y = m_shape[iii].position.y; // For ease of use
        if(0 < x && x < m_w->getSize().x && 0 < y && y < m_w->getSize().y) { // False if the vertex is out of the window
            m_shape[m_shape.getVertexCount() - 1] = m_shape[iii]; // Set the last vertex to the current center one
            double initAngle = 0;
            if(iii != 0) { // First run doesn't have any lines to work from, so just start with 0
                // Set our initial angle to be that of the line we came on, so that we draw lines relative to that
                initAngle += atan(-(m_shape[iii].position.y - m_shape[iii - 1].position.y) /
                                   (m_shape[iii].position.x - m_shape[iii - 1].position.x));
                if (x >= m_shape[iii - 1].position.x) initAngle += PI; // Compensate for the restricted range of atan
            }
            for(double angle = initAngle; angle < TAU + initAngle; angle += delta) {
                // Create the next vertex
                sf::Vertex vert = sf::Vertex(sf::Vector2f(m_shape[iii].position.x + m_scale * cos(angle),
                                                          m_shape[iii].position.y - m_scale * sin(angle)));
                vert.color = Colorgen(m_shape.getVertexCount() - 1); // Set the color
                if(IsGood(vert, m_shape[m_shape.getVertexCount() - 1])) { // Don't draw over existing lines
                    m_shape.append(vert); // Add the vertex (completes a line with the center vertex)
                    m_shape.append(m_shape[iii]); // Add the center vertex again to begin more lines
                }
            }
        }
        if(iii == 0) iii--; // Want to only deal with odd-numbered vertices after first run
    }
    m_shape.resize(m_shape.getVertexCount() - 1); // Remove the last vertex, since we have one too many
}

void Diagram::MakePolyhedron() {
    m_scale = 200;
    double theta = GetAngle();  // Calculate our deflection angle
    double length = 2 * m_scale * cos(theta); // How long each side is
    m_vertices = std::vector<sf::Vector3f>(2);
    m_vertices[0] = sf::Vector3f(m_center.x + m_scale, m_center.y, m_center.z); // Starting with a point on the sphere
    m_vertices[1] = sf::Vector3f(m_vertices[0].x - length * cos(theta),    // First edge to be created
                                 m_vertices[0].y + length * sin(theta),
                                 m_vertices[0].z);

    for(int iii = 1; iii < m_vertices.size() && iii < 1000; iii += 2) { // Main loop for creating the other edges
        for(double phi = TAU / m_r * m_s; phi < TAU * m_s; phi += TAU / m_r * m_s) {
            // Rotate the starting vertex of the line by phi around the line connecting the center and the ending vertex
            sf::Vector3f nextVert = RotatePointAboutLine(m_vertices[iii - 1], phi, m_center, m_vertices[iii]);
            if(fabs(nextVert.x) < .0001) nextVert.x = 0; // Deal with rounding error a little bit around the axes
            if(fabs(nextVert.y) < .0001) nextVert.y = 0;
            if(fabs(nextVert.z) < .0001) nextVert.z = 0;
            if(IsGood(m_vertices[iii], nextVert)) {     // If the edge isn't already present
                m_vertices.push_back(m_vertices[iii]);  // Add the starting and ending points
                m_vertices.push_back(nextVert);
            }
        }
    }

    if(m_displayEdges) { // If we're only concerned with the edges
        // Put the vertices into a 2D vertex array for drawing
        m_shape = sf::VertexArray(sf::Lines, 0);
        for(int iii = 0; iii < m_vertices.size(); iii++) {
            m_shape.append(sf::Vertex(sf::Vector2f(m_vertices[iii].x,   // Just drop the z coordinate for 2D
                                                   m_vertices[iii].y)));
            m_shape[iii].color = Colorgen(iii);
        }
    } else { // We want to display all the faces of the polyhedron
        // This is our set of faces (themselves a collection of points), or a collection of a collection of points
        m_faces = std::vector< std::vector<sf::Vector3f> >(0);
        // Go through each edge and draw the two faces that border it
        for(int iii = 1; iii < m_vertices.size(); iii += 2) { // Our vertices are in edge pairs, so go by 2s
            std::vector<sf::Vector3f> face = std::vector<sf::Vector3f>(m_p); // One of the two faces
            std::vector<sf::Vector3f> oppFace = std::vector<sf::Vector3f>(m_p); // The other face

            face[0] = m_vertices[iii - 1]; // Add the edge's vertices to both faces, since we know they're on it
            face[1] = m_vertices[iii];
            oppFace[0] = m_vertices[iii - 1];
            oppFace[1] = m_vertices[iii];

            for(int jjj = 2; jjj < face.size(); jjj++) {
                face[jjj] = RotatePointAboutLine(face[jjj - 2], TAU / m_r * m_s,
                                                 m_center, face[jjj - 1]);
                oppFace[jjj] = RotatePointAboutLine(oppFace[jjj - 2], -TAU / m_r * m_s,
                                                 m_center, oppFace[jjj - 1]);
            }
            if(IsGood(face)) { // If the face doesn't already exist, add it to our vector
                m_faces.push_back(face);
                m_colors.push_back(Colorgen(iii)); // Also add the corresponding color to the color vector
            }
            if(IsGood(oppFace)) { // Do the same as above for the other face
                m_faces.push_back(oppFace);
                m_colors.push_back(Colorgen(iii + 1));
            }
        }
        if(m_r == 5 && m_s == 2) { // Special method for Great Dodecahedron -- split each pentagonal face into triangles
            m_colors.clear();
            std::vector<std::vector<sf::Vector3f> > triangleFaces = std::vector<std::vector<sf::Vector3f> >(0);
            for(int iii = 0; iii < m_faces.size(); iii++) {
                for(int jjj = 0; jjj < m_faces[iii].size(); jjj++) {
                    std::vector<sf::Vector3f> triFace = std::vector<sf::Vector3f>(0);
                    triFace.push_back(m_faces[iii][jjj % m_faces[iii].size()]);
                    triFace.push_back(m_faces[iii][(jjj + 1) % m_faces[iii].size()]);
                    triFace.push_back(m_faces[iii][(jjj + 2) % m_faces[iii].size()]);

                    triangleFaces.push_back(triFace);
                    m_colors.push_back(Colorgen(rand()));
                }
            }
                m_faces = triangleFaces;
        }
        m_vertices.clear();
        RotateSolid(0, 0, 0, true);
    }
}

void Diagram::RotateSolid(int xDir, int yDir, int zDir, bool autoRotate, double rotateSpeed) {
    // If we're not rotating, or we don't have any faces or vertices to rotate, just finish now
    if(xDir == 0 && yDir == 0 && zDir == 0 && !autoRotate ||
       (m_r == -1 || m_tess) || (m_faces.size() < 2 && m_vertices.size() < 2)) return;
    m_rotateSpeed = rotateSpeed; // Set our new speed
    if(autoRotate) { // If we're set to rotate automatically, do so around this arbitrary axis
        xDir = 10;
        yDir = -8;
        zDir = 10;
    }

    if(m_displayEdges) { // If we're displaying just the edges, not the faces
        for(int iii = 0; iii < m_vertices.size(); iii++) { // Rotate each point around our axis
            m_vertices[iii] = RotatePointAboutLine(m_vertices[iii], PI / m_rotateSpeed,
                                               m_center, m_center + sf::Vector3f(xDir, yDir, zDir));
        }
        m_shape = sf::VertexArray(sf::Lines, 0); // Clear out m_shape so we can add our newly rotates points instead
        for(int iii = 0; iii < m_vertices.size(); iii++) {
            sf::Vertex vert = sf::Vertex(sf::Vector2f(m_vertices[iii].x,    // Create our vertex
                                                      m_vertices[iii].y));
            vert.color = Colorgen(iii); // Set the color pseudo-randomly
            m_shape.append(vert); // Add the vertex to m_shape
        }
    } else {
        for(int iii = 0; iii < m_faces.size(); iii++) { // Rotate each vertex around our axis
            for(int jjj = 0; jjj < m_faces[iii].size(); jjj++) {
                m_faces[iii][jjj] = RotatePointAboutLine(m_faces[iii][jjj], PI / m_rotateSpeed,
                                                     m_center, m_center + sf::Vector3f(xDir, yDir, zDir));
            }
        }

        OrderFaces(); // Rearrange faces so that we draw the ones furthest from the camera first
        if(m_faces[0].size() > 3) // Our faces aren't triangles, so we want to connect 4 vertices at a time
            m_shape = sf::VertexArray(sf::Quads, 0);
        else                      // Our faces are triangles, so we can just connect the vertices and be done with it
            m_shape = sf::VertexArray(sf::Triangles, 0);
        for(int iii = 0; iii < m_faces.size(); iii++) { // Go through each face
            for(int jjj = 0; jjj < m_faces[iii].size(); jjj += 2) { // Go through the vertices
                // Create a vertex for each set of three/four consecutive vertices
                sf::Vertex firstVert = sf::Vertex(sf::Vector2f(m_faces[iii][m_q * jjj % m_p].x,
                                                               m_faces[iii][m_q * jjj % m_p].y));
                sf::Vertex secondVert = sf::Vertex(sf::Vector2f(m_faces[iii][m_q * (jjj + 1) % m_p].x,
                                                                m_faces[iii][m_q * (jjj + 1) % m_p].y));
                sf::Vertex thirdVert = sf::Vertex(sf::Vector2f(m_faces[iii][m_q * (jjj + 2) % m_p].x,
                                                               m_faces[iii][m_q * (jjj + 2) % m_p].y));
                sf::Vertex fourthVert = sf::Vertex(sf::Vector2f(m_faces[iii][m_q * (jjj + 3) % m_p].x,
                                                                m_faces[iii][m_q * (jjj + 3) % m_p].y));

                // Set all vertices to the same color so the face has a uniform color
                firstVert.color = m_colors[iii];
                secondVert.color = m_colors[iii];
                thirdVert.color = m_colors[iii];
                fourthVert.color = m_colors[iii];

                // Add the vertices to be drawn
                m_shape.append(firstVert);
                m_shape.append(secondVert);
                m_shape.append(thirdVert);
                if(m_faces[iii].size() > 3) // Don't add the fourth vertex if we're using triangles
                    m_shape.append(fourthVert);
                else
                    break; // We only had three vertices on the face, so we're done after one iteration
            }
        }
    }
}

void Diagram::ToggleEdges() {
    m_displayEdges = !m_displayEdges; // Invert our current display setting
    MakePoly(); // Remake our polyhedron with the new setting
}

void Diagram::ToggleColors() {
    m_allWhite = !m_allWhite;
    MakePoly();
}

// Generate the colors for the lines so it's not all white and boring
sf::Color Diagram::Colorgen(int seed) {
    if(m_allWhite && (m_r <= 1 || m_tess || m_displayEdges))
        return sf::Color::White;
    srand(109732421*seed);
    int hue = (int)((seed * 42 * 23657) * 15274) % 360;
    return HSVtoRGB(hue, 1, 1);
}

void Diagram::Draw() {
    m_w->draw(m_shape);
}

sf::Vector3f Diagram::RotatePointAboutLine(sf::Vector3f p, double theta, sf::Vector3f p1, sf::Vector3f p2) {
    // Note: This code modified from http://paulbourke.net/geometry/rotate/example.c
    sf::Vector3f u,q1,q2;
    double d = 0;

    /* Step 1 */
    q1.x = p.x - p1.x;
    q1.y = p.y - p1.y;
    q1.z = p.z - p1.z;

    u.x = p2.x - p1.x;
    u.y = p2.y - p1.y;
    u.z = p2.z - p1.z;

    double magnitude = sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
    u.x /= magnitude;
    u.y /= magnitude;
    u.z /= magnitude;
    d = sqrt(u.y * u.y + u.z * u.z);

    /* Step 2 */
    if (d != 0) {
        q2.x = q1.x;
        q2.y = q1.y * u.z / d - q1.z * u.y / d;
        q2.z = q1.y * u.y / d + q1.z * u.z / d;
    } else {
        q2 = q1;
    }

    /* Step 3 */
    q1.x = q2.x * d - q2.z * u.x;
    q1.y = q2.y;
    q1.z = q2.x * u.x + q2.z * d;

    /* Step 4 */
    q2.x = q1.x * cos(theta) - q1.y * sin(theta);
    q2.y = q1.x * sin(theta) + q1.y * cos(theta);
    q2.z = q1.z;

    /* Inverse of step 3 */
    q1.x =   q2.x * d + q2.z * u.x;
    q1.y =   q2.y;
    q1.z = - q2.x * u.x + q2.z * d;

    /* Inverse of step 2 */
    if (d != 0) {
        q2.x =   q1.x;
        q2.y =   q1.y * u.z / d + q1.z * u.y / d;
        q2.z = - q1.y * u.y / d + q1.z * u.z / d;
    } else {
        q2 = q1;
    }

   /* Inverse of step 1 */
    q1.x = q2.x + p1.x;
    q1.y = q2.y + p1.y;
    q1.z = q2.z + p1.z;
    return q1;
}

// Converts a string with a number in it to an integer containing that number
int ToInt(std::string str) {
    int ret = 0;
    for(int i = str.length() - 1; i >= 0; i--) {
        ret += pow(10, str.length() - 1 - i) * (str[i] - '0');
    }
    return ret;
}

// Returns the greatest common factor of the inputs
int GreatestCommonFactor(int a, int b) {
    int c = 0;
    while(b != 0) {
        c = a% b;
        a = b;
        b = c;
    }
    return a;
}

// Convert HSV color values to RGB color values
sf::Color HSVtoRGB(int hue, double sat, double val) {
    hue %= 360;
    while(hue < 0) hue += 360;

    if(sat < 0) sat = 0.0;
    if(sat > 1) sat = 1.0;

    if(val < 0) val = 0.0;
    if(val > 1) val = 1.0;

    int h = hue / 60;
    double f = double(hue) / 60 - h;
    double p = val * (1 - sat);
    double q = val * (1 - sat * f);
    double t = val * (1 - sat * (1 - f));

    switch(h)
    {
        default:
        case 0:
        case 6: return sf::Color(val * 255, t * 255, p * 255);
        case 1: return sf::Color(q * 255, val * 255, p * 255);
        case 2: return sf::Color(p * 255, val * 255, t * 255);
        case 3: return sf::Color(p * 255, q * 255, val * 255);
        case 4: return sf::Color(t * 255, p * 255, val * 255);
        case 5: return sf::Color(val * 255, p * 255, q * 255);
    }
}
