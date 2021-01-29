# VAIC_20_21
This repository contains V5 example code and documentation for VEX AI Competition 2020/2021. For more help with the VEX AI System, check out the <a href="#additional-resources">Additional Resources</a> section below.

# Nvidia Jetson Nano to VEX V5 Brain Communications:
This git project contains an VEXcode Pro V5 example project, which uses a new class that is a subclass of the vex::serial_link class. The new class adds the background tasks needed to keep robot to robot communication running and demonstrates a simple protocol for exchanging location data between robots.

<img src="https://kb.vex.com/hc/article_attachments/360068664092/ai_demo.png"></img>

The example project can be built for either manager or worker robot, it’s completely symmetrical. To switch between these modes, comment or un-comment the following line in main.cpp to control the build:
#define MANAGER_ROBOT 1


# VEX V5 Robot-to-Robot Communications (VEXlink):
The new VEXlink features have been added to the latest version of VEXcode Pro V5 (versions 2.0.2 and later). Downloaded VEXcode Pro V5 <a href="https://www.vexrobotics.com/vexcode-download">here</a>.

For more detailed information on using the V5 VEXlink API, <a href="https://drive.google.com/file/d/13mTA6BT7CPskJzh4YgsfAfoH9OgK75Hn/view">this document</a> explains the new libraries and how to use them effectively for robot-to-robot communication.

## Additional Resources

<!-- GETTING STARTED -->
<ul>
    <li>
      <a><strong>Get Started</strong></a>
      <ul>
        <li> <a href="https://kb.vex.com/hc/en-us/articles/360048984271-Getting-Started-With-the-VEX-AI-System">Getting Started With the VEX AI System</a></li>
        <li><a href="https://kb.vex.com/hc/en-us/articles/360048479792-Assembling-the-AI-Challenge-Field">Assembling the AI Challenge Field</a></li>
      </ul>
    </li>
  <li><strong>Setting Up VEX AI</strong></li>
  <ul>
    <li><a href="https://kb.vex.com/hc/en-us/articles/360048489612-Understanding-the-Parts-of-the-AI-System">Understanding the Parts of the AI System</a></li>
    <li><a href="https://kb.vex.com/hc/en-us/articles/360048973491-Connecting-and-Powering-the-Jetson-for-VEX-AI">Connecting and Powering the Jetson for VEX AI</a></li>
    <li><a href="https://kb.vex.com/hc/en-us/articles/360048489132-Installing-the-Intel-Dual-Band-Wi-Fi-and-Antennas-for-VEX-AI">Installing the Intel Dual Band Wi-Fi and Antennas for VEX AI</a></li>
    <li><a href="https://kb.vex.com/hc/en-us/articles/360048980931-Focusing-the-FLIR-Camera-for-VEX-AI">Focusing the FLIR Camera for VEX AI</a></li>
    <li><a href="https://kb.vex.com/hc/en-us/articles/360048983291-3D-Printing-Housings-for-the-VEX-AI-System">3D Printing Housings for the VEX AI System</a></li>
    </ul>
      <li class="section-list-item"><strong>Using VEX AI</strong></li>
    <ul>
                                  <li>
                              <a href="https://kb.vex.com/hc/en-us/articles/360048969891-Accessing-the-Dashboard-of-the-VEX-AI-Intel-Camera">Accessing the Dashboard of the VEX AI Intel Camera</a>
                            </li>
                            <li>
                              <a href="https://kb.vex.com/hc/en-us/articles/360048490972-Updating-the-VEX-AI-System">Updating the VEX AI System</a>
                            </li>
                            <li>
                              <a href="https://kb.vex.com/hc/en-us/articles/360049619171-Coding-the-VEX-AI-Robot">Coding the VEX AI Robot</a>
                            </li>
       </ul>




                   
