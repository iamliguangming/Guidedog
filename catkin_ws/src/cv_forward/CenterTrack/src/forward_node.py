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
from forward_detector import ForwardDetector
from utils.utils import AverageMeter

class forward:
    def __init__(self, opt):
        self.CAMERA_CALIB = np.array([[721.53772, 0, 0, 44.85728], [0, 721.53772, 0, 0.21638], [0, 0, 1, 0.00275]])
        self.Dataset = dataset_factory[opt.test_dataset]
        self.opt = opts().update_dataset_info_and_set_heads(opt, self.Dataset)
        self.detector = ForwardDetector(opt)
        self.frame_id = 1
        time_stats = ['tot', 'load', 'pre', 'net', 'dec', 'post']
        self.avg_time_stats = {t: AverageMeter() for t in time_stats}
        self.bridge = CvBridge()
        self.image_sub = rospy.Subscriber("/rrbot/camera1/image_rect_color", Image, self.callback)
        self.state_pub = rospy.Publisher("guidedog_modelstates", ModelStates, queue_size=10)
        self.input_meta = {}
        self.input_meta['calib'] = self.CAMERA_CALIB

    def callback(self, data):
        try:
            cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
        except CvBridgeError as e:
            print(e)
            return

        # cv2.imshow("Image_window", cv_image)
        if self.opt.tracking and self.frame_id == 1:
            self.detector.reset_tracking()
        # img_path = "/home/zhuoyu/Documents/inciepo/CenterTrack/data/kitti_tracking/data_tracking_image_2/training/image_02/0000/" + str(
        #     self.frame_id).zfill(6) + ".png"
        # cv_image = cv2.imread(img_path)
        ret = self.detector.run(cv_image, self.input_meta)
        results = ret['results']

        temp_states = ModelStates()
        for result in results:
            temp_states.name.append('class:' + str(result['class']) + ' tracking_id:' + str(result['tracking_id']))
            temp_pose = Pose()
            temp_twist = Twist()
            temp_point = Point()
            temp_quat = Quaternion()
            temp_bbox = Vector3()
            temp_useless = Vector3()
            temp_point.x = result['loc'][0]
            temp_point.y = result['loc'][1]
            temp_point.z = result['loc'][2]
            temp_quat.x = 0
            temp_quat.y = 0
            temp_quat.z = math.sin(result['rot_y']/2)
            temp_quat.w = math.cos(result['rot_y']/2)
            temp_pose.position = temp_point
            temp_pose.orientation = temp_quat
            temp_bbox.x = result['dim'][0]
            temp_bbox.y = result['dim'][1]
            temp_bbox.z = result['dim'][2]
            temp_useless.x = 0
            temp_useless.y = 0
            temp_useless.z = 0
            temp_twist.linear = temp_bbox
            temp_twist.angular = temp_useless
            temp_states.pose.append(temp_pose)
            temp_states.twist.append(temp_twist)
        self.state_pub.publish(temp_states)
        print(temp_states.name)
        for t in self.avg_time_stats:
            self.avg_time_stats[t].update(ret[t])
        self.frame_id += 1
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
