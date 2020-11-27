import os
import matplotlib.pyplot as plt

for i in range(21):
    raw_image_path = "/mnt/new/guidedog/raw_image/" + '{:04d}'.format(i)
    save_path = "/mnt/new/guidedog/image/" + '{:04d}'.format(i)
    os.makedirs(save_path, exist_ok=True)
    entries = os.listdir(raw_image_path)
    image_time_list=[]
    a = list(map(lambda x: x.replace('.png',''),entries))
    flag= False
    for entry in a:
        image_time_list.append(int(entry))
    idx_max = len(image_time_list)
    image_time_list.sort()
    for j, time in enumerate(image_time_list):
        image_name = raw_image_path + '/' + "{}.png".format(time)
        image = plt.imread(image_name)
        plt.imsave(save_path + "/" + '{:06d}'.format(j) + '.png', image)


