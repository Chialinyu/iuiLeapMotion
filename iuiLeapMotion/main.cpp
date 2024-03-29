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
int tapSettingDone;
float MaxVelocity;
float HighestFingerPosition;
float LowestFingerPosition;
long HighestFingerTimestamp;
long LowestFingerTimestamp; 

int sd;
struct sockaddr_in broadcastAddr;
std::ostringstream buf_out;
int key_tap = 0;
int ret;

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


void socket_setup(int port){
    //----------Socket Server Set up------------------------------------------------
    
    int sockfd;
    int w;
    socklen_t adr_clnt_len;
    // struct sockaddr_in adr_inet;
    struct sockaddr_in adr_clnt;
    adr_clnt_len = sizeof(adr_clnt);
    int len;
    
    //printf("等待 Client 端...\n");
    /*
     bzero(&adr_inet, sizeof(adr_inet));
     adr_inet.sin_family = AF_INET;
     adr_inet.sin_addr.s_addr = inet_addr("192.168.0.110");
     adr_inet.sin_port = htons(port);
     
     bzero(&adr_clnt, sizeof(adr_clnt));
     adr_clnt.sin_family = AF_INET;
     adr_clnt.sin_addr.s_addr = inet_addr("192.168.0.108");
     adr_clnt.sin_port = htons(port);
     
     len = sizeof(adr_clnt);
     
     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
     
     if (sockfd == -1) {
     perror("socket error");
     exit(1);
     }
     */
    //---------End Socket Server Setup-------------------------
    
    //---------------------------------------------
    // Open a socket
    
    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd<=0) {
        perror("Error: Could not open socket");
    }
    
    
    // Set socket options
    // Enable broadcast
    int broadcastEnable=1;
    ret=setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret) {
        perror("Error: Could not open set socket to broadcast mode");
        close(sd);
    }
    
    // Since we don't call bind() here, the system decides on the port for us, which is what we want.
    
    // Configure the port and ip we want to send to
    
    // Make an endpoint
    memset(&broadcastAddr, 0, sizeof broadcastAddr);
    broadcastAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "169.254.255.255", &broadcastAddr.sin_addr); /*192.168.0.255*//*169.254.255.255*/
    // Set the self broadcast IP address
    broadcastAddr.sin_port = htons(port); // Set port 8000
    
    // Send the broadcast request, ie "Any upnp devices out there?"
    
    //---------------------------------------------
    
    
    /* bind is for recvfrom
     w = bind(sockfd,(struct sockaddr *)&adr_inet, sizeof(adr_inet));
     
     if (w == -1) {
     perror("bind error");
     exit(1);
     }
     */
    
    /*---------- End Socket Server Set up-----------*/
    
}

void SampleListener::onFrame(const Controller& controller) {
    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();

//    std::cout << "Frame id: " << frame.id()
//    << ", timestamp: " << frame.timestamp()
//    << ", hands: " << frame.hands().count()
//    << ", fingers: " << frame.fingers().count()
//    << ", tools: " << frame.tools().count()
//    << ", gestures: " << frame.gestures().count() << std::endl;

    //------------------------------------------------------------old client socket--------------
    
    /*
     //--set up udp socket client--------------
     int sockfd;
     int z;
     int hand_level = 0;
     char buf[80];
     std::ostringstream buf_out;
     struct sockaddr_in adr_srvr;
     socklen_t addrlen = sizeof(adr_srvr);
     int port = 8000;
     int key_tap = 0;
     
     
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
     //--end server set up----------------------
     */
    
    //clean buf_out
    buf_out.str("");
    buf_out.clear();
    
    if (!frame.hands().empty()) {
        // Get the first hand
        Hand hand = frame.hands()[0];
        if(frame.hands().count() != 1){
            const Hand hand0 = frame.hands()[0];
            const Hand hand1 = frame.hands()[1];
            if(hand0.palmPosition().y >= hand1.palmPosition().y)
                hand = frame.hands()[1];
        }
        
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
        
        
        //---Socket Client start---------------------------------------------
        /*
         if( hand.palmPosition().y > 25 && 100 >= hand.palmPosition().y){
         hand_level = 1;
         }
         else if(hand.palmPosition().y > 100 && 175 >= hand.palmPosition().y){
         hand_level = 2;
         }
         else{
         hand_level = 0;
         }
         */
        
        //printf("高度: %f, 手指位置： %f", direction.pitch() * RAD_TO_DEG, fingers[0].tipPosition());
        // buf_out << fingers[0].tipPosition().y << ", " << fingers[0].tipPosition().x << ", " << fingers[0].tipPosition().z;
        
        
        //put finger position to buf_out
        buf_out << fingers[0].tipPosition().y << ", " << fingers[0].tipPosition().x << ", " << fingers[0].tipPosition().z << ", " << hand.palmPosition().y << ", " << hand.palmPosition().x << ", " << hand.palmPosition().z << ", " << fingers[0].tipVelocity().y;
        
        //sprintf(buf, "高度: %f\n",direction.pitch() * RAD_TO_DEG );
        //std::string s = stringstream.str();
        //const char* p = s.c_str();
        
        if (!tapSettingDone && abs(fingers[0].tipVelocity().y) > abs(hand.palmVelocity().y)
                            && !fingers.empty()) {
            if(fingers[0].tipVelocity().y < MaxVelocity && fingers[0].tipVelocity().y < 0){
                MaxVelocity = fingers[0].tipVelocity().y;
            }
            if (fingers[0].tipPosition().y > HighestFingerPosition && fingers[0].tipVelocity().y < 0) {
                HighestFingerPosition = fingers[0].tipPosition().y;
                HighestFingerTimestamp = frame.timestamp();
//                std::cout << "HighestTimestamp: " << HighestFingerTimestamp << std::endl;
//                std::cout << buf_out.str() << std::endl;
            }
            if (fingers[0].tipPosition().y < LowestFingerPosition && fingers[0].tipVelocity().y < 0) {
                LowestFingerPosition = fingers[0].tipPosition().y;
                LowestFingerTimestamp = frame.timestamp();
//                std::cout << "LowestTimestamp: " << LowestFingerTimestamp << std::endl;
//                std::cout << buf_out.str() << std::endl;
            }
        }
        
    }
    else{
        
        buf_out << "0, 0, 0, 0, 0, 0, 0, 0";
    }
    //---End Socket Client----------------------------------------
    
    //-----------------------------------------------------end of old client socket--------------
    
    //Set new keytap configuration
    if(tapSettingDone){
        controller.config().setFloat("Gesture.KeyTap.MinDistance", 2.0f);
        
        if ( MaxVelocity > -50 ){
            controller.config().setFloat("Gesture.KeyTap.MinDownVelocity", fabs(MaxVelocity));
        }
        
        if ( fabs(LowestFingerTimestamp - HighestFingerTimestamp)/1000000 > 0.1) {
            controller.config().setFloat("Gesture.KeyTap.HistorySeconds", fabs(LowestFingerTimestamp - HighestFingerTimestamp)/1000000);
        }
        controller.config().save();
    }
    
    // Get gestures
    const GestureList gestures = frame.gestures();
    for (int g = 0; g < gestures.count(); ++g) {
        Gesture gesture = gestures[g];
        
        switch (gesture.type()) {
//            case Gesture::TYPE_CIRCLE:
//            {
//                CircleGesture circle = gesture;
//                std::string clockwiseness;
//                
//                if (circle.pointable().direction().angleTo(circle.normal()) <= PI/4) {
//                    clockwiseness = "clockwise";
//                } else {
//                    clockwiseness = "counterclockwise";
//                }
//                
//                // Calculate angle swept since last frame
//                float sweptAngle = 0;
//                if (circle.state() != Gesture::STATE_START) {
//                    CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
//                    sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
//                }
//                std::cout << "Circle id: " << gesture.id()
//                << ", state: " << gesture.state()
//                << ", progress: " << circle.progress()
//                << ", radius: " << circle.radius()
//                << ", angle " << sweptAngle * RAD_TO_DEG
//                <<  ", " << clockwiseness << std::endl;
//                break;
//            }
//            case Gesture::TYPE_SWIPE:
//            {
//                SwipeGesture swipe = gesture;
//                std::cout << "Swipe id: " << gesture.id()
//                << ", state: " << gesture.state()
//                << ", direction: " << swipe.direction()
//                << ", speed: " << swipe.speed() << std::endl;
//                break;
//            }
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
//            case Gesture::TYPE_SCREEN_TAP:
//            {
//                ScreenTapGesture screentap = gesture;
//                std::cout << "Screen Tap id: " << gesture.id()
//                << ", state: " << gesture.state()
//                << ", position: " << screentap.position()
//                << ", direction: " << screentap.direction()<< std::endl;
//                break;
//            }
            default:
//                std::cout << "Unknown gesture type." << std::endl;
                break;
        }
    }
    
    if (!frame.hands().empty() || !gestures.empty()) {
 //       std::cout << std::endl;
    }
    
    
    //socket add key_tap
    if (!frame.hands().empty()){
        buf_out << ", " << key_tap;
    }
    
    //--socket send message-------------------
    /*
     w = (int)sendto(sockfd, buf_out.str().c_str(), strlen(buf_out.str().c_str()), 0, (struct sockaddr *)&adr_clnt, sizeof(adr_clnt));
     //w = (int)send(sockfd, buf_out.str().c_str(), sizeof(buf_out), 0);
     
     if (w < 0) {
     perror("send error");
     exit(1);
     }
     */
    socket_setup(8000);
/*
    printf("%d.%d.%d.%d\n",
           int(broadcastAddr.sin_addr.s_addr&0xFF),
           int((broadcastAddr.sin_addr.s_addr&0xFF00)>>8),
           int((broadcastAddr.sin_addr.s_addr&0xFF0000)>>16),
           int((broadcastAddr.sin_addr.s_addr&0xFF000000)>>24));
*/
    ret = sendto(sd, buf_out.str().c_str(), strlen(buf_out.str().c_str()), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
//    buf_out << std::endl;
//    std::cout << buf_out.str() << std::endl;
    key_tap = 0;
    
    if (ret<0) {
        perror("Error: Could not open send broadcast");
        close(sd);
    }
    
    close(sd);
    
    //--end socket send message------------------
    
}

void SampleListener::onFocusGained(const Controller& controller) {
    std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
    std::cout << "Focus Lost" << std::endl;
}

int main() {
    
    
    // Create a sample listener and controller
    SampleListener listener_transmitting;
    Controller controller;
    
    SampleListener listener_tapSetting;
    
    tapSettingDone = 0;
    MaxVelocity = 0;
    HighestFingerPosition = 0;
    LowestFingerPosition = 1000;
    int input;
    while (scanf("%d", &input) == 1) {
        if (input == 1) { //tapSetting Mode
            std::cout << "Start Setting KeyTap!\n";
            tapSettingDone = 0;
            MaxVelocity = 0;
            HighestFingerPosition = 0;
            LowestFingerPosition = 1000;
            // Have the sample listener receive events from the controller
            controller.addListener(listener_tapSetting);
            printf("tapSetting......\n");
            //std::cout << "Press Enter to quit..." << std::endl;
            //std::cin.get();
        }else if(input == 2){
            // Remove the sample listener when done
            controller.removeListener(listener_tapSetting);
            std::cout << "HighestTapVelocity: " << MaxVelocity << std::endl;
            std::cout << "HistorySeconds: " << fabs(LowestFingerTimestamp - HighestFingerTimestamp)/1000000 <<std::endl;
            tapSettingDone = 1;
        }else if(input == 3){
            std::cout << "Start Transmitting!\n";
            controller.addListener(listener_transmitting);
            
        }
    }
    
    //    std::thread mThread( define_level );
    
    // Keep this process running until Enter is pressed
    
    
    return 0;
}


