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
  vector<string> lines = readLines("simple.obj");
  for(const string& line : lines){
    cout << line << endl;
    vector<string> parts = split(line, ' ');
    cout << parts[0] << endl;
  }



  /// Example of how to debug with couts.
  /// You must uncomment the include and namespace lines above
  // cout << "Hello, world" << endl;

  Vector3 temp = Vector3{0, 0, 0};
  temp += Vector3{1, 1, 1};

  // cout << temp << endl;

  /// Width of the final image
  const int w = 640;

  /// Height of the final image
  const int h = 640;

  /// The 1d array for storing the pixel we have generated
  unsigned char pixels[w * h * 3];

  Triangle triangle = Triangle{Vector3{-1, 1, 0}, Vector3{-1, -1, 0}, Vector3{1, -1, 0}};
  Triangle triangle2 = Triangle{Vector3{1, 1, .5}, Vector3{-1, -1, -.5}, Vector3{1, -1, .5}};

  vector<Triangle> triangles = {triangle, triangle2};
 
  
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
      Vector3 camera_origin = Vector3{0, 0, 1};
      Vector3 camera_look_at = Vector3{0, 0, 0};
      Vector3 camera_up = Vector3{0, 1, 0};

      Vector3 target{
          x / (float)w * 2 - 1, -(y / (float)h * 2 - 1), 0};

      // cout<<"One"<<endl;
      Vector3 temp = (target - camera_origin);
      // cout<<temp<<endl;
      Vector3 ray = temp.normalize();
      // cout << ray << endl;
      // cout<<"Two"<<endl;


      float closest_t = 1000000;

      for (const Triangle &triangle : triangles)
      {

        // Find where we hit the plane
        Vector3 edge_one = triangle.one - triangle.two;
        Vector3 edge_two = triangle.three - triangle.two;
        Vector3 p = edge_two.cross(edge_one);
        Vector3 abc = p.normalize();
        float d = -abc.dot(triangle.one);

        float t = -(abc.dot(camera_origin) + d) / abc.dot(ray);

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

          r = 255 * u + 255 * v + 255 * a;
          g = 140 * u + 0 * v + 255 * a;
          b = 0 * u + 0 * v + 0 * a;
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
