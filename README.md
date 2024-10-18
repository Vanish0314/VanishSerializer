# Vanish Serializer

A single-file header-only library for serializing and deserializing data.

## Features
-[x] Supports basic data types (int, float, double, bool, string, vector, etc.)
-[x] Supports custom data types by implementing the ISerializable interface
-[x] Supports little-endian and big-endian byte order
-[] Supports binary and text serialization formats

## Usage

Add DataStream.hpp to your project.

## Example

Simple Data
```cpp
#include <iostream>
#include <vector>
#include <string>
#include "DataStream.hpp"

using namespace Vanish::Serialize;

int main()
{
    // Create a DataStream object
    DataStream ds;

    // Write some data to the stream
    int x = 5;
    string s = "Hello, world!";
    vector<int> v = {1, 2, 3, 4, 5};

    ds<<x<<s<<v;
    // Read the data from the stream
    int y;
    string t;
    vector<int> w;

    ds>>y>>t>>w;

    // Print the data
    std::cout<<"x: "<<x<<std::endl;
    std::cout<<"s: "<<s<<std::endl;
    std::cout<<"v: "<<v<<std::endl;
    std::cout<<"y: "<<y<<std::endl;
    std::cout<<"t: "<<t<<std::endl;
    std::cout<<"w: "<<w<<std::endl;

    return 0;
}
```


Custom Data
```cpp
class MyData : ISerializable
{
public:
    int a;
    string b;
    vector<int> c;

    SERIALIZE_FUNC(a,b,c)
}
```