#ifndef SIMPLE_LAYER_H_
#define SIMPLE_LAYER_H_
#include <ros/ros.h>
#include <costmap_2d/layer.h>
#include <costmap_2d/layered_costmap.h>
#include <costmap_2d/GenericPluginConfig.h>
#include <dynamic_reconfigure/server.h>
#include <custom_costmap/updateDesiredRegion.h>

namespace custom_costmap
{

class SimpleLayer : public costmap_2d::Layer
{
public:
  SimpleLayer();
  ~SimpleLayer();

  virtual void onInitialize();
  virtual void updateBounds(double robot_x, double robot_y, double robot_yaw, double* min_x, double* min_y, double* max_x,
                             double* max_y);
  virtual void updateCosts(costmap_2d::Costmap2D& master_grid, int min_i, int min_j, int max_i, int max_j);

private:
  void reconfigureCB(costmap_2d::GenericPluginConfig &config, uint32_t level);

  double mark_x_, mark_y_;
  dynamic_reconfigure::Server<costmap_2d::GenericPluginConfig> *dsrv_;

  // the costmap modifying services
  bool updateDesiredRegionService(custom_costmap::updateDesiredRegion::Request &req, custom_costmap::updateDesiredRegion::Response &res);

  // the service will call this function internally
  bool updateDesiredRegion(geometry_msgs::PolygonStamped user_polygon, bool outcome);

  // variables
  ros::ServiceServer updateService_;

  
};
}
#endif
