/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file ROS2Condition.h
 * @authors: Stefano Bernagozzi <stefano.bernagozzi@iit.it>
 */


#include <ROS2Condition.h>

ROS2Condition::ROS2Condition(const string name, const NodeConfiguration& config) :
        ConditionNode(name, config),
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

PortsList ROS2Condition::providedPorts()
{
    return { InputPort<std::string>("nodeName"),
             InputPort<std::string>("topicName"),
             InputPort<std::string>("interface"),
             InputPort<std::string>("suffixMonitor") };
}



NodeStatus ROS2Action::tick()
{
    RCLCPP_DEBUG(rclcpp::get_logger("rclcpp"), "Node %s sending tick to skill", ActionNodeBase::name().c_str());
    sendStart();
    std::this_thread::sleep_for (std::chrono::milliseconds(100));    
    auto status = ROS2Node::requestAck();
    switch (status) {
        case message.SKILL_SUCCESS:
            return NodeStatus::SUCCESS;
        case message.SKILL_FAILURE:
            return NodeStatus::FAILURE;
        default:
            break;
    }
    return NodeStatus::FAILURE;
}
