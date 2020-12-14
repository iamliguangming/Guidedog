#!/usr/bin/env python3

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import roslib

roslib.load_manifest('cv_forward')
import sys
import rospy

from std_msgs.msg import String
from sensor_msgs.msg import Image
from gazebo_msgs.msg import ModelStates
from geometry_msgs.msg import Pose, Twist, Point, Quaternion, Vector3

from cv_bridge import CvBridge, CvBridgeError
import _init_paths
import os
import json
import cv2
import numpy as np
import time
import torch
import copy
import math

from opts import opts
from dataset.dataset_factory import dataset_factory
# from forward_detector import ForwardDetector
from detector import Detector
from utils.utils import AverageMeter

CLASS_NAME = {1: "man", 2: "obstacle", 3: "traffic_light"}


class forward:
    def __init__(self, opt):
        self.CAMERA_CALIB = np.array([[762.7249337622711, 0, 640.5, 0],
                         [0, 762.7249337622711, 192.5, 0],
                        [0, 0, 1, 0]])
        self.Dataset = dataset_factory[opt.test_dataset]
        self.opt = opts().update_dataset_info_and_set_heads(opt, self.Dataset)
        self.detector_1 = Detector(opt)
        self.detector_2 = Detector(opt)
        self.detector_3 = Detector(opt)
        self.frame_id_1 = 1
        self.frame_id_2 = 1
        self.frame_id_3 = 1
        time_stats = ['tot', 'load', 'pre', 'net', 'dec', 'post']
        self.avg_time_stats = {t: AverageMeter() for t in time_stats}
        self.bridge = CvBridge()
        self.image1_sub = rospy.Subscriber("/rrbot/camera1/image_rect_color", Image, self.callback1)
        self.image2_sub = rospy.Subscriber("/rrbot/camera2/image_rect_color", Image, self.callback2)
        self.image3_sub = rospy.Subscriber("/rrbot/camera3/image_rect_color", Image, self.callback3)
        self.state_pub = rospy.Publisher("/rlocation", ModelStates, queue_size=1)
        self.input_meta = {}
        self.input_meta['calib'] = self.CAMERA_CALIB
        self.vel_his = 0
        self.count = 0
        self.old_x = 0
    def callback1(self, data):
        try:
            cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
        except CvBridgeError as e:
            print(e)
            return

        # cv2.imshow("Image_window", cv_image)
        if self.opt.tracking and self.frame_id_1 == 1:
            self.detector_1.reset_tracking()
        # img_path = "/home/zhuoyu/Documents/inciepo/CenterTrack/data/kitti_tracking/data_tracking_image_2/training/image_02/0000/" + str(
        #     self.frame_id).zfill(6) + ".png"
        # cv_image = cv2.imread(img_path)
        ret = self.detector_1.run(cv_image, self.input_meta)
        results = ret['results']

        temp_states = ModelStates()
        if len(results) == 0:
            temp_states.name.append('1' + 'nan')
            print("nan1")
        for result in results:
            temp_states.name.append('1' + CLASS_NAME[result['class']] + str(result['tracking_id']))
            temp_pose = Pose()
            temp_twist = Twist()
            temp_point = Point()
            temp_quat = Quaternion()
            temp_bbox = Vector3()
            temp_useless = Vector3()
            temp_point.x = result['loc'][2]
            temp_point.y = result['loc'][0]
            temp_point.z = -result['loc'][1]
            # if self.old_x !=0:
            #     vel = temp_point.x - self.old_x
            #     self.vel_his = (self.vel_his * self.count + vel) / (self.count + 1)
            # self.old_x = temp_point.x
            print(result['rot_y'])
            temp_quat.x = 0
            temp_quat.y = 0
            temp_quat.z = math.sin(result['rot_y'] / 2)
            temp_quat.w = math.cos(result['rot_y'] / 2)
            temp_pose.position = temp_point
            temp_pose.orientation = temp_quat
            temp_bbox.x = result['dim'][2]
            temp_bbox.y = result['dim'][1]
            temp_bbox.z = result['dim'][0]
            temp_useless.x = 0
            temp_useless.y = 0
            temp_useless.z = 0
            temp_twist.linear = temp_bbox
            temp_twist.angular = temp_useless
            temp_states.pose.append(temp_pose)
            temp_states.twist.append(temp_twist)
        self.state_pub.publish(temp_states)
        # print(temp_states.name)
        for t in self.avg_time_stats:
            self.avg_time_stats[t].update(ret[t])
        self.frame_id_1 += 1
        # for t in self.avg_time_stats:
        #     self.avg_time_stats[t].update(ret[t])
        # for t in self.avg_time_stats:
        #     if t == 'tot':
        #         print(t, self.avg_time_stats[t].avg)
    def callback2(self, data):
        try:
            cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
        except CvBridgeError as e:
            print(e)
            return

        # cv2.imshow("Image_window", cv_image)
        if self.opt.tracking and self.frame_id_2 == 1:
            self.detector_2.reset_tracking()
        # img_path = "/home/zhuoyu/Documents/inciepo/CenterTrack/data/kitti_tracking/data_tracking_image_2/training/image_02/0000/" + str(
        #     self.frame_id).zfill(6) + ".png"
        # cv_image = cv2.imread(img_path)
        ret = self.detector_2.run(cv_image, self.input_meta)
        results = ret['results']

        temp_states = ModelStates()
        if len(results) == 0:
            temp_states.name.append('2' + 'nan')
            print("nan2")
        for result in results:
            temp_states.name.append('2' + CLASS_NAME[result['class']] + str(result['tracking_id']))
            temp_pose = Pose()
            temp_twist = Twist()
            temp_point = Point()
            temp_quat = Quaternion()
            temp_bbox = Vector3()
            temp_useless = Vector3()
            temp_point.x = result['loc'][2]
            temp_point.y = result['loc'][0]
            temp_point.z = -result['loc'][1]
            # if self.old_x !=0:
            #     vel = temp_point.x - self.old_x
            #     self.vel_his = (self.vel_his * self.count + vel) / (self.count + 1)
            # self.old_x = temp_point.x
            print(result['rot_y'])
            temp_quat.x = 0
            temp_quat.y = 0
            temp_quat.z = math.sin(result['rot_y'] / 2)
            temp_quat.w = math.cos(result['rot_y'] / 2)
            temp_pose.position = temp_point
            temp_pose.orientation = temp_quat
            temp_bbox.x = result['dim'][2]
            temp_bbox.y = result['dim'][1]
            temp_bbox.z = result['dim'][0]
            temp_useless.x = 0
            temp_useless.y = 0
            temp_useless.z = 0
            temp_twist.linear = temp_bbox
            temp_twist.angular = temp_useless
            temp_states.pose.append(temp_pose)
            temp_states.twist.append(temp_twist)
        self.state_pub.publish(temp_states)
        # print(temp_states.name)
        for t in self.avg_time_stats:
            self.avg_time_stats[t].update(ret[t])
        self.frame_id_2 += 1
        # for t in self.avg_time_stats:
        #     self.avg_time_stats[t].update(ret[t])
        # for t in self.avg_time_stats:
        #     if t == 'tot':
        #         print(t, self.avg_time_stats[t].avg)
    def callback3(self, data):
        try:
            cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
        except CvBridgeError as e:
            print(e)
            return

        # cv2.imshow("Image_window", cv_image)
        if self.opt.tracking and self.frame_id_3 == 1:
            self.detector_3.reset_tracking()
        # img_path = "/home/zhuoyu/Documents/inciepo/CenterTrack/data/kitti_tracking/data_tracking_image_2/training/image_02/0000/" + str(
        #     self.frame_id).zfill(6) + ".png"
        # cv_image = cv2.imread(img_path)
        ret = self.detector_3.run(cv_image, self.input_meta)
        results = ret['results']

        temp_states = ModelStates()
        if len(results) == 0:
            temp_states.name.append('3' + 'nan')
            print("nan3")
        for result in results:
            temp_states.name.append('3' + CLASS_NAME[result['class']] + str(result['tracking_id']))
            temp_pose = Pose()
            temp_twist = Twist()
            temp_point = Point()
            temp_quat = Quaternion()
            temp_bbox = Vector3()
            temp_useless = Vector3()
            temp_point.x = result['loc'][2]
            temp_point.y = result['loc'][0]
            temp_point.z = -result['loc'][1]
            # if self.old_x !=0:
            #     vel = temp_point.x - self.old_x
            #     self.vel_his = (self.vel_his * self.count + vel) / (self.count + 1)
            # self.old_x = temp_point.x
            print(result['rot_y'])
            temp_quat.x = 0
            temp_quat.y = 0
            temp_quat.z = math.sin(result['rot_y'] / 2)
            temp_quat.w = math.cos(result['rot_y'] / 2)
            temp_pose.position = temp_point
            temp_pose.orientation = temp_quat
            temp_bbox.x = result['dim'][2]
            temp_bbox.y = result['dim'][1]
            temp_bbox.z = result['dim'][0]
            temp_useless.x = 0
            temp_useless.y = 0
            temp_useless.z = 0
            temp_twist.linear = temp_bbox
            temp_twist.angular = temp_useless
            temp_states.pose.append(temp_pose)
            temp_states.twist.append(temp_twist)
        self.state_pub.publish(temp_states)
        # print(temp_states.name)
        for t in self.avg_time_stats:
            self.avg_time_stats[t].update(ret[t])
        self.frame_id_3 += 1
        # for t in self.avg_time_stats:
        #     self.avg_time_stats[t].update(ret[t])
        # for t in self.avg_time_stats:
        #     if t == 'tot':
        #         print(t, self.avg_time_stats[t].avg)


def main(args):
    opt = opts().parse()
    os.environ['CUDA_VISIBLE_DEVICES'] = opt.gpus_str
    fw = forward(opt)
    rospy.init_node('forward_node', anonymous=True)
    try:
        rospy.spin()
    except KeyboardInterrupt:
        print("Shutting down")


if __name__ == '__main__':
    main(sys.argv)
