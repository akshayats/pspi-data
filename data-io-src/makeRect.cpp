// classes example
#include <iostream>
#include </usr/include/pcl-1.7/pcl/io/pcd_io.h>
#include </usr/include/pcl-1.7/pcl/point_types.h>
using namespace std;

class Rectangle {
    int width, height;
  public:
    void set_values (int,int);
    int area() {return width*height;}
};

void Rectangle::set_values (int x, int y) {
  width = x;
  height = y;
}

int main () {
	cout << "Hello World!" << endl;
	Rectangle rect;
	rect.set_values (3,4);
	cout << "area: " << rect.area()<< endl <<endl;
	return 0;
}