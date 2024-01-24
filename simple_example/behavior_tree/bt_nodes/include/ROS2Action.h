/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file ROS2Action.h
 * @authors: Stefano Bernagozzi <stefano.bernagozzi@iit.it>
 */

#pragma once


#include <ROS2Node.h>
#include <string>

class ROS2Action :  public ActionNodeBase, public ROS2Node
        // public virtual ActionNodeBase because the BT factory accepts only classes that explicitly inherits from ActionNodeBase or ConditionNode
{
public:
    ROS2Action (const string name, const NodeConfiguration &config);
    void halt() override;
    NodeStatus tick() override;

    static PortsList providedPorts();
};

