#include <pluginlib/class_list_macros.h> //nclude the pluginlib macros that allow us to register classes as plugins
#include <pluginlib_tutorials_/polygon_base.h> //register the Triangle class as a plugin
#include <pluginlib_tutorials_/polygon_plugins.h> //register the Square class as a plugin

PLUGINLIB_EXPORT_CLASS(polygon_plugins::Triangle, polygon_base::RegularPolygon)
PLUGINLIB_EXPORT_CLASS(polygon_plugins::Square, polygon_base::RegularPolygon)

