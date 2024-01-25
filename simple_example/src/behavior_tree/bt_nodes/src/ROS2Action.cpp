/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file ROS2Action.cpp
 * @authors: Stefano Bernagozzi <stefano.bernagozzi@iit.it>
 */


#include <ROS2Action.h>

ROS2Action::ROS2Action(const string name, const NodeConfiguration& config) :
        ActionNodeBase(name, config),
        ROS2Node(name, config)
{
            Optional<std::string> node_name = getInput<std::string>("nodeName");
    if (node_name)
    {
        m_name = node_name.value();
    }


    Optional<std::string> topic_name = getInput<std::string>("topicName");
    if (topic_name)
    {
        m_topicName = topic_name.value();
    }
    Optional<std::string> suffix_monitor = getInput<std::string>("suffixMonitor");
    if (suffix_monitor)
    {
        m_suffixMonitor = suffix_monitor.value();
    }
    Optional<std::string> interface = getInput<std::string>("interface");
    bool ok = init();

    if(!ok)
    {
       RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Something went wrong in the node init() of %s", name.c_str());
    }
}

NodeStatus ROS2Action::tick()
{
    auto message = bt_interfaces::msg::RequestAck();
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Node %s sending tick to skill", ActionNodeBase::name().c_str());
    while(!m_started) {
        m_started = sendStart();
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(100));    
    auto status = ROS2Node::requestAck();
    switch (status) {
        case message.SKILL_RUNNING:
            return NodeStatus::RUNNING;// may be two different enums (ros2 and BT library). Making sure that the return status are the correct ones
        case message.SKILL_SUCCESS:
            return NodeStatus::SUCCESS;
        case message.SKILL_FAILURE:
            return NodeStatus::FAILURE;
        default:
            break;
    }
    return NodeStatus::FAILURE;
}

PortsList ROS2Action::providedPorts()
{
    return { InputPort<std::string>("nodeName"), 
             InputPort<std::string>("topicName"), 
             InputPort<std::string>("interface"),
             InputPort<std::string>("suffixMonitor")  };
}

void ROS2Action::halt()
{
    RCLCPP_DEBUG(rclcpp::get_logger("rclcpp"), "Node %s sending halt to skill", ActionNodeBase::name().c_str());
    while(m_started) {
        m_started = !sendStop();
    }
}
