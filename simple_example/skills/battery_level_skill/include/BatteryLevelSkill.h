/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

# pragma once

#include <thread>
#include <rclcpp/rclcpp.hpp>
#include "BatteryLevelSkillSM.h"
#include "BatteryLevelDataModel.h"
#include <bt_interfaces/msg/request_ack.hpp>
#include <bt_interfaces/srv/request_ack.hpp>
#include <bt_interfaces/srv/send_start.hpp>
#include <bt_interfaces/srv/send_stop.hpp>

class BatteryLevelSkill
{
public:
    BatteryLevelSkill(std::string name );

    bool start(int argc, char * argv[]);
    static void spin(std::shared_ptr<rclcpp::Node> node);
    void request_ack( [[maybe_unused]] const std::shared_ptr<bt_interfaces::srv::RequestAck::Request> request,
          std::shared_ptr<bt_interfaces::srv::RequestAck::Response>      response);
    void send_start( [[maybe_unused]] const std::shared_ptr<bt_interfaces::srv::SendStart::Request> request,
                     [[maybe_unused]] std::shared_ptr<bt_interfaces::srv::SendStart::Response>      response);
    void send_stop([[maybe_unused]] const std::shared_ptr<bt_interfaces::srv::SendStop::Request> request,
                   [[maybe_unused]] std::shared_ptr<bt_interfaces::srv::SendStop::Response>      response);

private:
    std::shared_ptr<std::thread> m_threadSpin;
    std::shared_ptr<rclcpp::Node> m_node;
    rclcpp::Service<bt_interfaces::srv::RequestAck>::SharedPtr m_requestAckService;
    rclcpp::Service<bt_interfaces::srv::SendStart>::SharedPtr m_sendStartService;
    rclcpp::Service<bt_interfaces::srv::SendStop>::SharedPtr m_sendStopService;
    std::string m_name;
    BatteryLevelDataModel m_dataModel;
    BatteryLevelSkillSM m_stateMachine;
};
