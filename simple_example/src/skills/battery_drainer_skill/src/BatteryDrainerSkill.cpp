/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include "BatteryDrainerSkill.h"

#include <QTimer>
#include <QDebug>
#include <QTime>
#include <iostream>
#include <QStateMachine>

BatteryDrainerSkill::BatteryDrainerSkill(std::string name ) :
        m_name(std::move(name))
{
    m_dataModel.set_name(name+"dataModel");
    m_stateMachine.setDataModel(&m_dataModel);
}


void BatteryDrainerSkill::spin(std::shared_ptr<rclcpp::Node> node)
{
    rclcpp::spin(node);  
    rclcpp::shutdown();  
}


bool BatteryDrainerSkill::start(int argc, char*argv[])
{

    if(!rclcpp::ok())
    {
        rclcpp::init(/*argc*/ argc, /*argv*/ argv);
    }
    m_node = rclcpp::Node::make_shared(m_name);

    
    RCLCPP_DEBUG(m_node->get_logger(), "BatteryDrainerSkill::start");
    std::cout << "BatteryDrainerSkill::start";
    m_requestAckService = m_node->create_service<bt_interfaces::srv::RequestAck>("/BatteryDrainerSkill/RequestAck",  
                                                                                std::bind(&BatteryDrainerSkill::request_ack,
                                                                                this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2));
    m_sendStartService = m_node->create_service<bt_interfaces::srv::SendStart>("/BatteryDrainerSkill/SendStart",  
                                                                                std::bind(&BatteryDrainerSkill::send_start,
                                                                                this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2));
    m_sendStopService = m_node->create_service<bt_interfaces::srv::SendStop>("/BatteryDrainerSkill/SendStop",  
                                                                                std::bind(&BatteryDrainerSkill::send_stop,
                                                                                this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2));

    m_stateMachine.start();
    m_threadSpin = std::make_shared<std::thread>(spin, m_node);
    return true;
}

void BatteryDrainerSkill::request_ack( [[maybe_unused]] const std::shared_ptr<bt_interfaces::srv::RequestAck::Request> request,
                                       std::shared_ptr<bt_interfaces::srv::RequestAck::Response>      response)
{
    auto message = bt_interfaces::msg::RequestAck();
        for (const auto& state : m_stateMachine.activeStateNames()) {
            if (state == "drain") {
                m_stateMachine.submitEvent("CMD_OK");
                response->status.status = message.SKILL_RUNNING;
            }
            if (state == "idle") {
                m_stateMachine.submitEvent("CMD_OK");
                response->status.status = message.SKILL_IDLE;
            }
        }
    response->is_ok = true;
}

void BatteryDrainerSkill::send_start( [[maybe_unused]] const std::shared_ptr<bt_interfaces::srv::SendStart::Request> request,
                                       [[maybe_unused]] std::shared_ptr<bt_interfaces::srv::SendStart::Response>      response)
{
    RCLCPP_DEBUG(m_node->get_logger(), "BatteryDrainerSkill::send_start");    
    std::cout << "BatteryDrainerSkill::send_start";    
    m_stateMachine.submitEvent("CMD_START");
    response->is_ok = true;
}

void BatteryDrainerSkill::send_stop( [[maybe_unused]] const std::shared_ptr<bt_interfaces::srv::SendStop::Request> request,
                                       [[maybe_unused]] std::shared_ptr<bt_interfaces::srv::SendStop::Response>      response)
{

    RCLCPP_DEBUG(m_node->get_logger(), "BatteryDrainerSkill::send_start");    
    std::cout <<  "BatteryDrainerSkill::send_stop";    
    m_stateMachine.submitEvent("CMD_STOP",  QStateMachine::HighPriority);
    response->is_ok = true;
}
