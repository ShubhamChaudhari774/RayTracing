/// Include the image write library and enable it
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/// Uncomment these lines if you want to debug using couts
#include <iostream>
#include <vector>
#include "Vector3.hpp"
#include "Triangle.hpp"
#include "file_util.h"

using namespace std;

int main()
{

  ///Demonstrate file_util
  vector<string> lines = readLines("cube.obj");
  vector<Vector3> vertices;
  vector<Triangle> triangles;

  for(const string& line : lines){
    vector<string> parts = split(line, ' ');
    if(parts.empty()) continue;

    // ADDED: Read vertex coordinates
    if(parts[0] == "v" && parts.size() >= 4){
      vertices.push_back(Vector3{stof(parts[1]), stof(parts[2]), stof(parts[3])});
    }

    // ADDED: Texture coordinates are not needed
    if(parts[0] == "vt"){
      continue;
    }

    // ADDED: Normals are calculated in the ray tracer
    if(parts[0] == "vn"){
      continue;
    }

    // ADDED: Convert OBJ faces into triangles
    if(parts[0] == "f"){
      vector<int> face;

      for(size_t i = 1; i < parts.size(); i++){
        if(parts[i].empty()) continue;

        vector<string> indices = split(parts[i], '/');
        int vertex = stoi(indices[0]);
        int index = vertex > 0 ? vertex - 1 : (int)vertices.size() + vertex;

        if(index < 0 || index >= (int)vertices.size()){
          cerr << "Invalid OBJ vertex index" << endl;
          return 1;
        }

        face.push_back(index);
      }

      for(size_t i = 1; i + 1 < face.size(); i++){
        triangles.push_back(Triangle{
          vertices[face[0]], vertices[face[i]], vertices[face[i + 1]]
        });
      }
    }
  }

  cout << "Loaded " << triangles.size() << " triangles" << endl;
  if(triangles.empty()) return 1;



  /// Example of how to debug with couts.
  /// You must uncomment the include and namespace lines above
  // cout << "Hello, world" << endl;

  Vector3 temp = Vector3{0, 0, 0};
  temp += Vector3{1, 1, 1};

  // cout << temp << endl;

  /// Width of the final image
  const int w = 960;

  /// Height of the final image
  const int h = 960;

  /// The 1d array for storing the pixel we have generated
  unsigned char pixels[w * h * 3];

  // Triangle triangle = Triangle{Vector3{-1, 1, 0}, Vector3{-1, -1, 0}, Vector3{1, -1, 0}};
  // Triangle triangle2 = Triangle{Vector3{1, 1, .5}, Vector3{-1, -1, -.5}, Vector3{1, -1, .5}};

  // vector<Triangle> triangles = {triangle, triangle2};
  // Four triangular sides of the pyramid
 
  
  Vector3 background_color = Vector3{16, 27, 56};

  /// Loop over each line doing down
  for (int y = 0; y < h; y++)
  {
    /// While going down, loop over every pixel going right
    for (int x = 0; x < w; x++)
    {

      /// Figure out our index in the 1d array of pixels
      int index = y * w * 3 + x * 3;

      /// The value of the red channel (0-255)
      unsigned char r = background_color.x;

      /// The value of the green channel (0-255)
      unsigned char g = background_color.y;

      /// The value of the blue channel (0-255)
      unsigned char b = background_color.z;

      // How do I ray trace?
            // How do I ray trace?
      Vector3 camera_origin = Vector3{3, 2, 4}; // CHANGED: Show three sides of the cube
      Vector3 camera_look_at = Vector3{0, 0, 0};
      Vector3 camera_up = Vector3{0, 1, 0};

      // ADDED: Calculate camera direction and axes
      Vector3 forward = (camera_look_at - camera_origin).normalize();
      Vector3 camera_right = forward.cross(camera_up).normalize();
      Vector3 camera_true_up = camera_right.cross(forward).normalize();

      // CHANGED: Zoom in to make the cube larger
      Vector3 target = camera_origin + forward
          + camera_right * (((x + 0.5f) / w * 2.0f - 1.0f) * 0.6f)
          + camera_true_up * ((1.0f - (y + 0.5f) / h * 2.0f) * 0.6f);

      // cout<<"One"<<endl;
      Vector3 temp = (target - camera_origin);
      // cout<<temp<<endl;
      Vector3 ray = temp.normalize();
      // cout << ray << endl;
      // cout<<"Two"<<endl;


      float closest_t = 1000000;

      int triangle_number = 0; // ADDED: Track triangles for different shades
      for (const Triangle &triangle : triangles)
      {
        triangle_number++; // ADDED: Count each triangle
        // Find where we hit the plane
        Vector3 edge_one = triangle.one - triangle.two;
        Vector3 edge_two = triangle.three - triangle.two;
        Vector3 p = edge_two.cross(edge_one);
        Vector3 abc = p.normalize();
        float d = -abc.dot(triangle.one);

        // ADDED: Avoid division by zero for parallel rays
        float denominator = abc.dot(ray);
        if(abs(denominator) < 0.000001f) continue;
        float t = -(abc.dot(camera_origin) + d) / denominator;

        // ADDED: Ignore intersections behind the camera
        if(t <= 0) continue;

        if(t > closest_t)
          continue;
        

        // cout << abc << " " << d << " " << t << endl;

        Vector3 collision_point = ray * t + camera_origin;

        Vector3 minus_one = collision_point - triangle.one;
        Vector3 minus_two = collision_point - triangle.two;
        Vector3 minus_three = collision_point - triangle.three;

        float triangle_area = p.length() / 2;

        float area_one = minus_two.cross(minus_three).length() / 2;
        float area_two = minus_three.cross(minus_one).length() / 2;
        float area_three = minus_one.cross(minus_two).length() / 2;

        float u = area_one / triangle_area;
        float v = area_two / triangle_area;
        float a = area_three / triangle_area;

        float sum = u + v + a;

        float diff = abs(1 - sum);

        if (diff > .00001)
        {
          // I'm not in the triangle
        }
        else
        {
          // I'm in the triangle
          closest_t = t;
          // CHANGED: Three face colors with subtle triangle shading
          Vector3 face_color;

          if(abs(abc.y) > 0.5f)
            face_color = Vector3{255, 166, 64}; // ADDED: Orange

          else if(abs(abc.x) > 0.5f)
            face_color = Vector3{62, 204, 219}; // ADDED: Cyan

          else
            face_color = Vector3{170, 118, 232}; // ADDED: Purple

          // ADDED: Give alternating triangles slightly different shades
          float shade = (triangle_number % 2 == 0) ? 0.92f : 1.0f;

          r = (unsigned char)(face_color.x * shade);
          g = (unsigned char)(face_color.y * shade);
          b = (unsigned char)(face_color.z * shade);
           
        }
      }

      /// Assign the r, g, and b values to the pixel at index
      pixels[index] = r;
      pixels[index + 1] = g;
      pixels[index + 2] = b;
    }
  }

  /// Write the final image as a png
  stbi_write_png("image.png", w, h, 3, pixels, w * 3);
  return 0;
}
