# pluginlib_tutorials
Writing and Using a Simple Plugin

http://wiki.ros.org/pluginlib/Tutorials/Writing%20and%20Using%20a%20Simple%20Plugin



## Getting ready
First, install pre-made `pluginlib_tutorials` pkg by doing the following where %ROS_DISTRO% can be {`fuerte`, `groovy,hydro,indigo,jade`} etc.:

```
$ apt-get install ros-%ROS_DISTRO%-common-tutorials
```
Then we'll need to create a package to do our work in. It is recommended to create this package inside the `catkin_ws/src/ directory`.

version groovy_and_newer
```
$ catkin_create_pkg pluginlib_tutorials_ roscpp pluginlib
```
  
version fuerte_and_older
```
$ roscreate-pkg pluginlib_tutorials_ roscpp pluginlib
```
  
## Create a Base Class
Ok, now we'll create a base class from which all of our plugins will inherit. For this example, we'll be creating a couple of `RegularPolygon` objects and using them, so we need to create the `RegularPolygon` class. Open your favorite editor, edit `catkin_ws/src/pluginlib_tutorials_/include/pluginlib_tutorials_/polygon_base.h`, and paste the following inside of it:

```
#ifndef PLUGINLIB_TUTORIALS__POLYGON_BASE_H_
#define PLUGINLIB_TUTORIALS__POLYGON_BASE_H_

namespace polygon_base
{
  class RegularPolygon
  {
    public:
      virtual void initialize(double side_length) = 0;
      virtual double area() = 0;
      virtual ~RegularPolygon(){}

    protected:
      RegularPolygon(){}
  };
};
#endif
```
This code above should be pretty self explanatory... we're creating an abstract class called `RegularPolygon`. One thing to notice is the presence of the `initialize` method. With [[pluginlib]], a constructor without parameters is required for classes so, if any parameters are required, we use the `initialize` method to initialize the object.

## Create the Plugins
For this example, we'll create two `RegularPolygon` plugins, the first will be a `Triangle` and the second a `Square`. Open up `include/pluginlib_tutorials_/polygon_plugins.h` and paste the following into it:

```
#ifndef PLUGINLIB_TUTORIALS__POLYGON_PLUGINS_H_
#define PLUGINLIB_TUTORIALS__POLYGON_PLUGINS_H_
#include <pluginlib_tutorials_/polygon_base.h>
#include <cmath>

namespace polygon_plugins
{
  class Triangle : public polygon_base::RegularPolygon
  {
    public:
      Triangle(){}

      void initialize(double side_length)
      {
        side_length_ = side_length;
      }

      double area()
      {
        return 0.5 * side_length_ * getHeight();
      }

      double getHeight()
      {
        return sqrt((side_length_ * side_length_) - ((side_length_ / 2) * (side_length_ / 2)));
      }

    private:
      double side_length_;
  };

  class Square : public polygon_base::RegularPolygon
  {
    public:
      Square(){}

      void initialize(double side_length)
      {
        side_length_ = side_length;
      }

      double area()
      {
        return side_length_ * side_length_;
      }

    private:
      double side_length_;

  };
};
#endif
```
This code should also be pretty clear. We've created two classes that inherit from `RegularPolygon` that we'll use as our plugins.

## Registering the Plugins
So far, we've just created some standard C++ classes. Now, we'll start to do the [[pluginlib]] specific work as we declare our `Triangle` and `Square` classes as plugins. Open up `src/polygon_plugins.cpp` and paste the following into it:

version groovy_and_newer

```
#include <pluginlib/class_list_macros.h>
#include <pluginlib_tutorials_/polygon_base.h>
#include <pluginlib_tutorials_/polygon_plugins.h>

PLUGINLIB_EXPORT_CLASS(polygon_plugins::Triangle, polygon_base::RegularPolygon)
PLUGINLIB_EXPORT_CLASS(polygon_plugins::Square, polygon_base::RegularPolygon)
```

Let's look at a couple of lines in a bit more detail.

<<CodeRef(declare, 1, 1)>>
Here, we include the [[pluginlib]] macros that allow us to register classes as plugins.

<<CodeRef(declare, 5, 5)>>
Here, we register the `Triangle` class as a plugin. Let's go through the arguments to the `PLUGINLIB_EXPORT_CLASS` macro:
 * 1: The fully-qualified type of the plugin class, in this case, `polygon_plugins::Triangle`.
 * 2: The fully-qualified type of the base class, in this case, `polygon_base::RegularPolygon`.
  
version fuerte_and_older

```
#include <pluginlib/class_list_macros.h>
#include <pluginlib_tutorials_/polygon_base.h>
#include <pluginlib_tutorials_/polygon_plugins.h>

PLUGINLIB_DECLARE_CLASS(pluginlib_tutorials_, regular_triangle, polygon_plugins::Triangle, polygon_base::RegularPolygon)
PLUGINLIB_DECLARE_CLASS(pluginlib_tutorials_, regular_square, polygon_plugins::Square, polygon_base::RegularPolygon)
```

Let's look at a couple of lines in a bit more detail.

<<CodeRef(declare, 1, 1)>>
Here, we include the [[pluginlib]] macros that allow us to register classes as plugins.

<<CodeRef(declare, 5, 5)>>
Here, we register the `Triangle` class as a plugin. Let's go through the arguments to the `PLUGINLIB_DECLARE_CLASS` macro:
 * 1: The namespace in which the `Triangle` plugin will live. Typically, we use the name of the package that contains the library that `Triangle` is a part of. In this case, that's `pluginlib_tutorials_` which is the name of the package we created in step one of this tutorial.
 * 2: The name we wish to give to the plugin.... we'll call ours `regular_triangle`.
 * 3: The fully-qualified type of the plugin class, in this case, `polygon_plugins::Triangle`.
 * 4: The fully-qualified type of the base class, in this case, `polygon_base::RegularPolygon`.
  
## Building the Plugin Library
To actually build the library, add the following lines to your `CMakeLists.txt` file:

version groovy_and_newer
```
include_directories(include)
add_library(polygon_plugins src/polygon_plugins.cpp)
```
You should then be able to compile the code you've written so far by running `catkin_make` in the top folder of your `catkin` workspace.
  
version fuerte_and_older
```
rosbuild_add_library(polygon_plugins src/polygon_plugins.cpp)
```
You should then be able to compile the code you've written so far by running `rosmake`.
  
## Making the Plugins Available to the ROS Toolchain
The steps above make it so that instances of our plugins can be created once the library they exist in is loaded, but the plugin loader still needs a way to find that library and to know what to reference within that library. To this end, we'll also create an XML file that, along with a special export line in the package manifest, makes all the necessary information about our plugins available to the ROS toolchain.

## The Plugin XML File
Open up an editor and paste the following into `polygon_plugins.xml`, which should be in the top level of the package (along with CMakeLists.txt and package.xml):

version fuerte_and_older
```
<library path="lib/libpolygon_plugins">
  <class name="pluginlib_tutorials_/regular_triangle" type="polygon_plugins::Triangle" base_class_type="polygon_base::RegularPolygon">
    <description>This is a triangle plugin.</description>
  </class>
  <class name="pluginlib_tutorials_/regular_square" type="polygon_plugins::Square" base_class_type="polygon_base::RegularPolygon">
    <description>This is a square plugin.</description>
  </class>
</library>
```

There are a couple of lines that we'll look at in more detail.

<<CodeRef(plugin_xml, 1, 1)>>
The `library` tag gives the relative path to a library that contains the plugins that we want to export. In this case, that's `lib/libpolygon_plugins`

<<CodeRef(plugin_xml, 2, 4)>>
The `class` tag declares a plugin that we want to export from our library. Let's go through its parameters:
 * `name`: This refers to the name of the plugin that we're exported given as `plugin_namespace/PluginName`. Since we used the package name as the namespace for our `regular_triangle` plugin... this results in us using `pluginlib_tutorials_/regular_triangle`.
 * `type`: The fully qualified type of the plugin. For us, that's `polygon_plugins::Triangle`.
 * `base_class`: The fully qualified base class type for the plugin. For us, that's `polygon_base::RegularPolygon`.
 * `description`: A description of the plugin and what it does.
  
version groovy_and_newer
```
<library path="lib/libpolygon_plugins">
  <class type="polygon_plugins::Triangle" base_class_type="polygon_base::RegularPolygon">
    <description>This is a triangle plugin.</description>
  </class>
  <class type="polygon_plugins::Square" base_class_type="polygon_base::RegularPolygon">
    <description>This is a square plugin.</description>
  </class>
</library>
```
There are a couple of lines that we'll look at in more detail.

<<CodeRef(plugin_xml, 1, 1)>>
The `library` tag gives the relative path to a library that contains the plugins that we want to export. In this case, that's `lib/libpolygon_plugins`

<<CodeRef(plugin_xml, 2, 4)>>
The `class` tag declares a plugin that we want to export from our library. Let's go through its parameters:
 * `type`: The fully qualified type of the plugin. For us, that's `polygon_plugins::Triangle`.
 * `base_class`: The fully qualified base class type for the plugin. For us, that's `polygon_base::RegularPolygon`.
 * `description`: A description of the plugin and what it does.
 * `name` (not used in the example above): This refers to the name of the plugin that we're exported given as `plugin_namespace/PluginName`. Since we used the package name as the namespace for our `regular_triangle` plugin... this results in us using `pluginlib_tutorials_/regular_triangle`. After [[pluginlib#pluginlib.2BAC8-pluginlib_groovy.pluginlib_Legacy_.22Lookup_Name.22|the API change]], this attribute is no longer required.
  
## Exporting Plugins
To export the plugins that we created, we'll have to add the following lines to our either `manifest.xml` with `rosbuild` pkg or `package.xml` with `catkin` package:

```
<export>
  <pluginlib_tutorials_ plugin="${prefix}/polygon_plugins.xml" />
</export>
```
The name of the tag, `pluginlib_tutorials_` above, should correspond to the package where the '''base_class''' for the plugin lives. In this case, the base class and the inherited plugin classes live in the same package, but in most real-world situations this will not be the case. From there, the `plugin` attribute should be set to point to the XML file generated in the step above.

To verify that things are working, first build the workspace and source the resulting setup file, then try running the following `rospack` command:

```
rospack plugins --attrib=plugin pluginlib_tutorials_
```
You should see output giving the full path to the `polygon_plugins.xml` file. This means that the ROS toolchain is setup properly to work with your plugin.

## Using a Plugin
Now that we've successfully created and exported some `RegularPolygon` plugins, let's use them. Open up `src/polygon_loader.cpp` and paste the following into it:

```
#include <pluginlib/class_loader.h>
#include <pluginlib_tutorials_/polygon_base.h>

int main(int argc, char** argv)
{
  pluginlib::ClassLoader<polygon_base::RegularPolygon> poly_loader("pluginlib_tutorials_", "polygon_base::RegularPolygon");

  try
  {
    boost::shared_ptr<polygon_base::RegularPolygon> triangle = poly_loader.createInstance("polygon_plugins::Triangle");
    triangle->initialize(10.0);

    boost::shared_ptr<polygon_base::RegularPolygon> square = poly_loader.createInstance("polygon_plugins::Square");
    square->initialize(10.0);

    ROS_INFO("Triangle area: %.2f", triangle->area());
    ROS_INFO("Square area: %.2f", square->area());
  }
  catch(pluginlib::PluginlibException& ex)
  {
    ROS_ERROR("The plugin failed to load for some reason. Error: %s", ex.what());
  }

  return 0;
}
```
Let's look at a few of the key lines:

<<CodeRef(loader, 1, 2)>> Here, we include the `ClassLoader` from [[pluginlib]] as well as the `RegularPolygon` interface.

<<CodeRef(loader, 6, 6)>> Here, we create a `ClassLoader` that we'll use to load plugins. It takes two arguments. The first, is the name of the package that contains the plugin base class, in our case, `pluginlib_tutorials_`. The second, is the fully qualified type of the base class, in our case, `polygon_base::RegularPolygon`.

<<CodeRef(loader, 13, 14)>> Here, we actually load the `pluginlib_tutorials_/regulare_triangle` plugin. Since plugin constructors cannot have arguments, we call the `initialize` function just after the object is constructed to initialize the object.

<<CodeRef(loader, 19, 22)>> Here, we check if something goes wrong when loading our plugins so that we can give useful feedback to the user.

## Running the Code
To run the code we just wrote, we'll add the following line to our `CMakeLists.txt` file:

version fuerte_and_older
```
rosbuild_add_executable(polygon_loader src/polygon_loader.cpp)
```
Next, we'll build everything by running the `rosmake` command.

Finally, run the `bin/polygon_loader` executable.
  
version groovy_and_newer
```
add_executable(polygon_loader src/polygon_loader.cpp)
target_link_libraries(polygon_loader ${catkin_LIBRARIES})
```
Next, we'll build everything by running the `catkin_make` command in the home directory of your `catkin` workspace.

Finally, run the `devel/lib/pluginlib_tutorials_/polygon_loader` executable.
  
You should get output similar to that shown below:

```
[ INFO] [WallTime: 1279658450.869089666]: Triangle area: 43.30
[ INFO] [WallTime: 1279658450.869138007]: Square area: 100.00
```
Congratulations! You've just written and used your first plugins.

