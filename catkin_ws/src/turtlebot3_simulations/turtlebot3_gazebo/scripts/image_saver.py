# rospy for the subscriber
import rospy
# ROS Image message
from sensor_msgs.msg import Image
# ROS Image message -> OpenCV2 image converter
from cv_bridge import CvBridge, CvBridgeError
# OpenCV2 for saving an image
import cv2

# Instantiate CvBridge
bridge = CvBridge()

def image_callback1(msg):
    print("Received an image!")
    try:
        # Convert your ROS Image message to OpenCV2
        cv2_img = bridge.imgmsg_to_cv2(msg, "bgr8")
    except CvBridgeError as e:
        print(e)
    else:
        # Save your OpenCV2 image as a jpeg 
        time = msg.header.stamp
        cv2.imwrite("/mnt/new/image_collect/1/"+str(time)+'.png', cv2_img)

def image_callback2(msg):
    print("Received an image!")
    try:
        # Convert your ROS Image message to OpenCV2
        cv2_img = bridge.imgmsg_to_cv2(msg, "bgr8")
    except CvBridgeError as e:
        print(e)
    else:
        # Save your OpenCV2 image as a jpeg
        time = msg.header.stamp
        cv2.imwrite("/mnt/new/image_collect/2/"+str(time)+'.png', cv2_img)

def image_callback3(msg):
    print("Received an image!")
    try:
        # Convert your ROS Image message to OpenCV2
        cv2_img = bridge.imgmsg_to_cv2(msg, "bgr8")
    except CvBridgeError as e:
        print(e)
    else:
        # Save your OpenCV2 image as a jpeg
        time = msg.header.stamp
        cv2.imwrite("/mnt/new/image_collect/3/"+str(time)+'.png', cv2_img)

def main():
    rospy.init_node('image_listener')
    # Define your image topic
    image_topic1 = "/rrbot/camera1/image_rect_color"
    image_topic2 = "/rrbot/camera2/image_rect_color"
    image_topic3 = "/rrbot/camera3/image_rect_color"
    # Set up your subscriber and define its callback
    rospy.Subscriber(image_topic1, Image, image_callback1)
    rospy.Subscriber(image_topic2, Image, image_callback2)
    rospy.Subscriber(image_topic3, Image, image_callback3)
    # Spin until ctrl + c
    rospy.spin()

if __name__ == '__main__':
    main()