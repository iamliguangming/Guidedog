import bagpy
import matplotlib
from bagpy import bagreader
import os
import pandas as pd
from scipy.spatial.transform import Rotation as R
import string
import matplotlib.image as mpimg
import matplotlib.pyplot as plt
from matplotlib import patches
import pandas as pd
import numpy as np
matplotlib.use('Agg')
b = bagreader('samplev1.bag')

# replace the topic name as per your need
model_states, t_model = b.message_by_topic('/gazebo/model_states')
camera_infos, t_camera = b.message_by_topic('/rrbot/camera1/camera_info')
# df_laser = pd.read_csv(LASER_MSG)
 # prints laser data in the form of pandas dataframe

# def world_camera(wor)
image_folder_name = 'training_set'
entries = os.listdir('.\\{}'.format(image_folder_name))
image_time_list=[]
a = list(map(lambda x: x.replace('.jpeg',''),entries))
flag= False
for entry in a:
    image_time_list.append(int(entry))
idx_max = len(image_time_list)
image_list_tracking_idx = 0
import math

def Remain_area_rate_man(bbox_1, bbox_2):
  x_1a, y_1a, x_1b, y_1b = bbox_1[:, 0], bbox_1[:, 1], bbox_1[:, 2], bbox_1[:, 3]
  x_2a, y_2a, x_2b, y_2b = bbox_2[:, 0], bbox_2[:, 1], bbox_2[:, 2], bbox_2[:, 3]

  x_a = np.maximum(x_1a, x_2a)
  y_a = np.maximum(y_1a, y_2a)

  x_b = np.minimum(x_1b, x_2b)
  y_b = np.minimum(y_1b, y_2b)
  inter_area = np.clip((x_b - x_a), a_min=0, a_max=None,out=(x_b - x_a)) * np.clip((y_b - y_a), a_min=0, a_max=None,out=(y_b - y_a))
  area_box1 = np.clip((x_1b - x_1a), a_min=0, a_max=None,out=(x_1b - x_1a)) * np.clip((y_1b - y_1a), a_min=0, a_max=None,out=(y_1b - y_1a))
  iou = (inter_area + 1e-3) / (area_box1 + 1e-3)
  return iou

def IOU(bbox_1, bbox_2):
  x_1a, y_1a, x_1b, y_1b = bbox_1[:, 0], bbox_1[:, 1], bbox_1[:, 2], bbox_1[:, 3]
  x_2a, y_2a, x_2b, y_2b = bbox_2[:, 0], bbox_2[:, 1], bbox_2[:, 2], bbox_2[:, 3]

  x_a = np.maximum(x_1a, x_2a)
  y_a = np.maximum(y_1a, y_2a)

  x_b = np.minimum(x_1b, x_2b)
  y_b = np.minimum(y_1b, y_2b)
  inter_area = np.clip((x_b - x_a), a_min=0, a_max=None,out=(x_b - x_a)) * np.clip((y_b - y_a), a_min=0, a_max=None,out=(y_b - y_a))
  area_box1 = np.clip((x_1b - x_1a), a_min=0, a_max=None,out=(x_1b - x_1a)) * np.clip((y_1b - y_1a), a_min=0, a_max=None,out=(y_1b - y_1a))
  area_box2 = np.clip((x_2b - x_2a), a_min=0, a_max=None,out=(x_2b - x_2a)) * np.clip((y_2b - y_2a), a_min=0, a_max=None,out=(y_2b - y_2a))
  union_area = area_box1 + area_box2 - inter_area
  iou = (inter_area + 1e-9) / (union_area + 1e-9)
  return iou

os.makedirs("resultcheck", exist_ok=True)
with open('train_label.txt', 'w') as f:

    for f_idx, msgs in enumerate(model_states):
            name = msgs.name
            pose = msgs.pose
            time_state = t_model[f_idx]
            camera_info = camera_infos[0]
            K = np.asarray(camera_info.K).reshape(3,3) #(3,3)
            sec = time_state.secs
            nsec = time_state.nsecs
            time_state = int(sec* 1e9 + nsec)
            current_image_stamp = 0
            if f_idx == 0 and time_state > image_time_list[image_list_tracking_idx]:
                raise Exception("There is no model_state_info for the starting image, try delete some images!")
            if time_state < image_time_list[image_list_tracking_idx]:
                new_f_idx = f_idx
                continue
            elif time_state == image_time_list[image_list_tracking_idx]:
                current_image_stamp = image_time_list[image_list_tracking_idx]
                image_list_tracking_idx+=1
                new_f_idx = f_idx
                pass
            else:
                name = model_states[f_idx-1].name
                pose = model_states[f_idx-1].pose
                time_state = t_model[f_idx-1]
                sec = time_state.secs
                nsec = time_state.nsecs
                time_state = int(sec * 1e9 + nsec)
                current_image_stamp = image_time_list[image_list_tracking_idx]  #number before .jpeg
                image_list_tracking_idx += 1
                new_f_idx = f_idx - 1

            turtlebot3_idx = 0
            for idx,i in enumerate(name):
                    if "burger" in i:
                        turtlebot3_idx = idx
            turtlebot3_x = pose[turtlebot3_idx].position.x
            turtlebot3_y = pose[turtlebot3_idx].position.y
            turtlebot3_z = pose[turtlebot3_idx].position.z
            turtlebot3_xq = pose[turtlebot3_idx].orientation.x
            turtlebot3_yq = pose[turtlebot3_idx].orientation.y
            turtlebot3_zq = pose[turtlebot3_idx].orientation.z
            turtlebot3_wq = pose[turtlebot3_idx].orientation.w
            r= R.from_quat([turtlebot3_xq,turtlebot3_yq,turtlebot3_zq,turtlebot3_wq])
            r_mat = r.as_matrix()
            r_mat_inv = r_mat.T
            t = np.array([turtlebot3_x,turtlebot3_y,turtlebot3_z])
            t_inv = -r_mat_inv @ t
            t_mat = np.array([t_inv[0],t_inv[1],t_inv[2],1])
            h_link_world = np.zeros((4,4))
            h_link_world[:3,:3]= r_mat_inv
            h_link_world[:, 3] = t_mat
            h_cam_link = np.zeros((4,4))
            h_cam_link[3,3] = 1
            h_cam_link[:3,:3]=np.array([
                    [0,-1,0],
                    [0,0,-1],
                    [1,0,0]
            ])
            h_cam_world = h_cam_link @ h_link_world

            img = ".\\{}\\{}.jpeg".format(image_folder_name,current_image_stamp)
            img_vis = mpimg.imread(img)
            # fig, ax = plt.subplots(1, 1)
            fig, ax = plt.subplots()
            ax.imshow(img_vis)
            man_flag = False

            ##############################
            light_f = False
            light_color =  -1
            ##############################
            list_frame = []
            name_list = ["man", "fire_hydrant", "trash_can", "oak_tree", "stop_sign", "mailbox","stop_light"]
            for idx,i in enumerate(name):
                ex_flag = False
                for object in name_list:
                    if object in i:
                        ###########################
                        if object == "stop_light":
                            if "stop_light_green" in i:
                                light_color = 0
                            else:
                                light_color = 1
                            light_f = True
                        ###########################
                        obj_class = object
                        ex_flag = True
                        break
                if  ex_flag == False:
                    continue
                ###########################
                # if man_f == False:
                #     continue
                ###########################
                i = i.replace("_","")
                track_id = int(i.strip(string.ascii_letters))

                manw_x = pose[idx].position.x
                manw_y = pose[idx].position.y
                manw_z = pose[idx].position.z
                manw_xq = pose[idx].orientation.x
                manw_yq = pose[idx].orientation.y
                manw_zq = pose[idx].orientation.z
                manw_wq = pose[idx].orientation.w
                r = R.from_quat([manw_xq, manw_yq, manw_zq, manw_wq])
                r_mat = r.as_matrix()
                t_mat = np.array([manw_x, manw_y, manw_z, 1])
                h_world_man = np.zeros((4, 4))
                h_world_man[:3, :3] = r_mat
                h_world_man[:, 3] = t_mat

                if obj_class == "man":
                    local_point1 = np.array([-0.25, -0.25, 0.25, 1])
                    local_point2 = np.array([0.25, 0.25, 0.25, 1])
                    local_point3 = np.array([-0.25, 0.25, 0.25, 1])
                    local_point4 = np.array([0.25, -0.25, 0.25, 1])
                    local_point5 = np.array([-0.25, -0.25, -1.7, 1])
                    local_point6 = np.array([0.25, 0.25, -1.7, 1])
                    local_point7 = np.array([-0.25, 0.25, -1.7, 1])
                    local_point8 = np.array([0.25, -0.25, -1.7, 1])
                elif obj_class == "oak_tree":
                    local_point1 = np.array([-0.4, -0.3, 1.2, 1])
                    local_point2 = np.array([0.2, 0.3, 1.2, 1])
                    local_point3 = np.array([-0.4, 0.3, 1.2, 1])
                    local_point4 = np.array([0.2, -0.3, 1.2, 1])
                    local_point5 = np.array([-0.4, -0.3, -0.8, 1])
                    local_point6 = np.array([0.2, 0.3, -0.8, 1])
                    local_point7 = np.array([-0.4, 0.3, -0.8, 1])
                    local_point8 = np.array([0.2, -0.3, -0.8, 1])
                elif obj_class == "stop_sign":
                    local_point1 = np.array([-0.25, -0.05, 0.9, 1])
                    local_point2 = np.array([0.25, 0.01, 0.9, 1])
                    local_point3 = np.array([-0.25, 0.01, 0.9, 1])
                    local_point4 = np.array([0.25, -0.05, 0.9, 1])
                    local_point5 = np.array([-0.25, -0.05, 0.3, 1])
                    local_point6 = np.array([0.25, 0.01, 0.3, 1])
                    local_point7 = np.array([-0.25, 0.01, 0.3, 1])
                    local_point8 = np.array([0.25, -0.05, 0.3, 1])
                elif obj_class == "trash_can":
                    local_point1 = np.array([-0.25, -0.25, -0.1, 1])
                    local_point2 = np.array([0.25, 0.25, -0.1, 1])
                    local_point3 = np.array([-0.25, 0.25, -0.1, 1])
                    local_point4 = np.array([0.25, -0.25, -0.1, 1])
                    local_point5 = np.array([-0.25, -0.25, -0.9, 1])
                    local_point6 = np.array([0.25, 0.25, -0.9, 1])
                    local_point7 = np.array([-0.25, 0.25, -0.9, 1])
                    local_point8 = np.array([0.25, -0.25, -0.9, 1])
                elif obj_class == "fire_hydrant":
                    local_point1 = np.array([-0.2, -0.2, 0.1, 1])
                    local_point2 = np.array([0.2, 0.2, 0.1, 1])
                    local_point3 = np.array([-0.2, 0.2, 0.1, 1])
                    local_point4 = np.array([0.2, -0.2, 0.1, 1])
                    local_point5 = np.array([-0.2, -0.2, -0.9, 1])
                    local_point6 = np.array([0.2, 0.2, -0.9, 1])
                    local_point7 = np.array([-0.2, 0.2, -0.9, 1])
                    local_point8 = np.array([0.2, -0.2, -0.9, 1])
                elif obj_class == "stop_light":
                    local_point1 = np.array([-0.2, 0, -0.9, 1])
                    local_point2 = np.array([0.2, -0.05, -0.9, 1])
                    local_point3 = np.array([-0.2, -0.05, -0.9, 1])
                    local_point4 = np.array([0.2, 0, -0.9, 1])
                    local_point5 = np.array([-0.2, 0, -1.75, 1])
                    local_point6 = np.array([0.2, -0.05, -1.75, 1])
                    local_point7 = np.array([-0.2, -0.05, -1.75, 1])
                    local_point8 = np.array([0.2, 0, -1.75, 1])
                elif obj_class == "mailbox":
                    local_point1 = np.array([-0.07, -0.15, 0.1, 1])
                    local_point2 = np.array([0.07, 0.15, 0.1, 1])
                    local_point3 = np.array([-0.07, 0.15, 0.1, 1])
                    local_point4 = np.array([0.07, -0.15, 0.1, 1])
                    local_point5 = np.array([-0.07, -0.15, -0.15, 1])
                    local_point6 = np.array([0.07, 0.15, -0.15, 1])
                    local_point7 = np.array([-0.07, 0.15, -0.15, 1])
                    local_point8 = np.array([0.07, -0.15, -0.15, 1])



                cam_point1= (h_cam_world @ h_world_man @ local_point1).astype(float)
                cam_point2 = (h_cam_world @ h_world_man @ local_point2).astype(float)
                cam_point3 = (h_cam_world @ h_world_man @ local_point3).astype(float)
                cam_point4 = (h_cam_world @ h_world_man @ local_point4).astype(float)
                cam_point5 = (h_cam_world @ h_world_man @ local_point5).astype(float)
                cam_point6 = (h_cam_world @ h_world_man @ local_point6).astype(float)
                cam_point7 = (h_cam_world @ h_world_man @ local_point7).astype(float)
                cam_point8 = (h_cam_world @ h_world_man @ local_point8).astype(float)
                cam_point = (h_cam_world @ h_world_man @ np.array([0,0,0,1])).astype(float)
                cam_point_front = (h_cam_world @ h_world_man @ np.array([1, 0, 0, 1])).astype(float)
                cam_point_up = (h_cam_world @ h_world_man @ np.array([0, 0, 1, 1])).astype(float)
                # in camera frame
                vector_front_in_cam = (cam_point_front - cam_point)
                vector_front_in_cam_xz = np.array([vector_front_in_cam[0],vector_front_in_cam[2]])
                base_cam_x_axis = np.array([1,0])
                theta = math.atan2(vector_front_in_cam_xz[1]+1e-9,vector_front_in_cam_xz[0]+1e-9)
                rotation_y = theta
                theta_ray = math.atan2(cam_point[2]+1e-9,cam_point[0]+1e-9)
                thetal = theta - theta_ray + math.radians(360)
                if thetal > math.radians(180):
                    thetal -= math.radians(360)
                elif thetal < math.radians(-180):
                    thetal += math.radians(360)
                alpha = thetal

                p = np.zeros((3,4))
                p[:3,:3]= K
                pix_coord1 = (p @ cam_point1 / cam_point1[2]).astype(float)
                pix_coord2 = (p @ cam_point2 / cam_point2[2]).astype(float)
                pix_coord3 = (p @ cam_point3 / cam_point3[2]).astype(float)
                pix_coord4 = (p @ cam_point4 / cam_point4[2]).astype(float)
                pix_coord5 = (p @ cam_point5 / cam_point5[2]).astype(float)
                pix_coord6 = (p @ cam_point6 / cam_point6[2]).astype(float)
                pix_coord7 = (p @ cam_point7 / cam_point7[2]).astype(float)
                pix_coord8 = (p @ cam_point8 / cam_point8[2]).astype(float)
                pix_coord = (p @ cam_point / cam_point[2]).astype(float)
                pix_coord_front = (p @ cam_point_front / cam_point_front[2]).astype(float)
                pix_coord_up = (p @ cam_point_up / cam_point_up[2]).astype(float)

                if min(cam_point1[2],cam_point2[2],cam_point3[2],cam_point4[2],cam_point5[2],cam_point6[2],cam_point7[2],cam_point8[2]) < 0:
                    continue
                umax=max(np.array([pix_coord1[0],pix_coord2[0],pix_coord3[0],pix_coord4[0],
                            pix_coord5[0],pix_coord6[0],pix_coord7[0],pix_coord8[0]]))
                umin=min(np.array([pix_coord1[0],pix_coord2[0],pix_coord3[0],pix_coord4[0],
                            pix_coord5[0],pix_coord6[0],pix_coord7[0],pix_coord8[0]]))
                vmax=max(np.array([pix_coord1[1],pix_coord2[1],pix_coord3[1],pix_coord4[1],
                            pix_coord5[1],pix_coord6[1],pix_coord7[1],pix_coord8[1]]))
                vmin=min(np.array([pix_coord1[1],pix_coord2[1],pix_coord3[1],pix_coord4[1],
                            pix_coord5[1],pix_coord6[1],pix_coord7[1],pix_coord8[1]]))

                if umax <= 0 or umin >=camera_info.width or vmax <= 0 or vmin >=camera_info.height or cam_point[2] > 12:
                    continue

                bbox_1 = np.array([umin,vmin,umax, vmax])
                bbox_1 = np.expand_dims(bbox_1, axis=0)
                bbox_2 = np.array([0,0,camera_info.width, camera_info.height])
                bbox_2 = np.expand_dims(bbox_2, axis=0)
                trunc = 3
                if umax <=camera_info.width and vmax <= camera_info.height and umin >=0 and vmin>=0:
                    trunc = 0
                else:
                    if Remain_area_rate_man(bbox_1, bbox_2)[0] < 0.5:
                        trunc = 2                                     #largely truncated
                    elif Remain_area_rate_man(bbox_1, bbox_2)[0] < 1:
                        trunc = 1                                     #partly truncated

                umax = max(0, min(camera_info.width, umax))
                vmax = max(0,min(camera_info.height, vmax))
                umin = min(max(0, umin), camera_info.width)
                vmin = min(max(0, vmin), camera_info.height)
                bbox = [umin,vmin,umax, vmax]
                rect = patches.Rectangle((umin, vmin), umax - umin, vmax - vmin,
                                         fill=False, color='yellow')
                ax.add_patch(rect)
                ax.text(umin, vmin, str(obj_class)+str(track_id), fontsize=6, color="white")

                each_obj = []
                if obj_class == "man":
                    color = -1
                    each_obj += [image_list_tracking_idx - 1, track_id, obj_class, trunc, 0, alpha, umin, vmin, umax,
                                vmax, 0.5, 0.5, 1.95, cam_point[0], cam_point[1], cam_point[2], rotation_y, color]
                elif obj_class == "fire_hydrant":
                    color = -1
                    each_obj += [image_list_tracking_idx - 1, -1, obj_class, trunc, 0, alpha, umin, vmin, umax,
                                vmax, 0.4, 0.4, 0.8, cam_point[0], cam_point[1], cam_point[2], rotation_y, color]
                elif obj_class == "trash_can":
                    color = -1
                    each_obj += [image_list_tracking_idx - 1, -1, obj_class, trunc, 0, alpha, umin, vmin, umax,
                                vmax, 0.5, 0.5, 0.8, cam_point[0], cam_point[1], cam_point[2], rotation_y, color]
                elif obj_class == "oak_tree":
                    color = -1
                    each_obj += [image_list_tracking_idx - 1, -1, obj_class, trunc, 0, alpha, umin, vmin, umax,
                                vmax, 0.6, 0.6, 2, cam_point[0], cam_point[1], cam_point[2], rotation_y, color]
                elif obj_class == "stop_sign":
                    color = -1
                    each_obj += [image_list_tracking_idx - 1, -1, obj_class, trunc, 0, alpha, umin, vmin, umax,
                                vmax, 0.5, 0.06, 0.6, cam_point[0], cam_point[1], cam_point[2], rotation_y, color]
                elif obj_class == "mailbox":
                    color = -1
                    each_obj += [image_list_tracking_idx - 1, -1, obj_class, trunc, 0, alpha, umin, vmin, umax,
                                vmax, 0.14, 0.3, 0.25, cam_point[0], cam_point[1], cam_point[2], rotation_y, color]
                elif obj_class == "stop_light":
                    color = light_color
                    each_obj += [image_list_tracking_idx - 1, -1, obj_class, trunc, 0, alpha, umin, vmin, umax,
                                vmax, 0.4, 0.05, 0.85, cam_point[0], cam_point[1], cam_point[2], rotation_y, color]



                # assert len(each_obj) == 18
                list_frame.append(each_obj)
                man_flag = True

            if man_flag == False and idx == len(name) - 1:
                continue

            else:
                    result_dir = ".\\resultcheck\\{}.jpeg".format(current_image_stamp)
                    plt.savefig(result_dir)
                    plt.close('all')
                    array_frame = pd.DataFrame(list_frame)

                    bboxes = array_frame.iloc[:,[6,7,8,9,15]].to_numpy()
                    array_frame = array_frame.sort_values(by=[15])    # sort using camera_z
                    bboxes = bboxes[bboxes[:, 4].argsort()]  # sort using camera_z
                    num_box = bboxes.shape[0]
                    for x in range(num_box):
                        flag = 0
                        for y in range(x):
                            if y == x:
                                continue
                            bbox1 = np.expand_dims(bboxes[x, :4], axis=0)
                            bbox2 = np.expand_dims(bboxes[y, :4], axis=0)
                            iou  = Remain_area_rate_man(bbox1, bbox2)
                            # iou = IOU(bbox1, bbox2)
                            if iou < 1e-5:
                                continue
                            elif iou > 0.5:
                                flag = 2
                                array_frame.iloc[x, 4] = 2
                                break
                            elif iou > 1e-5:
                                array_frame.iloc[x, 4] = max(flag, 1)
                                flag = 1
                            # print('{}/{} iou {}'.format(x, y, flag))

                    list_frame = array_frame.values.tolist()

                    for listitem in list_frame:
                        for item in listitem:
                            f.write("%s " % item)
                        f.write('\n')

            if image_list_tracking_idx == idx_max:
                break



