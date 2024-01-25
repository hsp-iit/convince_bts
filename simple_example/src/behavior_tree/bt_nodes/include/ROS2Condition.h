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


#pragma once

#include <ROS2Node.h>
#include <string>
#include<behaviortree_cpp_v3/condition_node.h>

class ROS2Condition :  public ConditionNode, public ROS2Node
{
public:
    ROS2Condition(const string name, const NodeConfiguration& config);
    NodeStatus tick() override;

    static PortsList providedPorts();
};
