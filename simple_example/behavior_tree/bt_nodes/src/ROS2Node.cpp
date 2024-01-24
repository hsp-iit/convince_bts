/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file ROS2Node.cpp
 * @authors: Stefano Bernagozzi <stefano.bernagozzi@iit.it>
 */

#include "ROS2Node.h"
#include <behaviortree_cpp_v3/leaf_node.h>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

using namespace std;
using namespace BT;

ROS2Node::ROS2Node(const std::string& name, const NodeConfiguration& config)
{

}

// PortsList ROS2Node::providedPorts()
// {
//     return { InputPort<std::string>("nodeName") };
//     return { InputPort<std::string>("topicName") };
//     return { InputPort<std::string>("interface") };
// }

bool ROS2Node::init()
{

    if(!rclcpp::ok())
    {
        rclcpp::init(/*argc*/ 0, /*argv*/ nullptr);
    }

    m_node = rclcpp::Node::make_shared(m_name);
    m_client = m_node->create_client<bt_interfaces::srv::RequestAck>(m_topicName + "/RequestAck" + m_suffixMonitor);
    m_clientStart = m_node->create_client<bt_interfaces::srv::SendStart>(m_topicName + "/SendStart" + m_suffixMonitor);
    m_clientStop = m_node->create_client<bt_interfaces::srv::SendStop>(m_topicName + "/SendStop" + m_suffixMonitor);
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),"name " << m_name << "topicname " << m_topicName << "suffixmonitor " << m_suffixMonitor);
    
    return true;

}


bool ROS2Node::stop()
{
    rclcpp::shutdown();
    return 0;
}


NodeStatus ROS2Node::tick()
{

    auto message = bt_interfaces::msg::RequestAck();
    // yDebug() << "Node" << name << "ticked";
    // SkillAck status = m_bt_request.request_ack();
    int status = requestStatus();
    if(status == message.SKILL_IDLE)
    {
      // m_bt_request.send_start();
      std::this_thread::sleep_for (std::chrono::milliseconds(100));
    }
    // status = m_bt_request.request_ack();
    while(status == message.SKILL_IDLE)
    {
        // status = m_bt_request.request_ack();
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
        // yDebug() << "Node" << name  << " status " << int(status) << "WAITING";

    }

    switch (status) {
        case message.SKILL_RUNNING:
        //    yDebug() << "Node" << name << "returns running";
            return NodeStatus::RUNNING;// may be two different enums (thrift and BT library). Making sure that the return status are the correct ones
        case message.SKILL_SUCCESS:
        //    yDebug() << "Node" << name << "returns success";
            return NodeStatus::SUCCESS;
        case message.SKILL_FAILURE:
        //    yDebug() << "Node" << name << "returns failure";
            return NodeStatus::FAILURE;
        default:
        //    yError() << "Invalid return status "<< status << "  received by node"   << name;
            break;
    }

    return NodeStatus::FAILURE;
}


NodeStatus ROS2Node::status()
{
    auto message = bt_interfaces::msg::RequestAck();
    // yDebug() << "Node" << name << "getting status";
    // SkillAck status = m_bt_request.request_ack();
    // TODO send a request on topic ;
    int status = requestStatus();
    switch (status) {
        case message.SKILL_RUNNING:
        //    yDebug() << "Node" << name << "returns running";
            return NodeStatus::RUNNING;// may be two different enums (thrift and BT library). Making sure that the return status are the correct ones
        case message.SKILL_SUCCESS:
        //    yDebug() << "Node" << name << "returns success";
            return NodeStatus::SUCCESS;
        case message.SKILL_FAILURE:
        //    yDebug() << "Node" << name << "returns failure";
            return NodeStatus::FAILURE;
        case message.SKILL_IDLE:
        //    yDebug() << "Node" << name << "returns failure";
            return NodeStatus::IDLE;
        default:
        //    yError() << "Invalid return status for received by node " << name;
            return NodeStatus::FAILURE;
    }
}


int ROS2Node::requestStatus() 
{
    std::lock_guard<std::mutex> lock(m_requestMutex);
    auto msg = bt_interfaces::msg::RequestAck();
    auto requestStart = std::make_shared<bt_interfaces::srv::SendStart::Request>();
    while (!m_clientStart->wait_for_service(1s)) {
        if (!rclcpp::ok()) {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service start. Exiting.");
        return 0;
        }
        RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "service start not available, waiting again..." << m_name);
    }
    auto resultStart = m_clientStart->async_send_request(requestStart);

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    if (rclcpp::spin_until_future_complete(m_node, resultStart) ==
        rclcpp::FutureReturnCode::SUCCESS) {
        auto request = std::make_shared<bt_interfaces::srv::RequestAck::Request>();

        while (!m_client->wait_for_service(1s)) {
            if (!rclcpp::ok()) {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service RequestAck. Exiting.");
            return 0;
            }
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service RequestAck not available, waiting again...");
        }

        auto result = m_client->async_send_request(request);
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
        if (rclcpp::spin_until_future_complete(m_node, result) ==
            rclcpp::FutureReturnCode::SUCCESS) {
            return result.get()->status.status;
        }
    }
    return msg.SKILL_FAILURE;
}

