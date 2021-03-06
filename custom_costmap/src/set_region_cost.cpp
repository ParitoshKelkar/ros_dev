#include <ros/ros.h>
//#include <actionlib/client/simple_action_client.h>
#include <costmap_2d/costmap_2d_ros.h>
#include <costmap_2d/costmap_2d.h>

#include <geometry_msgs/PolygonStamped.h>
#include <geometry_msgs/PointStamped.h>


#include <costmap_2d/footprint.h>
#include <tf/transform_listener.h>

#include <custom_costmap/geometry_tools.h>
#include <custom_costmap/simple_layer.h>
#include <custom_costmap/updateDesiredRegion.h>

#include <ros/wall_timer.h>

#include <move_base_msgs/MoveBaseAction.h>

#include <visualization_msgs/Marker.h>
#include <boost/foreach.hpp>

namespace custom_costmap{

class setRegionCost{

private:

    ros::NodeHandle nh_;
    ros::NodeHandle private_nh_;

    ros::Subscriber point_;
    ros::Publisher point_viz_pub_;
    ros::WallTimer point_viz_timer_;
    geometry_msgs::PolygonStamped input_;
    geometry_msgs::PolygonStamped test_;
//    costmap_2d::Costmap2D costmap_;
    tf::TransformListener tf_listener_;        
    ros::ServiceClient updateDesiredRegionClient;
    custom_costmap::updateDesiredRegion srv;
    boost::shared_ptr<costmap_2d::Costmap2DROS> custom_costmap_ros_;


    //SimpleLayer Simple;
   
    bool waiting_for_center_;

    /**
     * @brief Publish markers for visualization of points for boundary polygon.
     */
    void vizPubCb(){


       ros::ServiceClient updateDesiredRegionClient = private_nh_.serviceClient<custom_costmap::updateDesiredRegion>("/set_region_cost/my_costmap/simple_layer/update_desired_region");

//	if (!updateDesiredRegionClient.waitForExistence()){
//		ROS_WARN("COULDNT CONTACT SERIVCE");
//	}
		

	ROS_INFO("INVOKED_TIMER_Cb");
        visualization_msgs::Marker points, line_strip;

        points.header = line_strip.header = input_.header;
        points.ns = line_strip.ns = "polygon_points";

        points.id = 0;
        line_strip.id = 1;

        points.type = visualization_msgs::Marker::SPHERE_LIST;
        line_strip.type = visualization_msgs::Marker::LINE_STRIP;

		srv.request.user_polygon = test_;
		std::cout<<test_;
		if(updateDesiredRegionClient.call(srv)){
			ROS_INFO("successfully called service");
		}
		else{
			ROS_INFO("FAIL");
		}		

	ROS_INFO("BEFORE ~");
        if(!input_.polygon.points.empty()){
	   
	   ROS_INFO("NOT EMPTY %f",input_.polygon.points[0].x);
 
            points.action = line_strip.action = visualization_msgs::Marker::ADD;
            points.pose.orientation.w = line_strip.pose.orientation.w = 1.0;

            points.scale.x = points.scale.y = 0.1;
            line_strip.scale.x = 0.05;

            BOOST_FOREACH(geometry_msgs::Point32 point, input_.polygon.points){
                line_strip.points.push_back(costmap_2d::toPoint(point));
                points.points.push_back(costmap_2d::toPoint(point));
	
		
            }
		srv.request.user_polygon = test_;
		std::cout<<test_;
		if(updateDesiredRegionClient.call(srv)){
			ROS_INFO("successfully called service");
		}
		else{
			ROS_INFO("FAIL");
		}		
	

            if(waiting_for_center_){
                line_strip.points.push_back(costmap_2d::toPoint(input_.polygon.points.front()));
                points.color.a = points.color.r = line_strip.color.r = line_strip.color.a = 1.0;
            }else{
                points.color.a = points.color.b = line_strip.color.b = line_strip.color.a = 1.0;
            }
        }else{
            points.action = line_strip.action = visualization_msgs::Marker::DELETE;
        }
        point_viz_pub_.publish(points);
        point_viz_pub_.publish(line_strip);
	ROS_INFO("JUST BEFORE EXIT");
    }

    /**
     * @brief Build boundary polygon from points received through rviz gui.
     * @param point Received point from rviz
     */
    void pointCb(const geometry_msgs::PointStampedConstPtr& point){
	
	ROS_INFO("PT CLICKED");      
        double average_distance = polygonPerimeter(input_.polygon) / input_.polygon.points.size();

       ros::ServiceClient updateDesiredRegionClient = private_nh_.serviceClient<custom_costmap::updateDesiredRegion>("/set_region_cost/my_costmap/simple_layer/update_desired_region");

	if (!updateDesiredRegionClient.waitForExistence()){
		ROS_WARN("COULDNT CONTACT SERIVCE");
	}
	else{
		ROS_INFO("CONTACTED");
	}

        visualization_msgs::Marker points, line_strip;




   //     if(waiting_for_center_){
   //         //flag is set so this is the last point of boundary polygon, i.e. center

   //         if(!pointInPolygon(point->point,input_.polygon)){
   //             ROS_ERROR("Center not inside polygon, restarting");
   //         }else{
   //             actionlib::SimpleActionClient<frontier_exploration::ExploreTaskAction> exploreClient("explore_server", true);
   //             exploreClient.waitForServer();
   //             ROS_INFO("Sending goal");
   //             frontier_exploration::ExploreTaskGoal goal;
   //             goal.explore_center = *point;
   //             goal.explore_boundary = input_;
   //             exploreClient.sendGoal(goal);
   //         }
   //         waiting_for_center_ = false;
   //         input_.polygon.points.clear();

   //     }else if(input_.polygon.points.empty()){
   	  ROS_INFO("BEFORE 1st IF");
          if(input_.polygon.points.empty()){
            //first control point, so initialize header of boundary polygon
		ROS_INFO("1st IF");
            input_.header = point->header;
            input_.polygon.points.push_back(costmap_2d::toPoint32(point->point));
		srv.request.user_polygon = input_;
		std::cout<<input_;
		if(updateDesiredRegionClient.call(srv)){
			ROS_INFO("successfully called service");
		}
		else{
			ROS_INFO("FAIL");
		}		
	

        }else if(input_.header.frame_id != point->header.frame_id){
            ROS_ERROR("Frame mismatch, restarting polygon selection");
            input_.polygon.points.clear();

        }else if(input_.polygon.points.size() > 1 && pointsNearby(input_.polygon.points.front(), point->point,
                                                                    average_distance*0.1)){
            //check if last boundary point, i.e. nearby to first point

            if(input_.polygon.points.size() < 3){
                ROS_ERROR("Not a valid polygon, restarting");
                input_.polygon.points.clear();
            }else{
                input_.polygon.points.push_back(costmap_2d::toPoint32(point->point));
                input_.header.stamp = ros::Time::now();		
                ROS_WARN("polygon complete");
		ROS_INFO("Initiating Service");
		srv.request.user_polygon = input_;
		std::cout<<input_;
		if(updateDesiredRegionClient.call(srv)){
			ROS_INFO("successfully called service");
		}
		else{
			ROS_INFO("FAIL");
		}		
            }

        }else{

            //otherwise, must be a regular point inside boundary polygon
            ROS_INFO("add");
            input_.polygon.points.push_back(costmap_2d::toPoint32(point->point));
            input_.header.stamp = ros::Time::now();
		srv.request.user_polygon = input_;
		std::cout<<input_;
		if(updateDesiredRegionClient.call(srv)){
			ROS_INFO("successfully called service");
		}
		else{
			ROS_INFO("FAIL");
		}		
	
        }
	ROS_INFO("DIDNT GO INTO ANYTHING");
        points.header = line_strip.header = input_.header;
        points.ns = line_strip.ns = "polygon_points";

        points.id = 0;
        line_strip.id = 1;

        points.type = visualization_msgs::Marker::SPHERE_LIST;
        line_strip.type = visualization_msgs::Marker::LINE_STRIP;

            points.action = line_strip.action = visualization_msgs::Marker::ADD;
            points.pose.orientation.w = line_strip.pose.orientation.w = 1.0;

            points.scale.x = points.scale.y = 0.1;
            line_strip.scale.x = 0.05;

            BOOST_FOREACH(geometry_msgs::Point32 point, input_.polygon.points){
                line_strip.points.push_back(costmap_2d::toPoint(point));
                points.points.push_back(costmap_2d::toPoint(point));
            }

            if(waiting_for_center_){
                line_strip.points.push_back(costmap_2d::toPoint(input_.polygon.points.front()));
                points.color.a = points.color.r = line_strip.color.r = line_strip.color.a = 1.0;
            }else{
                points.color.a = points.color.b = line_strip.color.b = line_strip.color.a = 1.0;
            }
        //else{
          //  points.action = line_strip.action = visualization_msgs::Marker::DELETE;
       // }


        point_viz_pub_.publish(points);
        point_viz_pub_.publish(line_strip);

	

    }

public:

    /**
     * @brief Constructor for the client.
     */
    setRegionCost() : //implicit assignment - initialization list
        tf_listener_(ros::Duration(10.0)),
	nh_(),
	private_nh_("~"),
        waiting_for_center_(false)
    {
	
        input_.header.frame_id = "map";
        point_ = nh_.subscribe("/clicked_point",10,&setRegionCost::pointCb, this);	
        point_viz_pub_ = nh_.advertise<visualization_msgs::Marker>("polygon_marker", 10);
//        point_viz_timer_ = nh_.createWallTimer(ros::WallDuration(0.1), boost::bind(&setRegionCost::vizPubCb, this));
        custom_costmap_ros_ = boost::shared_ptr<costmap_2d::Costmap2DROS>(new costmap_2d::Costmap2DROS("my_costmap", tf_listener_));
	
       updateDesiredRegionClient = private_nh_.serviceClient<custom_costmap::updateDesiredRegion>("/set_region_cost/my_costmap/simple_layer/update_desired_region");
	if (!updateDesiredRegionClient.waitForExistence()){
		ROS_WARN("COULDNT CONTACT SERIVCE");
	}
	else{
		ROS_INFO("DONE CONTACTING");
	}

//	test_.header.frame_id= "map";
//	test_.header.stamp=ros::Time::now();
//	test_.header.seq = 0;
//	test_.polygon.points[0].x = 10;
//	test_.polygon.points[0].y = 10;
//	test_.polygon.points[0].z = 0;
        ROS_INFO("Please use the 'Point' tool in Rviz to select Polygon.");
	std::cout<<test_;
    }    

};

}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "set_region_cost");

    custom_costmap::setRegionCost client;
    ros::spin();
    return 0;
}

