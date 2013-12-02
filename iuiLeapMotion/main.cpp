//
//  main.cpp
//  iuiLeapMotion
//
//  Created by Amy on 13/10/3.
//  Copyright (c) 2013年 Amy. All rights reserved.
//
/******************************************************************************\
 * Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               *
 * Leap Motion proprietary and confidential. Not for distribution.              *
 * Use subject to the terms of the Leap Motion SDK Agreement available at       *
 * https://developer.leapmotion.com/sdk_agreement, or another agreement         *
 * between Leap Motion and you, your company or other organization.             *
 \******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <thread>

#include <iostream>
#include "Leap.h"
using namespace Leap;


class SampleListener : public Listener {
public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
};

void SampleListener::onInit(const Controller& controller) {
    std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
    controller.enableGesture(Gesture::TYPE_CIRCLE);
    controller.enableGesture(Gesture::TYPE_KEY_TAP);
    controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
    controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
    //Note: not dispatched when running in a debugger.
    std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
    std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();
    std::cout << "Frame id: " << frame.id()
    << ", timestamp: " << frame.timestamp()
    << ", hands: " << frame.hands().count()
    << ", fingers: " << frame.fingers().count()
    << ", tools: " << frame.tools().count()
    << ", gestures: " << frame.gestures().count() << std::endl;
    
    //--set up udp socket---------------------------------
    int sockfd;
    int z;
    int hand_level = 0;
    char buf[80];
    std::ostringstream buf_out;
    struct sockaddr_in adr_srvr;
    socklen_t addrlen = sizeof(adr_srvr);
    int port = 8000;
    int key_tap = 0;
    
    //for recvfrom
    
    
    
    // printf("連結 server...\n");
    bzero(&adr_srvr, sizeof(adr_srvr));
    adr_srvr.sin_family = AF_INET;
    adr_srvr.sin_addr.s_addr = inet_addr("192.168.0.107");
    adr_srvr.sin_port = htons(port);
    
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd == -1) {
        perror("socket error");
        exit(1);
    }
    //--end set up------------------------------------------
   
    if (!frame.hands().empty()) {
        // Get the first hand
        const Hand hand = frame.hands()[0];
        
        // Check if the hand has any fingers
        const FingerList fingers = hand.fingers();
        if (!fingers.empty()) {
            // Calculate the hand's average finger tip position
            Vector avgPos;
            for (int i = 0; i < fingers.count(); ++i) {
                avgPos += fingers[i].tipPosition();
            }
            avgPos /= (float)fingers.count();
            
            /* temp mark
            std::cout << "Hand has " << fingers.count()
            << " fingers, average finger tip position" << avgPos << std::endl;
            */
        }
        
        /* temp mark
        // Get the hand's sphere radius and palm position
        std::cout << "Hand sphere radius: " << hand.sphereRadius()
        << " mm, palm position: " << hand.palmPosition() << std::endl;
        */
        
        // Get the hand's normal vector and direction
        const Vector normal = hand.palmNormal();
        const Vector direction = hand.direction();
        
        /* temp mark
        // Calculate the hand's pitch, roll, and yaw angles
        std::cout << "Hand pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
        << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
        << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
        */
        
        //---Socket Client---------------------------------------------
         
    //     if( direction.pitch() * RAD_TO_DEG >= 0 ) {
             

       
            if( hand.palmPosition().y > 25 && 100 >= hand.palmPosition().y){
                hand_level = 1;
            }
            else if(hand.palmPosition().y > 100 && 175 >= hand.palmPosition().y){
                hand_level = 2;
            }
            else{
                hand_level = 0;
            }
        
            //printf("高度: %f, 手指位置： %f", direction.pitch() * RAD_TO_DEG, fingers[0].tipPosition());
           // buf_out << fingers[0].tipPosition().y << ", " << fingers[0].tipPosition().x << ", " << fingers[0].tipPosition().z;
            buf_out << fingers[0].tipPosition().y << ", " << fingers[0].tipPosition().x << ", " << fingers[0].tipPosition().z;
            //sprintf(buf, "高度: %f\n",direction.pitch() * RAD_TO_DEG );
             
             /*
              std::string s = stringstream.str();
              const char* p = s.c_str(); 
             */             

    //     }
         
         /*
         printf("傳送結束字串...\n");
         sprintf(buf, "stop\n");
         z = sendto(sockfd,buf, sizeof(buf), 0, (struct sockaddr *)&adr_srvr, sizeof(adr_srvr));
        
         if (z < 0) {
            perror("sendto error");
            exit(1);
         }
         
         close(sockfd);
         printf("訊息傳送結束.\n");
         exit(0);
         */
         
        //---Socket Client----------------------------------------
    }
    else{
        buf_out << "0, 0, 0, 0";
    }
    
    // Get gestures
    const GestureList gestures = frame.gestures();
    for (int g = 0; g < gestures.count(); ++g) {
        Gesture gesture = gestures[g];
        
        switch (gesture.type()) {
            case Gesture::TYPE_CIRCLE:
            {
                CircleGesture circle = gesture;
                std::string clockwiseness;
                
                if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
                    clockwiseness = "clockwise";
                } else {
                    clockwiseness = "counterclockwise";
                }
                
                // Calculate angle swept since last frame
                float sweptAngle = 0;
                if (circle.state() != Gesture::STATE_START) {
                    CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
                    sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
                }
                std::cout << "Circle id: " << gesture.id()
                << ", state: " << gesture.state()
                << ", progress: " << circle.progress()
                << ", radius: " << circle.radius()
                << ", angle " << sweptAngle * RAD_TO_DEG
                <<  ", " << clockwiseness << std::endl;
                break;
            }
            case Gesture::TYPE_SWIPE:
            {
                SwipeGesture swipe = gesture;
                std::cout << "Swipe id: " << gesture.id()
                << ", state: " << gesture.state()
                << ", direction: " << swipe.direction()
                << ", speed: " << swipe.speed() << std::endl;
                break;
            }
            case Gesture::TYPE_KEY_TAP:
            {
                KeyTapGesture tap = gesture;
                std::cout << "*********** Key Tap id: " << gesture.id()
                << ", state: " << gesture.state()
                << ", position: " << tap.position()
                << ", direction: " << tap.direction()<< std::endl;

                key_tap = 1;
                break;
            }
            case Gesture::TYPE_SCREEN_TAP:
            {
                ScreenTapGesture screentap = gesture;
                std::cout << "Screen Tap id: " << gesture.id()
                << ", state: " << gesture.state()
                << ", position: " << screentap.position()
                << ", direction: " << screentap.direction()<< std::endl;
                break;
            }
            default:
                std::cout << "Unknown gesture type." << std::endl;
                break;
        }
    }
    
    if (!frame.hands().empty() || !gestures.empty()) {
        std::cout << std::endl;
    }
    
    //--socket send message-------------------
    if (!frame.hands().empty()){
        buf_out << ", " << key_tap;
    }
    
    buf_out << std::endl;
    std::cout << buf_out.str() << std::endl;
    
    z = (int)sendto(sockfd, buf_out.str().c_str(), sizeof(buf_out), 0, (struct sockaddr *)&adr_srvr, sizeof(adr_srvr));
    
    if (z < 0) {
        perror("sendto error");
        exit(1);
    }
    
    close(sockfd);
    //--end socket send message------------------
}

void SampleListener::onFocusGained(const Controller& controller) {
    std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
    std::cout << "Focus Lost" << std::endl;
}

/*
void define_level(){
    while (1) {
        z = (int)recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&adr_clnt, &adr_clnt_len);
    }
}
*/
int main() {
    
    
    // Create a sample listener and controller
    SampleListener listener;
    Controller controller;
    
    // Have the sample listener receive events from the controller
    controller.addListener(listener);
    
//    std::thread mThread( define_level );
    
    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();
    
    // Remove the sample listener when done
    controller.removeListener(listener);
    
    return 0;
}


